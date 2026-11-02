#include <iostream>
#include <vector>
#include <string>
#include "../include/Utils.h"
#include "../include/Exceptions.h"
#include "../include/Product.h"
#include "../include/Supplier.h"
#include "../include/Transaction.h"
#include "../include/FinancialManager.h"
#include "../include/User.h"
#include "../include/Inventory.h"
#include "../include/FileHandler.h"

// Forward function declarations for menus
void runAdminMenu(Inventory& inventory, FileHandler& fileHandler, User* currentUser, std::vector<User*>& users);
void runStaffMenu(Inventory& inventory, FileHandler& fileHandler, User* currentUser);
User* loginUser(const std::vector<User*>& users, FileHandler& fileHandler);
void handleAddProduct(Inventory& inventory, FileHandler& fileHandler, const std::string& username);
void handleSearchProduct(const Inventory& inventory);
void handleAddStaffUser(std::vector<User*>& users, FileHandler& fileHandler, const std::string& adminUser);

int main() {
    Utils::clearScreen();

    // 1. Core Systems Initialization
    Inventory inventory;
    FileHandler fileHandler;
    std::vector<User*> users = fileHandler.loadUsers();

    // Load data from disk & seed initial records if empty
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
            // Polymorphic User Pointer (OOP Concept: Polymorphism)
            User* currentUser = loginUser(users, fileHandler);

            if (currentUser != nullptr) {
                // Role-Based dynamic routing
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

    // Auto-save on exit
    fileHandler.saveProducts(inventory.getProducts());
    fileHandler.saveTransactions(inventory.getTransactions());
    fileHandler.saveFinancials(inventory.getFinancialManager());
    fileHandler.saveUsers(users);
    fileHandler.logActivity("SYSTEM", "Inventory Management System safely shut down");

    // Clean up polymorphic User memory
    for (User* u : users) {
        delete u;
    }
    users.clear();

    Utils::clearScreen();
    Utils::printHeader("THANK YOU FOR USING IMS");
    std::cout << "   All inventory data and transactions have been safely saved.\n";
    std::cout << "   Goodbye!\n";
    Utils::printLine('=', 65);
    return 0;
}

// User Authentication
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

// ============================================================================
// ADMIN MENU (Full Access)
// ============================================================================
void runAdminMenu(Inventory& inventory, FileHandler& fileHandler, User* currentUser, std::vector<User*>& users) {
    bool adminSession = true;

    while (adminSession) {
        Utils::clearScreen();
        currentUser->showDashboard();

        // Low stock warning banner
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
                case 1:
                    handleAddProduct(inventory, fileHandler, currentUser->getUsername());
                    break;
                case 2:
                    Utils::clearScreen();
                    inventory.displayAllProducts();
                    Utils::pause();
                    break;
                case 3:
                    handleSearchProduct(inventory);
                    break;
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
                case 8:
                    Utils::clearScreen();
                    inventory.displayLowStockProducts();
                    Utils::pause();
                    break;
                case 9: {
                    Utils::clearScreen();
                    Utils::printHeader("SORT INVENTORY");
                    std::cout << "   1. Sort by ID (Ascending)\n";
                    std::cout << "   2. Sort by Name (Alphabetical)\n";
                    std::cout << "   3. Sort by Quantity (Ascending)\n";
                    std::cout << "   4. Sort by Selling Price (High to Low)\n";
                    int sortChoice = Utils::getValidatedInput<int>("   Select sorting option (1-4): ", 1, 4);
                    inventory.sortProducts(sortChoice);
                    std::cout << "\n   [OK] Inventory sorted. Viewing results:\n";
                    inventory.displayAllProducts();
                    Utils::pause();
                    break;
                }
                case 10:
                    Utils::clearScreen();
                    inventory.displayFinancialSummary();
                    Utils::pause();
                    break;
                case 11:
                    Utils::clearScreen();
                    inventory.displayTransactions();
                    Utils::pause();
                    break;
                case 12:
                    handleAddStaffUser(users, fileHandler, currentUser->getUsername());
                    break;
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

// ============================================================================
// STAFF MENU (Operational / Cashier Access)
// ============================================================================
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
                case 1:
                    Utils::clearScreen();
                    inventory.displayAllProducts();
                    Utils::pause();
                    break;
                case 2:
                    handleSearchProduct(inventory);
                    break;
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
                case 5:
                    Utils::clearScreen();
                    inventory.displayLowStockProducts();
                    Utils::pause();
                    break;
                case 6:
                    Utils::clearScreen();
                    inventory.displayTransactions();
                    Utils::pause();
                    break;
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

// Add Product Helper (Polymorphic creation)
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

// Search Product Helper (Demonstrates Function Overloading)
void handleSearchProduct(const Inventory& inventory) {
    Utils::clearScreen();
    Utils::printHeader("SEARCH INVENTORY (POLYMORPHIC OVERLOADING)");
    std::cout << "   1. Search by Product ID (Exact Lookup)\n";
    std::cout << "   2. Search by Name or Category (Keyword Search)\n";
    std::cout << "   3. Search by Price Range (Filter)\n";
    int searchChoice = Utils::getValidatedInput<int>("   Select search criteria (1-3): ", 1, 3);

    if (searchChoice == 1) {
        // Calls Overload 1: searchProduct(int)
        int id = Utils::getValidatedInput<int>("   Enter Product ID: ", 1, 999999);
        Product* p = inventory.searchProduct(id);
        if (p) {
            p->displayDetails();
        } else {
            std::cout << "\n   [!] No product found matching ID " << id << ".\n";
        }
    }
    else if (searchChoice == 2) {
        // Calls Overload 2: searchProduct(const string&)
        std::string query = Utils::getNonEmptyString("   Enter keyword to search (Name or Category): ");
        auto results = inventory.searchProduct(query);
        if (results.empty()) {
            std::cout << "\n   [!] No products found matching query \"" << query << "\".\n";
        } else {
            std::cout << "\n   Found " << results.size() << " matching product(s):\n";
            for (const auto* p : results) {
                p->displayDetails();
            }
        }
    }
    else if (searchChoice == 3) {
        // Calls Overload 3: searchProduct(double, double)
        double minP = Utils::getValidatedInput<double>("   Enter Minimum Price ($): ", 0.0, 100000.0);
        double maxP = Utils::getValidatedInput<double>("   Enter Maximum Price ($): ", minP, 100000.0);
        auto results = inventory.searchProduct(minP, maxP);
        if (results.empty()) {
            std::cout << "\n   [!] No products found in price range $" << minP << " - $" << maxP << ".\n";
        } else {
            std::cout << "\n   Found " << results.size() << " product(s) in range $" << minP << " - $" << maxP << ":\n";
            for (const auto* p : results) {
                p->displayDetails();
            }
        }
    }
    Utils::pause();
}

// Register Staff User
void handleAddStaffUser(std::vector<User*>& users, FileHandler& fileHandler, const std::string& adminUser) {
    Utils::clearScreen();
    Utils::printHeader("REGISTER NEW STAFF ACCOUNT");
    std::string username = Utils::getNonEmptyString("   Enter New Staff Username: ");

    // Check if username already exists
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
