#include "Formula.hpp"

#include <stdexcept>
#include <utility>

Formula::Ptr Formula::makeAtom(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("El atomo necesita un nombre.");
    }

    auto formula = std::make_shared<Formula>(FormulaType::Atom);
    formula->atom = name;
    return formula;
}

Formula::Ptr Formula::makeNot(Ptr operand) {
    if (!operand) {
        throw std::invalid_argument("La negacion necesita una formula.");
    }

    auto formula = std::make_shared<Formula>(FormulaType::Not);
    formula->left = std::move(operand);
    return formula;
}

Formula::Ptr Formula::makeBinary(
    FormulaType type,
    Ptr left,
    Ptr right
) {
    if (type != FormulaType::And &&
        type != FormulaType::Or &&
        type != FormulaType::Implies &&
        type != FormulaType::Iff) {
        throw std::invalid_argument("Se esperaba un conectivo binario.");
    }

    if (!left || !right) {
        throw std::invalid_argument("El conectivo necesita dos formulas.");
    }

    auto formula = std::make_shared<Formula>(type);
    formula->left = std::move(left);
    formula->right = std::move(right);
    return formula;
}

std::string Formula::toString() const {
    if (type == FormulaType::Atom) {
        return atom;
    }

    if (type == FormulaType::Not) {
        return u8"¬" + left->toString();
    }

    std::string symbol;

    switch (type) {
        case FormulaType::And:     symbol = u8" ∧ "; break;
        case FormulaType::Or:      symbol = u8" ∨ "; break;
        case FormulaType::Implies: symbol = u8" → "; break;
        case FormulaType::Iff:     symbol = u8" ↔ "; break;
        default:
            throw std::logic_error("Tipo de formula desconocido.");
    }

    return "(" + left->toString() + symbol + right->toString() + ")";
}