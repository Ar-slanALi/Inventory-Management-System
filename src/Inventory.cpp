#include "../include/Inventory.h"
#include "../include/Utils.h"
#include <iomanip>
#include <algorithm>

Inventory::Inventory() {}

// Destructor cleans up dynamically allocated Product objects
Inventory::~Inventory() {
    clearAll();
}

void Inventory::clearAll() {
    for (Product* p : products) {
        delete p;
    }
    products.clear();
}

// Add Product with Duplicate ID validation
void Inventory::addProduct(Product* product) {
    if (!product) return;

    for (const Product* p : products) {
        if (p && p->getId() == product->getId()) {
            throw DuplicateProductException("Product with ID " + std::to_string(product->getId()) + " already exists!");
        }
    }
    products.push_back(product);
}

// Update existing product
bool Inventory::updateProduct(int id) {
    Product* p = searchProduct(id);
    if (!p) {
        throw ProductNotFoundException("Cannot update: Product ID " + std::to_string(id) + " not found!");
    }

    std::cout << "\n   Current Product Details:";
    p->displayDetails();

    std::cout << "\n   Enter New Information (Leave blank or enter current value to keep unchanged):\n";
    std::cout << "   Enter new product name: ";
    std::string newName;
    std::getline(std::cin, newName);
    if (!newName.empty()) p->setName(newName);

    std::cout << "   Enter new category: ";
    std::string newCat;
    std::getline(std::cin, newCat);
    if (!newCat.empty()) p->setCategory(newCat);

    std::cout << "   Update Cost Price? (1 = Yes, 0 = Keep $" << p->getCostPrice() << "): ";
    int choice = Utils::getValidatedInput<int>("", 0, 1);
    if (choice == 1) {
        double newCost = Utils::getValidatedInput<double>("   Enter new cost price ($): ", 0.01, 100000.0);
        p->setCostPrice(newCost);
    }

    std::cout << "   Update Selling Price? (1 = Yes, 0 = Keep $" << p->getSellingPrice() << "): ";
    choice = Utils::getValidatedInput<int>("", 0, 1);
    if (choice == 1) {
        double newPrice = Utils::getValidatedInput<double>("   Enter new selling price ($): ", 0.01, 100000.0);
        p->setSellingPrice(newPrice);
    }

    std::cout << "   Update Reorder Alert Level? (1 = Yes, 0 = Keep " << p->getReorderThreshold() << "): ";
    choice = Utils::getValidatedInput<int>("", 0, 1);
    if (choice == 1) {
        int newThreshold = Utils::getValidatedInput<int>("   Enter new threshold: ", 1, 1000);
        p->setReorderThreshold(newThreshold);
    }

    return true;
}

// Delete product
bool Inventory::deleteProduct(int id) {
    for (auto it = products.begin(); it != products.end(); ++it) {
        if ((*it)->getId() == id) {
            delete *it;
            products.erase(it);
            return true;
        }
    }
    throw ProductNotFoundException("Cannot delete: Product ID " + std::to_string(id) + " does not exist.");
}

// Stock In
void Inventory::stockIn(int id, int quantity, const std::string& username) {
    if (quantity <= 0) {
        throw InvalidInputException("Stock-in quantity must be greater than zero.");
    }

    Product* p = searchProduct(id);
    if (!p) {
        throw ProductNotFoundException("Product ID " + std::to_string(id) + " not found!");
    }

    p->addStock(quantity);
    financialManager.recordStockIn(*p, quantity);

    Transaction tx(p->getId(), p->getName(), "STOCK_IN", quantity, p->getCostPrice(), username, Utils::getCurrentDateTime());
    transactions.push_back(tx);
}

// Stock Out (Sale/Billing)
Transaction Inventory::stockOut(int id, int quantity, const std::string& username, bool printInvoice) {
    if (quantity <= 0) {
        throw InvalidInputException("Sales quantity must be positive!");
    }

    Product* p = searchProduct(id);
    if (!p) {
        throw ProductNotFoundException("Product ID " + std::to_string(id) + " not found!");
    }

    if (p->getQuantity() < quantity) {
        throw InsufficientStockException("Insufficient stock! Available: " + std::to_string(p->getQuantity()) + ", Requested: " + std::to_string(quantity));
    }

    p->deductStock(quantity);
    financialManager.recordSale(*p, quantity);

    Transaction tx(p->getId(), p->getName(), "SALE", quantity, p->getSellingPrice(), username, Utils::getCurrentDateTime());
    transactions.push_back(tx);

    if (printInvoice) {
        tx.printReceipt();
    }

    return tx;
}

// ============================================================================
// OOP CONCEPT: Function Overloading (Compile-Time Polymorphism)
// ============================================================================

// Overload 1: Search by Product ID
Product* Inventory::searchProduct(int id) const {
    for (Product* p : products) {
        if (p && p->getId() == id) {
            return p;
        }
    }
    return nullptr;
}

// Overload 2: Search by Name or Category substring (Case-insensitive)
std::vector<Product*> Inventory::searchProduct(const std::string& query) const {
    std::vector<Product*> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    for (Product* p : products) {
        if (!p) continue;
        std::string lowerName = p->getName();
        std::string lowerCat = p->getCategory();
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        std::transform(lowerCat.begin(), lowerCat.end(), lowerCat.begin(), ::tolower);

        if (lowerName.find(lowerQuery) != std::string::npos || lowerCat.find(lowerQuery) != std::string::npos) {
            results.push_back(p);
        }
    }
    return results;
}

// Overload 3: Search by Price Range
std::vector<Product*> Inventory::searchProduct(double minPrice, double maxPrice) const {
    std::vector<Product*> results;
    for (Product* p : products) {
        if (p && p->getSellingPrice() >= minPrice && p->getSellingPrice() <= maxPrice) {
            results.push_back(p);
        }
    }
    return results;
}

// Display all products in formatted table
void Inventory::displayAllProducts() const {
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

// Low Stock Products
void Inventory::displayLowStockProducts() const {
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

int Inventory::getLowStockCount() const {
    int count = 0;
    for (const Product* p : products) {
        if (p && p->isLowStock()) count++;
    }
    return count;
}

// Sorting Algorithm
void Inventory::sortProducts(int criterion) {
    if (products.size() < 2) return;

    switch (criterion) {
        case 1: // Sort by ID Ascending
            std::sort(products.begin(), products.end(), [](const Product* a, const Product* b) {
                return a->getId() < b->getId();
            });
            break;
        case 2: // Sort by Name Alphabetical
            std::sort(products.begin(), products.end(), [](const Product* a, const Product* b) {
                return a->getName() < b->getName();
            });
            break;
        case 3: // Sort by Quantity (Ascending)
            std::sort(products.begin(), products.end(), [](const Product* a, const Product* b) {
                return a->getQuantity() < b->getQuantity();
            });
            break;
        case 4: // Sort by Selling Price (Descending)
            std::sort(products.begin(), products.end(), [](const Product* a, const Product* b) {
                return a->getSellingPrice() > b->getSellingPrice();
            });
            break;
        default:
            break;
    }
}

void Inventory::displayTransactions() const {
    if (transactions.empty()) {
        std::cout << "\n   [i] No transaction history recorded yet.\n";
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

    for (const auto& tx : transactions) {
        tx.displaySummary();
    }
    std::cout << "====================================================================================================================\n";
}

void Inventory::displayFinancialSummary() const {
    financialManager.displayFinancialReport(products);
}

const std::vector<Product*>& Inventory::getProducts() const { return products; }
const std::vector<Transaction>& Inventory::getTransactions() const { return transactions; }
FinancialManager& Inventory::getFinancialManager() { return financialManager; }
const FinancialManager& Inventory::getFinancialManager() const { return financialManager; }

void Inventory::addTransaction(const Transaction& tx) {
    transactions.push_back(tx);
}

void Inventory::setFinancialManager(const FinancialManager& fm) {
    this->financialManager = fm;
}
