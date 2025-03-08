#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

typedef struct {
    size_t ones;
    size_t zeros;
    double ratio;
} FrequencyStats;

typedef struct {
    size_t runs;
    double expected;
    double deviation;
} RunsStats;

typedef struct {
    size_t peaks;
    double ratio;
} SpectralStats;

static uint64_t get_hardware_random(void) {
    uint64_t buffer;
    
    #ifdef _WIN32
    if (!SystemFunction036(&buffer, sizeof(buffer))) {
        fprintf(stderr, "Failed to generate random number\n");
        exit(EXIT_FAILURE);
    }
    #else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0 || read(fd, &buffer, sizeof(buffer)) != sizeof(buffer)) {
        fprintf(stderr, "Failed to read from /dev/urandom\n");
        exit(EXIT_FAILURE);
    }
    close(fd);
    #endif
    
    return buffer;
}

static char* generate_bit_sequence(size_t length) {
    char* bits = malloc(length + 1);
    if (!bits) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    const size_t chunk_size = 64;
    const size_t num_chunks = length / chunk_size;
    size_t pos = 0;
    
    for (size_t i = 0; i < num_chunks; i++) {
        uint64_t rand_val = get_hardware_random();
        for (int j = chunk_size - 1; j >= 0; j--) {
            bits[pos++] = ((rand_val >> j) & 1) ? '1' : '0';
        }
    }
    
    bits[length] = '\0';
    return bits;
}

static size_t count_bit_ones(const char* bits) {
    size_t count = 0;
    for (size_t i = 0; bits[i]; i++) {
        count += (bits[i] == '1');
    }
    return count;
}

static const char* evaluate_frequency(const char* bits) {
    const size_t len = strlen(bits);
    FrequencyStats stats = {
        .ones = count_bit_ones(bits),
        .zeros = len - stats.ones,
        .ratio = (double)stats.ones / len
    };
    
    printf("Frequency Test: %zu ones, %zu zeros, ", stats.ones, stats.zeros);
    
    if (stats.ratio >= 0.48 && stats.ratio <= 0.52) {
        printf("Verdict: PASS (ratio: %.4f)\n", stats.ratio);
        return "PASS";
    }
    
    printf("Verdict: FAIL (ratio: %.4f)\n", stats.ratio);
    return "FAIL";
}

static const char* evaluate_runs(const char* bits) {
    const size_t len = strlen(bits);
    RunsStats stats = {
        .runs = 1,
        .expected = (2.0 * len - 1.0) / 3.0
    };
    
    for (size_t i = 1; i < len; i++) {
        stats.runs += (bits[i] != bits[i-1]);
    }
    
    stats.deviation = fabs(stats.runs - stats.expected) / stats.expected;
    
    printf("Runs Test: Observed runs = %zu, Expected runs ≈ %.2f, ", 
           stats.runs, stats.expected);
           
    if (stats.deviation < 0.05) {
        printf("Verdict: PASS (deviation: %.4f)\n", stats.deviation);
        return "PASS";
    }
    
    printf("Verdict: FAIL (deviation: %.4f)\n", stats.deviation);
    return "FAIL";
}

static double calculate_chi_square(const size_t* observed, 
                                 const double* expected, 
                                 const size_t n) {
    double chi2 = 0;
    for (size_t i = 0; i < n; i++) {
        const double diff = observed[i] - expected[i];
        chi2 += (diff * diff) / expected[i];
    }
    return chi2;
}

static double calculate_bit_entropy(const char* bits) {
    const size_t len = strlen(bits);
    const size_t ones = count_bit_ones(bits);
    const double p1 = (double)ones / len;
    const double p0 = 1.0 - p1;
    
    return -(p0 * log2(p0) + p1 * log2(p1));
}

static size_t find_longest_ones_run(const char* bits) {
    size_t max_run = 0;
    size_t current_run = 0;
    
    for (size_t i = 0; bits[i]; i++) {
        if (bits[i] == '1') {
            current_run++;
            max_run = (current_run > max_run) ? current_run : max_run;
        } else {
            current_run = 0;
        }
    }
    return max_run;
}

static const char* evaluate_spectral(const char* bits) {
    const size_t len = strlen(bits);
    SpectralStats stats = {.peaks = 0};
    
    for (size_t i = 1; i < len - 1; i++) {
        stats.peaks += (bits[i] == '1' && bits[i-1] == '0' && bits[i+1] == '0');
    }
    
    stats.ratio = (double)stats.peaks / len;
    printf("Spectral Test: Number of significant peaks = %zu, ", stats.peaks);
    
    if (stats.ratio >= 0.45 && stats.ratio <= 0.55) {
        printf("Verdict: PASS (peak ratio: %.4f)\n", stats.ratio);
        return "PASS";
    }
    
    printf("Verdict: FAIL (peak ratio: %.4f)\n", stats.ratio);
    return "FAIL";
}

static const char* evaluate_serial_patterns(const char* bits) {
    const size_t len = strlen(bits);
    size_t counts[4] = {0};
    
    for (size_t i = 0; i < len - 1; i++) {
        const int pattern = (bits[i] == '1') * 2 + (bits[i+1] == '1');
        counts[pattern]++;
    }
    
    const double expected = (len - 1) / 4.0;
    const double expected_arr[4] = {expected, expected, expected, expected};
    const double chi2 = calculate_chi_square(counts, expected_arr, 4);
    
    printf("Serial Test: Chi2 = %.2f, ", chi2);
    
    if (chi2 < 7.815) {
        printf("Verdict: PASS\n");
        return "PASS";
    }
    
    printf("Verdict: FAIL\n");
    return "FAIL";
}

static const char* evaluate_autocorrelation(const char* bits) {
    const size_t len = strlen(bits);
    size_t matches = 0;
    
    for (size_t i = 0; i < len - 1; i++) {
        matches += (bits[i] == bits[i+1]);
    }
    
    const double correlation = (double)matches / (len - 1);
    printf("Autocorrelation Test (lag=1): correlation = %.5f, ", correlation);
    
    if (correlation >= 0.45 && correlation <= 0.55) {
        printf("Verdict: PASS\n");
        return "PASS";
    }
    
    printf("Verdict: FAIL\n");
    return "FAIL";
}

static const char* evaluate_byte_distribution(const char* bits) {
    size_t counts[256] = {0};
    const size_t len = strlen(bits);
    const size_t byte_count = len / 8;
    
    for (size_t i = 0; i < byte_count; i++) {
        unsigned char byte = 0;
        for (size_t j = 0; j < 8; j++) {
            byte = (byte << 1) | (bits[i*8 + j] == '1');
        }
        counts[byte]++;
    }
    
    const double expected = (double)byte_count / 256;
    double expected_arr[256];
    for (int i = 0; i < 256; i++) {
        expected_arr[i] = expected;
    }
    
    const double chi2 = calculate_chi_square(counts, expected_arr, 256);
    printf("Chi-Square Test: Chi2 = %.2f, ", chi2);
    
    if (chi2 < 300) {
        printf("Verdict: PASS\n");
        return "PASS";
    }
    
    printf("Verdict: FAIL\n");
    return "FAIL";
}

int main(void) {
    printf("\n=== HARDWARE RNG QUALITY ASSESSMENT ===\n\n");
    
    #ifdef _WIN32
    printf("Using Windows Crypto API for random number generation\n");
    #else
    printf("Using /dev/urandom for random number generation\n");
    #endif
    
    printf("Testing with 100,000,000 random bits for statistical significance\n\n");
    
    char* const bits = generate_bit_sequence(100000000);
    const char* results[8];
    
    results[0] = evaluate_frequency(bits);
    results[1] = evaluate_runs(bits);
    results[2] = evaluate_byte_distribution(bits);
    
    const double entropy = calculate_bit_entropy(bits);
    printf("Entropy: %.5f bits per bit (ideal = 1.0 for max randomness), ", entropy);
    results[3] = (entropy >= 0.99) ? "PASS" : "FAIL";
    printf("Verdict: %s\n", results[3]);
    
    const size_t max_run = find_longest_ones_run(bits);
    const double expected_max_run = log2(strlen(bits));
    const double run_deviation = fabs(max_run - expected_max_run) / expected_max_run;
    
    printf("Longest Run of Ones Test: Max run = %zu, Expected ≈ %.15f, ", 
           max_run, expected_max_run);
    results[4] = (run_deviation < 0.05) ? "PASS" : "FAIL";
    printf("Verdict: %s (deviation: %.4f)\n", results[4], run_deviation);
    
    results[5] = evaluate_spectral(bits);
    results[6] = evaluate_serial_patterns(bits);
    results[7] = evaluate_autocorrelation(bits);
    
    size_t pass_count = 0;
    for (int i = 0; i < 8; i++) {
        pass_count += (strcmp(results[i], "PASS") == 0);
    }
    
    const double pass_percentage = (pass_count * 100.0) / 8;
    
    printf("\n=== FINAL VERDICT ===\n");
    printf("Passed %zu out of 8 tests (%.1f%%)\n", pass_count, pass_percentage);
    
    const char* verdict;
    if (pass_percentage >= 90) {
        verdict = "HIGH QUALITY: Very likely TRNG";
    } else if (pass_percentage >= 70) {
        verdict = "GOOD QUALITY: Likely TRNG";
    } else if (pass_percentage >= 50) {
        verdict = "MODERATE QUALITY: Run this test again sometimes it varies between tests";
    } else {
        verdict = "LOW QUALITY: probably an issue here";
    }
    
    printf("OVERALL VERDICT: %s\n", verdict);
    
    free(bits);
    return EXIT_SUCCESS;
}
