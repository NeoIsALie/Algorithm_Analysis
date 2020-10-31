#ifndef TIMSORT_H
#define TIMSORT_H

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>
#include <functional>
#include <vector>
#include <limits>
#include "utils.h"
#include "timsort_stack_buffer.h"
#include "minrun.h"
#include "compiler.h"

namespace tim {

namespace internal {

template <class It,
	  class Comp>
struct TimSort
{
	using value_type = iterator_value_type_t<It>;
	TimSort(It begin_it, It end_it, Comp comp_func):
		stack_buffer{},
		heap_buffer{},
		start(begin_it),
		stop(end_it),
		position(begin_it),
		comp(comp_func),
		minrun(compute_minrun<value_type>(end_it - begin_it)),
		min_gallop(default_min_gallop)
	{
		fill_run_stack();
		collapse_run_stack();
	}


	void fill_run_stack()
	{
		push_next_run();
		if(not (position < stop))
			return;
		push_next_run();
		while(position < stop)
		{
			resolve_invariants();
			push_next_run();
		}
	}

	inline void collapse_run_stack()
	{
		for(auto count = stack_buffer.run_count() - 1; count > 0; --count)
			merge_BC();
	}

	void push_next_run()
	{
		if(const std::size_t remain = stop - position;
		   COMPILER_LIKELY_(remain > 1))
		{
			std::size_t idx = 2;
			if(comp(position[1], position[0]))
			{
				while(idx < remain and comp(position[idx], position[idx - 1]))
					++idx;
				std::reverse(position, position + idx);
			}
			while(idx < remain and not comp(position[idx], position[idx - 1]))
				++idx;
			if(idx < remain and idx < minrun)
			{
				auto extend_to = std::min(minrun, remain);
				finish_insertion_sort(position, position + idx, position + extend_to, comp);
				idx = extend_to;
			}
			position += idx;
		}
		else
		{
			position = stop;
		}
		stack_buffer.push(position - start);
	}

	void resolve_invariants()
	{
		auto run_count = stack_buffer.run_count();
		do{
			if(((run_count > 2) and stack_buffer.merge_ABC_case_1())
			   or ((run_count > 3) and stack_buffer.merge_ABC_case_2()))
				if(stack_buffer.merge_AB())
					merge_AB();
				else
					merge_BC();
			else if(stack_buffer.merge_BC())
				merge_BC();
			else
				break;
			--run_count;
		} while(run_count > 1);
	}

	void merge_BC()
	{
		merge_runs(start + get_offset<2>(),
			   start + get_offset<1>(),
			   start + get_offset<0>());
		stack_buffer.template remove_run<1>();
	}

	void merge_AB()
	{
		merge_runs(start + get_offset<3>(),
			   start + get_offset<2>(),
			   start + get_offset<1>());
		stack_buffer.template remove_run<2>();
	}

	template <std::size_t I>
	inline auto get_offset() const noexcept
	{
		return stack_buffer.template get_offset<I>();
	}

	void merge_runs(It begin, It mid, It end)
	{

		begin = gallop_upper_bound(begin, mid, *mid, comp);
		end = gallop_upper_bound(std::make_reverse_iterator(end),
					 std::make_reverse_iterator(mid),
					 mid[-1],
					 [comp=this->comp](auto&& a, auto&& b){
					    return comp(std::forward<decltype(b)>(b), std::forward<decltype(a)>(a));
					 }).base();

		if(COMPILER_LIKELY_(begin < mid or mid < end))
		{
			if((end - mid) > (mid - begin))
				do_merge(begin, mid, end, comp);
			else
				do_merge(std::make_reverse_iterator(end),
					 std::make_reverse_iterator(mid),
					 std::make_reverse_iterator(begin),
					 [comp=this->comp](auto&& a, auto&& b){
						// reverse the comparator
						return comp(std::forward<decltype(b)>(b),
							    std::forward<decltype(a)>(a));
					 }
				);
		}
	}

	template <class Iter, class Cmp>
	void do_merge(Iter begin, Iter mid, Iter end, Cmp cmp)
	{
		// check to see if we can use the run stack as a temporary buffer
		if(stack_buffer.can_acquire_merge_buffer(begin, mid))
		{
			// allocate the merge buffer on the stack
			auto stack_mem = stack_buffer.move_to_merge_buffer(begin, mid);
			gallop_merge(stack_mem, stack_mem + (mid - begin),
						     mid, end,
						     begin, cmp);
		}
		else
		{
			heap_buffer.reserve(mid - begin);
			if constexpr(can_forward_memcpy_v<Iter> or not can_reverse_memcpy_v<Iter>)
			{
				if constexpr (can_forward_memcpy_v<Iter>)
				{
					heap_buffer.resize(mid - begin);
					std::memcpy(heap_buffer.data(), get_memcpy_iterator(begin), (mid - begin) * sizeof(value_type));
				}
				else
				{
					heap_buffer.assign(std::make_move_iterator(begin), std::make_move_iterator(mid));
				}
				gallop_merge(heap_buffer.begin(), heap_buffer.end(),
							     mid, end,
							     begin, cmp);
			}
			else
			{
				heap_buffer.resize(mid - begin);
				std::memcpy(heap_buffer.data(), get_memcpy_iterator(mid - 1), (mid - begin) * sizeof(value_type));
				gallop_merge(heap_buffer.rbegin(), heap_buffer.rend(),
							     mid, end,
							     begin, cmp);
			}
			heap_buffer.clear();
		}
	}

	template <class LeftIt, class RightIt, class DestIt, class Cmp>
	void gallop_merge(LeftIt lbegin, LeftIt lend, RightIt rbegin, RightIt rend, DestIt dest, Cmp cmp)
	{

		for(std::size_t num_galloped=0, lcount=0, rcount=0 ; ;)
		{
			for(lcount=(num_galloped > 0), rcount=0, num_galloped=0;;)
			{
				if(cmp(*rbegin, *lbegin))
				{
					// move from the right-hand-side
					*dest = std::move(*rbegin);
					++dest;
					++rbegin;
					++rcount;
					// merge is done.  copy the remaining elements from the left range and return
					if(not (rbegin < rend))
					{
						move_or_memcpy(lbegin, lend, dest);
						return;
					}
					else if(rcount >= min_gallop)
						goto gallop_right; // continue this run in galloping mode
					lcount = 0;
				}
				else
				{
					// move from the left-hand side
					*dest = std::move(*lbegin);
					++dest;
					++lbegin;
					++lcount;
					// don't need to check if we reached the end.  that will happen on the right-hand-side
					if(lcount >= min_gallop)
						goto gallop_left; // continue this run in galloping mode
					rcount = 0;
				}
			}
			COMPILER_UNREACHABLE_;
			// GALLOP SEARCH MODE
			while(lcount >= gallop_win_dist or rcount >= gallop_win_dist) {
				// decrement min_gallop every time we continue the gallop loop
				if(min_gallop > 1)
					--min_gallop;

				num_galloped = 1;
			    gallop_left:
				lcount = lend - lbegin;
				while((num_galloped < lcount) and not cmp(*rbegin, lbegin[num_galloped]))
					num_galloped = 2 * num_galloped + 1;
				if(lcount > num_galloped)
					lcount = num_galloped;
				lcount = std::upper_bound(lbegin + (num_galloped / 2), lbegin + lcount, *rbegin, cmp) - lbegin;
				dest = move_or_memcpy(lbegin, lbegin + lcount, dest);
				lbegin += lcount;

				num_galloped = 1;
			    gallop_right: // when jumping here from the linear merge loop, num_galloped is set to zero
				rcount = rend - rbegin;
				while((num_galloped < rcount) and cmp(rbegin[num_galloped], *lbegin))
					num_galloped = 2 * num_galloped + 1;
				if(rcount > num_galloped)
					rcount = num_galloped;
				rcount = std::lower_bound(rbegin + (num_galloped / 2), rbegin + rcount, *lbegin, cmp) - rbegin;
				dest = std::move(rbegin, rbegin + rcount, dest);
				rbegin += rcount;

				if(not (rbegin < rend))
				{
					move_or_memcpy(lbegin, lend, dest);
					return;
				}
			}
			++min_gallop;
			*dest = std::move(*lbegin);
			++dest;
			++lbegin;
		}
		COMPILER_UNREACHABLE_;
	}

	timsort_stack_buffer<std::size_t, value_type> stack_buffer;
	/** Fallback heap-allocated array used for merge buffer. */
	std::vector<value_type> heap_buffer;
	/** 'begin' iterator to the range being sorted. */
	const It start;
	/** 'end' iterator to the range being sorted. */
	const It stop;

	It position;
	/** Comparator used to sort the range. */
	Comp comp;
	/** Minimum length of a run */
	const std::size_t minrun;

	std::size_t min_gallop = default_min_gallop;

	static constexpr const std::size_t default_min_gallop = gallop_win_dist;
};




template <class It, class Comp>
static void _timsort(It begin, It end, Comp comp)
{
	using value_type = iterator_value_type_t<It>;
	std::size_t len = end - begin;
	if(len > max_minrun<value_type>())
		TimSort<It, Comp>(begin, end, comp);
	else
		finish_insertion_sort(begin, begin + (end > begin), end, comp);
}

} /* namespace internal */


template <class It, class Comp>
void timsort(It begin, It end, Comp comp)
{
	internal::_timsort(begin, end, comp);
}


template <class It>
void timsort(It begin, It end)
{
	timsort(begin, end, tim::internal::DefaultComparator{});
}



} /* namespace tim */


#include "undef_compiler.h"

#endif /* TIMSORT_H */
