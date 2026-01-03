#include "unique_ptr.h"
#include <memory>

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
    my_unique_ptr<A, _Deleter> p{new A, dl};
    *p = {1,2,'a'};
    std::cout << p->b << std::endl;
    std::cout << p << std::endl;
    // p.reset();
    if(!p)
        std::cout << "no p" << std::endl;
    
    my_unique_ptr<int> p1{new int(10)};
    my_unique_ptr<int> p2{new int(20)};

    std::cout << *p1 << std::endl;
    std::cout << *p2 << std::endl;

    p1.swap(p2);

    std::cout << *p1 << std::endl;
    std::cout << *p2 << std::endl;

    return 0;
}