#ifndef FINANCIAL_MANAGER_H
#define FINANCIAL_MANAGER_H

#include <iostream>
#include <string>
#include <vector>

// Forward declaration
class Product;

// ============================================================================
// OOP CONCEPT: Friend Class & Business Domain Modeling
// FinancialManager is declared as a friend of Product, granting access
// to product pricing and internal cost parameters for financial auditing.
// Demonstrates:
// 1. Friend Class utilization
// 2. Financial metrics calculation (Revenue, COGS, Profit, Margin)
// 3. Inventory valuation reporting
// ============================================================================

class FinancialManager {
private:
    double totalRevenue;          // Cumulative sales revenue
    double totalCostOfGoodsSold;  // Total purchase cost of items sold
    double totalProcurementCost;  // Total money spent restocking inventory
    int totalItemsSold;

public:
    FinancialManager();
    FinancialManager(double revenue, double cogs, double procurement, int itemsSold);

    // Friend relationship allows inspecting Product internals
    void recordSale(const Product& product, int quantitySold);
    void recordStockIn(const Product& product, int quantityAdded);

    // Financial Analysis
    double getGrossProfit() const;
    double getProfitMarginPercentage() const;
    double getTotalRevenue() const;
    double getTotalCostOfGoodsSold() const;
    double getTotalProcurementCost() const;
    int getTotalItemsSold() const;

    // Reporting
    void displayFinancialReport(const std::vector<Product*>& activeInventory) const;

    // Persistence
    std::string serialize() const;
    void deserialize(const std::string& line);
};

#endif // FINANCIAL_MANAGER_H
