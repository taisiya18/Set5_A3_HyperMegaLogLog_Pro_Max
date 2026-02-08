#ifndef HASHFUNCGEN_H
#define HASHFUNCGEN_H

#include <string>
#include <cstdint>
#include <functional>

class HashFuncGen {
private:
    uint32_t seed;
    
    // MurmurHash3 32-bit версия
    static uint32_t murmur3_32(const std::string& key, uint32_t seed);
    
    // Вспомогательные функции для MurmurHash3
    static inline uint32_t rotl32(uint32_t x, int8_t r) {
        return (x << r) | (x >> (32 - r));
    }
    
    static inline uint32_t fmix32(uint32_t h) {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }
    
public:
    // Конструктор с seed для хеш-функции
    HashFuncGen(uint32_t seed = 42);
    
    // Основная хеш-функция: U -> M = 2^32
    uint32_t hash(const std::string& str) const;
    
    // Получение seed
    uint32_t getSeed() const;
    
    // Установка нового seed
    void setSeed(uint32_t new_seed);
    
    // Тестирование равномерности распределения
    static void testUniformity(const std::vector<std::string>& data, 
                               uint32_t seed = 42,
                               int num_buckets = 1000);
};

#endif // HASHFUNCGEN_H
