#pragma once
#include <iostream>
#include <memory>
#include <type_traits>

#define _NODISCARD [[nodiscard]]

template<typename Ptr, typename Deleter = std::default_delete<Ptr>>
class my_unique_ptr
{
    Ptr* pointer;
    Deleter deleter;
public:
    // Constructors && Destructor
    my_unique_ptr() noexcept;
    my_unique_ptr(std::nullptr_t) noexcept;
    explicit my_unique_ptr(Ptr* _Ptr) noexcept;
    my_unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept;
    my_unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept;
    my_unique_ptr(my_unique_ptr&& u) noexcept;
    my_unique_ptr(const my_unique_ptr&) = delete;
    ~my_unique_ptr();
    
    // Operators
    auto operator=(my_unique_ptr&& u) noexcept -> my_unique_ptr&;
    auto operator=(std::nullptr_t) noexcept -> my_unique_ptr&;
    auto operator=(const my_unique_ptr&) -> my_unique_ptr& = delete;
    _NODISCARD auto operator*() const noexcept -> Ptr&;
    _NODISCARD auto operator->() const noexcept -> Ptr*;
    explicit operator bool() const noexcept;

    // Members
    _NODISCARD auto get() const noexcept -> Ptr*;
    _NODISCARD auto get_deleter() const noexcept -> const Deleter&;
    auto release() noexcept -> Ptr*;
    auto reset(Ptr* _Ptr = nullptr) noexcept -> void;
    auto swap(my_unique_ptr& other) noexcept -> void;
};

template<typename Ptr, typename Deleter>
auto operator<<(std::ostream &os, const my_unique_ptr<Ptr, Deleter>& u) -> std::ostream& {
    os << u.get();
    return os;
};

template<typename Ptr, typename Deleter>
auto operator==(const my_unique_ptr<Ptr, Deleter>& u, const my_unique_ptr<Ptr, Deleter>& _u) -> bool {
    return u.get() == _u.get();
}

template<typename Ptr, typename Deleter>
auto operator!=(const my_unique_ptr<Ptr, Deleter>& u, const my_unique_ptr<Ptr, Deleter>& _u) -> bool {
    return u.get() != _u.get();
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr() noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = nullptr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(std::nullptr_t) noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = nullptr;
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
    deleter = std::move(_Dltr);
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>); // deleter must be a nothrow move constructible
    pointer = _Ptr;
    deleter = std::move(_Dltr);
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::my_unique_ptr(my_unique_ptr&& u) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<decltype(u.deleter)>); // deleter must be a nothrow move constructible
    pointer = u.pointer;
    deleter = std::move(u.deleter);
    u.pointer = nullptr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr, Deleter>::~my_unique_ptr()
{
    if(pointer)
        deleter(pointer); // or get_deleter(get())
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::operator=(my_unique_ptr&& u) noexcept -> my_unique_ptr&
{
    static_assert(std::is_nothrow_move_constructible_v<decltype(u.deleter)>); // deleter must be a nothrow move constructible
    if(this != &u) {
        deleter(pointer);
        pointer = u.pointer;
        deleter = std::move(u.deleter);
        u.pointer = nullptr;
    }
    return *this;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::operator=(std::nullptr_t) noexcept -> my_unique_ptr&
{
    reset(pointer);
    return *this;
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
my_unique_ptr<Ptr, Deleter>::operator bool() const noexcept
{
    return pointer != nullptr;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::get() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::get_deleter() const noexcept -> const Deleter&
{
    return deleter;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::release() noexcept -> Ptr*
{
    Ptr* ptr = pointer;
    pointer = nullptr;
    return ptr;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::reset(Ptr* _Ptr) noexcept -> void
{
    if(pointer)
        deleter(pointer);
    pointer = _Ptr;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr, Deleter>::swap(my_unique_ptr &other) noexcept -> void
{
    std::swap(pointer, other.pointer);
    std::swap(deleter, other.deleter);
}

template<typename Ptr, typename Deleter>
class my_unique_ptr<Ptr[], Deleter>
{
    Ptr* pointer;
    Deleter deleter;
public:
    // Constructors && Destructor
    my_unique_ptr() noexcept;
    my_unique_ptr(std::nullptr_t) noexcept;
    explicit my_unique_ptr(Ptr* _Ptr) noexcept;
    my_unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept;
    my_unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept;
    my_unique_ptr(my_unique_ptr&& u) noexcept;
    my_unique_ptr(const my_unique_ptr&) = delete;
    ~my_unique_ptr();
    
    // Operators
    auto operator=(my_unique_ptr&& u) noexcept -> my_unique_ptr&;
    auto operator=(std::nullptr_t) noexcept -> my_unique_ptr&;
    auto operator=(const my_unique_ptr&) -> my_unique_ptr& = delete;
    _NODISCARD auto operator[](size_t i) const -> Ptr&;
    explicit operator bool() const noexcept;

    // Members
    _NODISCARD auto get() const noexcept -> Ptr*;
    _NODISCARD auto get_deleter() const noexcept -> const Deleter&;
    _NODISCARD auto release() noexcept -> Ptr*;
    auto reset(Ptr* _Ptr = nullptr) noexcept -> void;
    auto swap(my_unique_ptr& other) noexcept -> void;
};

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr[], Deleter>::my_unique_ptr() noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = nullptr;
    std::cout << "Array constructor" << std::endl;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr[], Deleter>::my_unique_ptr(std::nullptr_t) noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = nullptr;
    std::cout << "Array constructor" << std::endl;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr[], Deleter>::my_unique_ptr(Ptr* _Ptr) noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = _Ptr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr[], Deleter>::my_unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept
{
    static_assert(std::is_nothrow_copy_constructible_v<Deleter>); // deleter must be a nothrow copy constructible
    pointer = _Ptr;
    deleter = std::move(_Dltr);
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr[], Deleter>::my_unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>); // deleter must be a nothrow move constructible
    pointer = _Ptr;
    deleter = std::move(_Dltr);
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr[], Deleter>::my_unique_ptr(my_unique_ptr&& u) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<decltype(u.deleter)>); // deleter must be a nothrow move constructible
    pointer = u.pointer;
    deleter = std::move(u.deleter);
    u.pointer = nullptr;
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr[], Deleter>::~my_unique_ptr()
{
    if(pointer)
        deleter(pointer);
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr[], Deleter>::operator=(my_unique_ptr&& u) noexcept -> my_unique_ptr&
{
    static_assert(std::is_nothrow_move_constructible_v<decltype(u.deleter)>); // deleter must be a nothrow move constructible
    if(this != &u) {
        deleter(pointer);
        pointer = u.pointer;
        deleter = std::move(u.deleter);
        u.pointer = nullptr;
    }
    return *this;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr[], Deleter>::operator=(std::nullptr_t) noexcept -> my_unique_ptr&
{
    reset(pointer);
    return *this;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr[], Deleter>::operator[](size_t i) const -> Ptr&
{
    return pointer[i];
}

template <typename Ptr, typename Deleter>
my_unique_ptr<Ptr[], Deleter>::operator bool() const noexcept
{
    return pointer != nullptr;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr[], Deleter>::get() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr[], Deleter>::get_deleter() const noexcept -> const Deleter&
{
    return deleter;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr[], Deleter>::reset(Ptr* _Ptr) noexcept -> void
{
    if(pointer)
        deleter(pointer);
    pointer = _Ptr;
}

template <typename Ptr, typename Deleter>
auto my_unique_ptr<Ptr[], Deleter>::release() noexcept -> Ptr*
{
    Ptr* ptr = pointer;
    pointer = nullptr;
    return ptr;
}