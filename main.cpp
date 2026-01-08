#include "unique_ptr.h"
#include <memory>
#include "make_unique.h"

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

void deleter(A* a) {
    std::cout << "Custom deleter" << std::endl;
    delete a;
}

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

    // _p.get(); !warning

    std::cout << *_p << std::endl;
    std::cout << *p1 << std::endl;
    std::cout << *p2 << std::endl;

    p1.swap(p2);

    std::cout << *p1 << std::endl;
    std::cout << *p2 << std::endl;

    return 0;
}