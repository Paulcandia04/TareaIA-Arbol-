#include "Tableau.hpp"

#include <stdexcept>
#include <utility>

TableauResult Tableau::build(
    const FormulaList& knowledgeBase
) const {
    TableauResult result;
    result.root = std::make_unique<TableauNode>();

    for (const auto& formula : knowledgeBase) {
        if (!formula) {
            throw std::invalid_argument(
                "La base contiene una formula nula."
            );
        }

        result.root->entries.push_back({formula, false});
    }

    develop(*result.root, result);

    return result;
}

void Tableau::develop(
    TableauNode& node,
    TableauResult& result
) const {
    Model assignments;

    for (const auto& entry : node.entries) {
        const auto& formula = entry.formula;

        if (!isLiteral(formula)) {
            continue;
        }

        const bool value = formula->type == FormulaType::Atom;

        const std::string symbol = value
            ? formula->atom
            : formula->left->atom;

        const auto existing = assignments.find(symbol);

        if (existing != assignments.end() &&
            existing->second != value) {
            node.closed = true;
            node.contradiction =
                symbol + u8" y ¬" + symbol;

            ++result.closedBranches;
            return;
        }

        assignments[symbol] = value;
    }

    const std::size_t none = node.entries.size();
    std::size_t selected = none;
    Expansion selectedExpansion;

    for (std::size_t i = 0; i < node.entries.size(); ++i) {
        const auto& entry = node.entries[i];

        if (entry.applied || isLiteral(entry.formula)) {
            continue;
        }

        auto expansion = expandFormula(entry.formula);

        if (selected == none || expansion.size() == 1) {
            selected = i;
            selectedExpansion = std::move(expansion);
        }

        if (selectedExpansion.size() == 1) {
            break;
        }
    }

    if (selected == none) {
        result.models.push_back(std::move(assignments));
        return;
    }

    node.entries[selected].applied = true;

    for (const auto& alternative : selectedExpansion) {
        auto child = std::make_unique<TableauNode>();

        child->entries = node.entries;
        child->inheritedCount = node.entries.size();

        for (const auto& formula : alternative) {
            child->entries.push_back({formula, false});
        }

        develop(*child, result);

        node.children.push_back(std::move(child));
    }
}