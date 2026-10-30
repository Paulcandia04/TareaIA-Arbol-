#pragma once

#include "Expansion.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct TableauEntry {
    Formula::Ptr formula;
    bool applied = false;
};

struct TableauNode {
    std::vector<TableauEntry> entries;
    std::vector<std::unique_ptr<TableauNode>> children;

    std::size_t inheritedCount = 0;

    bool closed = false;
    std::string contradiction;
};

using Model = std::map<std::string, bool>;

struct TableauResult {
    std::unique_ptr<TableauNode> root;
    std::vector<Model> models;
    std::size_t closedBranches = 0;

    bool isSatisfiable() const {
        return !models.empty();
    }
};

class Tableau {
public:
    TableauResult build(const FormulaList& knowledgeBase) const;

private:
    void develop(
        TableauNode& node,
        TableauResult& result
    ) const;
};