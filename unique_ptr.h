#include <iostream>
#include <memory>
#include <type_traits>

template<typename Ptr, typename Deleter = std::default_delete<Ptr>>
class my_unique_ptr
{
    Ptr* pointer;
    Deleter deleter;
public:
    // Constructors && Destructor
    my_unique_ptr(std::nullptr_t) noexcept;
    explicit my_unique_ptr(Ptr* _Ptr) noexcept;
    my_unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept;
    my_unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept;
    my_unique_ptr(my_unique_ptr&& u) noexcept;
    my_unique_ptr(const my_unique_ptr&) = delete;
    ~my_unique_ptr();
    
    // Operators
    auto operator=(const my_unique_ptr&) -> my_unique_ptr& = delete;
    auto operator*() const noexcept -> Ptr&;
    auto operator->() const noexcept -> Ptr*;

    // Members
    auto get() const noexcept -> Ptr*;
    auto reset(Ptr* _Ptr = nullptr) noexcept -> void;
    auto get_deleter() const noexcept -> const Deleter&;
};

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(std::nullptr_t) noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(Ptr* _Ptr) noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = _Ptr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept
{
    static_assert(std::is_nothrow_copy_constructible_v<Deleter>); // deleter must be a nothrow copy constructible
    pointer = _Ptr;
    deleter = _Dltr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>); // deleter must be a nothrow move constructible
    pointer = _Ptr;
    deleter = _Dltr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(my_unique_ptr&& u) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<decltype(u.deleter)>); // deleter must be a nothrow move constructible
    pointer = u.pointer;
    u.pointer = nullptr;
    deleter = u.deleter;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::~my_unique_ptr()
{
    deleter(pointer);
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::operator*() const noexcept -> Ptr&
{
    return *pointer;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::operator->() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::get() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::reset(Ptr* _Ptr) noexcept -> void
{
    deleter(pointer);
    pointer = _Ptr;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::get_deleter() const noexcept -> const Deleter&
{
    return deleter;
}
