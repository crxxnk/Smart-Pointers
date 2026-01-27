#include "unique_ptr.hpp"
#include <memory>
#include "shared_ptr.hpp"

struct A {
    int x,y;
    char b;
};

struct _Deleter {
    _Deleter() = default;
    // _Deleter(_Deleter &&other) = delete;
    ~_Deleter() = default;
    void operator()(A* a) {
        std::cout << "Custom deleter" << std::endl;
        delete a;
    }
};

void deleter(int* a) {
    std::cout << "Custom deleter(int)" << std::endl;
    delete a;
}

template<typename T>
struct Custom_Allocator
{
    using value_type = T;
    Custom_Allocator() = default;

    template<typename U>
    Custom_Allocator(const Custom_Allocator<U>&) {}

    T* allocate(std::size_t n) {
        std::cout << "Allocating " << n << " objects\n";
        return static_cast<T*>(::operator new(n*sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) {
        std::cout << "Deallocating " << n << " objects\n";
        ::operator delete(p);
    }

    void destroy(T* p) {
        p->~T();
    }
};

struct Bar { 
    int cock;
};

struct Foo {
    Bar bar;
};

int main()
{
    _Deleter dl;
    // my_unique_ptr<A, decltype(&deleter)> p{new A, &deleter};
    iosp::unique_ptr<A, _Deleter> p(new A(1,2,'a'), dl);
    // *p = {1,2,'a'};

    iosp::unique_ptr<int> _p = iosp::make_unique<int>(15); // make_unique doesn't support custom deleters

    std::cout << p->b << std::endl;
    std::cout << p << std::endl;
    // p.reset();
    if(!p)
        std::cout << "no p" << std::endl;
    
    iosp::unique_ptr<int> p1{new int(10)};
    iosp::unique_ptr<int> p2{new int(20)};

    Custom_Allocator<control_block> alloc;

    iosp::shared_ptr<int> sharedptr{new int, deleter, alloc};
    std::cout << "use count:" << sharedptr.use_count() << std::endl;

    iosp::shared_ptr<int> sharedptr2{sharedptr};
    std::cout << "use count:" << sharedptr.use_count() << std::endl;
    iosp::shared_ptr<int> sharedptr3{nullptr, deleter, alloc};

    std::cout << "coktr" << sizeof(control_block) << std::endl;

    iosp::shared_ptr<int> sharedptr4 = iosp::make_shared<int>(19);
    std::cout << *sharedptr4 << std::endl;

    iosp::shared_ptr<Foo> f = iosp::make_shared<Foo>(Bar(10));
    iosp::shared_ptr<Bar> specific_data(f, &f->bar);

    std::cout << "Aliasing use count" << f.use_count() << std::endl;

    // _p.get(); !warning

    std::cout << *_p << std::endl;
    std::cout << *p1 << std::endl;
    std::cout << *p2 << std::endl;

    p1.swap(p2);

    std::cout << *p1 << std::endl;
    std::cout << *p2 << std::endl;

    iosp::unique_ptr<int[]> p_arr(new int[3]);
    for(size_t i = 0; i < 3; i++) {
        p_arr[i] = i+1;
        std::cout << p_arr[i] << std::endl;
    }

    return 0;
}
