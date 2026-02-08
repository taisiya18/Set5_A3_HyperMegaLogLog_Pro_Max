#include "RandomStreamGen.h"
#include <iostream>

constexpr char RandomStreamGen::CHARSET[];

RandomStreamGen::RandomStreamGen(size_t n_elements, unsigned int seed)
    : rng(seed),
      length_dist(1, 30),  // Длина строки от 1 до 30
      char_dist(0, CHARSET_SIZE - 1),
      total_elements(n_elements) {
}

std::string RandomStreamGen::generateRandomString() {
    int length = length_dist(rng);
    std::string result;
    result.reserve(length);
    
    for (int i = 0; i < length; ++i) {
        result += CHARSET[char_dist(rng)];
    }
    
    return result;
}

void RandomStreamGen::generateStream() {
    stream.clear();
    stream.reserve(total_elements);
    
    for (size_t i = 0; i < total_elements; ++i) {
        stream.push_back(generateRandomString());
        
        // Прогресс для больших потоков
        if ((i + 1) % 100000 == 0) {
            std::cout << "Generated " << (i + 1) << " elements..." << std::endl;
        }
    }
}

std::vector<std::string> RandomStreamGen::getStreamPart(double percentage) const {
    if (percentage < 0 || percentage > 100) {
        throw std::invalid_argument("Percentage must be between 0 and 100");
    }
    
    size_t end_index = static_cast<size_t>((percentage / 100.0) * stream.size());
    return std::vector<std::string>(stream.begin(), stream.begin() + end_index);
}

std::vector<std::string> RandomStreamGen::getStreamPartByIndex(size_t end_index) const {
    if (end_index > stream.size()) {
        end_index = stream.size();
    }
    
    return std::vector<std::string>(stream.begin(), stream.begin() + end_index);
}

const std::vector<std::string>& RandomStreamGen::getFullStream() const {
    return stream;
}

size_t RandomStreamGen::size() const {
    return stream.size();
}

void RandomStreamGen::clear() {
    stream.clear();
    stream.shrink_to_fit();
}
