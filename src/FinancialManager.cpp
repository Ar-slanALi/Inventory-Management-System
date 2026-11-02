#include "../include/FinancialManager.h"
#include "../include/Product.h"
#include <iomanip>
#include <sstream>

FinancialManager::FinancialManager()
    : totalRevenue(0.0), totalCostOfGoodsSold(0.0), totalProcurementCost(0.0), totalItemsSold(0) {}

FinancialManager::FinancialManager(double revenue, double cogs, double procurement, int itemsSold)
    : totalRevenue(revenue), totalCostOfGoodsSold(cogs),
      totalProcurementCost(procurement), totalItemsSold(itemsSold) {}

// Notice: Because FinancialManager is a friend of Product, it can access product.costPrice and product.sellingPrice directly!
void FinancialManager::recordSale(const Product& product, int quantitySold) {
    if (quantitySold <= 0) return;
    double revenue = quantitySold * product.sellingPrice;
    double cost = quantitySold * product.costPrice;

    totalRevenue += revenue;
    totalCostOfGoodsSold += cost;
    totalItemsSold += quantitySold;
}

void FinancialManager::recordStockIn(const Product& product, int quantityAdded) {
    if (quantityAdded <= 0) return;
    totalProcurementCost += (quantityAdded * product.costPrice);
}

double FinancialManager::getGrossProfit() const {
    return totalRevenue - totalCostOfGoodsSold;
}

double FinancialManager::getProfitMarginPercentage() const {
    if (totalRevenue <= 0.0) return 0.0;
    return (getGrossProfit() / totalRevenue) * 100.0;
}

double FinancialManager::getTotalRevenue() const { return totalRevenue; }
double FinancialManager::getTotalCostOfGoodsSold() const { return totalCostOfGoodsSold; }
double FinancialManager::getTotalProcurementCost() const { return totalProcurementCost; }
int FinancialManager::getTotalItemsSold() const { return totalItemsSold; }

void FinancialManager::displayFinancialReport(const std::vector<Product*>& activeInventory) const {
    double currentInventoryCostValue = 0.0;
    double currentInventoryRetailValue = 0.0;
    int currentTotalStock = 0;

    for (const auto* prod : activeInventory) {
        if (prod) {
            currentInventoryCostValue += prod->calculateCostValue();
            currentInventoryRetailValue += prod->calculateStockValue();
            currentTotalStock += prod->getQuantity();
        }
    }

    double potentialProfit = currentInventoryRetailValue - currentInventoryCostValue;

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
    std::cout << "    - Total Inventory Units in Stock:" << currentTotalStock << " units\n";
    std::cout << "    - Inventory Value (At Cost):     $" << std::fixed << std::setprecision(2) << currentInventoryCostValue << "\n";
    std::cout << "    - Inventory Value (At Retail):   $" << std::fixed << std::setprecision(2) << currentInventoryRetailValue << "\n";
    std::cout << "    - Projected Potential Profit:    $" << std::fixed << std::setprecision(2) << potentialProfit << "\n";
    std::cout << "---------------------------------------------------------------\n";
    std::cout << " 3. PROCUREMENT & EXPENSES:\n";
    std::cout << "    - Cumulative Procurement Spent:  $" << std::fixed << std::setprecision(2) << totalProcurementCost << "\n";
    std::cout << "===============================================================\n";
}

std::string FinancialManager::serialize() const {
    std::ostringstream ss;
    ss << totalRevenue << ";" << totalCostOfGoodsSold << ";"
       << totalProcurementCost << ";" << totalItemsSold;
    return ss.str();
}

void FinancialManager::deserialize(const std::string& line) {
    if (line.empty()) return;
    std::stringstream ss(line);
    std::string token;

    if (std::getline(ss, token, ';')) totalRevenue = std::stod(token);
    if (std::getline(ss, token, ';')) totalCostOfGoodsSold = std::stod(token);
    if (std::getline(ss, token, ';')) totalProcurementCost = std::stod(token);
    if (std::getline(ss, token, ';')) totalItemsSold = std::stoi(token);
}
