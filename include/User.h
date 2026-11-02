#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>

// ============================================================================
// OOP CONCEPT: Abstract Base Class, Inheritance & Polymorphism
// Hierarchy:
//            User (Abstract Base)
//             ├── Admin (Derived Class - Full Permissions)
//             └── Staff (Derived Class - Operational / Cashier Permissions)
// Demonstrates:
// 1. Data Encapsulation & Hiding (Protected fields)
// 2. Pure Virtual Functions & Runtime Polymorphism
// 3. Virtual Destructors
// 4. Role-Based Access Control (RBAC)
// ============================================================================

class User {
protected:
    std::string username;
    std::string password;
    std::string fullName;
    std::string role;          // "ADMIN" or "STAFF"

public:
    User();
    User(const std::string& username, const std::string& password,
         const std::string& fullName, const std::string& role);

    virtual ~User() = default;

    // Authentication method
    bool checkCredentials(const std::string& inputUser, const std::string& inputPass) const;

    // Pure Virtual Functions (Polymorphism)
    virtual void showDashboard() const = 0;
    virtual bool hasAdminPrivileges() const = 0;
    virtual std::string getRoleTitle() const = 0;

    // Getters
    std::string getUsername() const;
    std::string getFullName() const;
    std::string getRole() const;

    // Setters
    void setFullName(const std::string& name);
    void setPassword(const std::string& newPass);

    // File Serialization
    virtual std::string serialize() const;
};

// ============================================================================
// Derived Class: Admin
// ============================================================================
class Admin : public User {
public:
    Admin();
    Admin(const std::string& username, const std::string& password, const std::string& fullName);

    virtual ~Admin() override = default;

    // Polymorphic overrides
    virtual void showDashboard() const override;
    virtual bool hasAdminPrivileges() const override;
    virtual std::string getRoleTitle() const override;
};

// ============================================================================
// Derived Class: Staff (Cashier / Inventory Associate)
// ============================================================================
class Staff : public User {
private:
    std::string shiftTiming; // e.g., "Morning", "Evening", "Night"

public:
    Staff();
    Staff(const std::string& username, const std::string& password,
          const std::string& fullName, const std::string& shift = "Morning");

    virtual ~Staff() override = default;

    // Polymorphic overrides
    virtual void showDashboard() const override;
    virtual bool hasAdminPrivileges() const override;
    virtual std::string getRoleTitle() const override;

    std::string getShiftTiming() const;
    void setShiftTiming(const std::string& shift);

    virtual std::string serialize() const override;
};

#endif // USER_H
