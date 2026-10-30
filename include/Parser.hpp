#pragma once

#include "AtomRegistry.hpp"
#include "Formula.hpp"

#include <string>

class Parser {
public:
    explicit Parser(AtomRegistry& registry);

    Formula::Ptr parse(const std::string& text);

private:
    AtomRegistry& registry;

    Formula::Ptr parseNormalized(const std::string& text);
    static std::string normalize(const std::string& text);
};