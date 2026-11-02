#include "../include/Transaction.h"
#include <iomanip>
#include <sstream>

int Transaction::nextTransactionId = 1001;

int Transaction::getNextTransactionId() {
    return nextTransactionId;
}

void Transaction::setNextTransactionId(int nextId) {
    if (nextId > nextTransactionId) {
        nextTransactionId = nextId;
    }
}

Transaction::Transaction()
    : transactionId(0), productId(0), productName("Unknown"),
      type("NONE"), quantity(0), unitPrice(0.0), totalAmount(0.0),
      performedBy("system"), timestamp("N/A") {}

Transaction::Transaction(int prodId, const std::string& prodName, const std::string& type,
                         int qty, double unitPrice, const std::string& performedBy,
                         const std::string& timestamp)
    : transactionId(nextTransactionId++), productId(prodId), productName(prodName),
      type(type), quantity(qty), unitPrice(unitPrice),
      totalAmount(qty * unitPrice), performedBy(performedBy), timestamp(timestamp) {}

int Transaction::getTransactionId() const { return transactionId; }
int Transaction::getProductId() const { return productId; }
std::string Transaction::getProductName() const { return productName; }
std::string Transaction::getType() const { return type; }
int Transaction::getQuantity() const { return quantity; }
double Transaction::getUnitPrice() const { return unitPrice; }
double Transaction::getTotalAmount() const { return totalAmount; }
std::string Transaction::getPerformedBy() const { return performedBy; }
std::string Transaction::getTimestamp() const { return timestamp; }

void Transaction::printReceipt() const {
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

void Transaction::displaySummary() const {
    std::cout << "| " << std::setw(6) << transactionId
              << " | " << std::setw(8) << productId
              << " | " << std::setw(20) << std::left << (productName.length() > 20 ? productName.substr(0, 17) + "..." : productName)
              << " | " << std::setw(10) << std::left << type
              << " | " << std::setw(5) << std::right << quantity
              << " | $" << std::setw(9) << std::fixed << std::setprecision(2) << totalAmount
              << " | " << std::setw(10) << std::left << performedBy
              << " | " << std::setw(19) << timestamp << " |\n";
}

std::string Transaction::serialize() const {
    std::ostringstream ss;
    ss << transactionId << ";" << productId << ";" << productName << ";"
       << type << ";" << quantity << ";" << unitPrice << ";"
       << totalAmount << ";" << performedBy << ";" << timestamp;
    return ss.str();
}

Transaction Transaction::deserialize(const std::string& line) {
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

    if (t.transactionId >= nextTransactionId) {
        nextTransactionId = t.transactionId + 1;
    }
    return t;
}

std::ostream& operator<<(std::ostream& os, const Transaction& t) {
    os << "Txn #" << t.transactionId << " [" << t.type << "] Product ID: " << t.productId
       << " Qty: " << t.quantity << " Total: $" << std::fixed << std::setprecision(2) << t.totalAmount;
    return os;
}
