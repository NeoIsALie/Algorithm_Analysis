#ifndef UTILS_H
#define UTILS_H
#include <algorithm>
#include "compiler.h"
#include "memcpy_algos.h"
#include "iter.h"
#include "minrun.h"


namespace tim {
namespace internal {


inline constexpr const std::size_t gallop_win_dist = 7;

struct DefaultComparator
{
	template <class Left, class Right>
	inline bool operator()(Left&& left, Right&& right) const
	{
		return std::forward<Left>(left) < std::forward<Right>(right);
	}
};

template <class It, class T, class Comp>
inline It gallop_upper_bound(It begin, It end, const T& value, Comp comp)
{
	std::size_t i = 0;
	std::size_t len = end - begin;
	for(; i < len and not comp(value, begin[i]); i = 2 * i + 1) { /* LOOP */ }

	if(len > i)
		len = i;
	len -= (i / 2);
	begin += (i / 2);
	// hand-rolled std::upper_bound.
	for(; len > 0;)
	{
		i = len / 2;
		if (comp(value, begin[i]))
			len = i;
		else
		{
			begin += (i + 1);
			len -= (i + 1);
		}
	}
	return begin;
}


template <class It>
inline void rotate_left(It begin, It end)
{

	using value_type = iterator_value_type_t<It>;
	constexpr std::size_t use_temporary_upper_limit = 3 * sizeof(void*);
	constexpr bool use_temporary = sizeof(value_type) < use_temporary_upper_limit;
	if constexpr(use_temporary)
	{
		// for small types, implement using a temporary.
		if(end - begin > 1)
		{
			value_type temp = std::move(end[-1]);
			std::move_backward(begin, end - 1, end);
			*begin = std::move(temp);
		}
	}
	else
	{
		// for large types, implement as a series of adjacent swaps
		for(end -= (end > begin); end > begin; --end)
			std::swap(end[-1], *end);
	}
}


template <class It, class Comp>
void finish_insertion_sort(It begin, It mid, It end, Comp comp)
{
	using value_type = iterator_value_type_t<It>;
	if constexpr(std::is_scalar_v<value_type>
		     and (   std::is_same_v<Comp, std::less<>>
		          or std::is_same_v<Comp, std::less<value_type>>
		          or std::is_same_v<Comp, std::greater<>>
		          or std::is_same_v<Comp, std::greater<value_type>>
		          or std::is_same_v<Comp, DefaultComparator>)
	)
	{
		while(mid < end)
		{
			for(auto pos = mid; pos > begin and comp(*pos, pos[-1]); --pos)
				std::swap(pos[-1], *pos);
			++mid;
		}
	}
	else
	{
		for(const auto stop = begin + std::min(max_minrun<value_type>() / 4, std::size_t(end - begin)); mid < stop; ++mid)
			for(auto pos = mid; pos > begin and comp(*pos, pos[-1]); --pos)
				std::swap(pos[-1], *pos);
		for(; mid < end; ++mid)
			rotate_left(std::upper_bound(begin, mid, *mid, comp), mid + 1);
	}

}

} /* namespace internal */
} /* namespace tim */


#endif /* UTILS_H */
