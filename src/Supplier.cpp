#include "../include/Supplier.h"
#include <sstream>

// Default Constructor
Supplier::Supplier() : id(0), name("N/A"), contactNumber("N/A"), email("N/A") {}

// Parameterized Constructor using Member Initializer List
Supplier::Supplier(int id, const std::string& name, const std::string& contact, const std::string& email)
    : id(id), name(name), contactNumber(contact), email(email) {}

// Getters
int Supplier::getId() const { return id; }
std::string Supplier::getName() const { return name; }
std::string Supplier::getContactNumber() const { return contactNumber; }
std::string Supplier::getEmail() const { return email; }

// Setters
void Supplier::setId(int id) { this->id = id; }
void Supplier::setName(const std::string& name) { this->name = name; }
void Supplier::setContactNumber(const std::string& contact) { this->contactNumber = contact; }
void Supplier::setEmail(const std::string& email) { this->email = email; }

// Serialization helper using '|' as delimiter
std::string Supplier::serialize() const {
    return std::to_string(id) + "|" + name + "|" + contactNumber + "|" + email;
}

void Supplier::deserialize(const std::string& data) {
    if (data.empty()) return;
    std::stringstream ss(data);
    std::string token;
    
    if (std::getline(ss, token, '|')) id = std::stoi(token);
    if (std::getline(ss, token, '|')) name = token;
    if (std::getline(ss, token, '|')) contactNumber = token;
    if (std::getline(ss, token, '|')) email = token;
}

// Operator Overloading <<
std::ostream& operator<<(std::ostream& os, const Supplier& s) {
    os << s.name << " (Phone: " << s.contactNumber << ", Email: " << s.email << ")";
    return os;
}
