#include <iostream>
#include <string>

// extern void foo_common();

void foo_B(const std::string& text) {
    // foo_common();
    std::cout << "B: " << text << std::endl;
}
