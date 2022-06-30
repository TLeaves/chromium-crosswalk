#include <iostream>
#include <string>
#include <thread>

template<class T>
void Test(T t) {
    std::cout << t << std::endl;
}

void foo_common() {
    const char* k1 = "asdasdasdasdasdasdasddasdasdasdasdasdasdasdasda";
    const char* k2 = "asdasdasdasdasdasdasddasdasdasdasdasdasqweqweqweqwe";
    const char* k3 = "asdasdasdasdasasdasdcvvsdasdasdasdasqweqweqweqwe";
    std::cout << "common!" << k1 << k2 << k3 << std::endl;

    Test(1);
    Test("2");
    Test(true);

    std::thread t([&](){
        std::cout << "common threaded!" << k1 << k2 << k3 << std::endl;
    });
    t.join();
}

void foo_asdadasd() {
    std::thread t([](){
        std::cout << "foo_asdadasd!" << std::endl;
    });
    t.join();
}

void foo_common_ex() {

}