#include <iostream>
#include <string>

template <typename Container>
constexpr auto size(const Container& c) -> decltype(c.size()) {
  return c.size();
}

template <typename T, size_t N>
constexpr size_t size(const T (&array)[N]) noexcept {
  return N;
}

template <typename T>
constexpr size_t size(const T (&array)[0]) noexcept {
  return 0;
}

extern void foo_A(const std::string& text);
extern void foo_B(const std::string& text);

// extern void foo_common();
extern void foo_common_ex();

int main() {
    // foo_common();
    foo_common_ex();
    foo_A("this is A");
    foo_B("this is B");

    const char* const messages[] = {
        "OK",
        "Not OK",
        "File not found"
    };
    std::cout << size(messages) << std::endl;

    const char* const messages2[] = { };
    std::cout << size(messages2) << std::endl;

    return 0;
}