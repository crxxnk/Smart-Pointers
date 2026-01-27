#include "../../unique_ptr.hpp"
#include <memory>
#include "../../shared_ptr.hpp"
#include <iostream>

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

int main() {
    
}