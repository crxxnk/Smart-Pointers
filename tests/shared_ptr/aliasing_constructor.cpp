#include "../unique_ptr.hpp"
#include "../shared_ptr.hpp"

struct Test {
    int a;
    int b;

    Test(int x, int y) : a(x), b(y) {
        std::cout << "Test constructed\n";
    }

    ~Test() {
        std::cout << "Test destroyed\n";
    }
};

int main()
{
    iosp::shared_ptr<Test> sp = iosp::make_shared<Test>(10, 20);

    std::cout << "sp.use_count(): " << sp.use_count() << "\n";
    std::cout << "sp.get(): " << sp.get() << "\n";

    std::cout << "\n---- create aliasing shared_ptr ----\n";
    iosp::shared_ptr<int> alias(sp, &sp->b);

    std::cout << "alias.use_count(): " << alias.use_count() << "\n";
    std::cout << "alias.get(): " << alias.get() << "\n";
    std::cout << "alias value: " << *alias << "\n";

    std::cout << "\n---- destroy original shared_ptr ----\n";
    sp = nullptr;

    std::cout << "alias.use_count() after sp reset: "
              << alias.use_count() << "\n";
    std::cout << "alias value still valid: " << *alias << "\n";

    std::cout << "\n---- destroy aliasing shared_ptr ----\n";
    alias = nullptr;

    std::cout << "end of program\n";
    return 0;
}