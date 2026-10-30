#include "Expansion.hpp"

#include <stdexcept>

bool isLiteral(const Formula::Ptr& formula) {
    if (!formula) {
        return false;
    }

    return formula->type == FormulaType::Atom ||
        (formula->type == FormulaType::Not &&
         formula->left &&
         formula->left->type == FormulaType::Atom);
}

Expansion expandFormula(const Formula::Ptr& formula) {
    if (!formula || isLiteral(formula)) {
        throw std::invalid_argument(
            "La expansion requiere una formula compuesta."
        );
    }

    const auto neg = [](Formula::Ptr operand) {
        return Formula::makeNot(operand);
    };

    const auto a = formula->left;
    const auto b = formula->right;

    switch (formula->type) {
        case FormulaType::And:
            return {{a, b}};

        case FormulaType::Or:
            return {{a}, {b}};

        case FormulaType::Implies:
            return {{neg(a)}, {b}};

        case FormulaType::Iff:
            return {{a, b}, {neg(a), neg(b)}};

        case FormulaType::Not:
            break;

        default:
            throw std::logic_error("Tipo de formula inesperado.");
    }

    const auto inner = formula->left;

    if (inner->type == FormulaType::Not) {
        return {{inner->left}};
    }

    const auto innerA = inner->left;
    const auto innerB = inner->right;

    switch (inner->type) {
        case FormulaType::And:
            return {{neg(innerA)}, {neg(innerB)}};

        case FormulaType::Or:
            return {{neg(innerA), neg(innerB)}};

        case FormulaType::Implies:
            return {{innerA, neg(innerB)}};

        case FormulaType::Iff:
            return {
                {innerA, neg(innerB)},
                {neg(innerA), innerB}
            };

        default:
            throw std::logic_error(
                "No se puede expandir esta negacion."
            );
    }
}