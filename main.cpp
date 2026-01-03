#include "unique_ptr.h"
#include <memory>

struct A {
    int x,y;
    char b;
};

void my_deleter(A* a) {
    std::cout << "Custom deleter" << std::endl;
    delete a;
}

int main()
{
    my_unique_ptr<A> p(new A);
    *p = {1,2,'a'};
    std::cout << p->b << std::endl;
    p.reset();
    if(!p)
        std::cout << "no p" << std::endl;
    

    return 0;
}