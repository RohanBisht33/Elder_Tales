#include "Core/RNG.h"
#include <iostream>
#include <cassert>
#include <vector>

void TestDeterminism() {
    std::cout << "Testing RNG Determinism..." << std::endl;

    Core::RNG rng1(12345);
    Core::RNG rng2(12345);

    for (int i = 0; i < 100; ++i) {
        int r1 = rng1.range(0, 100);
        int r2 = rng2.range(0, 100);
        
        if (r1 != r2) {
            std::cerr << "Determinism FAILED at iteration " << i << std::endl;
            std::exit(1);
        }
    }

    std::cout << "RNG Determinism PASSED." << std::endl;
}

int main() {
    TestDeterminism();
    return 0;
}
