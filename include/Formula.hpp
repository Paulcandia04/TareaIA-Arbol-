#pragma once

#include <memory>
#include <string>

enum class FormulaType {
    Atom,
    Not,
    And,
    Or,
    Implies,
    Iff
};

struct Formula {
    using Ptr = std::shared_ptr<const Formula>;

    FormulaType type;
    std::string atom;

    Ptr left;
    Ptr right;

    explicit Formula(FormulaType formulaType)
        : type(formulaType) {}

    static Ptr makeAtom(const std::string& name);
    static Ptr makeNot(Ptr operand);
    static Ptr makeBinary(FormulaType type, Ptr left, Ptr right);

    std::string toString() const;
};