#include "../unique_ptr.hpp"
#include "../shared_ptr.hpp"

int main()
{
    auto sp1 = iosp::make_shared<int>(1); // ref count = 1
    auto sp2 = iosp::make_shared<int>(2); // ref count = 1

    std::cout << sp1.use_count() << std::endl;
    std::cout << sp2.use_count() << std::endl;

    std::cout << "Assign sp2 to sp1\n";
    sp1 = sp2; // copy assignment

    std::cout << sp1.use_count() << std::endl;
    std::cout << sp2.use_count() << std::endl;

    return 0;
}