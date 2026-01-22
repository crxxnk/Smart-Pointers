#pragma once
#include <iostream>
#include <memory>
#include <type_traits>

#define _NODISCARD [[nodiscard]]

namespace iosp { // implementation of smart pointers
    template<typename Ptr, typename Deleter = std::default_delete<Ptr>>
    class unique_ptr;

    template<typename Ptr, typename Deleter>
    class unique_ptr<Ptr[], Deleter>;

    template<typename T, typename... Args>
    _NODISCARD auto make_unique(Args&&... args) -> iosp::unique_ptr<T>;

    template<typename T>
    _NODISCARD auto make_unique(size_t size) -> iosp::unique_ptr<T>;
};

template<typename T, typename... Args>
_NODISCARD auto iosp::make_unique(Args&&... args) -> iosp::unique_ptr<T>
{
    return iosp::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
_NODISCARD auto iosp::make_unique(size_t size) -> iosp::unique_ptr<T>
{
    
}

template<typename Ptr, typename Deleter>
class iosp::unique_ptr
{
    Ptr* pointer;
    Deleter deleter;
public:
    // Constructors && Destructor
    unique_ptr() noexcept;
    unique_ptr(std::nullptr_t) noexcept;
    explicit unique_ptr(Ptr* _Ptr) noexcept;
    unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept;
    unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept;
    unique_ptr(unique_ptr&& u) noexcept;
    unique_ptr(const unique_ptr&) = delete;
    ~unique_ptr();
    
    // Operators
    auto operator=(unique_ptr&& u) noexcept -> unique_ptr&;
    auto operator=(std::nullptr_t) noexcept -> unique_ptr&;
    auto operator=(const unique_ptr&) -> unique_ptr& = delete;
    _NODISCARD auto operator*() const noexcept -> Ptr&;
    _NODISCARD auto operator->() const noexcept -> Ptr*;
    explicit operator bool() const noexcept;

    // Members
    _NODISCARD auto get() const noexcept -> Ptr*;
    _NODISCARD auto get_deleter() const noexcept -> const Deleter&;
    auto release() noexcept -> Ptr*;
    auto reset(Ptr* _Ptr = nullptr) noexcept -> void;
    auto swap(unique_ptr& other) noexcept -> void;
};

template<typename Ptr, typename Deleter>
auto operator<<(std::ostream &os, const iosp::unique_ptr<Ptr, Deleter>& u) -> std::ostream& {
    os << u.get();
    return os;
};

template<typename Ptr, typename Deleter>
auto operator==(const iosp::unique_ptr<Ptr, Deleter>& u, const iosp::unique_ptr<Ptr, Deleter>& _u) -> bool {
    return u.get() == _u.get();
}

template<typename Ptr, typename Deleter>
auto operator!=(const iosp::unique_ptr<Ptr, Deleter>& u, const iosp::unique_ptr<Ptr, Deleter>& _u) -> bool {
    return u.get() != _u.get();
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr, Deleter>::unique_ptr() noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = nullptr;
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr, Deleter>::unique_ptr(std::nullptr_t) noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = nullptr;
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr, Deleter>::unique_ptr(Ptr* _Ptr) noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = _Ptr;
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr, Deleter>::unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept
{
    static_assert(std::is_nothrow_copy_constructible_v<Deleter>); // deleter must be a nothrow copy constructible
    pointer = _Ptr;
    deleter = std::move(_Dltr);
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr, Deleter>::unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>); // deleter must be a nothrow move constructible
    pointer = _Ptr;
    deleter = std::move(_Dltr);
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr, Deleter>::unique_ptr(unique_ptr&& u) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<decltype(u.deleter)>); // deleter must be a nothrow move constructible
    pointer = u.pointer;
    deleter = std::move(u.deleter);
    u.pointer = nullptr;
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr, Deleter>::~unique_ptr()
{
    if(pointer)
        deleter(pointer); // or get_deleter(get())
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr, Deleter>::operator=(unique_ptr&& u) noexcept -> unique_ptr&
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
auto iosp::unique_ptr<Ptr, Deleter>::operator=(std::nullptr_t) noexcept -> unique_ptr&
{
    reset(pointer);
    return *this;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr, Deleter>::operator*() const noexcept -> Ptr&
{
    return *pointer;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr, Deleter>::operator->() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr, Deleter>::operator bool() const noexcept
{
    return pointer != nullptr;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr, Deleter>::get() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr, Deleter>::get_deleter() const noexcept -> const Deleter&
{
    return deleter;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr, Deleter>::release() noexcept -> Ptr*
{
    Ptr* ptr = pointer;
    pointer = nullptr;
    return ptr;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr, Deleter>::reset(Ptr* _Ptr) noexcept -> void
{
    if(pointer)
        deleter(pointer);
    pointer = _Ptr;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr, Deleter>::swap(unique_ptr &other) noexcept -> void
{
    std::swap(pointer, other.pointer);
    std::swap(deleter, other.deleter);
}

template<typename Ptr, typename Deleter>
class iosp::unique_ptr<Ptr[], Deleter>
{
    Ptr* pointer;
    Deleter deleter;
public:
    // Constructors && Destructor
    unique_ptr() noexcept;
    unique_ptr(std::nullptr_t) noexcept;
    explicit unique_ptr(Ptr* _Ptr) noexcept;
    unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept;
    unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept;
    unique_ptr(unique_ptr&& u) noexcept;
    unique_ptr(const unique_ptr&) = delete;
    ~unique_ptr();
    
    // Operators
    auto operator=(unique_ptr&& u) noexcept -> unique_ptr&;
    auto operator=(std::nullptr_t) noexcept -> unique_ptr&;
    auto operator=(const unique_ptr&) -> unique_ptr& = delete;
    _NODISCARD auto operator[](size_t i) const -> Ptr&;
    explicit operator bool() const noexcept;

    // Members
    _NODISCARD auto get() const noexcept -> Ptr*;
    _NODISCARD auto get_deleter() const noexcept -> const Deleter&;
    _NODISCARD auto release() noexcept -> Ptr*;
    auto reset(Ptr* _Ptr = nullptr) noexcept -> void;
    auto swap(unique_ptr& other) noexcept -> void;
};

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr[], Deleter>::unique_ptr() noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = nullptr;
    std::cout << "Array constructor" << std::endl;
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr[], Deleter>::unique_ptr(std::nullptr_t) noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = nullptr;
    std::cout << "Array constructor" << std::endl;
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr[], Deleter>::unique_ptr(Ptr* _Ptr) noexcept
{
    static_assert(std::is_nothrow_default_constructible_v<Deleter>); // deleter must be a nothrow default constructible
    pointer = _Ptr;
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr[], Deleter>::unique_ptr(Ptr* _Ptr, const Deleter& _Dltr) noexcept
{
    static_assert(std::is_nothrow_copy_constructible_v<Deleter>); // deleter must be a nothrow copy constructible
    pointer = _Ptr;
    deleter = std::move(_Dltr);
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr[], Deleter>::unique_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>); // deleter must be a nothrow move constructible
    pointer = _Ptr;
    deleter = std::move(_Dltr);
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr[], Deleter>::unique_ptr(unique_ptr&& u) noexcept
{
    static_assert(std::is_nothrow_move_constructible_v<decltype(u.deleter)>); // deleter must be a nothrow move constructible
    pointer = u.pointer;
    deleter = std::move(u.deleter);
    u.pointer = nullptr;
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr[], Deleter>::~unique_ptr()
{
    if(pointer)
        deleter(pointer);
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr[], Deleter>::operator=(unique_ptr&& u) noexcept -> unique_ptr&
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
auto iosp::unique_ptr<Ptr[], Deleter>::operator=(std::nullptr_t) noexcept -> unique_ptr&
{
    reset(pointer);
    return *this;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr[], Deleter>::operator[](size_t i) const -> Ptr&
{
    return pointer[i];
}

template <typename Ptr, typename Deleter>
iosp::unique_ptr<Ptr[], Deleter>::operator bool() const noexcept
{
    return pointer != nullptr;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr[], Deleter>::get() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr[], Deleter>::get_deleter() const noexcept -> const Deleter&
{
    return deleter;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr[], Deleter>::reset(Ptr* _Ptr) noexcept -> void
{
    if(pointer)
        deleter(pointer);
    pointer = _Ptr;
}

template <typename Ptr, typename Deleter>
auto iosp::unique_ptr<Ptr[], Deleter>::release() noexcept -> Ptr*
{
    Ptr* ptr = pointer;
    pointer = nullptr;
    return ptr;
}