#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include "Product.h"
#include "User.h"
#include "Transaction.h"
#include "FinancialManager.h"
#include "Inventory.h"

// ============================================================================
// OOP CONCEPT: File Handling (Stream Persistence & Polymorphic Deserialization)
// Manages disk I/O operations for Products, Users, Transactions, and Financials.
// Demonstrates:
// 1. std::ifstream and std::ofstream usage
// 2. Polymorphic object reconstruction from persistent records
// 3. System activity logging
// ============================================================================

class FileHandler {
private:
    std::string productsFilePath;
    std::string usersFilePath;
    std::string transactionsFilePath;
    std::string financialsFilePath;
    std::string logsFilePath;

public:
    FileHandler(const std::string& productsPath = "data/products.txt",
                const std::string& usersPath = "data/users.txt",
                const std::string& txPath = "data/transactions.txt",
                const std::string& finPath = "data/financials.txt",
                const std::string& logPath = "data/logs.txt");

    // Product persistence (Polymorphic save & load)
    void saveProducts(const std::vector<Product*>& products);
    void loadProducts(Inventory& inventory);

    // User persistence
    void saveUsers(const std::vector<User*>& users);
    std::vector<User*> loadUsers();

    // Transaction persistence
    void saveTransactions(const std::vector<Transaction>& transactions);
    void loadTransactions(Inventory& inventory);

    // Financial persistence
    void saveFinancials(const FinancialManager& fm);
    void loadFinancials(Inventory& inventory);

    // Audit Logging
    void logActivity(const std::string& user, const std::string& action);

    // Initialize Default Data if files are missing
    void seedInitialDataIfEmpty(Inventory& inventory, std::vector<User*>& users);
};

#endif // FILE_HANDLER_H
