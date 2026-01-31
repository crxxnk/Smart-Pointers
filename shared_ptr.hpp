#pragma once
#include <type_traits>
#include <atomic>
#include <memory>
#include <iostream>
#include "weak_ptr.hpp"

#define DEBUG

namespace iosp { // implementation of smart pointers
    template<typename Ptr>
    class shared_ptr;

    template<typename T, typename... Args>
    _NODISCARD auto make_shared(Args&&... args) -> iosp::shared_ptr<T>;
    template<typename T>
    _NODISCARD auto make_shared(size_t size) -> iosp::shared_ptr<T>;
}

template<typename, typename = void>
struct is_allocator : std::false_type{};

template<typename A>
struct is_allocator<A, std::void_t<
    typename A::value_type,
    decltype(std::declval<A&>().allocate(std::size_t{})),
    decltype(std::declval<A&>().deallocate(std::declval<typename A::value_type*>(), std::size_t{}))
>> : std::true_type{};

struct control_block;
template<typename Ptr, typename Deleter = std::default_delete<Ptr>, typename Allocator = void>
struct object_owner_alloc;
template<typename Ptr, typename Deleter = std::default_delete<Ptr>>
struct object_owner;
template<typename T>
struct make_shared_control_block;

struct control_block
{
    std::atomic_size_t strong_ref{1};
    std::atomic_size_t weak_ref{0};
    virtual ~control_block() = default;
    virtual auto destroy() -> void = 0;
    control_block() = default;
    control_block(const control_block&) = delete;
    control_block& operator=(const control_block&) = delete;
};

template<typename T>
struct make_shared_control_block : control_block
{
    void destroy() {
        T* obj = reinterpret_cast<T*>(reinterpret_cast<char*>(this) + sizeof(make_shared_control_block));
        obj->~T();
        this->~control_block();
        ::operator delete(this);
    }
};

template<typename T, typename... Args>
_NODISCARD auto iosp::make_shared(Args&&... args) -> iosp::shared_ptr<T>
{
    size_t cb_size = sizeof(make_shared_control_block<T>);
    size_t obj_size = sizeof(T);
    void* mem = ::operator new (cb_size+obj_size);
    make_shared_control_block<T>* cb;
    cb = new (mem) make_shared_control_block<T>();
    T* obj = new (reinterpret_cast<char*>(mem) + cb_size) T(std::forward<Args>(args)...); 
    return iosp::shared_ptr<T>(obj, cb);
};

#ifdef DEBUG
    static size_t alloc_count = 0;
#endif

template<typename Ptr, typename Deleter, typename Allocator>
struct object_owner_alloc : public control_block
{
    Ptr* pointer;
    Deleter deleter;
    
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<object_owner_alloc<Ptr, Deleter, Allocator>>;
    Alloc allocator;

    object_owner_alloc(Ptr* p, Deleter d, Alloc a) : pointer(p), deleter(std::move(d)), allocator(std::move(a)) {}
    void destroy() override {
        if(pointer) {
            deleter(pointer);
            pointer = nullptr;
        }

        using _Alloc_Traits = std::allocator_traits<Alloc>;
        _Alloc_Traits::destroy(allocator, this);
        _Alloc_Traits::deallocate(allocator, this, 1);
    }
};

template<typename Ptr, typename Deleter>
struct object_owner : public control_block
{
    Ptr* pointer;
    Deleter deleter;

    object_owner(Ptr* p, Deleter d) : pointer(p), deleter(std::move(d)) {}
    void destroy() override {
        if(pointer) {
            deleter(pointer);
            pointer = nullptr;
        }
    }
};

template<typename Ptr>
class iosp::shared_ptr
{
    Ptr* pointer;
    control_block* cb;

    template<typename>
    friend class shared_ptr; // Every instantiation of shared_ptr is a friend of every other instantiation

public:
    // Constructors && Destructor
    shared_ptr() noexcept;
    shared_ptr(std::nullptr_t) noexcept;

    template<typename Y>
    explicit shared_ptr(Y* _Ptr);
    // checks if Deleter is the control_block so the compiler knows if it has to choose this or the private constructor
    template <typename Y, typename Deleter, typename std::enable_if_t<!std::is_base_of_v<control_block, std::remove_pointer_t<Deleter>>>>
    shared_ptr(Y* _Ptr, Deleter _Dltr);

    template<typename Deleter>
    shared_ptr(std::nullptr_t _Ptr, Deleter _Dltr);

    template<typename Y, typename Deleter, typename Allocator>
    shared_ptr(Y* _Ptr, Deleter _Dltr, Allocator _Alloc); // Custom allocator for the control block
    
    template<typename Deleter, typename Allocator>
    shared_ptr(std::nullptr_t _Ptr, Deleter _Dltr, Allocator _Alloc);

    template<typename Y>
    shared_ptr(const shared_ptr<Y>& s, Ptr* _Ptr) noexcept; // Aliasing constructor

    shared_ptr(const shared_ptr& s) noexcept;
    shared_ptr(shared_ptr&& s) noexcept;

    template<typename Y>
    explicit shared_ptr(shared_ptr<Y>&& s) noexcept;

    // template<typename Y>
    // explicit shared_ptr(const iosp::weak_ptr<Y>& w) = delete; //! NOT IMPLEMENTED YET

    template<typename Y, typename Deleter>
    shared_ptr(iosp::unique_ptr<Y, Deleter>&& u);

    ~shared_ptr();

private:
    template<typename Y>
    shared_ptr(Y* _Ptr, control_block* _CB) : pointer(_Ptr), cb(_CB) {}
    template<typename T, typename... Args>
    friend auto iosp::make_shared(Args&&... args) -> iosp::shared_ptr<T>;
public:
    // Operators
    auto operator=(const shared_ptr& s) -> shared_ptr&;
    template<typename Y>
    auto operator=(const shared_ptr<Y>& s) noexcept -> shared_ptr&;
    auto operator=(shared_ptr&& s) noexcept -> shared_ptr&;
    template<typename Y>
    auto operator=(shared_ptr<Y>&& s) noexcept -> shared_ptr&;
    template<typename Y, typename Deleter>
    auto operator=(unique_ptr<Y, Deleter>&& u) -> shared_ptr&;

    _NODISCARD auto operator*() const noexcept -> Ptr&;
    _NODISCARD auto operator->() const noexcept -> Ptr*;
    explicit operator bool() const noexcept;

    // Members
    _NODISCARD auto get() const noexcept -> Ptr*;
    _NODISCARD auto unique() const noexcept -> bool;
    _NODISCARD auto use_count() const noexcept -> std::size_t;
    // _NODISCARD auto get_deleter() const noexcept -> const Deleter&;
    auto reset() noexcept -> void;
    template <typename Y>
    auto reset(Y* _Ptr) -> void;
    template<typename Y, typename Deleter>
    auto reset(Y* _Ptr, Deleter _Dltr) -> void;
    template<typename Y, typename Deleter, typename Allocator>
    auto reset(Y* _Ptr, Deleter _Dltr, Allocator _Alloc) -> void;

    auto swap(shared_ptr& other) noexcept -> void;
};

template <typename Ptr>
iosp::shared_ptr<Ptr>::shared_ptr() noexcept
{
    pointer = nullptr;
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::shared_ptr(std::nullptr_t) noexcept
{
    pointer = nullptr;
}

template <typename Ptr>
template <typename Y>
iosp::shared_ptr<Ptr>::shared_ptr(Y *_Ptr)
{
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");
    pointer = _Ptr;

    if(_Ptr)
        cb = new object_owner<Ptr>(_Ptr, std::default_delete<Ptr>{});
    else
        cb = nullptr;
}

template <typename Ptr>
template <typename Y, typename Deleter, typename std::enable_if_t<!std::is_base_of_v<control_block, std::remove_pointer_t<Deleter>>>>
iosp::shared_ptr<Ptr>::shared_ptr(Y *_Ptr, Deleter _Dltr)
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>);
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");
    pointer = _Ptr;
    cb = new object_owner<Ptr, Deleter>(_Ptr, std::move(_Dltr));
}

template <typename Ptr>
template <typename Deleter>
iosp::shared_ptr<Ptr>::shared_ptr(std::nullptr_t _Ptr, Deleter _Dltr)
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>);
    pointer = nullptr;
    cb = new object_owner<Ptr, Deleter>(nullptr, std::move(_Dltr));
}

template <typename Ptr>
template <typename Y, typename Deleter, typename Allocator>
iosp::shared_ptr<Ptr>::shared_ptr(Y* _Ptr, Deleter _Dltr, Allocator _Alloc)
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>);
    static_assert(is_allocator<Allocator>::value);
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");

    using _CB = object_owner_alloc<Ptr, Deleter, Allocator>;
    std::cout << "size of ptr_base " << sizeof(_CB) << std::endl;
    using _Alloc_CB = typename std::allocator_traits<Allocator>::template rebind_alloc<_CB>; // custom allocator is converted to now allocate the control block
                                                                                             // creates a new allocator type that has the same behavior as Allocator
                                                                                             // but changes its value_type to _CB

    _Alloc_CB alloc_cb(_Alloc);
    _CB* mem = std::allocator_traits<_Alloc_CB>::allocate(alloc_cb, 1); // allocators only allocate raw memory and do not construct an object
    alloc_count++;
    try {
        cb = new (mem) _CB(_Ptr, std::move(_Dltr), alloc_cb); // constructs a control_block object on top of the allocated memory
        pointer = _Ptr;
    } catch(...) {
        std::allocator_traits<_Alloc_CB>::deallocate(alloc_cb, mem, 1);
        throw;
    }
}

template <typename Ptr>
template <typename Deleter, typename Allocator>
iosp::shared_ptr<Ptr>::shared_ptr(std::nullptr_t _Ptr, Deleter _Dltr, Allocator _Alloc)
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>);
    static_assert(is_allocator<Allocator>::value);

    using _CB = object_owner_alloc<Ptr, Deleter, Allocator>;
    using _Alloc_CB = typename std::allocator_traits<Allocator>::template rebind_alloc<_CB>;

    _Alloc_CB alloc_cb(_Alloc);
    _CB* mem = std::allocator_traits<_Alloc_CB>::allocate(alloc_cb, 1);
    try {
        cb = new (mem) _CB(nullptr, std::move(_Dltr), alloc_cb);
        pointer = nullptr;
    } catch(...) {
        std::allocator_traits<_Alloc_CB>::deallocate(alloc_cb, mem, 1);
        throw;
    }
}

template <typename Ptr>
template <typename Y>
iosp::shared_ptr<Ptr>::shared_ptr(const shared_ptr<Y>& s, Ptr* _Ptr) noexcept
{
    pointer = _Ptr;
    cb = s.cb;
    if(cb)
        cb->strong_ref.fetch_add(1);
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::shared_ptr(const shared_ptr &s) noexcept
{
    pointer = s.pointer;
    cb = s.cb;
    if(cb)
        cb->strong_ref.fetch_add(1);
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::shared_ptr(shared_ptr&& s) noexcept
{
    pointer = s.pointer;
    cb = s.cb;
    s.pointer = nullptr;
    s.cb = nullptr;
}

template <typename Ptr>
template<typename Y>
iosp::shared_ptr<Ptr>::shared_ptr(shared_ptr<Y>&& s) noexcept
{
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");
    pointer = s.pointer;
    cb = s.cb;
    s.pointer = nullptr;
    s.cb = nullptr;
}

template <typename Ptr>
template <typename Y, typename Deleter>
iosp::shared_ptr<Ptr>::shared_ptr(iosp::unique_ptr<Y, Deleter> &&u)
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>);
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");

    auto p = u.release();
    try {
        cb = new object_owner<Ptr, Deleter>(p, std::move(u.get_deleter()));
        pointer = p;
    } catch (...) {
        u.get_deleter()(p);
        throw;
    }
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::~shared_ptr()
{
    if(cb && cb->strong_ref.fetch_sub(1) == 1) {
        cb->destroy();
    }
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::operator=(const shared_ptr& s) -> shared_ptr&
{
    if(this != &s) {
        if(cb && cb->strong_ref.fetch_sub(1) == 1)
            cb->destroy();
        
        pointer = s.pointer;
        cb = s.cb;

        if(cb)
            cb->strong_ref.fetch_add(1);
    }

    return *this;
}

template <typename Ptr>
template <typename Y>
auto iosp::shared_ptr<Ptr>::operator=(const shared_ptr<Y>& s) noexcept -> shared_ptr&
{
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");
    if(this != &s) {
        if(cb && cb->strong_ref.fetch_sub(1) == 1)
            cb->destroy();
        
        pointer = s.pointer;
        cb = s.cb;

        if(cb)
            cb->strong_ref.fetch_add(1);
    }

    return *this;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::operator=(shared_ptr &&s) noexcept -> shared_ptr&
{
    if(this != &s) {
        if(cb && cb->strong_ref.fetch_sub(1) == 1)
            cb->destroy();
        pointer = s.pointer;
        cb = s.cb;
    }
    return *this;
}

template <typename Ptr>
template <typename Y>
auto iosp::shared_ptr<Ptr>::operator=(shared_ptr<Y>&& s) noexcept -> shared_ptr&
{
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");
    if(this != &s) {
        if(cb && cb->strong_ref.fetch_sub(1) == 1)
            cb->destroy();
        pointer = s.pointer;
        cb = s.cb;

        s.pointer = nullptr;
        s.cb = nullptr;
    }
    return *this;
}

template <typename Ptr>
template <typename Y, typename Deleter>
auto iosp::shared_ptr<Ptr>::operator=(unique_ptr<Y, Deleter> &&u) -> shared_ptr&
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>);
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");

    if(cb && cb->strong_ref.fetch_sub(1) == 1)
        cb->destroy();

    auto p = u.release();
    try {
        cb = new object_owner<Ptr, Deleter>(p, std::move(u.get_deleter()));
        pointer = p;
    } catch(...) {
        u.get_deleter()(p);
        cb = nullptr;
        pointer = nullptr;
        throw;
    }
    return *this;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::operator*() const noexcept -> Ptr&
{
    return *pointer;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::operator->() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::operator bool() const noexcept
{
    return pointer != nullptr;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::get() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::unique() const noexcept -> bool
{
    return cb->strong_ref == 1 ? true : false;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::use_count() const noexcept -> std::size_t
{
    return cb ? cb->strong_ref.load() : 0;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::reset() noexcept -> void
{

}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::swap(shared_ptr &other) noexcept -> void
{

}