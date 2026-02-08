#include "HyperLogLog.h"
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <iostream>

HyperLogLog::HyperLogLog(uint8_t b, uint32_t seed) 
    : B(b), 
      m(1ULL << b),  // 2^B
      registers(m, 0),
      hasher(seed) {
    if (B < 4 || B > 16) {
        throw std::invalid_argument("B must be between 4 and 16");
    }
}

uint8_t HyperLogLog::leadingZeros(uint32_t hash, uint8_t bits_to_skip) const {
    // Пропускаем первые bits_to_skip бит (они используются для индекса)
    // Оставшиеся биты: 32 - bits_to_skip
    
    uint32_t mask = (1U << (32 - bits_to_skip)) - 1;
    uint32_t w = hash & mask;  // Оставляем только младшие (32 - B) бит
    
    if (w == 0) {
        return 32 - bits_to_skip + 1;
    }
    
    // Подсчет ведущих нулей + 1
    uint8_t leading = 1;
    uint32_t bit_mask = 1U << (31 - bits_to_skip);
    
    while ((w & bit_mask) == 0 && bit_mask > 0) {
        leading++;
        bit_mask >>= 1;
    }
    
    return leading;
}

double HyperLogLog::alpha_m() const {
    // Константы коррекции смещения для разных m
    switch (m) {
        case 16:    return 0.673;
        case 32:    return 0.697;
        case 64:    return 0.709;
        default:    return 0.7213 / (1.0 + 1.079 / m);
    }
}

void HyperLogLog::add(const std::string& item) {
    // 1. Хешируем элемент
    uint32_t hash = hasher.hash(item);
    
    // 2. Первые B бит - индекс регистра
    uint32_t j = hash >> (32 - B);
    
    // 3. Оставшиеся (32 - B) бит - для подсчета ведущих нулей
    uint8_t w = leadingZeros(hash, B);
    
    // 4. Обновляем регистр максимальным значением
    if (w > registers[j]) {
        registers[j] = w;
    }
}

uint64_t HyperLogLog::estimate() const {
    // 1. Вычисляем сумму 2^(-M[j])
    double sum = 0.0;
    int zero_count = 0;
    
    for (size_t j = 0; j < m; ++j) {
        sum += std::pow(2.0, -static_cast<double>(registers[j]));
        if (registers[j] == 0) {
            zero_count++;
        }
    }
    
    // 2. Базовая оценка
    double estimate = alpha_m() * m * m / sum;
    
    // 3. Коррекция для малых значений (Small range correction)
    if (estimate <= 2.5 * m) {
        if (zero_count != 0) {
            estimate = m * std::log(static_cast<double>(m) / zero_count);
        }
    }
    
    // 4. Коррекция для больших значений (Large range correction)
    // Для 32-битного хеша: 2^32 / 30 ≈ 143,165,576
    const double pow_32 = 4294967296.0;  // 2^32
    if (estimate > pow_32 / 30.0) {
        estimate = -pow_32 * std::log(1.0 - estimate / pow_32);
    }
    
    return static_cast<uint64_t>(estimate);
}

void HyperLogLog::clear() {
    std::fill(registers.begin(), registers.end(), 0);
}

uint64_t exactCount(const std::vector<std::string>& stream) {
    std::unordered_set<std::string> unique_elements(stream.begin(), stream.end());
    return unique_elements.size();
}
