#include "validator.h"
#include <iostream>

int main() {
    std::string input;
    std::cout << "Enter a credit card number: ";
    std::getline(std::cin, input);  // Read the entire line including spaces

    validate_card(input);

    return 0;
}
