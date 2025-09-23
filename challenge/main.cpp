#include "hmi.hpp"
#include "operations.hpp"

#include <iostream>
#include <sstream>

int main() {
    while (true) {
        HMI::showMenu();
        u_int8_t choice = HMI::getOperationChoice();

    }

    return 0;
}
