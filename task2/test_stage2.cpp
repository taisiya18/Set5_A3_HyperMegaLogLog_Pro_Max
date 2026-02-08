#include "RandomStreamGen.h"
#include "HashFuncGen.h"
#include "HyperLogLog.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <map>

// Структура для хранения результатов одного теста
struct TestResult {
    size_t stream_size;
    double percentage;
    uint64_t exact_count;
    uint64_t hll_estimate;
    double relative_error;
};

// Структура для хранения статистики по нескольким потокам
struct Statistics {
    double mean_estimate;
    double std_dev;
    double mean_error;
};

// Функция для вычисления статистик
Statistics computeStatistics(const std::vector<uint64_t>& estimates, uint64_t exact) {
    Statistics stats;
    
    // Среднее
    double sum = 0.0;
    for (uint64_t est : estimates) {
        sum += est;
    }
    stats.mean_estimate = sum / estimates.size();
    
    // Стандартное отклонение
    double variance = 0.0;
    for (uint64_t est : estimates) {
        double diff = est - stats.mean_estimate;
        variance += diff * diff;
    }
    variance /= estimates.size();
    stats.std_dev = std::sqrt(variance);
    
    // Средняя относительная ошибка
    stats.mean_error = std::abs(stats.mean_estimate - exact) / exact;
    
    return stats;
}

int main() {
    std::cout << "=== Этап 2: Реализация и оценка HyperLogLog ===" << std::endl;
    
    // Параметры тестирования
    const uint8_t B = 14;  // Выбираем B = 14 (16384 регистра)
    const size_t num_streams = 10;  // Количество потоков для статистики
    const size_t stream_size = 1000000;  // Размер каждого потока
    
    std::cout << "\nПараметры:" << std::endl;
    std::cout << "B = " << static_cast<int>(B) << " (количество регистров: " 
              << (1 << B) << ")" << std::endl;
    std::cout << "Количество потоков: " << num_streams << std::endl;
    std::cout << "Размер потока: " << stream_size << std::endl;
    
    // Теоретическая оценка ошибки
    double theoretical_error_1 = 1.04 / std::sqrt(1 << B);
    double theoretical_error_2 = 1.32 / std::sqrt(1 << B);
    std::cout << "\nТеоретическая ошибка:" << std::endl;
    std::cout << "1.04/√m = " << std::fixed << std::setprecision(4) 
              << theoretical_error_1 * 100 << "%" << std::endl;
    std::cout << "1.32/√m = " << theoretical_error_2 * 100 << "%" << std::endl;
    
    // Шаги для анализа (проценты потока)
    std::vector<double> percentages = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    
    // Хранилище для результатов
    std::vector<std::vector<TestResult>> all_results(num_streams);
    
    // Для каждого шага храним оценки от всех потоков
    std::map<double, std::vector<uint64_t>> estimates_by_step;
    std::map<double, uint64_t> exact_by_step;
    
    std::cout << "\n--- Генерация и тестирование потоков ---" << std::endl;
    
    for (size_t stream_idx = 0; stream_idx < num_streams; ++stream_idx) {
        std::cout << "\nПоток #" << (stream_idx + 1) << ":" << std::endl;
        
        // Генерация потока с уникальным seed
        RandomStreamGen streamGen(stream_size, 1000 + stream_idx * 100);
        streamGen.generateStream();
        
        // Тестируем на каждом шаге
        for (double pct : percentages) {
            auto part = streamGen.getStreamPart(pct);
            
            // Точный подсчет
            uint64_t exact = exactCount(part);
            
            // HyperLogLog оценка
            HyperLogLog hll(B, 42);
            for (const auto& item : part) {
                hll.add(item);
            }
            uint64_t estimate = hll.estimate();
            
            // Относительная ошибка
            double error = std::abs(static_cast<double>(estimate) - exact) / exact;
            
            // Сохраняем результат
            TestResult result;
            result.stream_size = part.size();
            result.percentage = pct;
            result.exact_count = exact;
            result.hll_estimate = estimate;
            result.relative_error = error;
            all_results[stream_idx].push_back(result);
            
            // Добавляем в общую статистику
            estimates_by_step[pct].push_back(estimate);
            if (stream_idx == 0) {
                exact_by_step[pct] = exact;
            }
            
            std::cout << std::setw(5) << pct << "%: " 
                      << "Exact=" << std::setw(7) << exact
                      << ", HLL=" << std::setw(7) << estimate
                      << ", Error=" << std::fixed << std::setprecision(2) 
                      << std::setw(6) << (error * 100) << "%" << std::endl;
        }
    }
    
    // Вычисляем и выводим статистику
    std::cout << "\n=== Статистика по всем потокам ===" << std::endl;
    std::cout << std::setw(8) << "Step" 
              << std::setw(12) << "Exact F0"
              << std::setw(12) << "Mean(Nt)"
              << std::setw(12) << "Std(Nt)"
              << std::setw(12) << "Error(%)"
              << std::endl;
    std::cout << std::string(56, '-') << std::endl;
    
    std::ofstream stats_file("statistics.csv");
    stats_file << "percentage,exact,mean_estimate,std_dev,mean_error\n";
    
    for (double pct : percentages) {
        auto stats = computeStatistics(estimates_by_step[pct], exact_by_step[pct]);
        
        std::cout << std::setw(7) << pct << "%"
                  << std::setw(12) << exact_by_step[pct]
                  << std::setw(12) << static_cast<uint64_t>(stats.mean_estimate)
                  << std::setw(12) << static_cast<uint64_t>(stats.std_dev)
                  << std::setw(11) << std::fixed << std::setprecision(2)
                  << (stats.mean_error * 100) << "%"
                  << std::endl;
        
        stats_file << pct << ","
                   << exact_by_step[pct] << ","
                   << stats.mean_estimate << ","
                   << stats.std_dev << ","
                   << stats.mean_error << "\n";
    }
    stats_file.close();
    
    // Сохраняем детальные результаты для графика №1 (первый поток)
    std::ofstream detail_file("single_stream.csv");
    detail_file << "percentage,stream_size,exact_count,hll_estimate,error\n";
    for (const auto& result : all_results[0]) {
        detail_file << result.percentage << ","
                    << result.stream_size << ","
                    << result.exact_count << ","
                    << result.hll_estimate << ","
                    << result.relative_error << "\n";
    }
    detail_file.close();
    
    // Анализ распределения регистров
    std::cout << "\n=== Анализ распределения по регистрам ===" << std::endl;
    
    HyperLogLog hll_analysis(B, 42);
    RandomStreamGen test_stream(stream_size, 2000);
    test_stream.generateStream();
    
    for (const auto& item : test_stream.getFullStream()) {
        hll_analysis.add(item);
    }
    
    const auto& registers = hll_analysis.getRegisters();
    std::vector<int> value_counts(33, 0);  // Максимум 32 ведущих нуля + 1
    
    for (uint8_t val : registers) {
        if (val < value_counts.size()) {
            value_counts[val]++;
        }
    }
    
    std::cout << "Распределение значений в регистрах:" << std::endl;
    for (size_t i = 0; i < value_counts.size(); ++i) {
        if (value_counts[i] > 0) {
            double pct = (value_counts[i] * 100.0) / registers.size();
            std::cout << "Значение " << std::setw(2) << i << ": "
                      << std::setw(6) << value_counts[i]
                      << " (" << std::fixed << std::setprecision(2)
                      << std::setw(5) << pct << "%)" << std::endl;
        }
    }
    
    std::cout << "\n=== Обоснование выбора B = " << static_cast<int>(B) << " ===" << std::endl;
    std::cout << "1. Количество регистров: " << (1 << B) << std::endl;
    std::cout << "2. Память: " << (1 << B) << " байт ≈ " 
              << ((1 << B) / 1024.0) << " КБ" << std::endl;
    std::cout << "3. Теоретическая точность: ~" 
              << std::fixed << std::setprecision(2)
              << (theoretical_error_1 * 100) << "%" << std::endl;
    std::cout << "4. Хороший баланс между точностью и памятью" << std::endl;
    
    std::cout << "\nФайлы с данными созданы:" << std::endl;
    std::cout << "- statistics.csv (статистика по всем потокам)" << std::endl;
    std::cout << "- single_stream.csv (детальные данные одного потока)" << std::endl;
    
    std::cout << "\n=== Этап 2 завершен! ===" << std::endl;
    
    return 0;
}
