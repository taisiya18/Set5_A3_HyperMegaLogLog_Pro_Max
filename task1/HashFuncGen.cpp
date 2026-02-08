#include "HashFuncGen.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

HashFuncGen::HashFuncGen(uint32_t seed) : seed(seed) {}

uint32_t HashFuncGen::hash(const std::string& str) const {
    return murmur3_32(str, seed);
}

uint32_t HashFuncGen::getSeed() const {
    return seed;
}

void HashFuncGen::setSeed(uint32_t new_seed) {
    seed = new_seed;
}

// MurmurHash3 32-bit implementation
uint32_t HashFuncGen::murmur3_32(const std::string& key, uint32_t seed) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(key.c_str());
    const int len = key.length();
    const int nblocks = len / 4;
    
    uint32_t h1 = seed;
    
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    
    // Body
    const uint32_t* blocks = reinterpret_cast<const uint32_t*>(data + nblocks * 4);
    
    for (int i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i];
        
        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;
        
        h1 ^= k1;
        h1 = rotl32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }
    
    // Tail
    const uint8_t* tail = data + nblocks * 4;
    uint32_t k1 = 0;
    
    switch (len & 3) {
        case 3: k1 ^= tail[2] << 16; [[fallthrough]];
        case 2: k1 ^= tail[1] << 8;  [[fallthrough]];
        case 1: k1 ^= tail[0];
                k1 *= c1;
                k1 = rotl32(k1, 15);
                k1 *= c2;
                h1 ^= k1;
    }
    
    // Finalization
    h1 ^= len;
    h1 = fmix32(h1);
    
    return h1;
}

void HashFuncGen::testUniformity(const std::vector<std::string>& data,
                                  uint32_t seed,
                                  int num_buckets) {
    HashFuncGen hasher(seed);
    std::vector<int> buckets(num_buckets, 0);
    
    // Распределяем хеши по бакетам
    for (const auto& str : data) {
        uint32_t hash_value = hasher.hash(str);
        int bucket = hash_value % num_buckets;
        buckets[bucket]++;
    }
    
    // Вычисляем статистики
    double expected = static_cast<double>(data.size()) / num_buckets;
    double variance = 0.0;
    int min_count = buckets[0];
    int max_count = buckets[0];
    
    for (int count : buckets) {
        variance += (count - expected) * (count - expected);
        min_count = std::min(min_count, count);
        max_count = std::max(max_count, count);
    }
    
    variance /= num_buckets;
    double std_dev = std::sqrt(variance);
    
    // Выводим результаты
    std::cout << "\n=== Test Uniformity of Hash Function ===" << std::endl;
    std::cout << "Total elements: " << data.size() << std::endl;
    std::cout << "Number of buckets: " << num_buckets << std::endl;
    std::cout << "Expected per bucket: " << std::fixed << std::setprecision(2) 
              << expected << std::endl;
    std::cout << "Min count: " << min_count << std::endl;
    std::cout << "Max count: " << max_count << std::endl;
    std::cout << "Variance: " << variance << std::endl;
    std::cout << "Std deviation: " << std_dev << std::endl;
    std::cout << "Coefficient of variation: " 
              << (std_dev / expected * 100) << "%" << std::endl;
    
    // Chi-square test
    double chi_square = 0.0;
    for (int count : buckets) {
        double diff = count - expected;
        chi_square += (diff * diff) / expected;
    }
    std::cout << "Chi-square statistic: " << chi_square << std::endl;
    std::cout << "========================================\n" << std::endl;
}
