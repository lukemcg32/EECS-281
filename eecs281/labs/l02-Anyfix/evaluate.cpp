// Project identifier: 1CAEF3A0FEDD0DEC26BA9808C69D4D22A9962768

#include "evaluate.hpp"
#include <stack>
#include <cstdint>
#include <stdexcept>


std::int64_t evaluate(std::string const& expression) {

    std::stack<std::int64_t> operands;
    std::stack<char> operators;

    auto applyOperator = [&](char op) {
        if (operands.size() < 2) throw std::runtime_error("Not enough operands.");
        auto right = operands.top(); 
        operands.pop();
        auto left = operands.top(); 
        operands.pop();

        switch (op) {
            case '+': operands.push(left + right); 
                break;
            case '-': operands.push(left - right); 
                break;
            case '*': operands.push(left * right); 
                break;
            case '/':
                if (right == 0) throw std::runtime_error("Division by zero.");
                operands.push(left / right); 
                break;
            default: throw std::runtime_error("Unknown operator.");
        }
    };

    for (char ch : expression) {
        if (std::isdigit(ch)) {
            operands.push(static_cast<int64_t>(ch - '0'));
        } else {
            operators.push(ch);
            // Since the context of anyfix might require immediate evaluation or waiting,
            // this simplistic approach might not suffice for all cases. You may need to
            // adjust when and how operators are applied based on the full ruleset.
        }

        if (operands.size() >= 2 && !operators.empty()) {
            applyOperator(operators.top());
            operators.pop();
        }
    }

    // apply any remaining operators
    while (!operators.empty() && operands.size() >= 2) {
        applyOperator(operators.top());
        operators.pop();
    }

    if (operands.size() != 1) {
        throw std::runtime_error("Invalid expression evaluation.");
    }

    return operands.top();
}
