#ifndef INVENTORY_H
#define INVENTORY_H

#include <iostream>
#include <vector>
#include <string>
#include "Product.h"
#include "Transaction.h"
#include "FinancialManager.h"
#include "Exceptions.h"

// ============================================================================
// OOP CONCEPT: Aggregation & Compile-Time Polymorphism (Function Overloading)
// 1. Aggregation: Inventory class maintains a collection of Product* pointers.
// 2. Function Overloading: Multiple searchProduct() methods with different signatures.
// 3. Exception Handling: Throws custom domain exceptions for business rule violations.
// 4. Memory Management: Clean destructor ensures no memory leaks for polymorphic objects.
// ============================================================================

class Inventory {
private:
    std::vector<Product*> products;          // Dynamic collection of polymorphic products
    std::vector<Transaction> transactions;   // Audit trail of all transactions
    FinancialManager financialManager;       // Financial ledger

public:
    Inventory();
    ~Inventory();

    // Prevent accidental shallow copying (rule of three/five)
    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) = delete;

    // Core Product Management (Admin tasks)
    void addProduct(Product* product);       // Throws DuplicateProductException
    bool updateProduct(int id);              // Throws ProductNotFoundException
    bool deleteProduct(int id);              // Throws ProductNotFoundException

    // Stock Movement Operations (Admin & Staff tasks)
    void stockIn(int id, int quantity, const std::string& username);
    Transaction stockOut(int id, int quantity, const std::string& username, bool printInvoice = true);

    // ========================================================================
    // OOP CONCEPT: Function Overloading (Compile-Time Polymorphism)
    // 3 overloaded variations of searchProduct
    // ========================================================================
    Product* searchProduct(int id) const;                                         // Overload 1: by ID
    std::vector<Product*> searchProduct(const std::string& nameOrCategory) const; // Overload 2: by keyword
    std::vector<Product*> searchProduct(double minPrice, double maxPrice) const;  // Overload 3: by price range

    // Product Display & Reporting
    void displayAllProducts() const;
    void displayLowStockProducts() const;
    void displayTransactions() const;
    void displayFinancialSummary() const;

    // Sorting (Sort by ID, Name, Quantity, Price)
    void sortProducts(int criterion);

    // Accessors for persistence
    const std::vector<Product*>& getProducts() const;
    const std::vector<Transaction>& getTransactions() const;
    FinancialManager& getFinancialManager();
    const FinancialManager& getFinancialManager() const;

    // Direct transaction recording (for file loader)
    void addTransaction(const Transaction& tx);
    void setFinancialManager(const FinancialManager& fm);

    // Helpers
    int getLowStockCount() const;
    void clearAll();
};

#endif // INVENTORY_H
