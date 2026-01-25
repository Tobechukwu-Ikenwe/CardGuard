#pragma once
#include <string>

/*
 * CardResult: Holds the results of validation for a single card number
 * - valid: true if passes Luhn (or low-confidence)
 * - luhn_pass: whether Luhn checksum passed
 * - entropy: bits per digit
 * - repetition_pass: check for repeated sequences
 * - issuer: VISA / MASTERCARD / UNKNOWN
 */
struct CardResult {
    bool valid;
    bool luhn_pass;
    double entropy;
    bool repetition_pass;
    std::string issuer;
};

// Function declarations
CardResult validate_card(const std::string &input);
