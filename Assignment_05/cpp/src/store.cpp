#include "store.hpp"

#include <fstream>
#include <stdexcept>

void Store::set(const std::string& key, const std::string& value) {
    data_[key] = value;
}

std::optional<std::string> Store::get(const std::string& key) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return std::nullopt;
    }
    return it->second;
}

bool Store::remove(const std::string& key) {
    return data_.erase(key) > 0;
}

std::vector<std::pair<std::string, std::string>> Store::list() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(data_.size());
    for (const auto& entry : data_) {
        result.push_back(entry);
    }
    return result;
}

// File format: each entry is written as two lines -- the key, then the
// value. This avoids ambiguity with a "key=value" delimiter if a value
// ever contained an '=' character.
void Store::save(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("could not open file for writing: " + filename);
    }
    for (const auto& [key, value] : data_) {
        out << key << '\n' << value << '\n';
    }
    // out (an ofstream) closes automatically via RAII when it goes out of
    // scope at the end of this function.
}

void Store::load(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("could not open file for reading: " + filename);
    }

    std::map<std::string, std::string> loaded;
    std::string key;
    std::string value;
    while (std::getline(in, key)) {
        if (!std::getline(in, value)) {
            throw std::runtime_error("malformed save file: " + filename);
        }
        loaded[key] = value;
    }

    data_ = std::move(loaded);
    // in (an ifstream) closes automatically via RAII when it goes out of
    // scope at the end of this function.
}
