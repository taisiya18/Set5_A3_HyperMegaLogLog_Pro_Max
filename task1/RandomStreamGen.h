#ifndef RANDOMSTREAMGEN_H
#define RANDOMSTREAMGEN_H

#include <string>
#include <vector>
#include <random>

class RandomStreamGen {
private:
    std::mt19937_64 rng;
    std::uniform_int_distribution<int> length_dist;
    std::uniform_int_distribution<int> char_dist;
    
    // Допустимые символы: A-Z, a-z, 0-9, тире
    static constexpr char CHARSET[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789-";
    static constexpr int CHARSET_SIZE = sizeof(CHARSET) - 1;
    
    size_t total_elements;
    std::vector<std::string> stream;
    
public:
    // Конструктор
    RandomStreamGen(size_t n_elements, unsigned int seed = std::random_device{}());
    
    // Генерация случайной строки
    std::string generateRandomString();
    
    // Генерация всего потока
    void generateStream();
    
    // Получение части потока (процент от 0 до 100)
    std::vector<std::string> getStreamPart(double percentage) const;
    
    // Получение части потока по индексу
    std::vector<std::string> getStreamPartByIndex(size_t end_index) const;
    
    // Получение всего потока
    const std::vector<std::string>& getFullStream() const;
    
    // Получение размера потока
    size_t size() const;
    
    // Очистка потока для экономии памяти
    void clear();
};

#endif // RANDOMSTREAMGEN_H
