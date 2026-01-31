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
    for (char c : input)
        if (!isdigit(c))
            return "Invalid credit card number";
    return input; // all digits, return original
}


std::string_view detect_issuer(std::string_view number) {
    // A static table of C-style strings (effectively pointers)
    static constexpr std::array<std::string_view, 256> lookup = [] {
        std::array<std::string_view, 256> table;
        table.fill("UNKNOWN");
        table['4'] = "VISA";
        table['5'] = "MASTERCARD";
        return table;
    }();

    // Use the character itself as the index. 
    // We use a guard to handle the empty string case branchlessly.
    size_t index = number.empty() ? 0 : static_cast<unsigned char>(number[0]);
    
    return lookup[index];

}

// Validate a credit card number using Luhn's algorithm
bool luhn_check(const std::string &number) {
    int sum = 0;               
    bool double_digit = false; // Flag to double every second digit from the right
   // Notice: 'it' is a pointer-like object (iterator), but dont be afraid, it's almost like you reveryday for loop
    // Reverse-iterator loop (safe way to traverse from last to first character)
    for (auto it = number.rbegin(); it != number.rend(); ++it) {
        int digit = *it - '0'; // Convert character to integer
        if (double_digit) {
            digit *= 2;
            if (digit > 9) digit -= 9; // Sum digits of doubled number
        }
        sum += digit;
        double_digit = !double_digit;
    }

    return sum % 10 == 0; // Valid if divisible by 10
}
// Notice: 'it' is a pointer-like object (iterator), but you don't need to fear it!


// Entropy: calculates the Shannon Entropy to measure the randomness of the digits
double calculate_entropy(const std::string &number) {
    std::unordered_map<char,int> freq; // A map to store how many times each digit (0-9) appears
    
    // Count the frequency of every character in the string
    for (char c : number) freq[c]++; 
    
    double entropy = 0.0;     // Initialize entropy sum
    int len = number.size();   // Total length of the string for probability math
    
    // Loop through each entry in the frequency map (k = character, v = count)
    for (auto &[k,v] : freq) {
        // Calculate probability 'p' (how often this digit appears relative to total length)
        double p = double(v) / len;
        
        // Shannon entropy formula: sum of -p * log2(p)
        // This calculates the information density in bits
        entropy += -p * log2(p);
    }
    return entropy; // Returns the total bits of randomness per digit
}
// Optimized Repetition Check: Zero heap allocations
bool repetition_check_optimized(std::string_view number) {
    // len is the size of the pattern we are looking for (e.g., "12" has len 2)
    for (size_t len = 1; len <= number.size() / 2; ++len) {
        
        // i is the starting position of the first pattern
        for (size_t i = 0; i + 2 * len <= number.size(); ++i) {
            
            // Create two "windows" (string_views) into the existing data
            // number.substr() on a string_view does NOT copy memory
            std::string_view first_part = number.substr(i, len);
            std::string_view second_part = number.substr(i + len, len);

            // This comparison happens directly on the original memory
            if (first_part == second_part) {
                return false; // Pattern like "1212" or "44" found
            }
        }
    }
    return true; // No back-to-back repeats found
}
/* ---------------------
    Main Validator
---------------------- */
CardResult validate_card(const std::string &input) {
    // Record the start time using a high-precision nanosecond clock
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Clean the input (remove spaces/dashes) before processing
    std::string normalized = normalize_input(input);

    CardResult res; // Object to store all our findings (issuer, luhn status, etc.)
    std::cout << "[INFO] Input normalized (spaces removed)\n";

    // Standard card length check: generally between 13 and 19 digits
    if (normalized.size() < 13 || normalized.size() > 19) {
        std::cout << "[INFO] Length check failed (" << normalized.size() << " digits)\n";
        res.valid = false;
        return res; // Stop immediately if length is wrong
    } else {
        std::cout << "[INFO] Length check passed (" << normalized.size() << " digits)\n";
    }

    // Step 1: Identify the card brand (Visa, Mastercard, etc.)
    res.issuer = detect_issuer(normalized);
    std::cout << "[INFO] Issuer pattern recognized: " << res.issuer << "\n";

    // Step 2: Run the mathematical Luhn algorithm
    res.luhn_pass = luhn_check(normalized);
    std::cout << "[INFO] Luhn checksum: " << (res.luhn_pass ? "PASS" : "FAIL") << "\n";

    // Step 3: Check for randomness (threshold 3.5 is common for secure IDs)
    res.entropy = calculate_entropy(normalized);
    bool entropy_pass = res.entropy >= 3.5;
    std::cout << "[INFO] Entropy score: " << res.entropy << " bits/digit (threshold: 3.5) "
              << (entropy_pass ? "PASS" : "FAIL") << "\n";

    // Step 4: Ensure the number isn't just a simple repeating pattern
    res.repetition_pass = repetition_check(normalized);
    std::cout << "[INFO] Repetition analysis: " << (res.repetition_pass ? "PASS" : "FAIL") << "\n";

    // Combine all results:
    // If it passes everything, it's fully valid.
    if (res.luhn_pass && entropy_pass && res.repetition_pass) {
        res.valid = true;
        std::cout << "[RESULT] Card number is VALID\n";
    } 
    // If it only passes Luhn, it might be real but is "low confidence" (suspicious)
    else if (res.luhn_pass) {
        res.valid = true;
        std::cout << "[RESULT] Card number is VALID (low confidence)\n";
    } 
    // Otherwise, it's definitely invalid
    else {
        res.valid = false;
        std::cout << "[RESULT] Card number is INVALID\n";
    }

    // Stop the clock and calculate how many nanoseconds the process took
    auto end_time = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    std::cout << "[TIME] Verification completed in " << ns << " ns\n";

    return res;
}
