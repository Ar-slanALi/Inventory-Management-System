#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>
#include <string>
#include <exception>

// ============================================================================
// OOP CONCEPT: Exception Handling (Custom Exception Hierarchy)
// Demonstrates:
// 1. Inheritance from standard library base class (std::exception)
// 2. Runtime Polymorphism via virtual what() const noexcept override
// 3. Encapsulation of error messages
// ============================================================================

class IMSException : public std::exception {
protected:
    std::string errorMessage;

public:
    explicit IMSException(const std::string& msg) : errorMessage(msg) {}
    virtual ~IMSException() noexcept = default;

    virtual const char* what() const noexcept override {
        return errorMessage.c_str();
    }
};

class InsufficientStockException : public IMSException {
public:
    explicit InsufficientStockException(const std::string& msg = "Error: Insufficient stock available for this operation!")
        : IMSException(msg) {}
};

class DuplicateProductException : public IMSException {
public:
    explicit DuplicateProductException(const std::string& msg = "Error: A product with this ID already exists in inventory!")
        : IMSException(msg) {}
};

class ProductNotFoundException : public IMSException {
public:
    explicit ProductNotFoundException(const std::string& msg = "Error: Product not found in inventory records!")
        : IMSException(msg) {}
};

class InvalidInputException : public IMSException {
public:
    explicit InvalidInputException(const std::string& msg = "Error: Invalid input value provided!")
        : IMSException(msg) {}
};

class AuthenticationException : public IMSException {
public:
    explicit AuthenticationException(const std::string& msg = "Error: Authentication failed! Invalid username or password.")
        : IMSException(msg) {}
};

#endif // EXCEPTIONS_H
