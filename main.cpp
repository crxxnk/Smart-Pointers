#include "unique_ptr.h"
#include <memory>

struct A{
    int x,y;
    char b;
};

int main()
{
    my_unique_ptr<int> p(new int);
    *p = 10;
    std::cout << *p << std::endl;

    return 0;
}