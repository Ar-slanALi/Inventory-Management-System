#include "../include/FileHandler.h"
#include "../include/Utils.h"
#include <fstream>
#include <sstream>

FileHandler::FileHandler(const std::string& productsPath,
                         const std::string& usersPath,
                         const std::string& txPath,
                         const std::string& finPath,
                         const std::string& logPath)
    : productsFilePath(productsPath), usersFilePath(usersPath),
      transactionsFilePath(txPath), financialsFilePath(finPath), logsFilePath(logPath) {}

// Polymorphic save of products
void FileHandler::saveProducts(const std::vector<Product*>& products) {
    std::ofstream outFile(productsFilePath);
    if (!outFile.is_open()) return;

    for (const Product* p : products) {
        if (p) {
            outFile << p->serialize() << "\n";
        }
    }
    outFile.close();
}

// Polymorphic load of products
void FileHandler::loadProducts(Inventory& inventory) {
    std::ifstream inFile(productsFilePath);
    if (!inFile.is_open()) return;

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string typeToken;
        if (!std::getline(ss, typeToken, ';')) continue;

        try {
            if (typeToken == "PERISHABLE") {
                std::string idStr, name, cat, qtyStr, costStr, priceStr, threshStr, supplierData, expiry, tempStr;
                std::getline(ss, idStr, ';');
                std::getline(ss, name, ';');
                std::getline(ss, cat, ';');
                std::getline(ss, qtyStr, ';');
                std::getline(ss, costStr, ';');
                std::getline(ss, priceStr, ';');
                std::getline(ss, threshStr, ';');
                std::getline(ss, supplierData, ';');
                std::getline(ss, expiry, ';');
                std::getline(ss, tempStr, ';');

                Supplier s;
                s.deserialize(supplierData);

                PerishableProduct* prod = new PerishableProduct(
                    std::stoi(idStr), name, cat, std::stoi(qtyStr),
                    std::stod(costStr), std::stod(priceStr),
                    std::stoi(threshStr), s, expiry, std::stod(tempStr)
                );
                inventory.addProduct(prod);
            }
            else if (typeToken == "ELECTRONICS") {
                std::string idStr, name, cat, qtyStr, costStr, priceStr, threshStr, supplierData, warrantyStr, brand;
                std::getline(ss, idStr, ';');
                std::getline(ss, name, ';');
                std::getline(ss, cat, ';');
                std::getline(ss, qtyStr, ';');
                std::getline(ss, costStr, ';');
                std::getline(ss, priceStr, ';');
                std::getline(ss, threshStr, ';');
                std::getline(ss, supplierData, ';');
                std::getline(ss, warrantyStr, ';');
                std::getline(ss, brand, ';');

                Supplier s;
                s.deserialize(supplierData);

                ElectronicsProduct* prod = new ElectronicsProduct(
                    std::stoi(idStr), name, cat, std::stoi(qtyStr),
                    std::stod(costStr), std::stod(priceStr),
                    std::stoi(threshStr), s, std::stoi(warrantyStr), brand
                );
                inventory.addProduct(prod);
            }
        } catch (...) {
            // Skip corrupted entries cleanly
        }
    }
    inFile.close();
}

void FileHandler::saveUsers(const std::vector<User*>& users) {
    std::ofstream outFile(usersFilePath);
    if (!outFile.is_open()) return;

    for (const User* u : users) {
        if (u) {
            outFile << u->serialize() << "\n";
        }
    }
    outFile.close();
}

std::vector<User*> FileHandler::loadUsers() {
    std::vector<User*> users;
    std::ifstream inFile(usersFilePath);
    if (!inFile.is_open()) return users;

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string roleToken, username, password, fullName;

        std::getline(ss, roleToken, ';');
        std::getline(ss, username, ';');
        std::getline(ss, password, ';');
        std::getline(ss, fullName, ';');

        if (roleToken == "ADMIN") {
            users.push_back(new Admin(username, password, fullName));
        } else if (roleToken == "STAFF") {
            std::string shift;
            std::getline(ss, shift, ';');
            if (shift.empty()) shift = "Morning";
            users.push_back(new Staff(username, password, fullName, shift));
        }
    }
    inFile.close();
    return users;
}

void FileHandler::saveTransactions(const std::vector<Transaction>& transactions) {
    std::ofstream outFile(transactionsFilePath);
    if (!outFile.is_open()) return;

    for (const auto& tx : transactions) {
        outFile << tx.serialize() << "\n";
    }
    outFile.close();
}

void FileHandler::loadTransactions(Inventory& inventory) {
    std::ifstream inFile(transactionsFilePath);
    if (!inFile.is_open()) return;

    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            Transaction tx = Transaction::deserialize(line);
            inventory.addTransaction(tx);
        }
    }
    inFile.close();
}

void FileHandler::saveFinancials(const FinancialManager& fm) {
    std::ofstream outFile(financialsFilePath);
    if (!outFile.is_open()) return;
    outFile << fm.serialize() << "\n";
    outFile.close();
}

void FileHandler::loadFinancials(Inventory& inventory) {
    std::ifstream inFile(financialsFilePath);
    if (!inFile.is_open()) return;

    std::string line;
    if (std::getline(inFile, line) && !line.empty()) {
        FinancialManager fm;
        fm.deserialize(line);
        inventory.setFinancialManager(fm);
    }
    inFile.close();
}

void FileHandler::logActivity(const std::string& user, const std::string& action) {
    std::ofstream outFile(logsFilePath, std::ios::app);
    if (outFile.is_open()) {
        outFile << "[" << Utils::getCurrentDateTime() << "] "
                << "User: " << user << " | Action: " << action << "\n";
        outFile.close();
    }
}

// Seed default accounts and products if none exist
void FileHandler::seedInitialDataIfEmpty(Inventory& inventory, std::vector<User*>& users) {
    // 1. Seed Users if empty
    if (users.empty()) {
        users.push_back(new Admin("admin", "admin123", "Muhammad Suleman (Admin)"));
        users.push_back(new Staff("staff", "staff123", "Store Cashier Associate", "Morning"));
        saveUsers(users);
        logActivity("SYSTEM", "Initialized default Admin and Staff accounts");
    }

    // 2. Seed Products if empty
    if (inventory.getProducts().empty()) {
        Supplier supFood(101, "Nestle Distribution", "+92-300-1122334", "orders@nestle.com");
        Supplier supTech(102, "Dell Technologies Ltd", "+92-321-9988776", "b2b@dell.com");
        Supplier supPharma(103, "Getz Pharma International", "+92-333-5566778", "supply@getz.com");

        // Perishable Items
        inventory.addProduct(new PerishableProduct(
            101, "Organic Whole Milk 1L", "Dairy", 45, 1.80, 2.50, 10, supFood, "2026-11-20", 4.0
        ));
        inventory.addProduct(new PerishableProduct(
            102, "Amoxicillin 500mg (10 Tab)", "Pharmacy", 15, 6.00, 9.50, 5, supPharma, "2027-05-15", 22.0
        ));
        inventory.addProduct(new PerishableProduct(
            103, "Fresh Farm Eggs (Dozen)", "Dairy", 8, 2.00, 3.20, 10, supFood, "2026-11-15", 8.0 // Low stock!
        ));

        // Electronics Items
        inventory.addProduct(new ElectronicsProduct(
            201, "Dell Wireless Mouse WM126", "Peripherals", 25, 12.00, 19.99, 5, supTech, 12, "Dell"
        ));
        inventory.addProduct(new ElectronicsProduct(
            202, "Logitech HD Webcam C920", "Accessories", 4, 45.00, 69.99, 5, supTech, 24, "Logitech" // Low stock!
        ));
        inventory.addProduct(new ElectronicsProduct(
            203, "Kingston 512GB NVMe SSD", "Storage", 18, 38.00, 54.99, 6, supTech, 36, "Kingston"
        ));

        saveProducts(inventory.getProducts());
        logActivity("SYSTEM", "Seeded default inventory with sample Perishable and Electronics products");
    }
}
