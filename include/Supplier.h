#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <iostream>
#include <string>

// ============================================================================
// OOP CONCEPT: Composition / Data Encapsulation
// A Supplier object is composed inside a Product object (Product HAS-A Supplier)
// Demonstrates:
// 1. Data Hiding (private attributes)
// 2. Constructors (Default & Parameterized with Member Initializer List)
// 3. Getters and Setters
// 4. Operator Overloading (Stream insertion <<)
// ============================================================================

class Supplier {
private:
    int id;
    std::string name;
    std::string contactNumber;
    std::string email;

public:
    // Default Constructor
    Supplier();

    // Parameterized Constructor (using member initializer list)
    Supplier(int id, const std::string& name, const std::string& contact, const std::string& email);

    // Getters
    int getId() const;
    std::string getName() const;
    std::string getContactNumber() const;
    std::string getEmail() const;

    // Setters
    void setId(int id);
    void setName(const std::string& name);
    void setContactNumber(const std::string& contact);
    void setEmail(const std::string& email);

    // Serialization for file handling (CSV-style delimiter)
    std::string serialize() const;
    void deserialize(const std::string& data);

    // Operator Overloading
    friend std::ostream& operator<<(std::ostream& os, const Supplier& s);
};

#endif // SUPPLIER_H
