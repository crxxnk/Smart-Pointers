#include <iostream>

template<typename Ptr, typename Deleter>
class my_unique_ptr
{
    Ptr* pointer;
    Deleter deleter = std::default_delete;
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

    // Members
    auto operator*() const noexcept -> Ptr&;
    auto operator->() const noexcept -> Ptr&;
    auto get() const noexcept -> Ptr*;
};

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(std::nullptr_t)
{
    static_assert(noexcept(_Dltr) && std::is_default_constructible<_Dltr>); // deleter must be a nothrow default constructible
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(Ptr* ptr)
{
    static_assert(noexcept(_Dltr) && std::is_default_constructible<_Dltr>); // deleter must be a nothrow default constructible
    pointer = ptr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept
{
    static_assert(noexcept(_Dltr) && std::is_copy_constructible<_Dltr>); // deleter must be a nothrow copy constructible
    pointer = ptr;
    deleter = _Dltr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept
{
    static_assert(noexcept(_Dltr) && std::is_move_constructible<_Dltr>); // deleter must be a nothrow move constructible
    pointer = ptr;
    deleter = _Dltr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(my_unique_ptr&& u) noexcept
{
    static_assert(noexcept(u.deleter) && std::is_move_constructible<u.deleter>); // deleter must be a nothrow move constructible
    pointer = u.pointer;
    u.pointer = nullptr;
    deleter = u.deleter;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::~my_unique_ptr()
{
    deleter(pointer);
}

template<typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::operator*() const noexcept -> Ptr&
{
    return *pointer;
}

template<typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::operator->() const noexcept -> Ptr&
{
    return pointer;
}

template<typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::get() const noexcept -> Ptr*
{
    return pointer;
}