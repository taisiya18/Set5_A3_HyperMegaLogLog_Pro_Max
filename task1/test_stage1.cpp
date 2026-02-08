#include "RandomStreamGen.h"
#include "HashFuncGen.h"
#include <iostream>
#include <iomanip>
#include <unordered_set>

int main() {
    std::cout << "=== Этап 1: Тестирование инфраструктуры ===" << std::endl;
    
    // 1. Тестирование RandomStreamGen
    std::cout << "\n--- Тест 1: Генерация потока данных ---" << std::endl;
    
    size_t stream_size = 100000;
    RandomStreamGen streamGen(stream_size, 12345);
    
    std::cout << "Генерация потока из " << stream_size << " элементов..." << std::endl;
    streamGen.generateStream();
    std::cout << "Поток сгенерирован успешно!" << std::endl;
    
    // Примеры сгенерированных строк
    std::cout << "\nПримеры сгенерированных строк:" << std::endl;
    const auto& full_stream = streamGen.getFullStream();
    for (int i = 0; i < 10; ++i) {
        std::cout << i + 1 << ". \"" << full_stream[i] << "\"" << std::endl;
    }
    
    // Статистика по длинам строк
    std::vector<int> length_counts(31, 0);
    for (const auto& str : full_stream) {
        if (str.length() <= 30) {
            length_counts[str.length()]++;
        }
    }
    
    std::cout << "\nРаспределение длин строк:" << std::endl;
    for (int i = 1; i <= 30; ++i) {
        if (length_counts[i] > 0) {
            double percentage = (length_counts[i] * 100.0) / stream_size;
            std::cout << "Длина " << std::setw(2) << i << ": " 
                      << std::setw(6) << length_counts[i] 
                      << " (" << std::fixed << std::setprecision(2) 
                      << percentage << "%)" << std::endl;
        }
    }
    
    // 2. Тестирование разбиения потока на части
    std::cout << "\n--- Тест 2: Разбиение потока на части ---" << std::endl;
    
    std::vector<double> percentages = {10, 25, 50, 75, 100};
    for (double pct : percentages) {
        auto part = streamGen.getStreamPart(pct);
        std::cout << pct << "% потока: " << part.size() << " элементов" << std::endl;
    }
    
    // 3. Тестирование HashFuncGen
    std::cout << "\n--- Тест 3: Хеш-функция ---" << std::endl;
    
    HashFuncGen hasher(42);
    
    // Примеры хешей
    std::cout << "\nПримеры хеш-значений:" << std::endl;
    for (int i = 0; i < 10; ++i) {
        const auto& str = full_stream[i];
        uint32_t hash_value = hasher.hash(str);
        std::cout << "hash(\"" << str << "\") = " 
                  << hash_value << " (0x" << std::hex 
                  << hash_value << std::dec << ")" << std::endl;
    }
    
    // Проверка на коллизии
    std::cout << "\n--- Тест 4: Проверка коллизий ---" << std::endl;
    std::unordered_set<uint32_t> hash_set;
    int collisions = 0;
    
    for (const auto& str : full_stream) {
        uint32_t hash_value = hasher.hash(str);
        if (hash_set.count(hash_value) > 0) {
            collisions++;
        }
        hash_set.insert(hash_value);
    }
    
    std::cout << "Всего элементов: " << stream_size << std::endl;
    std::cout << "Уникальных хешей: " << hash_set.size() << std::endl;
    std::cout << "Коллизий: " << collisions << std::endl;
    std::cout << "Процент коллизий: " << std::fixed << std::setprecision(4)
              << (collisions * 100.0 / stream_size) << "%" << std::endl;
    
    // 4. Тестирование равномерности распределения
    std::cout << "\n--- Тест 5: Равномерность распределения хеш-функции ---" << std::endl;
    HashFuncGen::testUniformity(full_stream, 42, 1000);
    
    // 5. Демонстрация работы с разными частями потока
    std::cout << "\n--- Тест 6: Анализ уникальных элементов по частям потока ---" << std::endl;
    
    std::vector<int> steps = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    std::cout << "\nКоличество уникальных строк на каждом шаге:" << std::endl;
    std::cout << std::setw(10) << "Процент" << std::setw(15) << "Размер" 
              << std::setw(20) << "Уникальные строки" << std::endl;
    std::cout << std::string(45, '-') << std::endl;
    
    for (int step : steps) {
        auto part = streamGen.getStreamPart(step);
        std::unordered_set<std::string> unique_strings(part.begin(), part.end());
        std::cout << std::setw(9) << step << "%" 
                  << std::setw(15) << part.size()
                  << std::setw(20) << unique_strings.size() << std::endl;
    }
    
    std::cout << "\n=== Все тесты этапа 1 пройдены успешно! ===" << std::endl;
    
    return 0;
}
