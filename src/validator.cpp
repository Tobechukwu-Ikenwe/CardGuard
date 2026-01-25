#include "validator.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <unordered_map>

/* ---------------------
   Helper Functions
---------------------- */

// Normalize input by removing spaces
std::string normalize_input(const std::string &input) {
    std::string normalized;
    for (char c : input)
        if (isdigit(c))
            normalized += c;
    return normalized;
}

// Detect issuer based on prefix (VISA: 4, MasterCard: 5)
std::string detect_issuer(const std::string &number) {
    if (number.empty()) return "UNKNOWN";
    if (number[0] == '4') return "VISA";
    if (number[0] == '5') return "MASTERCARD";
    return "UNKNOWN";
}

// Luhn's algorithm
bool luhn_check(const std::string &number) {
    int sum = 0;
    bool double_digit = false;
    for (int i = number.size() - 1; i >= 0; --i) {
        int digit = number[i] - '0';
        if (double_digit) {
            digit *= 2;
            if (digit > 9) digit -= 9;
        }
        sum += digit;
        double_digit = !double_digit;
    }
    return (sum % 10) == 0;
}

// Entropy: simple bits per digit
double calculate_entropy(const std::string &number) {
    std::unordered_map<char,int> freq;
    for (char c : number) freq[c]++;
    double entropy = 0.0;
    int len = number.size();
    for (auto &[k,v] : freq) {
        double p = double(v) / len;
        entropy += -p * log2(p);
    }
    return entropy;
}

// Repetition: checks for repeated sequences
bool repetition_check(const std::string &number) {
    for (size_t len = 1; len <= number.size()/2; ++len) {
        for (size_t i = 0; i + 2*len <= number.size(); ++i) {
            if (number.substr(i,len) == number.substr(i+len,len)) {
                return false; // repeated pattern found
            }
        }
    }
    return true;
}

/* ---------------------
   Main Validator
---------------------- */

CardResult validate_card(const std::string &input) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::string normalized = normalize_input(input);

    CardResult res;
    std::cout << "[INFO] Input normalized (spaces removed)\n";

    if (normalized.size() < 13 || normalized.size() > 19) {
        std::cout << "[INFO] Length check failed (" << normalized.size() << " digits)\n";
        res.valid = false;
        return res;
    } else {
        std::cout << "[INFO] Length check passed (" << normalized.size() << " digits)\n";
    }

    res.issuer = detect_issuer(normalized);
    std::cout << "[INFO] Issuer pattern recognized: " << res.issuer << "\n";

    res.luhn_pass = luhn_check(normalized);
    std::cout << "[INFO] Luhn checksum: " << (res.luhn_pass ? "PASS" : "FAIL") << "\n";

    res.entropy = calculate_entropy(normalized);
    bool entropy_pass = res.entropy >= 3.5;
    std::cout << "[INFO] Entropy score: " << res.entropy << " bits/digit (threshold: 3.5) "
              << (entropy_pass ? "PASS" : "FAIL") << "\n";

    res.repetition_pass = repetition_check(normalized);
    std::cout << "[INFO] Repetition analysis: " << (res.repetition_pass ? "PASS" : "FAIL") << "\n";

    // Result logic
    if (res.luhn_pass && entropy_pass && res.repetition_pass) {
        res.valid = true;
        std::cout << "[RESULT] Card number is VALID\n";
    } else if (res.luhn_pass) {
        res.valid = true;
        std::cout << "[RESULT] Card number is VALID (low confidence)\n";
    } else {
        res.valid = false;
        std::cout << "[RESULT] Card number is INVALID\n";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    std::cout << "[TIME] Verification completed in " << ns << " ns\n";

    return res;
}
