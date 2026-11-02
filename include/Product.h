#ifndef PRODUCT_H
#define PRODUCT_H

#include <iostream>
#include <string>
#include "Supplier.h"

// Forward declaration for Friend Class (OOP Concept: Friend Class)
class FinancialManager;

// ============================================================================
// OOP CONCEPT: Abstract Base Class & Run-Time Polymorphism
// 1. Data Encapsulation & Protection (protected members accessible by derived classes)
// 2. Static Members (total active product counter across all objects)
// 3. Composition (Supplier has-a relationship inside Product)
// 4. Pure Virtual Functions (interface contract for derived classes)
// 5. Virtual Destructor (safe polymorphic cleanup)
// 6. Operator Overloading (<< and ==)
// 7. Friend Class (FinancialManager can access internal cost metrics)
// ============================================================================

class Product {
protected:
    int id;
    std::string name;
    std::string category;
    int quantity;
    double costPrice;       // Purchase price paid to supplier
    double sellingPrice;    // Retail price charged to customer
    int reorderThreshold;   // Low-stock alert threshold
    Supplier supplier;      // Composition: Product HAS-A Supplier

    // Static Data Member (shared by all instances)
    static int totalProducts;

public:
    // Friend Class Declaration
    friend class FinancialManager;

    // Default Constructor
    Product();

    // Parameterized Constructor
    Product(int id, const std::string& name, const std::string& category,
            int quantity, double costPrice, double sellingPrice,
            int reorderThreshold, const Supplier& supplier);

    // Virtual Destructor (Crucial for polymorphic base classes)
    virtual ~Product();

    // Static Member Function
    static int getTotalProducts();

    // Pure Virtual Functions (Makes this class Abstract)
    virtual void displayDetails() const = 0;
    virtual std::string getProductType() const = 0;
    virtual std::string serialize() const = 0;

    // Virtual Function (Can be overridden by derived classes)
    virtual double calculateDiscountedPrice(double discountPercent) const;

    // Regular Concrete Methods
    double calculateStockValue() const;        // Value at selling price
    double calculateCostValue() const;         // Value at cost price
    bool isLowStock() const;
    void addStock(int amount);
    void deductStock(int amount);

    // Getters
    int getId() const;
    std::string getName() const;
    std::string getCategory() const;
    int getQuantity() const;
    double getCostPrice() const;
    double getSellingPrice() const;
    int getReorderThreshold() const;
    Supplier getSupplier() const;

    // Setters (with validation)
    void setId(int id);
    void setName(const std::string& name);
    void setCategory(const std::string& category);
    void setQuantity(int quantity);
    void setCostPrice(double price);
    void setSellingPrice(double price);
    void setReorderThreshold(int threshold);
    void setSupplier(const Supplier& supplier);

    // Operator Overloading
    bool operator==(const Product& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Product& p);
};

// ============================================================================
// OOP CONCEPT: Inheritance (Derived Class 1: PerishableProduct)
// Hierarchical Inheritance from Product
// Represents: Food, Dairy, Pharmaceuticals with Expiry & Storage Temperature
// ============================================================================

class PerishableProduct : public Product {
private:
    std::string expiryDate;             // Format: YYYY-MM-DD
    double storageTempCelsius;          // Required storage temperature

public:
    PerishableProduct();
    PerishableProduct(int id, const std::string& name, const std::string& category,
                      int quantity, double costPrice, double sellingPrice,
                      int reorderThreshold, const Supplier& supplier,
                      const std::string& expiryDate, double storageTemp);

    virtual ~PerishableProduct() override = default;

    // Polymorphic Overrides
    virtual void displayDetails() const override;
    virtual std::string getProductType() const override;
    virtual std::string serialize() const override;
    virtual double calculateDiscountedPrice(double discountPercent) const override;

    // Specific Getters & Setters
    std::string getExpiryDate() const;
    double getStorageTempCelsius() const;
    void setExpiryDate(const std::string& date);
    void setStorageTempCelsius(double temp);
};

// ============================================================================
// OOP CONCEPT: Inheritance (Derived Class 2: ElectronicsProduct)
// Hierarchical Inheritance from Product
// Represents: Tech gadgets, Appliances, Hardware with Warranty & Brand
// ============================================================================

class ElectronicsProduct : public Product {
private:
    int warrantyMonths;
    std::string brandName;

public:
    ElectronicsProduct();
    ElectronicsProduct(int id, const std::string& name, const std::string& category,
                       int quantity, double costPrice, double sellingPrice,
                       int reorderThreshold, const Supplier& supplier,
                       int warrantyMonths, const std::string& brandName);

    virtual ~ElectronicsProduct() override = default;

    // Polymorphic Overrides
    virtual void displayDetails() const override;
    virtual std::string getProductType() const override;
    virtual std::string serialize() const override;
    virtual double calculateDiscountedPrice(double discountPercent) const override;

    // Specific Getters & Setters
    int getWarrantyMonths() const;
    std::string getBrandName() const;
    void setWarrantyMonths(int months);
    void setBrandName(const std::string& brand);
};

#endif // PRODUCT_H
