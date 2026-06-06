#include "../../unique_ptr.hpp"
#include "../../shared_ptr.hpp"
#include <iostream>

struct alignas(32) Vector256 {
    float data[8];
};

int main()
{
    Vector256 vec = {1,2,3,4,5,6,7,8};
    iosp::shared_ptr<Vector256> ptr = iosp::make_shared<Vector256>(vec);

    uintptr_t ptr_addr = reinterpret_cast<uintptr_t>(ptr.get());

    std::cout << "address of ptr is " << ptr_addr << std::endl;
    std::cout << "alignment of ptr is " << alignof(*ptr.get()) << std::endl;
    std::cout << "it should be " << alignof(Vector256) << std::endl;
    std::cout << "address of ptr is divisible by its alignment " << (((ptr_addr % alignof(Vector256) == 0) ? "True" : "False")) << std::endl;

    return 0;
}