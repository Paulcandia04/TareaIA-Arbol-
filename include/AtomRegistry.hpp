#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct AtomEntry {
    std::string symbol;
    std::string meaning;
};

class AtomRegistry {
public:
    std::string getOrCreate(const std::string& proposition);

    const std::vector<AtomEntry>& entries() const;

private:
    std::vector<AtomEntry> atoms;
    std::unordered_map<std::string, std::size_t> indexByMeaning;

    static std::string generateSymbol(std::size_t index);
};