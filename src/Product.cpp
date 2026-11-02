#include "../include/Product.h"
#include <iomanip>
#include <sstream>

// ============================================================================
// Static Data Member Initialization
// ============================================================================
int Product::totalProducts = 0;

int Product::getTotalProducts() {
    return totalProducts;
}

// Default Constructor
Product::Product()
    : id(0), name("Unnamed"), category("General"), quantity(0),
      costPrice(0.0), sellingPrice(0.0), reorderThreshold(5), supplier() {
    totalProducts++;
}

// Parameterized Constructor
Product::Product(int id, const std::string& name, const std::string& category,
                 int quantity, double costPrice, double sellingPrice,
                 int reorderThreshold, const Supplier& supplier)
    : id(id), name(name), category(category), quantity(quantity),
      costPrice(costPrice), sellingPrice(sellingPrice),
      reorderThreshold(reorderThreshold), supplier(supplier) {
    totalProducts++;
}

// Virtual Destructor
Product::~Product() {
    if (totalProducts > 0) {
        totalProducts--;
    }
}

// Stock calculations
double Product::calculateStockValue() const {
    return quantity * sellingPrice;
}

double Product::calculateCostValue() const {
    return quantity * costPrice;
}

bool Product::isLowStock() const {
    return quantity <= reorderThreshold;
}

void Product::addStock(int amount) {
    if (amount > 0) {
        quantity += amount;
    }
}

void Product::deductStock(int amount) {
    if (amount > 0 && quantity >= amount) {
        quantity -= amount;
    }
}

// Virtual method for discount calculation (Base implementation)
double Product::calculateDiscountedPrice(double discountPercent) const {
    if (discountPercent < 0.0 || discountPercent > 100.0) return sellingPrice;
    return sellingPrice - (sellingPrice * (discountPercent / 100.0));
}

// Getters
int Product::getId() const { return id; }
std::string Product::getName() const { return name; }
std::string Product::getCategory() const { return category; }
int Product::getQuantity() const { return quantity; }
double Product::getCostPrice() const { return costPrice; }
double Product::getSellingPrice() const { return sellingPrice; }
int Product::getReorderThreshold() const { return reorderThreshold; }
Supplier Product::getSupplier() const { return supplier; }

// Setters
void Product::setId(int id) { this->id = id; }
void Product::setName(const std::string& name) { this->name = name; }
void Product::setCategory(const std::string& category) { this->category = category; }
void Product::setQuantity(int quantity) { this->quantity = (quantity >= 0) ? quantity : 0; }
void Product::setCostPrice(double price) { this->costPrice = (price >= 0.0) ? price : 0.0; }
void Product::setSellingPrice(double price) { this->sellingPrice = (price >= 0.0) ? price : 0.0; }
void Product::setReorderThreshold(int threshold) { this->reorderThreshold = (threshold >= 0) ? threshold : 0; }
void Product::setSupplier(const Supplier& supplier) { this->supplier = supplier; }

// Operator Overloading ==
bool Product::operator==(const Product& other) const {
    return this->id == other.id;
}

// Operator Overloading <<
std::ostream& operator<<(std::ostream& os, const Product& p) {
    os << "[" << p.id << "] " << p.name << " | Stock: " << p.quantity
       << " | Price: $" << std::fixed << std::setprecision(2) << p.sellingPrice;
    return os;
}

// ============================================================================
// PerishableProduct Implementation
// ============================================================================
PerishableProduct::PerishableProduct()
    : Product(), expiryDate("2026-12-31"), storageTempCelsius(4.0) {}

PerishableProduct::PerishableProduct(int id, const std::string& name, const std::string& category,
                                     int quantity, double costPrice, double sellingPrice,
                                     int reorderThreshold, const Supplier& supplier,
                                     const std::string& expiryDate, double storageTemp)
    : Product(id, name, category, quantity, costPrice, sellingPrice, reorderThreshold, supplier),
      expiryDate(expiryDate), storageTempCelsius(storageTemp) {}

void PerishableProduct::displayDetails() const {
    std::cout << "\n+-------------------------------------------------------------+\n";
    std::cout << "| [PERISHABLE PRODUCT] ID: " << std::setw(6) << std::left << id 
              << " Name: " << std::setw(28) << std::left << name << " |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "  Category:            " << category << "\n";
    std::cout << "  Current Stock:       " << quantity << " units " 
              << (isLowStock() ? "(LOW STOCK ALERT!)" : "") << "\n";
    std::cout << "  Reorder Level:       " << reorderThreshold << " units\n";
    std::cout << "  Cost Price:          $" << std::fixed << std::setprecision(2) << costPrice << "\n";
    std::cout << "  Selling Price:       $" << std::fixed << std::setprecision(2) << sellingPrice << "\n";
    std::cout << "  Expiry Date:         " << expiryDate << "\n";
    std::cout << "  Storage Temperature: " << storageTempCelsius << " °C\n";
    std::cout << "  Supplier:            " << supplier << "\n";
    std::cout << "+-------------------------------------------------------------+\n";
}

std::string PerishableProduct::getProductType() const {
    return "PERISHABLE";
}

// Serialization: TYPE;id;name;category;qty;cost;price;threshold;supplier_data;expiry;temp
std::string PerishableProduct::serialize() const {
    std::ostringstream ss;
    ss << "PERISHABLE;" << id << ";" << name << ";" << category << ";"
       << quantity << ";" << costPrice << ";" << sellingPrice << ";"
       << reorderThreshold << ";" << supplier.serialize() << ";"
       << expiryDate << ";" << storageTempCelsius;
    return ss.str();
}

// Perishable products offer clearance discount
double PerishableProduct::calculateDiscountedPrice(double discountPercent) const {
    // If regular discount applied, give extra 5% freshness clearance incentive
    double totalDiscount = discountPercent + 5.0;
    if (totalDiscount > 50.0) totalDiscount = 50.0; // cap discount at 50%
    return sellingPrice - (sellingPrice * (totalDiscount / 100.0));
}

std::string PerishableProduct::getExpiryDate() const { return expiryDate; }
double PerishableProduct::getStorageTempCelsius() const { return storageTempCelsius; }
void PerishableProduct::setExpiryDate(const std::string& date) { this->expiryDate = date; }
void PerishableProduct::setStorageTempCelsius(double temp) { this->storageTempCelsius = temp; }

// ============================================================================
// ElectronicsProduct Implementation
// ============================================================================
ElectronicsProduct::ElectronicsProduct()
    : Product(), warrantyMonths(12), brandName("Generic") {}

ElectronicsProduct::ElectronicsProduct(int id, const std::string& name, const std::string& category,
                                       int quantity, double costPrice, double sellingPrice,
                                       int reorderThreshold, const Supplier& supplier,
                                       int warrantyMonths, const std::string& brandName)
    : Product(id, name, category, quantity, costPrice, sellingPrice, reorderThreshold, supplier),
      warrantyMonths(warrantyMonths), brandName(brandName) {}

void ElectronicsProduct::displayDetails() const {
    std::cout << "\n+-------------------------------------------------------------+\n";
    std::cout << "| [ELECTRONICS PRODUCT] ID: " << std::setw(6) << std::left << id 
              << " Name: " << std::setw(26) << std::left << name << " |\n";
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "  Brand:               " << brandName << "\n";
    std::cout << "  Category:            " << category << "\n";
    std::cout << "  Current Stock:       " << quantity << " units " 
              << (isLowStock() ? "(LOW STOCK ALERT!)" : "") << "\n";
    std::cout << "  Reorder Level:       " << reorderThreshold << " units\n";
    std::cout << "  Cost Price:          $" << std::fixed << std::setprecision(2) << costPrice << "\n";
    std::cout << "  Selling Price:       $" << std::fixed << std::setprecision(2) << sellingPrice << "\n";
    std::cout << "  Warranty:            " << warrantyMonths << " Months Manufacturer Warranty\n";
    std::cout << "  Supplier:            " << supplier << "\n";
    std::cout << "+-------------------------------------------------------------+\n";
}

std::string ElectronicsProduct::getProductType() const {
    return "ELECTRONICS";
}

// Serialization: TYPE;id;name;category;qty;cost;price;threshold;supplier_data;warranty;brand
std::string ElectronicsProduct::serialize() const {
    std::ostringstream ss;
    ss << "ELECTRONICS;" << id << ";" << name << ";" << category << ";"
       << quantity << ";" << costPrice << ";" << sellingPrice << ";"
       << reorderThreshold << ";" << supplier.serialize() << ";"
       << warrantyMonths << ";" << brandName;
    return ss.str();
}

double ElectronicsProduct::calculateDiscountedPrice(double discountPercent) const {
    if (discountPercent < 0.0) return sellingPrice;
    if (discountPercent > 30.0) discountPercent = 30.0; // electronics max promotional discount 30%
    return sellingPrice - (sellingPrice * (discountPercent / 100.0));
}

int ElectronicsProduct::getWarrantyMonths() const { return warrantyMonths; }
std::string ElectronicsProduct::getBrandName() const { return brandName; }
void ElectronicsProduct::setWarrantyMonths(int months) { this->warrantyMonths = (months >= 0) ? months : 0; }
void ElectronicsProduct::setBrandName(const std::string& brand) { this->brandName = brand; }
