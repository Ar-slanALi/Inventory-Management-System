/**
 * ============================================================================
 * INVENTORY MANAGEMENT SYSTEM (IMS) - 2ND SEMESTER OOP PROJECT
 * ============================================================================
 * Language: C++ (C++11 / C++14 / C++17 Compatible)
 * Paradigm: Object-Oriented Programming (OOP)
 * Author:   Muhammad Suleman (Software Engineering Student)
 *
 * OOP CONCEPTS COVERED (From First Topic to End):
 * 1.  Classes & Objects
 * 2.  Encapsulation & Data Hiding (Private / Protected Members, Getters/Setters)
 * 3.  Constructors (Default, Parameterized with Initializer Lists) & Destructors
 * 4.  Virtual Destructors (Safe cleanup of polymorphic objects)
 * 5.  Static Data Members & Static Member Functions (Product::totalProducts)
 * 6.  Inheritance (Single & Hierarchical):
 *       - User (Abstract Base) -> Admin, Staff
 *       - Product (Abstract Base) -> PerishableProduct, ElectronicsProduct
 * 7.  Polymorphism:
 *       - Compile-time: Function Overloading (searchProduct), Operator Overloading (<<, ==)
 *       - Run-time: Virtual Functions, Pure Virtual Functions, Function Overriding
 * 8.  Abstraction (Abstract Base Classes & Interface Contracts)
 * 9.  Composition (Product HAS-A Supplier)
 * 10. Aggregation (Inventory HAS-A Dynamic Collection of Product*)
 * 11. Friend Class (FinancialManager is friend of Product)
 * 12. File Handling (Persistent streams for Products, Users, Transactions, Logs)
 * 13. Exception Handling (Custom IMSException hierarchy with try-catch blocks)
 * 14. Templates (Generic safe console input validation template)
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <exception>

// ============================================================================
// 1. CUSTOM EXCEPTION CLASSES (OOP CONCEPT: Exception Handling & Inheritance)
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
    explicit InsufficientStockException(const std::string& msg = "Error: Insufficient stock available!")
        : IMSException(msg) {}
};

class DuplicateProductException : public IMSException {
public:
    explicit DuplicateProductException(const std::string& msg = "Error: Product with this ID already exists!")
        : IMSException(msg) {}
};

class ProductNotFoundException : public IMSException {
public:
    explicit ProductNotFoundException(const std::string& msg = "Error: Product not found in inventory!")
        : IMSException(msg) {}
};

class InvalidInputException : public IMSException {
public:
    explicit InvalidInputException(const std::string& msg = "Error: Invalid input value provided!")
        : IMSException(msg) {}
};

// ============================================================================
// 2. TEMPLATES & UTILITIES (OOP CONCEPT: Generic Programming via Templates)
// ============================================================================
namespace Utils {
    template <typename T>
    T getValidatedInput(const std::string& prompt, T minVal, T maxVal) {
        T value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                if (value >= minVal && value <= maxVal) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return value;
                } else {
                    std::cout << "   [!] Input out of valid range (" << minVal << " to " << maxVal << "). Try again.\n";
                }
            } else {
                std::cout << "   [!] Invalid format! Please enter a valid number.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    inline std::string getNonEmptyString(const std::string& prompt) {
        std::string input;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            if (!input.empty()) return input;
            std::cout << "   [!] Input cannot be empty. Please enter text.\n";
        }
    }

    inline std::string getCurrentDateTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm localTm;
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&localTm, &nowTime);
#else
        localtime_r(&nowTime, &localTm);
#endif
        std::ostringstream ss;
        ss << std::put_time(&localTm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    inline void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    inline void pause() {
        std::cout << "\n   Press Enter to continue...";
        std::cin.get();
    }

    inline void printLine(char ch = '=', int len = 65) {
        for (int i = 0; i < len; ++i) std::cout << ch;
        std::cout << "\n";
    }

    inline void printHeader(const std::string& title) {
        printLine('=', 65);
        int padding = (65 - static_cast<int>(title.length())) / 2;
        if (padding < 0) padding = 0;
        for (int i = 0; i < padding; ++i) std::cout << " ";
        std::cout << title << "\n";
        printLine('=', 65);
    }
}

// ============================================================================
// 3. SUPPLIER CLASS (OOP CONCEPT: Composition - Product HAS-A Supplier)
// ============================================================================
class Supplier {
private:
    int id;
    std::string name;
    std::string contactNumber;
    std::string email;

public:
    Supplier() : id(0), name("N/A"), contactNumber("N/A"), email("N/A") {}
    Supplier(int id, const std::string& name, const std::string& contact, const std::string& email)
        : id(id), name(name), contactNumber(contact), email(email) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getContactNumber() const { return contactNumber; }
    std::string getEmail() const { return email; }

    void setId(int id) { this->id = id; }
    void setName(const std::string& name) { this->name = name; }
    void setContactNumber(const std::string& contact) { this->contactNumber = contact; }
    void setEmail(const std::string& email) { this->email = email; }

    std::string serialize() const {
        return std::to_string(id) + "|" + name + "|" + contactNumber + "|" + email;
    }

    void deserialize(const std::string& data) {
        if (data.empty()) return;
        std::stringstream ss(data);
        std::string token;
        if (std::getline(ss, token, '|')) id = std::stoi(token);
        if (std::getline(ss, token, '|')) name = token;
        if (std::getline(ss, token, '|')) contactNumber = token;
        if (std::getline(ss, token, '|')) email = token;
    }

    friend std::ostream& operator<<(std::ostream& os, const Supplier& s) {
        os << s.name << " (Phone: " << s.contactNumber << ", Email: " << s.email << ")";
        return os;
    }
};

// ============================================================================
// 4. PRODUCT HIERARCHY (OOP CONCEPTS: Abstraction, Inheritance, Polymorphism)
// ============================================================================
class FinancialManager; // Forward declaration for Friend class

class Product {
protected:
    int id;
    std::string name;
    std::string category;
    int quantity;
    double costPrice;
    double sellingPrice;
    int reorderThreshold;
    Supplier supplier; // Composition

    static int totalProducts; // Static member

public:
    friend class FinancialManager; // Friend Class

    Product() : id(0), name("Unnamed"), category("General"), quantity(0),
                costPrice(0.0), sellingPrice(0.0), reorderThreshold(5), supplier() {
        totalProducts++;
    }

    Product(int id, const std::string& name, const std::string& category,
            int quantity, double costPrice, double sellingPrice,
            int reorderThreshold, const Supplier& supplier)
        : id(id), name(name), category(category), quantity(quantity),
          costPrice(costPrice), sellingPrice(sellingPrice),
          reorderThreshold(reorderThreshold), supplier(supplier) {
        totalProducts++;
    }

    virtual ~Product() {
        if (totalProducts > 0) totalProducts--;
    }

    static int getTotalProducts() { return totalProducts; }

    // Pure Virtual Functions (Abstract Class Interface)
    virtual void displayDetails() const = 0;
    virtual std::string getProductType() const = 0;
    virtual std::string serialize() const = 0;

    // Virtual Function
    virtual double calculateDiscountedPrice(double discountPercent) const {
        if (discountPercent < 0.0 || discountPercent > 100.0) return sellingPrice;
        return sellingPrice - (sellingPrice * (discountPercent / 100.0));
    }

    double calculateStockValue() const { return quantity * sellingPrice; }
    double calculateCostValue() const { return quantity * costPrice; }
    bool isLowStock() const { return quantity <= reorderThreshold; }
    void addStock(int amount) { if (amount > 0) quantity += amount; }
    void deductStock(int amount) { if (amount > 0 && quantity >= amount) quantity -= amount; }

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getCategory() const { return category; }
    int getQuantity() const { return quantity; }
    double getCostPrice() const { return costPrice; }
    double getSellingPrice() const { return sellingPrice; }
    int getReorderThreshold() const { return reorderThreshold; }
    Supplier getSupplier() const { return supplier; }

    void setId(int id) { this->id = id; }
    void setName(const std::string& name) { this->name = name; }
    void setCategory(const std::string& cat) { this->category = cat; }
    void setQuantity(int q) { this->quantity = (q >= 0) ? q : 0; }
    void setCostPrice(double cp) { this->costPrice = (cp >= 0.0) ? cp : 0.0; }
    void setSellingPrice(double sp) { this->sellingPrice = (sp >= 0.0) ? sp : 0.0; }
    void setReorderThreshold(int t) { this->reorderThreshold = (t >= 0) ? t : 0; }
    void setSupplier(const Supplier& s) { this->supplier = s; }

    bool operator==(const Product& other) const { return this->id == other.id; }
    friend std::ostream& operator<<(std::ostream& os, const Product& p) {
        os << "[" << p.id << "] " << p.name << " | Stock: " << p.quantity << " | Price: $" << std::fixed << std::setprecision(2) << p.sellingPrice;
        return os;
    }
};

int Product::totalProducts = 0;

// Derived Class 1: PerishableProduct (Hierarchical Inheritance)
class PerishableProduct : public Product {
private:
    std::string expiryDate;
    double storageTempCelsius;

public:
    PerishableProduct() : Product(), expiryDate("2026-12-31"), storageTempCelsius(4.0) {}

    PerishableProduct(int id, const std::string& name, const std::string& category,
                      int quantity, double costPrice, double sellingPrice,
                      int reorderThreshold, const Supplier& supplier,
                      const std::string& expiryDate, double storageTemp)
        : Product(id, name, category, quantity, costPrice, sellingPrice, reorderThreshold, supplier),
          expiryDate(expiryDate), storageTempCelsius(storageTemp) {}

    virtual ~PerishableProduct() override = default;

    virtual void displayDetails() const override {
        std::cout << "\n+-------------------------------------------------------------+\n";
        std::cout << "| [PERISHABLE PRODUCT] ID: " << std::setw(6) << std::left << id 
                  << " Name: " << std::setw(28) << std::left << name << " |\n";
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "  Category:            " << category << "\n";
        std::cout << "  Current Stock:       " << quantity << " units " << (isLowStock() ? "(LOW STOCK ALERT!)" : "") << "\n";
        std::cout << "  Reorder Level:       " << reorderThreshold << " units\n";
        std::cout << "  Cost Price:          $" << std::fixed << std::setprecision(2) << costPrice << "\n";
        std::cout << "  Selling Price:       $" << std::fixed << std::setprecision(2) << sellingPrice << "\n";
        std::cout << "  Expiry Date:         " << expiryDate << "\n";
        std::cout << "  Storage Temperature: " << storageTempCelsius << " °C\n";
        std::cout << "  Supplier:            " << supplier << "\n";
        std::cout << "+-------------------------------------------------------------+\n";
    }

    virtual std::string getProductType() const override { return "PERISHABLE"; }

    virtual std::string serialize() const override {
        std::ostringstream ss;
        ss << "PERISHABLE;" << id << ";" << name << ";" << category << ";"
           << quantity << ";" << costPrice << ";" << sellingPrice << ";"
           << reorderThreshold << ";" << supplier.serialize() << ";"
           << expiryDate << ";" << storageTempCelsius;
        return ss.str();
    }

    virtual double calculateDiscountedPrice(double discountPercent) const override {
        double totalDiscount = discountPercent + 5.0;
        if (totalDiscount > 50.0) totalDiscount = 50.0;
        return sellingPrice - (sellingPrice * (totalDiscount / 100.0));
    }
};

// Derived Class 2: ElectronicsProduct (Hierarchical Inheritance)
class ElectronicsProduct : public Product {
private:
    int warrantyMonths;
    std::string brandName;

public:
    ElectronicsProduct() : Product(), warrantyMonths(12), brandName("Generic") {}

    ElectronicsProduct(int id, const std::string& name, const std::string& category,
                       int quantity, double costPrice, double sellingPrice,
                       int reorderThreshold, const Supplier& supplier,
                       int warrantyMonths, const std::string& brandName)
        : Product(id, name, category, quantity, costPrice, sellingPrice, reorderThreshold, supplier),
          warrantyMonths(warrantyMonths), brandName(brandName) {}

    virtual ~ElectronicsProduct() override = default;

    virtual void displayDetails() const override {
        std::cout << "\n+-------------------------------------------------------------+\n";
        std::cout << "| [ELECTRONICS PRODUCT] ID: " << std::setw(6) << std::left << id 
                  << " Name: " << std::setw(26) << std::left << name << " |\n";
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "  Brand:               " << brandName << "\n";
        std::cout << "  Category:            " << category << "\n";
        std::cout << "  Current Stock:       " << quantity << " units " << (isLowStock() ? "(LOW STOCK ALERT!)" : "") << "\n";
        std::cout << "  Reorder Level:       " << reorderThreshold << " units\n";
        std::cout << "  Cost Price:          $" << std::fixed << std::setprecision(2) << costPrice << "\n";
        std::cout << "  Selling Price:       $" << std::fixed << std::setprecision(2) << sellingPrice << "\n";
        std::cout << "  Warranty:            " << warrantyMonths << " Months Manufacturer Warranty\n";
        std::cout << "  Supplier:            " << supplier << "\n";
        std::cout << "+-------------------------------------------------------------+\n";
    }

    virtual std::string getProductType() const override { return "ELECTRONICS"; }

    virtual std::string serialize() const override {
        std::ostringstream ss;
        ss << "ELECTRONICS;" << id << ";" << name << ";" << category << ";"
           << quantity << ";" << costPrice << ";" << sellingPrice << ";"
           << reorderThreshold << ";" << supplier.serialize() << ";"
           << warrantyMonths << ";" << brandName;
        return ss.str();
    }

    virtual double calculateDiscountedPrice(double discountPercent) const override {
        if (discountPercent > 30.0) discountPercent = 30.0;
        return sellingPrice - (sellingPrice * (discountPercent / 100.0));
    }
};

// ============================================================================
// 5. TRANSACTION RECORD (OOP CONCEPT: Encapsulation & Operator Overloading)
// ============================================================================
class Transaction {
private:
    int transactionId;
    int productId;
    std::string productName;
    std::string type;
    int quantity;
    double unitPrice;
    double totalAmount;
    std::string performedBy;
    std::string timestamp;

    static int nextTransactionId;

public:
    Transaction() : transactionId(0), productId(0), productName("Unknown"), type("NONE"),
                    quantity(0), unitPrice(0.0), totalAmount(0.0), performedBy("system"), timestamp("N/A") {}

    Transaction(int prodId, const std::string& prodName, const std::string& type,
                int qty, double unitPrice, const std::string& performedBy, const std::string& timestamp)
        : transactionId(nextTransactionId++), productId(prodId), productName(prodName),
          type(type), quantity(qty), unitPrice(unitPrice), totalAmount(qty * unitPrice),
          performedBy(performedBy), timestamp(timestamp) {}

    static void setNextTransactionId(int id) { if (id > nextTransactionId) nextTransactionId = id; }

    void printReceipt() const {
        std::cout << "\n======================================================\n";
        std::cout << "           INVENTORY MANAGEMENT SYSTEM                \n";
        std::cout << "                 OFFICIAL RECEIPT                     \n";
        std::cout << "======================================================\n";
        std::cout << " Invoice No:     INV-" << transactionId << "\n";
        std::cout << " Date & Time:    " << timestamp << "\n";
        std::cout << " Cashier/Staff:  " << performedBy << "\n";
        std::cout << " Transaction:    " << type << "\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Product ID:     " << productId << "\n";
        std::cout << " Description:    " << productName << "\n";
        std::cout << " Quantity:       " << quantity << " units\n";
        std::cout << " Unit Price:     $" << std::fixed << std::setprecision(2) << unitPrice << "\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " TOTAL AMOUNT:   $" << std::fixed << std::setprecision(2) << totalAmount << "\n";
        std::cout << "======================================================\n";
        std::cout << "          Thank you for your business!                \n";
        std::cout << "======================================================\n";
    }

    void displaySummary() const {
        std::cout << "| " << std::setw(6) << transactionId
                  << " | " << std::setw(8) << productId
                  << " | " << std::setw(20) << std::left << (productName.length() > 20 ? productName.substr(0, 17) + "..." : productName)
                  << " | " << std::setw(10) << std::left << type
                  << " | " << std::setw(5) << std::right << quantity
                  << " | $" << std::setw(9) << std::fixed << std::setprecision(2) << totalAmount
                  << " | " << std::setw(10) << std::left << performedBy
                  << " | " << std::setw(19) << timestamp << " |\n";
    }

    std::string serialize() const {
        std::ostringstream ss;
        ss << transactionId << ";" << productId << ";" << productName << ";"
           << type << ";" << quantity << ";" << unitPrice << ";"
           << totalAmount << ";" << performedBy << ";" << timestamp;
        return ss.str();
    }

    static Transaction deserialize(const std::string& line) {
        Transaction t;
        if (line.empty()) return t;
        std::stringstream ss(line);
        std::string token;
        if (std::getline(ss, token, ';')) t.transactionId = std::stoi(token);
        if (std::getline(ss, token, ';')) t.productId = std::stoi(token);
        if (std::getline(ss, token, ';')) t.productName = token;
        if (std::getline(ss, token, ';')) t.type = token;
        if (std::getline(ss, token, ';')) t.quantity = std::stoi(token);
        if (std::getline(ss, token, ';')) t.unitPrice = std::stod(token);
        if (std::getline(ss, token, ';')) t.totalAmount = std::stod(token);
        if (std::getline(ss, token, ';')) t.performedBy = token;
        if (std::getline(ss, token, ';')) t.timestamp = token;
        if (t.transactionId >= nextTransactionId) nextTransactionId = t.transactionId + 1;
        return t;
    }
};

int Transaction::nextTransactionId = 1001;

// ============================================================================
// 6. FINANCIAL MANAGER (OOP CONCEPT: Friend Class & Domain Analytics)
// ============================================================================
class FinancialManager {
private:
    double totalRevenue;
    double totalCostOfGoodsSold;
    double totalProcurementCost;
    int totalItemsSold;

public:
    FinancialManager() : totalRevenue(0.0), totalCostOfGoodsSold(0.0), totalProcurementCost(0.0), totalItemsSold(0) {}
    FinancialManager(double rev, double cogs, double proc, int sold)
        : totalRevenue(rev), totalCostOfGoodsSold(cogs), totalProcurementCost(proc), totalItemsSold(sold) {}

    // Friend access to Product internals
    void recordSale(const Product& product, int quantitySold) {
        if (quantitySold <= 0) return;
        totalRevenue += (quantitySold * product.sellingPrice);
        totalCostOfGoodsSold += (quantitySold * product.costPrice);
        totalItemsSold += quantitySold;
    }

    void recordStockIn(const Product& product, int quantityAdded) {
        if (quantityAdded <= 0) return;
        totalProcurementCost += (quantityAdded * product.costPrice);
    }

    double getGrossProfit() const { return totalRevenue - totalCostOfGoodsSold; }
    double getProfitMarginPercentage() const {
        if (totalRevenue <= 0.0) return 0.0;
        return (getGrossProfit() / totalRevenue) * 100.0;
    }

    void displayFinancialReport(const std::vector<Product*>& activeInventory) const {
        double costVal = 0.0, retailVal = 0.0;
        int totalUnits = 0;
        for (const auto* p : activeInventory) {
            if (p) {
                costVal += p->calculateCostValue();
                retailVal += p->calculateStockValue();
                totalUnits += p->getQuantity();
            }
        }
        std::cout << "\n===============================================================\n";
        std::cout << "                 EXECUTIVE FINANCIAL REPORT                    \n";
        std::cout << "===============================================================\n";
        std::cout << " 1. SALES PERFORMANCE METRICS:\n";
        std::cout << "    - Total Units Sold:              " << totalItemsSold << " units\n";
        std::cout << "    - Gross Revenue Generated:       $" << std::fixed << std::setprecision(2) << totalRevenue << "\n";
        std::cout << "    - Cost of Goods Sold (COGS):     $" << std::fixed << std::setprecision(2) << totalCostOfGoodsSold << "\n";
        std::cout << "    - Net Gross Profit:              $" << std::fixed << std::setprecision(2) << getGrossProfit() << "\n";
        std::cout << "    - Net Profit Margin:             " << std::fixed << std::setprecision(2) << getProfitMarginPercentage() << " %\n";
        std::cout << "---------------------------------------------------------------\n";
        std::cout << " 2. CURRENT INVENTORY VALUATION:\n";
        std::cout << "    - Active Distinct Products:      " << activeInventory.size() << "\n";
        std::cout << "    - Total Units in Stock:          " << totalUnits << " units\n";
        std::cout << "    - Inventory Value (At Cost):     $" << std::fixed << std::setprecision(2) << costVal << "\n";
        std::cout << "    - Inventory Value (At Retail):   $" << std::fixed << std::setprecision(2) << retailVal << "\n";
        std::cout << "    - Projected Potential Profit:    $" << std::fixed << std::setprecision(2) << (retailVal - costVal) << "\n";
        std::cout << "---------------------------------------------------------------\n";
        std::cout << " 3. PROCUREMENT & EXPENSES:\n";
        std::cout << "    - Cumulative Procurement Spent:  $" << std::fixed << std::setprecision(2) << totalProcurementCost << "\n";
        std::cout << "===============================================================\n";
    }

    std::string serialize() const {
        std::ostringstream ss;
        ss << totalRevenue << ";" << totalCostOfGoodsSold << ";" << totalProcurementCost << ";" << totalItemsSold;
        return ss.str();
    }

    void deserialize(const std::string& line) {
        if (line.empty()) return;
        std::stringstream ss(line);
        std::string token;
        if (std::getline(ss, token, ';')) totalRevenue = std::stod(token);
        if (std::getline(ss, token, ';')) totalCostOfGoodsSold = std::stod(token);
        if (std::getline(ss, token, ';')) totalProcurementCost = std::stod(token);
        if (std::getline(ss, token, ';')) totalItemsSold = std::stoi(token);
    }
};

// ============================================================================
// 7. USER HIERARCHY (OOP CONCEPTS: Abstract Base Class, Inheritance, RBAC)
// ============================================================================
class User {
protected:
    std::string username;
    std::string password;
    std::string fullName;
    std::string role;

public:
    User() : username("guest"), password(""), fullName("Guest"), role("GUEST") {}
    User(const std::string& u, const std::string& p, const std::string& f, const std::string& r)
        : username(u), password(p), fullName(f), role(r) {}
    virtual ~User() = default;

    bool checkCredentials(const std::string& u, const std::string& p) const {
        return (this->username == u && this->password == p);
    }

    virtual void showDashboard() const = 0;
    virtual bool hasAdminPrivileges() const = 0;
    virtual std::string getRoleTitle() const = 0;

    std::string getUsername() const { return username; }
    std::string getFullName() const { return fullName; }
    std::string getRole() const { return role; }

    virtual std::string serialize() const {
        return role + ";" + username + ";" + password + ";" + fullName;
    }
};

class Admin : public User {
public:
    Admin() : User("admin", "admin123", "Muhammad Suleman (Admin)", "ADMIN") {}
    Admin(const std::string& u, const std::string& p, const std::string& f) : User(u, p, f, "ADMIN") {}

    virtual void showDashboard() const override {
        std::cout << "\n======================================================\n";
        std::cout << "             ADMINISTRATOR CONTROL PANEL              \n";
        std::cout << "======================================================\n";
        std::cout << " Logged in as:   " << fullName << " (" << username << ")\n";
        std::cout << " Role Authority: FULL ACCESS (Product CRUD, Staff, Financials)\n";
        std::cout << "======================================================\n";
    }

    virtual bool hasAdminPrivileges() const override { return true; }
    virtual std::string getRoleTitle() const override { return "System Administrator"; }
};

class Staff : public User {
private:
    std::string shiftTiming;

public:
    Staff() : User("staff", "staff123", "Store Cashier Associate", "STAFF"), shiftTiming("Morning") {}
    Staff(const std::string& u, const std::string& p, const std::string& f, const std::string& s = "Morning")
        : User(u, p, f, "STAFF"), shiftTiming(s) {}

    virtual void showDashboard() const override {
        std::cout << "\n======================================================\n";
        std::cout << "             STAFF & CASHIER PORTAL                   \n";
        std::cout << "======================================================\n";
        std::cout << " Logged in as:   " << fullName << " (" << username << ")\n";
        std::cout << " Shift Timing:   " << shiftTiming << "\n";
        std::cout << " Role Authority: OPERATIONAL (Billing, Stock Movement, Search)\n";
        std::cout << "======================================================\n";
    }

    virtual bool hasAdminPrivileges() const override { return false; }
    virtual std::string getRoleTitle() const override { return "Inventory Staff / Cashier"; }

    virtual std::string serialize() const override {
        return role + ";" + username + ";" + password + ";" + fullName + ";" + shiftTiming;
    }
};

// ============================================================================
// 8. INVENTORY CLASS (OOP CONCEPTS: Aggregation, Function Overloading)
// ============================================================================
class Inventory {
private:
    std::vector<Product*> products;
    std::vector<Transaction> transactions;
    FinancialManager financialManager;

public:
    Inventory() {}
    ~Inventory() { clearAll(); }

    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) = delete;

    void clearAll() {
        for (Product* p : products) delete p;
        products.clear();
    }

    void addProduct(Product* product) {
        if (!product) return;
        for (const Product* p : products) {
            if (p && p->getId() == product->getId()) {
                throw DuplicateProductException("Product with ID " + std::to_string(product->getId()) + " already exists!");
            }
        }
        products.push_back(product);
    }

    bool updateProduct(int id) {
        Product* p = searchProduct(id);
        if (!p) throw ProductNotFoundException("Product ID " + std::to_string(id) + " not found!");

        p->displayDetails();
        std::cout << "\n   Enter new name (leave blank to keep): ";
        std::string newName;
        std::getline(std::cin, newName);
        if (!newName.empty()) p->setName(newName);

        std::cout << "   Enter new category (leave blank to keep): ";
        std::string newCat;
        std::getline(std::cin, newCat);
        if (!newCat.empty()) p->setCategory(newCat);

        std::cout << "   Update Cost Price? (1 = Yes, 0 = Keep $" << p->getCostPrice() << "): ";
        int choice = Utils::getValidatedInput<int>("", 0, 1);
        if (choice == 1) p->setCostPrice(Utils::getValidatedInput<double>("   Enter new cost ($): ", 0.01, 100000.0));

        std::cout << "   Update Selling Price? (1 = Yes, 0 = Keep $" << p->getSellingPrice() << "): ";
        choice = Utils::getValidatedInput<int>("", 0, 1);
        if (choice == 1) p->setSellingPrice(Utils::getValidatedInput<double>("   Enter new price ($): ", 0.01, 100000.0));

        std::cout << "   Update Reorder Level? (1 = Yes, 0 = Keep " << p->getReorderThreshold() << "): ";
        choice = Utils::getValidatedInput<int>("", 0, 1);
        if (choice == 1) p->setReorderThreshold(Utils::getValidatedInput<int>("   Enter new threshold: ", 1, 1000));

        return true;
    }

    bool deleteProduct(int id) {
        for (auto it = products.begin(); it != products.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                products.erase(it);
                return true;
            }
        }
        throw ProductNotFoundException("Cannot delete: Product ID " + std::to_string(id) + " not found.");
    }

    void stockIn(int id, int qty, const std::string& username) {
        if (qty <= 0) throw InvalidInputException("Stock-in quantity must be positive.");
        Product* p = searchProduct(id);
        if (!p) throw ProductNotFoundException("Product ID " + std::to_string(id) + " not found!");

        p->addStock(qty);
        financialManager.recordStockIn(*p, qty);
        transactions.push_back(Transaction(p->getId(), p->getName(), "STOCK_IN", qty, p->getCostPrice(), username, Utils::getCurrentDateTime()));
    }

    Transaction stockOut(int id, int qty, const std::string& username, bool printInvoice = true) {
        if (qty <= 0) throw InvalidInputException("Sale quantity must be positive.");
        Product* p = searchProduct(id);
        if (!p) throw ProductNotFoundException("Product ID " + std::to_string(id) + " not found!");
        if (p->getQuantity() < qty) {
            throw InsufficientStockException("Insufficient stock! Available: " + std::to_string(p->getQuantity()) + ", Requested: " + std::to_string(qty));
        }

        p->deductStock(qty);
        financialManager.recordSale(*p, qty);
        Transaction tx(p->getId(), p->getName(), "SALE", qty, p->getSellingPrice(), username, Utils::getCurrentDateTime());
        transactions.push_back(tx);
        if (printInvoice) tx.printReceipt();
        return tx;
    }

    // FUNCTION OVERLOADING 1: by ID
    Product* searchProduct(int id) const {
        for (Product* p : products) {
            if (p && p->getId() == id) return p;
        }
        return nullptr;
    }

    // FUNCTION OVERLOADING 2: by Name / Category
    std::vector<Product*> searchProduct(const std::string& query) const {
        std::vector<Product*> results;
        std::string q = query;
        std::transform(q.begin(), q.end(), q.begin(), ::tolower);
        for (Product* p : products) {
            if (!p) continue;
            std::string n = p->getName(), c = p->getCategory();
            std::transform(n.begin(), n.end(), n.begin(), ::tolower);
            std::transform(c.begin(), c.end(), c.begin(), ::tolower);
            if (n.find(q) != std::string::npos || c.find(q) != std::string::npos) results.push_back(p);
        }
        return results;
    }

    // FUNCTION OVERLOADING 3: by Price Range
    std::vector<Product*> searchProduct(double minPrice, double maxPrice) const {
        std::vector<Product*> results;
        for (Product* p : products) {
            if (p && p->getSellingPrice() >= minPrice && p->getSellingPrice() <= maxPrice) results.push_back(p);
        }
        return results;
    }

    void displayAllProducts() const {
        if (products.empty()) {
            std::cout << "\n   [i] No products currently in inventory.\n";
            return;
        }
        std::cout << "\n=========================================================================================================\n";
        std::cout << "| " << std::setw(6) << "ID"
                  << " | " << std::setw(22) << std::left << "Product Name"
                  << " | " << std::setw(14) << std::left << "Category"
                  << " | " << std::setw(12) << std::left << "Type"
                  << " | " << std::setw(6) << std::right << "Stock"
                  << " | " << std::setw(10) << std::right << "Cost ($)"
                  << " | " << std::setw(10) << std::right << "Price ($)"
                  << " | " << std::setw(10) << "Status" << " |\n";
        std::cout << "=========================================================================================================\n";
        for (const Product* p : products) {
            if (!p) continue;
            std::string status = p->isLowStock() ? "LOW STOCK" : "OK";
            std::cout << "| " << std::setw(6) << p->getId()
                      << " | " << std::setw(22) << std::left << (p->getName().length() > 22 ? p->getName().substr(0, 19) + "..." : p->getName())
                      << " | " << std::setw(14) << std::left << (p->getCategory().length() > 14 ? p->getCategory().substr(0, 11) + "..." : p->getCategory())
                      << " | " << std::setw(12) << std::left << p->getProductType()
                      << " | " << std::setw(6) << std::right << p->getQuantity()
                      << " | " << std::setw(10) << std::fixed << std::setprecision(2) << p->getCostPrice()
                      << " | " << std::setw(10) << std::fixed << std::setprecision(2) << p->getSellingPrice()
                      << " | " << std::setw(10) << status << " |\n";
        }
        std::cout << "=========================================================================================================\n";
        std::cout << " Total Distinct Products: " << products.size() << " | Active Count in Memory: " << Product::getTotalProducts() << "\n";
    }

    void displayLowStockProducts() const {
        int count = 0;
        std::cout << "\n================================================================================\n";
        std::cout << "                       CRITICAL LOW STOCK ALERT REPORT                          \n";
        std::cout << "================================================================================\n";
        std::cout << "| " << std::setw(6) << "ID"
                  << " | " << std::setw(25) << std::left << "Product Name"
                  << " | " << std::setw(8) << std::right << "Current"
                  << " | " << std::setw(10) << std::right << "Threshold"
                  << " | " << std::setw(20) << std::left << "Supplier Contact" << " |\n";
        std::cout << "--------------------------------------------------------------------------------\n";
        for (const Product* p : products) {
            if (p && p->isLowStock()) {
                count++;
                std::cout << "| " << std::setw(6) << p->getId()
                          << " | " << std::setw(25) << std::left << p->getName()
                          << " | " << std::setw(8) << std::right << p->getQuantity()
                          << " | " << std::setw(10) << std::right << p->getReorderThreshold()
                          << " | " << std::setw(20) << std::left << p->getSupplier().getContactNumber() << " |\n";
            }
        }
        if (count == 0) {
            std::cout << "   All inventory items are currently above their reorder thresholds. Excellent!\n";
        } else {
            std::cout << "--------------------------------------------------------------------------------\n";
            std::cout << "   Attention: " << count << " item(s) require immediate reordering!\n";
        }
        std::cout << "================================================================================\n";
    }

    int getLowStockCount() const {
        int count = 0;
        for (const Product* p : products) if (p && p->isLowStock()) count++;
        return count;
    }

    void sortProducts(int criterion) {
        if (products.size() < 2) return;
        switch (criterion) {
            case 1: std::sort(products.begin(), products.end(), [](const Product* a, const Product* b) { return a->getId() < b->getId(); }); break;
            case 2: std::sort(products.begin(), products.end(), [](const Product* a, const Product* b) { return a->getName() < b->getName(); }); break;
            case 3: std::sort(products.begin(), products.end(), [](const Product* a, const Product* b) { return a->getQuantity() < b->getQuantity(); }); break;
            case 4: std::sort(products.begin(), products.end(), [](const Product* a, const Product* b) { return a->getSellingPrice() > b->getSellingPrice(); }); break;
        }
    }

    void displayTransactions() const {
        if (transactions.empty()) {
            std::cout << "\n   [i] No transactions recorded yet.\n";
            return;
        }
        std::cout << "\n====================================================================================================================\n";
        std::cout << "                                           AUDIT TRANSACTION LOG                                                    \n";
        std::cout << "====================================================================================================================\n";
        std::cout << "| " << std::setw(6) << "Txn ID"
                  << " | " << std::setw(8) << "Prod ID"
                  << " | " << std::setw(20) << std::left << "Product Name"
                  << " | " << std::setw(10) << std::left << "Type"
                  << " | " << std::setw(5) << std::right << "Qty"
                  << " | " << std::setw(10) << std::right << "Total ($)"
                  << " | " << std::setw(10) << std::left << "Staff/User"
                  << " | " << std::setw(19) << "Timestamp" << " |\n";
        std::cout << "--------------------------------------------------------------------------------------------------------------------\n";
        for (const auto& tx : transactions) tx.displaySummary();
        std::cout << "====================================================================================================================\n";
    }

    void displayFinancialSummary() const { financialManager.displayFinancialReport(products); }
    const std::vector<Product*>& getProducts() const { return products; }
    const std::vector<Transaction>& getTransactions() const { return transactions; }
    FinancialManager& getFinancialManager() { return financialManager; }
    void addTransaction(const Transaction& tx) { transactions.push_back(tx); }
    void setFinancialManager(const FinancialManager& fm) { this->financialManager = fm; }
};

// ============================================================================
// 9. FILE PERSISTENCE HANDLER (OOP CONCEPT: File Handling & Streams)
// ============================================================================
class FileHandler {
private:
    std::string productsFilePath;
    std::string usersFilePath;
    std::string transactionsFilePath;
    std::string financialsFilePath;
    std::string logsFilePath;

public:
    FileHandler(const std::string& p = "data/products.txt",
                const std::string& u = "data/users.txt",
                const std::string& t = "data/transactions.txt",
                const std::string& f = "data/financials.txt",
                const std::string& l = "data/logs.txt")
        : productsFilePath(p), usersFilePath(u), transactionsFilePath(t), financialsFilePath(f), logsFilePath(l) {}

    void saveProducts(const std::vector<Product*>& products) {
        std::ofstream outFile(productsFilePath);
        if (!outFile.is_open()) return;
        for (const Product* p : products) if (p) outFile << p->serialize() << "\n";
    }

    void loadProducts(Inventory& inventory) {
        std::ifstream inFile(productsFilePath);
        if (!inFile.is_open()) return;
        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string type;
            if (!std::getline(ss, type, ';')) continue;
            try {
                if (type == "PERISHABLE") {
                    std::string id, name, cat, qty, cost, price, thresh, supp, exp, temp;
                    std::getline(ss, id, ';'); std::getline(ss, name, ';'); std::getline(ss, cat, ';');
                    std::getline(ss, qty, ';'); std::getline(ss, cost, ';'); std::getline(ss, price, ';');
                    std::getline(ss, thresh, ';'); std::getline(ss, supp, ';'); std::getline(ss, exp, ';'); std::getline(ss, temp, ';');
                    Supplier s; s.deserialize(supp);
                    inventory.addProduct(new PerishableProduct(std::stoi(id), name, cat, std::stoi(qty), std::stod(cost), std::stod(price), std::stoi(thresh), s, exp, std::stod(temp)));
                } else if (type == "ELECTRONICS") {
                    std::string id, name, cat, qty, cost, price, thresh, supp, war, brand;
                    std::getline(ss, id, ';'); std::getline(ss, name, ';'); std::getline(ss, cat, ';');
                    std::getline(ss, qty, ';'); std::getline(ss, cost, ';'); std::getline(ss, price, ';');
                    std::getline(ss, thresh, ';'); std::getline(ss, supp, ';'); std::getline(ss, war, ';'); std::getline(ss, brand, ';');
                    Supplier s; s.deserialize(supp);
                    inventory.addProduct(new ElectronicsProduct(std::stoi(id), name, cat, std::stoi(qty), std::stod(cost), std::stod(price), std::stoi(thresh), s, std::stoi(war), brand));
                }
            } catch (...) {}
        }
    }

    void saveUsers(const std::vector<User*>& users) {
        std::ofstream outFile(usersFilePath);
        if (!outFile.is_open()) return;
        for (const User* u : users) if (u) outFile << u->serialize() << "\n";
    }

    std::vector<User*> loadUsers() {
        std::vector<User*> users;
        std::ifstream inFile(usersFilePath);
        if (!inFile.is_open()) return users;
        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string role, u, p, name;
            std::getline(ss, role, ';'); std::getline(ss, u, ';'); std::getline(ss, p, ';'); std::getline(ss, name, ';');
            if (role == "ADMIN") users.push_back(new Admin(u, p, name));
            else if (role == "STAFF") {
                std::string shift;
                std::getline(ss, shift, ';');
                if (shift.empty()) shift = "Morning";
                users.push_back(new Staff(u, p, name, shift));
            }
        }
        return users;
    }

    void saveTransactions(const std::vector<Transaction>& txs) {
        std::ofstream outFile(transactionsFilePath);
        if (!outFile.is_open()) return;
        for (const auto& tx : txs) outFile << tx.serialize() << "\n";
    }

    void loadTransactions(Inventory& inventory) {
        std::ifstream inFile(transactionsFilePath);
        if (!inFile.is_open()) return;
        std::string line;
        while (std::getline(inFile, line)) {
            if (!line.empty()) inventory.addTransaction(Transaction::deserialize(line));
        }
    }

    void saveFinancials(const FinancialManager& fm) {
        std::ofstream outFile(financialsFilePath);
        if (!outFile.is_open()) return;
        outFile << fm.serialize() << "\n";
    }

    void loadFinancials(Inventory& inventory) {
        std::ifstream inFile(financialsFilePath);
        if (!inFile.is_open()) return;
        std::string line;
        if (std::getline(inFile, line) && !line.empty()) {
            FinancialManager fm;
            fm.deserialize(line);
            inventory.setFinancialManager(fm);
        }
    }

    void logActivity(const std::string& user, const std::string& action) {
        std::ofstream outFile(logsFilePath, std::ios::app);
        if (outFile.is_open()) {
            outFile << "[" << Utils::getCurrentDateTime() << "] " << "User: " << user << " | Action: " << action << "\n";
        }
    }

    void seedInitialDataIfEmpty(Inventory& inventory, std::vector<User*>& users) {
        if (users.empty()) {
            users.push_back(new Admin("admin", "admin123", "Muhammad Suleman (Admin)"));
            users.push_back(new Staff("staff", "staff123", "Store Cashier Associate", "Morning"));
            saveUsers(users);
        }
        if (inventory.getProducts().empty()) {
            Supplier sFood(101, "Nestle Distribution", "+92-300-1122334", "orders@nestle.com");
            Supplier sTech(102, "Dell Technologies Ltd", "+92-321-9988776", "b2b@dell.com");
            Supplier sPharma(103, "Getz Pharma International", "+92-333-5566778", "supply@getz.com");

            inventory.addProduct(new PerishableProduct(101, "Organic Whole Milk 1L", "Dairy", 45, 1.80, 2.50, 10, sFood, "2026-11-20", 4.0));
            inventory.addProduct(new PerishableProduct(102, "Amoxicillin 500mg (10 Tab)", "Pharmacy", 15, 6.00, 9.50, 5, sPharma, "2027-05-15", 22.0));
            inventory.addProduct(new PerishableProduct(103, "Fresh Farm Eggs (Dozen)", "Dairy", 8, 2.00, 3.20, 10, sFood, "2026-11-15", 8.0)); // Low Stock

            inventory.addProduct(new ElectronicsProduct(201, "Dell Wireless Mouse WM126", "Peripherals", 25, 12.00, 19.99, 5, sTech, 12, "Dell"));
            inventory.addProduct(new ElectronicsProduct(202, "Logitech HD Webcam C920", "Accessories", 4, 45.00, 69.99, 5, sTech, 24, "Logitech")); // Low Stock
            inventory.addProduct(new ElectronicsProduct(203, "Kingston 512GB NVMe SSD", "Storage", 18, 38.00, 54.99, 6, sTech, 36, "Kingston"));

            saveProducts(inventory.getProducts());
        }
    }
};

// ============================================================================
// 10. MENUS & SYSTEM CONTROLLER
// ============================================================================
void runAdminMenu(Inventory& inventory, FileHandler& fileHandler, User* currentUser, std::vector<User*>& users);
void runStaffMenu(Inventory& inventory, FileHandler& fileHandler, User* currentUser);
User* loginUser(const std::vector<User*>& users, FileHandler& fileHandler);
void handleAddProduct(Inventory& inventory, FileHandler& fileHandler, const std::string& username);
void handleSearchProduct(const Inventory& inventory);
void handleAddStaffUser(std::vector<User*>& users, FileHandler& fileHandler, const std::string& adminUser);

int main() {
    Utils::clearScreen();

    Inventory inventory;
    FileHandler fileHandler;
    std::vector<User*> users = fileHandler.loadUsers();

    fileHandler.loadProducts(inventory);
    fileHandler.loadTransactions(inventory);
    fileHandler.loadFinancials(inventory);
    fileHandler.seedInitialDataIfEmpty(inventory, users);

    fileHandler.logActivity("SYSTEM", "Inventory Management System started");

    bool systemRunning = true;

    while (systemRunning) {
        Utils::clearScreen();
        Utils::printHeader("INVENTORY MANAGEMENT SYSTEM (IMS)");
        std::cout << "        Object-Oriented Programming (OOP) Semester Project      \n";
        std::cout << "             C++ Comprehensive OOP Implementation               \n";
        Utils::printLine('=', 65);
        std::cout << "\n   1. Login to IMS\n";
        std::cout << "   2. About System & OOP Concepts Architecture\n";
        std::cout << "   3. Exit Application\n\n";

        int startChoice = Utils::getValidatedInput<int>("   Enter your choice (1-3): ", 1, 3);

        if (startChoice == 1) {
            User* currentUser = loginUser(users, fileHandler);
            if (currentUser != nullptr) {
                if (currentUser->hasAdminPrivileges()) {
                    runAdminMenu(inventory, fileHandler, currentUser, users);
                } else {
                    runStaffMenu(inventory, fileHandler, currentUser);
                }
            }
        }
        else if (startChoice == 2) {
            Utils::clearScreen();
            Utils::printHeader("SYSTEM ARCHITECTURE & OOP CONCEPTS");
            std::cout << "   This application demonstrates all key OOP principles in C++:\n\n";
            std::cout << "   [1] Encapsulation:     Private/protected attributes with validated getters & setters.\n";
            std::cout << "   [2] Inheritance:       Abstract Product -> PerishableProduct, ElectronicsProduct.\n";
            std::cout << "                          Abstract User    -> Admin, Staff.\n";
            std::cout << "   [3] Polymorphism:      Runtime: Pure virtual displayDetails(), serialize(), showDashboard().\n";
            std::cout << "                          Compile-time: Overloaded searchProduct(id/name/price), operator<<, operator==.\n";
            std::cout << "   [4] Abstraction:       Abstract base classes (Product, User) with pure virtual methods.\n";
            std::cout << "   [5] Composition:       Product HAS-A Supplier object.\n";
            std::cout << "   [6] Aggregation:       Inventory HAS-A collection of Product* pointers.\n";
            std::cout << "   [7] Static Members:    Product::totalProducts counter & Transaction::nextTransactionId.\n";
            std::cout << "   [8] Friend Class:      FinancialManager is a friend of Product for profit audits.\n";
            std::cout << "   [9] File Handling:     Persistent text streams (products, users, transactions, logs).\n";
            std::cout << "   [10] Exceptions:       Custom domain hierarchy derived from std::exception.\n";
            std::cout << "   [11] Templates:        Utils::getValidatedInput<T> template function.\n";
            Utils::printLine('=', 65);
            Utils::pause();
        }
        else if (startChoice == 3) {
            systemRunning = false;
        }
    }

    fileHandler.saveProducts(inventory.getProducts());
    fileHandler.saveTransactions(inventory.getTransactions());
    fileHandler.saveFinancials(inventory.getFinancialManager());
    fileHandler.saveUsers(users);
    fileHandler.logActivity("SYSTEM", "Inventory Management System safely shut down");

    for (User* u : users) delete u;
    users.clear();

    Utils::clearScreen();
    Utils::printHeader("THANK YOU FOR USING IMS");
    std::cout << "   All inventory data and transactions have been safely saved.\n";
    std::cout << "   Goodbye!\n";
    Utils::printLine('=', 65);
    return 0;
}

User* loginUser(const std::vector<User*>& users, FileHandler& fileHandler) {
    int attempts = 3;
    while (attempts > 0) {
        Utils::clearScreen();
        Utils::printHeader("USER AUTHENTICATION");
        std::cout << "   Login Attempts Remaining: " << attempts << "\n\n";

        std::string username = Utils::getNonEmptyString("   Enter Username: ");
        std::string password = Utils::getNonEmptyString("   Enter Password: ");

        for (User* u : users) {
            if (u && u->checkCredentials(username, password)) {
                std::cout << "\n   [OK] Authentication Successful! Welcome, " << u->getFullName() << ".\n";
                fileHandler.logActivity(username, "Successful Login (" + u->getRole() + ")");
                Utils::pause();
                return u;
            }
        }
        attempts--;
        std::cout << "\n   [X] Invalid username or password!\n";
        fileHandler.logActivity(username, "Failed login attempt");
        Utils::pause();
    }
    std::cout << "\n   [!] Maximum login attempts reached. Returning to main screen...\n";
    Utils::pause();
    return nullptr;
}

void runAdminMenu(Inventory& inventory, FileHandler& fileHandler, User* currentUser, std::vector<User*>& users) {
    bool adminSession = true;
    while (adminSession) {
        Utils::clearScreen();
        currentUser->showDashboard();

        int lowStockCount = inventory.getLowStockCount();
        if (lowStockCount > 0) {
            std::cout << "   >>> ALERT: " << lowStockCount << " item(s) are at or below reorder threshold! <<<\n";
            Utils::printLine('-', 65);
        }

        std::cout << "\n   [PRODUCT MANAGEMENT]\n";
        std::cout << "   1. Add New Product (Perishable or Electronics)\n";
        std::cout << "   2. View All Products Catalog\n";
        std::cout << "   3. Search Products (By ID, Name/Category, or Price Range)\n";
        std::cout << "   4. Update Product Information\n";
        std::cout << "   5. Delete Product\n\n";

        std::cout << "   [INVENTORY & BILLING]\n";
        std::cout << "   6. Stock In (Procure/Restock Items)\n";
        std::cout << "   7. Stock Out / Sell Product (With Receipt)\n";
        std::cout << "   8. Critical Low Stock Alert Report\n";
        std::cout << "   9. Sort Products (ID, Name, Quantity, Price)\n\n";

        std::cout << "   [FINANCE & ADMINISTRATION]\n";
        std::cout << "   10. Executive Financial & Profitability Report\n";
        std::cout << "   11. View Transaction Audit Log\n";
        std::cout << "   12. Register New Staff Account\n";
        std::cout << "   13. Save Data & Logout\n\n";

        int choice = Utils::getValidatedInput<int>("   Select an option (1-13): ", 1, 13);
        try {
            switch (choice) {
                case 1: handleAddProduct(inventory, fileHandler, currentUser->getUsername()); break;
                case 2: Utils::clearScreen(); inventory.displayAllProducts(); Utils::pause(); break;
                case 3: handleSearchProduct(inventory); break;
                case 4: {
                    Utils::clearScreen();
                    Utils::printHeader("UPDATE PRODUCT");
                    int id = Utils::getValidatedInput<int>("   Enter Product ID to update: ", 1, 999999);
                    inventory.updateProduct(id);
                    fileHandler.saveProducts(inventory.getProducts());
                    fileHandler.logActivity(currentUser->getUsername(), "Updated product ID " + std::to_string(id));
                    std::cout << "\n   [OK] Product updated successfully!\n";
                    Utils::pause();
                    break;
                }
                case 5: {
                    Utils::clearScreen();
                    Utils::printHeader("DELETE PRODUCT");
                    int id = Utils::getValidatedInput<int>("   Enter Product ID to delete: ", 1, 999999);
                    inventory.deleteProduct(id);
                    fileHandler.saveProducts(inventory.getProducts());
                    fileHandler.logActivity(currentUser->getUsername(), "Deleted product ID " + std::to_string(id));
                    std::cout << "\n   [OK] Product removed from inventory.\n";
                    Utils::pause();
                    break;
                }
                case 6: {
                    Utils::clearScreen();
                    Utils::printHeader("STOCK IN (RESTOCK)");
                    int id = Utils::getValidatedInput<int>("   Enter Product ID: ", 1, 999999);
                    int qty = Utils::getValidatedInput<int>("   Enter Quantity to add: ", 1, 10000);
                    inventory.stockIn(id, qty, currentUser->getUsername());
                    fileHandler.saveProducts(inventory.getProducts());
                    fileHandler.saveTransactions(inventory.getTransactions());
                    fileHandler.saveFinancials(inventory.getFinancialManager());
                    fileHandler.logActivity(currentUser->getUsername(), "Stock In: ID " + std::to_string(id) + " Qty: " + std::to_string(qty));
                    std::cout << "\n   [OK] Stock updated successfully!\n";
                    Utils::pause();
                    break;
                }
                case 7: {
                    Utils::clearScreen();
                    Utils::printHeader("STOCK OUT / BILLING");
                    int id = Utils::getValidatedInput<int>("   Enter Product ID to sell: ", 1, 999999);
                    int qty = Utils::getValidatedInput<int>("   Enter Quantity to sell: ", 1, 10000);
                    inventory.stockOut(id, qty, currentUser->getUsername(), true);
                    fileHandler.saveProducts(inventory.getProducts());
                    fileHandler.saveTransactions(inventory.getTransactions());
                    fileHandler.saveFinancials(inventory.getFinancialManager());
                    fileHandler.logActivity(currentUser->getUsername(), "Sold Product ID " + std::to_string(id) + " Qty: " + std::to_string(qty));
                    Utils::pause();
                    break;
                }
                case 8: Utils::clearScreen(); inventory.displayLowStockProducts(); Utils::pause(); break;
                case 9: {
                    Utils::clearScreen();
                    Utils::printHeader("SORT INVENTORY");
                    std::cout << "   1. Sort by ID (Ascending)\n   2. Sort by Name (Alphabetical)\n   3. Sort by Quantity (Ascending)\n   4. Sort by Selling Price (High to Low)\n";
                    int sortChoice = Utils::getValidatedInput<int>("   Select sorting option (1-4): ", 1, 4);
                    inventory.sortProducts(sortChoice);
                    std::cout << "\n   [OK] Inventory sorted. Viewing results:\n";
                    inventory.displayAllProducts();
                    Utils::pause();
                    break;
                }
                case 10: Utils::clearScreen(); inventory.displayFinancialSummary(); Utils::pause(); break;
                case 11: Utils::clearScreen(); inventory.displayTransactions(); Utils::pause(); break;
                case 12: handleAddStaffUser(users, fileHandler, currentUser->getUsername()); break;
                case 13:
                    fileHandler.saveProducts(inventory.getProducts());
                    fileHandler.saveTransactions(inventory.getTransactions());
                    fileHandler.saveFinancials(inventory.getFinancialManager());
                    fileHandler.saveUsers(users);
                    fileHandler.logActivity(currentUser->getUsername(), "Admin logged out");
                    adminSession = false;
                    break;
            }
        } catch (const IMSException& ex) {
            std::cout << "\n   [IMS EXCEPTION]: " << ex.what() << "\n";
            fileHandler.logActivity(currentUser->getUsername(), std::string("Exception: ") + ex.what());
            Utils::pause();
        } catch (const std::exception& ex) {
            std::cout << "\n   [SYSTEM ERROR]: " << ex.what() << "\n";
            Utils::pause();
        }
    }
}

void runStaffMenu(Inventory& inventory, FileHandler& fileHandler, User* currentUser) {
    bool staffSession = true;
    while (staffSession) {
        Utils::clearScreen();
        currentUser->showDashboard();

        int lowStockCount = inventory.getLowStockCount();
        if (lowStockCount > 0) {
            std::cout << "   >>> ALERT: " << lowStockCount << " item(s) are low in stock! <<<\n";
            Utils::printLine('-', 65);
        }

        std::cout << "\n   1. View All Products\n";
        std::cout << "   2. Search Products (ID, Name, or Price)\n";
        std::cout << "   3. Point of Sale (Sell Product & Print Customer Bill)\n";
        std::cout << "   4. Stock In (Receive Goods into Inventory)\n";
        std::cout << "   5. Check Low Stock Items\n";
        std::cout << "   6. View Recent Transactions\n";
        std::cout << "   7. Save & Logout\n\n";

        int choice = Utils::getValidatedInput<int>("   Select an option (1-7): ", 1, 7);
        try {
            switch (choice) {
                case 1: Utils::clearScreen(); inventory.displayAllProducts(); Utils::pause(); break;
                case 2: handleSearchProduct(inventory); break;
                case 3: {
                    Utils::clearScreen();
                    Utils::printHeader("POINT OF SALE - CASHIER COUNTER");
                    int id = Utils::getValidatedInput<int>("   Enter Product ID to sell: ", 1, 999999);
                    int qty = Utils::getValidatedInput<int>("   Enter Quantity: ", 1, 1000);
                    inventory.stockOut(id, qty, currentUser->getUsername(), true);
                    fileHandler.saveProducts(inventory.getProducts());
                    fileHandler.saveTransactions(inventory.getTransactions());
                    fileHandler.saveFinancials(inventory.getFinancialManager());
                    fileHandler.logActivity(currentUser->getUsername(), "Staff sale: ID " + std::to_string(id) + " Qty: " + std::to_string(qty));
                    Utils::pause();
                    break;
                }
                case 4: {
                    Utils::clearScreen();
                    Utils::printHeader("RECEIVE INCOMING STOCK");
                    int id = Utils::getValidatedInput<int>("   Enter Product ID: ", 1, 999999);
                    int qty = Utils::getValidatedInput<int>("   Enter Quantity received: ", 1, 10000);
                    inventory.stockIn(id, qty, currentUser->getUsername());
                    fileHandler.saveProducts(inventory.getProducts());
                    fileHandler.saveTransactions(inventory.getTransactions());
                    fileHandler.saveFinancials(inventory.getFinancialManager());
                    fileHandler.logActivity(currentUser->getUsername(), "Staff Stock In: ID " + std::to_string(id) + " Qty: " + std::to_string(qty));
                    std::cout << "\n   [OK] Stock successfully recorded!\n";
                    Utils::pause();
                    break;
                }
                case 5: Utils::clearScreen(); inventory.displayLowStockProducts(); Utils::pause(); break;
                case 6: Utils::clearScreen(); inventory.displayTransactions(); Utils::pause(); break;
                case 7:
                    fileHandler.saveProducts(inventory.getProducts());
                    fileHandler.saveTransactions(inventory.getTransactions());
                    fileHandler.saveFinancials(inventory.getFinancialManager());
                    fileHandler.logActivity(currentUser->getUsername(), "Staff logged out");
                    staffSession = false;
                    break;
            }
        } catch (const IMSException& ex) {
            std::cout << "\n   [IMS EXCEPTION]: " << ex.what() << "\n";
            Utils::pause();
        } catch (const std::exception& ex) {
            std::cout << "\n   [SYSTEM ERROR]: " << ex.what() << "\n";
            Utils::pause();
        }
    }
}

void handleAddProduct(Inventory& inventory, FileHandler& fileHandler, const std::string& username) {
    Utils::clearScreen();
    Utils::printHeader("ADD NEW PRODUCT");
    std::cout << "   Select Product Category Type:\n";
    std::cout << "   1. Perishable Product (Food, Dairy, Medicine with Expiry)\n";
    std::cout << "   2. Electronics Product (Gadgets, Hardware with Warranty)\n";
    int typeChoice = Utils::getValidatedInput<int>("   Enter choice (1-2): ", 1, 2);

    int id = Utils::getValidatedInput<int>("   Enter Unique Product ID: ", 1, 999999);
    std::string name = Utils::getNonEmptyString("   Enter Product Name: ");
    std::string category = Utils::getNonEmptyString("   Enter Category: ");
    int quantity = Utils::getValidatedInput<int>("   Enter Initial Quantity: ", 0, 100000);
    double costPrice = Utils::getValidatedInput<double>("   Enter Cost / Purchase Price ($): ", 0.01, 100000.0);
    double sellingPrice = Utils::getValidatedInput<double>("   Enter Retail / Selling Price ($): ", 0.01, 100000.0);
    int threshold = Utils::getValidatedInput<int>("   Enter Low Stock Alert Threshold: ", 1, 1000);

    std::cout << "\n   [SUPPLIER INFORMATION (Composition)]\n";
    int supId = Utils::getValidatedInput<int>("   Enter Supplier ID: ", 1, 999999);
    std::string supName = Utils::getNonEmptyString("   Enter Supplier Name: ");
    std::string supPhone = Utils::getNonEmptyString("   Enter Supplier Phone: ");
    std::string supEmail = Utils::getNonEmptyString("   Enter Supplier Email: ");
    Supplier supplier(supId, supName, supPhone, supEmail);

    Product* newProduct = nullptr;
    if (typeChoice == 1) {
        std::string expiry = Utils::getNonEmptyString("   Enter Expiry Date (YYYY-MM-DD): ");
        double temp = Utils::getValidatedInput<double>("   Enter Storage Temp (°C): ", -50.0, 50.0);
        newProduct = new PerishableProduct(id, name, category, quantity, costPrice, sellingPrice, threshold, supplier, expiry, temp);
    } else {
        int warranty = Utils::getValidatedInput<int>("   Enter Warranty (Months): ", 0, 120);
        std::string brand = Utils::getNonEmptyString("   Enter Brand Name: ");
        newProduct = new ElectronicsProduct(id, name, category, quantity, costPrice, sellingPrice, threshold, supplier, warranty, brand);
    }

    inventory.addProduct(newProduct);
    fileHandler.saveProducts(inventory.getProducts());
    fileHandler.logActivity(username, "Added new " + newProduct->getProductType() + " product: " + name);

    std::cout << "\n   [OK] Product successfully added to inventory!\n";
    newProduct->displayDetails();
    Utils::pause();
}

void handleSearchProduct(const Inventory& inventory) {
    Utils::clearScreen();
    Utils::printHeader("SEARCH INVENTORY (POLYMORPHIC OVERLOADING)");
    std::cout << "   1. Search by Product ID (Exact Lookup)\n";
    std::cout << "   2. Search by Name or Category (Keyword Search)\n";
    std::cout << "   3. Search by Price Range (Filter)\n";
    int searchChoice = Utils::getValidatedInput<int>("   Select search criteria (1-3): ", 1, 3);

    if (searchChoice == 1) {
        int id = Utils::getValidatedInput<int>("   Enter Product ID: ", 1, 999999);
        Product* p = inventory.searchProduct(id);
        if (p) p->displayDetails();
        else std::cout << "\n   [!] No product found matching ID " << id << ".\n";
    }
    else if (searchChoice == 2) {
        std::string query = Utils::getNonEmptyString("   Enter keyword to search (Name or Category): ");
        auto results = inventory.searchProduct(query);
        if (results.empty()) {
            std::cout << "\n   [!] No products found matching query \"" << query << "\".\n";
        } else {
            std::cout << "\n   Found " << results.size() << " matching product(s):\n";
            for (const auto* p : results) p->displayDetails();
        }
    }
    else if (searchChoice == 3) {
        double minP = Utils::getValidatedInput<double>("   Enter Minimum Price ($): ", 0.0, 100000.0);
        double maxP = Utils::getValidatedInput<double>("   Enter Maximum Price ($): ", minP, 100000.0);
        auto results = inventory.searchProduct(minP, maxP);
        if (results.empty()) {
            std::cout << "\n   [!] No products found in price range $" << minP << " - $" << maxP << ".\n";
        } else {
            std::cout << "\n   Found " << results.size() << " product(s) in range $" << minP << " - $" << maxP << ":\n";
            for (const auto* p : results) p->displayDetails();
        }
    }
    Utils::pause();
}

void handleAddStaffUser(std::vector<User*>& users, FileHandler& fileHandler, const std::string& adminUser) {
    Utils::clearScreen();
    Utils::printHeader("REGISTER NEW STAFF ACCOUNT");
    std::string username = Utils::getNonEmptyString("   Enter New Staff Username: ");

    for (const auto* u : users) {
        if (u && u->getUsername() == username) {
            std::cout << "\n   [!] An account with username \"" << username << "\" already exists!\n";
            Utils::pause();
            return;
        }
    }

    std::string password = Utils::getNonEmptyString("   Enter Password: ");
    std::string fullName = Utils::getNonEmptyString("   Enter Staff Full Name: ");
    std::cout << "   Select Shift (1: Morning, 2: Evening, 3: Night): ";
    int shiftChoice = Utils::getValidatedInput<int>("", 1, 3);
    std::string shift = (shiftChoice == 1) ? "Morning" : (shiftChoice == 2 ? "Evening" : "Night");

    users.push_back(new Staff(username, password, fullName, shift));
    fileHandler.saveUsers(users);
    fileHandler.logActivity(adminUser, "Created new staff user: " + username);

    std::cout << "\n   [OK] Staff member \"" << fullName << "\" successfully registered!\n";
    Utils::pause();
}
