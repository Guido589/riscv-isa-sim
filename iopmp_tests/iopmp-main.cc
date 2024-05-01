#include <iostream>
#include "csr-tests.h"
#include "mmu-tests.h"
#include "iopmp-main.h"

void iopmp_tests_main() {
    std::cout << "Running IOPMP unit tests ..." << std::endl << std::endl;
    run_csr_tests();
    run_mmu_tests();
    exit(0);
}