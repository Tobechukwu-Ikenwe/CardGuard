/******************************************************************************
    Luhn's Algorithm — Credit Card Validation
*******************************************************************************/

#include <iostream>
#include <string>

// Function prototypes
int getDigit(const int number);
int sumOddDigits(const std::string cardNumber);
int sumEvenDigits(const std::string cardNumber);

int main() {
    std::string cardNumber;
    int result = 0;

    std::cout << "Enter a credit card number: ";
    std::cin >> cardNumber;

    // Sum of even and odd position digits
    result = sumEvenDigits(cardNumber) + sumOddDigits(cardNumber);

    // Check if result is divisible by 10 (Luhn's rule)
    if (result % 10 == 0)
        std::cout << "This is a valid credit card number.\n";
    else
        std::cout << "This is not a valid credit card number.\n";

    return 0;
}

/*
 * getDigit: If the number is a single digit, return it.
 *            If it's two digits (e.g., 18), return the sum of both digits (1 + 8 = 9).
 */
int getDigit(const int number) {
    if (number < 10)
        return number;
    else
        return number % 10 + (number / 10);
}

/*
 * sumOddDigits: Starting from the last digit, sum all digits in odd positions.
 * Example: for 12345 → sums 5 + 3 + 1
 */
int sumOddDigits(const std::string cardNumber) {
    int sum = 0;
    for (int i = cardNumber.size() - 1; i >= 0; i -= 2)
        sum += getDigit(cardNumber[i] - '0');  // Convert char to int
    return sum;
}

/*
 * sumEvenDigits: Starting from the second-to-last digit, double each digit,
 *                then sum their digits using getDigit().
 * Example: for 12345 → doubles 4 and 2 → (8, 4) → sum of digits 8 + 4
 */
int sumEvenDigits(const std::string cardNumber) {
    int sum = 0;
    for (int i = cardNumber.size() - 2; i >= 0; i -= 2)
        sum += getDigit((cardNumber[i] - '0') * 2);
    return sum;
}
