#pragma once
#include <type_traits>

namespace iosp {
    template<typename, typename = void>
    struct is_allocator : std::false_type{};
}

template<typename A>
struct iosp::is_allocator<A, std::void_t<
    typename A::value_type,
    decltype(std::declval<A&>().allocate(std::size_t{})),
    decltype(std::declval<A&>().deallocate(std::declval<typename A::value_type*>(), std::size_t{}))
>> : std::true_type{};