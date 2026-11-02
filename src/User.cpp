#include "../include/User.h"
#include <iomanip>

// ============================================================================
// User Base Class Implementation
// ============================================================================
User::User()
    : username("guest"), password(""), fullName("Guest User"), role("GUEST") {}

User::User(const std::string& username, const std::string& password,
           const std::string& fullName, const std::string& role)
    : username(username), password(password), fullName(fullName), role(role) {}

bool User::checkCredentials(const std::string& inputUser, const std::string& inputPass) const {
    return (this->username == inputUser && this->password == inputPass);
}

std::string User::getUsername() const { return username; }
std::string User::getFullName() const { return fullName; }
std::string User::getRole() const { return role; }

void User::setFullName(const std::string& name) { this->fullName = name; }
void User::setPassword(const std::string& newPass) { this->password = newPass; }

std::string User::serialize() const {
    return role + ";" + username + ";" + password + ";" + fullName;
}

// ============================================================================
// Admin Derived Class Implementation
// ============================================================================
Admin::Admin() : User("admin", "admin123", "System Administrator", "ADMIN") {}

Admin::Admin(const std::string& username, const std::string& password, const std::string& fullName)
    : User(username, password, fullName, "ADMIN") {}

void Admin::showDashboard() const {
    std::cout << "\n======================================================\n";
    std::cout << "             ADMINISTRATOR CONTROL PANEL              \n";
    std::cout << "======================================================\n";
    std::cout << " Logged in as:   " << fullName << " (" << username << ")\n";
    std::cout << " Role Authority: FULL ACCESS (Product CRUD, Staff, Financials)\n";
    std::cout << "======================================================\n";
}

bool Admin::hasAdminPrivileges() const {
    return true;
}

std::string Admin::getRoleTitle() const {
    return "System Administrator";
}

// ============================================================================
// Staff Derived Class Implementation
// ============================================================================
Staff::Staff()
    : User("staff", "staff123", "Store Cashier", "STAFF"), shiftTiming("Morning") {}

Staff::Staff(const std::string& username, const std::string& password,
             const std::string& fullName, const std::string& shift)
    : User(username, password, fullName, "STAFF"), shiftTiming(shift) {}

void Staff::showDashboard() const {
    std::cout << "\n======================================================\n";
    std::cout << "             STAFF & CASHIER PORTAL                   \n";
    std::cout << "======================================================\n";
    std::cout << " Logged in as:   " << fullName << " (" << username << ")\n";
    std::cout << " Shift Timing:   " << shiftTiming << "\n";
    std::cout << " Role Authority: OPERATIONAL (Billing, Stock Movement, Search)\n";
    std::cout << "======================================================\n";
}

bool Staff::hasAdminPrivileges() const {
    return false;
}

std::string Staff::getRoleTitle() const {
    return "Inventory Staff / Cashier";
}

std::string Staff::getShiftTiming() const { return shiftTiming; }
void Staff::setShiftTiming(const std::string& shift) { this->shiftTiming = shift; }

std::string Staff::serialize() const {
    return role + ";" + username + ";" + password + ";" + fullName + ";" + shiftTiming;
}
