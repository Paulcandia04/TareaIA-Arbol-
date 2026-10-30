#include "AtomRegistry.hpp"

#include <stdexcept>

std::string AtomRegistry::getOrCreate(
    const std::string& proposition
) {
    if (proposition.find_first_not_of(" \t\r\n") == std::string::npos) {
        throw std::invalid_argument("La proposicion no puede estar vacia.");
    }

    const auto existing = indexByMeaning.find(proposition);

    if (existing != indexByMeaning.end()) {
        return atoms[existing->second].symbol;
    }

    const std::size_t index = atoms.size();
    const std::string symbol = generateSymbol(index);

    atoms.push_back({symbol, proposition});
    indexByMeaning.emplace(proposition, index);

    return symbol;
}

const std::vector<AtomEntry>& AtomRegistry::entries() const {
    return atoms;
}

std::string AtomRegistry::generateSymbol(std::size_t index) {
    const std::string letters = "pqrstuvwxyzabcdefghijklmno";

    std::string symbol(1, letters[index % letters.size()]);

    const std::size_t cycle = index / letters.size();

    if (cycle > 0) {
        symbol += std::to_string(cycle);
    }

    return symbol;
}