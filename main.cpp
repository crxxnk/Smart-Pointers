#include "unique_ptr.h"
#include <memory>

struct A{
    int x,y;
    char b;
};

int main()
{
    std::unique_ptr<int> p(new int);

    return 0;
}