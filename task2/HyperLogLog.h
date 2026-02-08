#ifndef HYPERLOGLOG_H
#define HYPERLOGLOG_H

#include <vector>
#include <cstdint>
#include <string>
#include <cmath>
#include "HashFuncGen.h"

class HyperLogLog {
private:
    uint8_t B;                      // Количество бит для индекса (регистров)
    size_t m;                       // Количество регистров (2^B)
    std::vector<uint8_t> registers; // Регистры для хранения максимумов
    HashFuncGen hasher;             // Хеш-функция
    
    // Вспомогательная функция для подсчета ведущих нулей + 1
    uint8_t leadingZeros(uint32_t hash, uint8_t bits_to_skip) const;
    
    // Константа для коррекции смещения (bias correction)
    double alpha_m() const;
    
public:
    // Конструктор
    HyperLogLog(uint8_t b = 14, uint32_t seed = 42);
    
    // Добавление элемента в структуру
    void add(const std::string& item);
    
    // Получение оценки количества уникальных элементов
    uint64_t estimate() const;
    
    // Сброс всех регистров
    void clear();
    
    // Получение количества регистров
    size_t getM() const { return m; }
    
    // Получение параметра B
    uint8_t getB() const { return B; }
    
    // Получение состояния регистров (для анализа)
    const std::vector<uint8_t>& getRegisters() const { return registers; }
};

// Функция для точного подсчета уникальных элементов
uint64_t exactCount(const std::vector<std::string>& stream);

#endif // HYPERLOGLOG_H
