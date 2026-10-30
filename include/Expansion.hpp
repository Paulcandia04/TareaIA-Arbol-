#pragma once

#include "Formula.hpp"

#include <vector>

using FormulaList = std::vector<Formula::Ptr>;
using Expansion = std::vector<FormulaList>;

bool isLiteral(const Formula::Ptr& formula);

Expansion expandFormula(const Formula::Ptr& formula);