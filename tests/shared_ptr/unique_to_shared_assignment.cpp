#include "../../unique_ptr.hpp"
#include <memory>
#include "../../shared_ptr.hpp"
#include <iostream>

int main()
{
    iosp::shared_ptr<int> a = iosp::make_shared<int>(10);
    iosp::shared_ptr<int> _a = iosp::make_shared<int>(11);
    std::cout << *a << std::endl;
    std::cout << *_a << std::endl;

    std::cout << a.use_count() << std::endl;
    std::cout << _a.use_count() << std::endl;

    a = _a;
    std::cout << a.use_count() << std::endl;
    std::cout << _a.use_count() << std::endl << std::endl;

    a = iosp::unique_ptr<int>(new int(15));//iosp::make_unique<int>(15);

    std::shared_ptr<int> as = std::make_shared<int>(10);
    std::shared_ptr<int> _as = std::make_shared<int>(11);
    std::cout << as.use_count() << std::endl;
    std::cout << _as.use_count() << std::endl;

    as = _as;
    std::cout << as.use_count() << std::endl;
    std::cout << _as.use_count() << std::endl;

    return 0;
}