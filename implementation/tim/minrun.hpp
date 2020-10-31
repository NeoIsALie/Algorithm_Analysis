#ifndef TIMSORT_MINRUN_H
#define TIMSORT_MINRUN_H


namespace tim {
namespace internal {

template <class T>
static constexpr std::size_t max_minrun() noexcept
{
	if constexpr(sizeof(T) > (sizeof(void*) * 8))
		return 16;
	else if constexpr(sizeof(T) > (sizeof(void*) * 4))
		return 32;
	else
		return 64;
}

template <class T>
static constexpr std::size_t compute_minrun(std::size_t n) noexcept
{
	constexpr std::size_t minrun_max = max_minrun<T>();
	std::size_t r = 0;
	while (n >= minrun_max)
	{
		r |= (n & 1);
		n >>= 1;
	}
	return (n + r);
}


} /* namespace internal */
} /* namespace tim */


#endif /* TIMSORT_MINRUN_H */
