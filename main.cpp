#include "unique_ptr.h"
#include <memory>

struct A{
    int x,y;
    char b;
};

int main()
{
    std::unique_ptr<int> p(new int);
    *p = 10;
    p.reset();
    if(p)
        std::cout << *p << std::endl;
    std::cout << "no p" << std::endl;

    return 0;
}