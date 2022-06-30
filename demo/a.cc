#include <iostream>
#include <string>

// extern void foo_common();

void foo_A(const std::string& text) {
    // foo_common();
    std::cout << "A: " << text << std::endl;
}
