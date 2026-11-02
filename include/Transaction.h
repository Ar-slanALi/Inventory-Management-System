#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <string>

// ============================================================================
// OOP CONCEPT: Class Encapsulation, Static Members, and Operator Overloading
// Records individual inventory events (Stock-In, Sales/Billing, Adjustments)
// Demonstrates:
// 1. Static auto-incrementing transaction ID counter
// 2. Formatted billing / receipt generation
// 3. File serialization
// ============================================================================

class Transaction {
private:
    int transactionId;
    int productId;
    std::string productName;
    std::string type;          // "STOCK_IN", "SALE", "RETURN"
    int quantity;
    double unitPrice;
    double totalAmount;
    std::string performedBy;   // Username of Admin or Staff member
    std::string timestamp;

    static int nextTransactionId;

public:
    // Constructors
    Transaction();
    Transaction(int prodId, const std::string& prodName, const std::string& type,
                int qty, double unitPrice, const std::string& performedBy,
                const std::string& timestamp);

    // Static ID management
    static int getNextTransactionId();
    static void setNextTransactionId(int nextId);

    // Getters
    int getTransactionId() const;
    int getProductId() const;
    std::string getProductName() const;
    std::string getType() const;
    int getQuantity() const;
    double getUnitPrice() const;
    double getTotalAmount() const;
    std::string getPerformedBy() const;
    std::string getTimestamp() const;

    // Display & Receipt Generation
    void printReceipt() const;
    void displaySummary() const;

    // File Serialization
    std::string serialize() const;
    static Transaction deserialize(const std::string& line);

    // Operator Overloading
    friend std::ostream& operator<<(std::ostream& os, const Transaction& t);
};

#endif // TRANSACTION_H
