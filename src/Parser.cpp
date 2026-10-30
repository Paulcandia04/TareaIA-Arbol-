#include "Parser.hpp"

#include <stdexcept>
#include <utility>

namespace {

std::string trim(const std::string& text) {
    const auto first = text.find_first_not_of(' ');

    if (first == std::string::npos) {
        return "";
    }

    const auto last = text.find_last_not_of(' ');
    return text.substr(first, last - first + 1);
}


std::size_t findTopLevel(
    const std::string& text,
    const std::string& connector
) {
    int depth = 0;
    std::size_t found = std::string::npos;

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '(') {
            ++depth;
        } else if (text[i] == ')') {
            --depth;

            if (depth < 0) {
                throw std::invalid_argument(
                    "Hay un parentesis de cierre sin apertura."
                );
            }
        } else if (
            depth == 0 &&
            found == std::string::npos &&
            text.compare(i, connector.size(), connector) == 0
        ) {
            found = i;
        }
    }

    if (depth != 0) {
        throw std::invalid_argument("Falta cerrar un parentesis.");
    }

    return found;
}

bool hasOuterParentheses(const std::string& text) {
    if (text.size() < 2 ||
        text.front() != '(' ||
        text.back() != ')') {
        return false;
    }

    int depth = 0;

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '(') {
            ++depth;
        } else if (text[i] == ')') {
            --depth;
        }

        if (depth == 0 && i != text.size() - 1) {
            return false;
        }
    }

    return depth == 0;
}

} 

Parser::Parser(AtomRegistry& registry)
    : registry(registry) {}

Formula::Ptr Parser::parse(const std::string& text) {
    const std::string normalized = normalize(text);

    if (normalized.empty()) {
        throw std::invalid_argument("La regla no puede estar vacia.");
    }

    return parseNormalized(normalized);
}

Formula::Ptr Parser::parseNormalized(const std::string& input) {
    std::string text = trim(input);

    if (text.empty()) {
        throw std::invalid_argument("Falta una proposicion.");
    }

    findTopLevel(text, " ");

    while (hasOuterParentheses(text)) {
        text = trim(text.substr(1, text.size() - 2));

        if (text.empty()) {
            throw std::invalid_argument("Hay parentesis vacios.");
        }
    }

    const std::string iffConnector = " si y solo si ";
    auto position = findTopLevel(text, iffConnector);

    if (position != std::string::npos) {
        const auto left = parseNormalized(text.substr(0, position));
        const auto right = parseNormalized(
            text.substr(position + iffConnector.size())
        );

        return Formula::makeBinary(FormulaType::Iff, left, right);
    }

    if (text.rfind("si ", 0) == 0) {
        const std::string thenConnector = " entonces ";
        position = findTopLevel(text, thenConnector);

        if (position == std::string::npos) {
            throw std::invalid_argument(
                "La regla empieza con 'si', pero falta 'entonces'."
            );
        }

        std::string antecedent = trim(
            text.substr(3, position - 3)
        );

        if (!antecedent.empty() && antecedent.back() == ',') {
            antecedent.pop_back();
        }

        const auto left = parseNormalized(antecedent);
        const auto right = parseNormalized(
            text.substr(position + thenConnector.size())
        );

        return Formula::makeBinary(
            FormulaType::Implies, left, right
        );
    }

    const std::string orConnector = " o ";
    position = findTopLevel(text, orConnector);

    if (position != std::string::npos) {
        const auto left = parseNormalized(text.substr(0, position));
        const auto right = parseNormalized(
            text.substr(position + orConnector.size())
        );

        return Formula::makeBinary(FormulaType::Or, left, right);
    }

    const std::string andConnector = " y ";
    position = findTopLevel(text, andConnector);

    if (position != std::string::npos) {
        const auto left = parseNormalized(text.substr(0, position));
        const auto right = parseNormalized(
            text.substr(position + andConnector.size())
        );

        return Formula::makeBinary(FormulaType::And, left, right);
    }

    if (text == "no") {
        throw std::invalid_argument(
            "Falta la proposicion despues de 'no'."
        );
    }

    if (text.rfind("no ", 0) == 0) {
        return Formula::makeNot(
            parseNormalized(text.substr(3))
        );
    }

    const std::string padded = " " + text + " ";

    if (padded.find(" y ") != std::string::npos ||
        padded.find(" o ") != std::string::npos ||
        padded.find(" entonces ") != std::string::npos ||
        text.find('(') != std::string::npos ||
        text.find(')') != std::string::npos) {
        throw std::invalid_argument(
            "Expresion incompleta o parentesis mal ubicados."
        );
    }

    return Formula::makeAtom(registry.getOrCreate(text));
}

std::string Parser::normalize(const std::string& text) {
    std::string result;
    bool pendingSpace = false;

    for (unsigned char character : text) {
        if (character == ' ' || character == '\t' ||
            character == '\r' || character == '\n') {
            if (!result.empty()) {
                pendingSpace = true;
            }
            continue;
        }

        if (pendingSpace) {
            result += ' ';
            pendingSpace = false;
        }

        if (character >= 'A' && character <= 'Z') {
            character = static_cast<unsigned char>(
                character - 'A' + 'a'
            );
        }

        result += static_cast<char>(character);
    }

    const std::pair<std::string, std::string> replacements[] = {
        {u8"Á", u8"á"},
        {u8"É", u8"é"},
        {u8"Í", u8"í"},
        {u8"Ó", u8"ó"},
        {u8"Ú", u8"ú"},
        {u8"Ü", u8"ü"},
        {u8"Ñ", u8"ñ"}
    };

    for (const auto& replacement : replacements) {
        std::size_t position = 0;

        while ((position = result.find(
            replacement.first, position
        )) != std::string::npos) {
            result.replace(
                position,
                replacement.first.size(),
                replacement.second
            );

            position += replacement.second.size();
        }
    }

    if (!result.empty() && result.back() == '.') {
        result.pop_back();

        while (!result.empty() && result.back() == ' ') {
            result.pop_back();
        }
    }

    return result;
}