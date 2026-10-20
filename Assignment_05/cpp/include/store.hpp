#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

// Store owns the underlying key-value data. The command-processing layer
// never touches the underlying container directly -- it only calls these
// member functions.
class Store {
public:
    Store() = default;

    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key) const;
    bool remove(const std::string& key);
    std::vector<std::pair<std::string, std::string>> list() const;

    // SAVE/LOAD are the program's file-I/O side effects. They are
    // implemented using std::ofstream/std::ifstream (RAII file handles).
    void save(const std::string& filename) const;
    void load(const std::string& filename);

    // Default copy constructor/assignment are fine: they deep-copy the
    // underlying std::map, which is exactly what Transaction needs to
    // take a backup snapshot.

private:
    std::map<std::string, std::string> data_;
};
