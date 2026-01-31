#include "../../unique_ptr.hpp"
#include <memory>
#include "../../shared_ptr.hpp"
#include <iostream>

struct Foo
{
    int n1;
    int n2; 
    Foo(int a, int b) : n1(a), n2(b) {}
};

int main()
{
    auto p1 = iosp::make_shared<Foo>(1, 2);
    iosp::shared_ptr<int> p2(p1, &p1->n1);
    iosp::shared_ptr<int> p3(p1, &p1->n2);
 
    std::cout << std::boolalpha
              << "p2.owner_before(p3) " << p2.owner_before(p3) << '\n'
              << "p3.owner_before(p2) " << p3.owner_before(p2) << '\n';

    return 0;
}