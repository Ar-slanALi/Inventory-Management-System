#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int MAX_PRODUCTS = 100;

int ids[MAX_PRODUCTS];
string names[MAX_PRODUCTS];
string categories[MAX_PRODUCTS];
string suppliers[MAX_PRODUCTS];
int quantities[MAX_PRODUCTS];
double purchasePrices[MAX_PRODUCTS];
double sellingPrices[MAX_PRODUCTS];
int productCount = 0;

string adminUser = "";
string adminPass = "";

const string PRODUCTS_FILE = "data/products.dat";
const string ADMIN_FILE = "data/admin.dat";
const string LOGS_FILE = "data/logs.txt";
const string REPORTS_FILE = "data/reports.txt";
const string BACKUP_FILE = "data/backup.dat";

void showMenu();
int getMenuChoice();
void addProduct();
void viewProducts();
void searchProduct();
void updateProduct();
void deleteProduct();
void stockIn();
void stockOut();
void generateReport();
void sortProducts();
void lowStockAlert();
void backupData();

bool login();
void createDefaultAdmin();
void saveData();
void loadData();
void saveAdmin();
void loadAdmin();
void writeLog(const string& message);
bool isDuplicateID(int id);
int findProductIndex(int id);

void clearScreen() {
    for (int i = 0; i < 25; i++) {
        cout << endl;
    }
}

void pause() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

void printLine(int length) {
    for (int i = 0; i < length; i++) {
        cout << "=";
    }
    cout << endl;
}

string getDateTime() {
    return "---";
}

int main() {
    clearScreen();

    cout << "\n\n";
    printLine(50);
    cout << "   INVENTORY MANAGEMENT SYSTEM (IMS)" << endl;
    cout << "   Programming Fundamentals Project" << endl;
    printLine(50);
    cout << "\n   Welcome to IMS!" << endl;
    cout << "   This system helps you manage your products." << endl;
    cout << "\n   Starting the system..." << endl;

    pause();

    loadData();
    createDefaultAdmin();
    loadAdmin();

    if (!login()) {
        cout << "\n   Login failed! Maximum attempts exceeded." << endl;
        cout << "   Exiting system..." << endl;
        pause();
        return 0;
    }

    int choice;

    do {
        showMenu();
        choice = getMenuChoice();

        switch (choice) {
            case 1:
                addProduct();
                break;
            case 2:
                viewProducts();
                break;
            case 3:
                searchProduct();
                break;
            case 4:
                updateProduct();
                break;
            case 5:
                deleteProduct();
                break;
            case 6:
                stockIn();
                break;
            case 7:
                stockOut();
                break;
            case 8:
                generateReport();
                break;
            case 9:
                sortProducts();
                break;
            case 10:
                lowStockAlert();
                break;
            case 11:
                backupData();
                break;
            case 12:
                saveData();
                writeLog("LOGOUT - User exited the system");
                clearScreen();
                cout << "\n\n";
                printLine(50);
                cout << "   Thank you for using IMS!" << endl;
                cout << "   Goodbye!" << endl;
                printLine(50);
                cout << "\n\n";
                break;
            default:
                cout << "\n   Invalid choice! Please enter a number between 1 and 12." << endl;
                pause();
        }

    } while (choice != 12);

    return 0;
}

bool login() {
    int attempts = 3;
    string username, password;

    while (attempts > 0) {
        clearScreen();
        cout << "\n";
        printLine(50);
        cout << "          ADMIN LOGIN" << endl;
        printLine(50);
        cout << "\n";
        cout << "   Attempts Remaining: " << attempts << endl;
        cout << "\n";
        cout << "   Username: ";
        getline(cin, username);
        cout << "   Password: ";
        getline(cin, password);

        if (username == adminUser && password == adminPass) {
            cout << "\n   Login Successful! Welcome, " << adminUser << "!" << endl;
            writeLog("LOGIN SUCCESS - User: " + adminUser);
            pause();
            return true;
        } else {
            cout << "\n   Invalid username or password!" << endl;
            attempts--;
            writeLog("LOGIN FAILED - Attempted Username: " + username);
            pause();
        }
    }

    writeLog("LOGIN BLOCKED - Maximum attempts exceeded");
    return false;
}

void createDefaultAdmin() {
    ifstream checkFile(ADMIN_FILE);
    if (checkFile.good()) {
        checkFile.close();
        return;
    }
    checkFile.close();

    adminUser = "admin";
    adminPass = "admin123";
    saveAdmin();
}

void saveAdmin() {
    ofstream file(ADMIN_FILE);
    if (file.is_open()) {
        file << adminUser << endl;
        file << adminPass << endl;
        file.close();
    }
}

void loadAdmin() {
    ifstream file(ADMIN_FILE);
    if (file.is_open()) {
        getline(file, adminUser);
        getline(file, adminPass);
        file.close();
    }
}

void saveData() {
    ofstream file(PRODUCTS_FILE);
    if (file.is_open()) {
        file << productCount << endl;
        for (int i = 0; i < productCount; i++) {
            file << ids[i] << endl;
            file << names[i] << endl;
            file << categories[i] << endl;
            file << suppliers[i] << endl;
            file << quantities[i] << endl;
            file << purchasePrices[i] << endl;
            file << sellingPrices[i] << endl;
        }
        file.close();
    } else {
        cout << "\n   ERROR: Could not save data to file!" << endl;
    }
}

void loadData() {
    ifstream file(PRODUCTS_FILE);
    if (file.is_open()) {
        file >> productCount;
        if (productCount > 0 && productCount <= MAX_PRODUCTS) {
            for (int i = 0; i < productCount; i++) {
                file >> ids[i];
                file.ignore();
                getline(file, names[i]);
                getline(file, categories[i]);
                getline(file, suppliers[i]);
                file >> quantities[i];
                file >> purchasePrices[i];
                file >> sellingPrices[i];
                file.ignore();
            }
        } else {
            productCount = 0;
        }
        file.close();
    } else {
        productCount = 0;
    }
}

void writeLog(const string& message) {
    ofstream file(LOGS_FILE, ios::app);
    if (file.is_open()) {
        file << "[" << getDateTime() << "] " << message << endl;
        file.close();
    }
}

bool isDuplicateID(int id) {
    for (int i = 0; i < productCount; i++) {
        if (ids[i] == id) {
            return true;
        }
    }
    return false;
}

int findProductIndex(int id) {
    for (int i = 0; i < productCount; i++) {
        if (ids[i] == id) {
            return i;
        }
    }
    return -1;
}

void addProduct() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      ADD NEW PRODUCT" << endl;
    printLine(50);

    if (productCount >= MAX_PRODUCTS) {
        cout << "\n   ERROR: Product limit reached! Cannot add more products." << endl;
        pause();
        return;
    }

    int newId;
    cout << "\n   Enter Product ID: ";
    cin >> newId;

    if (isDuplicateID(newId)) {
        cout << "\n   ERROR: Product ID " << newId << " already exists!" << endl;
        cin.clear();
        cin.ignore(10000, '\n');
        pause();
        return;
    }

    cin.ignore(10000, '\n');

    int pos = productCount;

    ids[pos] = newId;

    cout << "   Enter Product Name: ";
    getline(cin, names[pos]);

    cout << "   Enter Category (e.g., Electronics, Grocery): ";
    getline(cin, categories[pos]);

    cout << "   Enter Supplier Name: ";
    getline(cin, suppliers[pos]);

    cout << "   Enter Quantity: ";
    cin >> quantities[pos];
    while (quantities[pos] < 0) {
        cout << "   Quantity cannot be negative. Enter again: ";
        cin >> quantities[pos];
    }

    cout << "   Enter Purchase Price (Rs.): ";
    cin >> purchasePrices[pos];
    while (purchasePrices[pos] < 0) {
        cout << "   Price cannot be negative. Enter again: ";
        cin >> purchasePrices[pos];
    }

    cout << "   Enter Selling Price (Rs.): ";
    cin >> sellingPrices[pos];
    while (sellingPrices[pos] < 0) {
        cout << "   Price cannot be negative. Enter again: ";
        cin >> sellingPrices[pos];
    }

    cin.ignore(10000, '\n');

    productCount++;

    saveData();

    writeLog("PRODUCT ADDED - ID: " + to_string(ids[pos]) + ", Name: " + names[pos]);

    cout << "\n   Product added successfully!" << endl;
    printLine(50);
    pause();
}

void viewProducts() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      VIEW ALL PRODUCTS" << endl;
    printLine(50);

    if (productCount == 0) {
        cout << "\n   No products in inventory." << endl;
        pause();
        return;
    }

    cout << "\n";
    cout << "   ID\t\tName\t\t\tCategory\tQty\tPurch.Price\tSell.Price" << endl;
    printLine(85);

    for (int i = 0; i < productCount; i++) {
        cout << "   " << ids[i]
             << "\t\t" << names[i]
             << "\t\t" << categories[i]
             << "\t" << quantities[i]
             << "\tRs." << purchasePrices[i]
             << "\t\tRs." << sellingPrices[i] << endl;
    }

    printLine(85);
    cout << "   Total Products: " << productCount << endl;

    pause();
}

void searchProduct() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      SEARCH PRODUCT" << endl;
    printLine(50);

    if (productCount == 0) {
        cout << "\n   No products in inventory to search." << endl;
        pause();
        return;
    }

    int searchChoice;
    cout << "\n   Search by:" << endl;
    cout << "   1. Product ID" << endl;
    cout << "   2. Product Name" << endl;
    cout << "   Enter choice: ";
    cin >> searchChoice;
    cin.ignore(10000, '\n');

    if (searchChoice == 1) {
        int searchID;
        cout << "\n   Enter Product ID to search: ";
        cin >> searchID;
        cin.ignore(10000, '\n');

        int index = findProductIndex(searchID);
        if (index != -1) {
            cout << "\n   Product Found!" << endl;
            printLine(40);
            cout << "   ID:            " << ids[index] << endl;
            cout << "   Name:          " << names[index] << endl;
            cout << "   Category:      " << categories[index] << endl;
            cout << "   Supplier:      " << suppliers[index] << endl;
            cout << "   Quantity:      " << quantities[index] << endl;
            cout << "   Purchase Price: Rs." << purchasePrices[index] << endl;
            cout << "   Selling Price:  Rs." << sellingPrices[index] << endl;
            printLine(40);
        } else {
            cout << "\n   Product with ID " << searchID << " not found!" << endl;
        }

    } else if (searchChoice == 2) {
        string searchName;
        cout << "\n   Enter Product Name to search: ";
        getline(cin, searchName);

        bool found = false;
        for (int i = 0; i < productCount; i++) {
            if (names[i].find(searchName) != string::npos) {
                if (!found) {
                    cout << "\n   Matching Products:" << endl;
                    printLine(85);
                    cout << "   ID\t\tName\t\t\tCategory\tQty\tPurch.Price\tSell.Price" << endl;
                    printLine(85);
                    found = true;
                }
                cout << "   " << ids[i]
                     << "\t\t" << names[i]
                     << "\t\t" << categories[i]
                     << "\t" << quantities[i]
                     << "\tRs." << purchasePrices[i]
                     << "\t\tRs." << sellingPrices[i] << endl;
            }
        }
        if (!found) {
            cout << "\n   No products found matching '" << searchName << "'!" << endl;
        } else {
            printLine(85);
        }
    } else {
        cout << "\n   Invalid search option!" << endl;
    }

    pause();
}

void updateProduct() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      UPDATE PRODUCT" << endl;
    printLine(50);

    if (productCount == 0) {
        cout << "\n   No products in inventory to update." << endl;
        pause();
        return;
    }

    int updateID;
    cout << "\n   Enter Product ID to update: ";
    cin >> updateID;
    cin.ignore(10000, '\n');

    int index = findProductIndex(updateID);
    if (index == -1) {
        cout << "\n   Product with ID " << updateID << " not found!" << endl;
        pause();
        return;
    }

    cout << "\n   Current Product Details:" << endl;
    printLine(40);
    cout << "   ID:            " << ids[index] << endl;
    cout << "   Name:          " << names[index] << endl;
    cout << "   Category:      " << categories[index] << endl;
    cout << "   Supplier:      " << suppliers[index] << endl;
    cout << "   Quantity:      " << quantities[index] << endl;
    cout << "   Purchase Price: Rs." << purchasePrices[index] << endl;
    cout << "   Selling Price:  Rs." << sellingPrices[index] << endl;
    printLine(40);

    int field;
    cout << "\n   Which field do you want to update?" << endl;
    cout << "   1. Name" << endl;
    cout << "   2. Category" << endl;
    cout << "   3. Supplier" << endl;
    cout << "   4. Quantity" << endl;
    cout << "   5. Purchase Price" << endl;
    cout << "   6. Selling Price" << endl;
    cout << "   Enter choice: ";
    cin >> field;
    cin.ignore(10000, '\n');

    switch (field) {
        case 1:
            cout << "   Enter new Name: ";
            getline(cin, names[index]);
            break;
        case 2:
            cout << "   Enter new Category: ";
            getline(cin, categories[index]);
            break;
        case 3:
            cout << "   Enter new Supplier: ";
            getline(cin, suppliers[index]);
            break;
        case 4:
            cout << "   Enter new Quantity: ";
            cin >> quantities[index];
            cin.ignore(10000, '\n');
            break;
        case 5:
            cout << "   Enter new Purchase Price: ";
            cin >> purchasePrices[index];
            cin.ignore(10000, '\n');
            break;
        case 6:
            cout << "   Enter new Selling Price: ";
            cin >> sellingPrices[index];
            cin.ignore(10000, '\n');
            break;
        default:
            cout << "\n   Invalid choice! No changes made." << endl;
            pause();
            return;
    }

    saveData();

    writeLog("PRODUCT UPDATED - ID: " + to_string(ids[index]) +
        ", Field: " + to_string(field));

    cout << "\n   Product updated successfully!" << endl;
    pause();
}

void deleteProduct() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      DELETE PRODUCT" << endl;
    printLine(50);

    if (productCount == 0) {
        cout << "\n   No products in inventory to delete." << endl;
        pause();
        return;
    }

    int deleteID;
    cout << "\n   Enter Product ID to delete: ";
    cin >> deleteID;
    cin.ignore(10000, '\n');

    int index = findProductIndex(deleteID);
    if (index == -1) {
        cout << "\n   Product with ID " << deleteID << " not found!" << endl;
        pause();
        return;
    }

    cout << "\n   Product Details:" << endl;
    printLine(40);
    cout << "   ID:            " << ids[index] << endl;
    cout << "   Name:          " << names[index] << endl;
    cout << "   Category:      " << categories[index] << endl;
    cout << "   Supplier:      " << suppliers[index] << endl;
    cout << "   Quantity:      " << quantities[index] << endl;
    cout << "   Purchase Price: Rs." << purchasePrices[index] << endl;
    cout << "   Selling Price:  Rs." << sellingPrices[index] << endl;
    printLine(40);

    char confirm;
    cout << "\n   Are you sure you want to delete this product? (y/n): ";
    cin >> confirm;
    cin.ignore(10000, '\n');

    if (confirm == 'y' || confirm == 'Y') {
        string logMsg = "PRODUCT DELETED - ID: " + to_string(ids[index]) +
            ", Name: " + names[index];

        for (int i = index; i < productCount - 1; i++) {
            ids[i] = ids[i + 1];
            names[i] = names[i + 1];
            categories[i] = categories[i + 1];
            suppliers[i] = suppliers[i + 1];
            quantities[i] = quantities[i + 1];
            purchasePrices[i] = purchasePrices[i + 1];
            sellingPrices[i] = sellingPrices[i + 1];
        }
        productCount--;

        saveData();
        writeLog(logMsg);

        cout << "\n   Product deleted successfully!" << endl;
    } else {
        cout << "\n   Deletion cancelled." << endl;
    }

    pause();
}

void stockIn() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      STOCK IN (Add Stock)" << endl;
    printLine(50);

    if (productCount == 0) {
        cout << "\n   No products in inventory." << endl;
        pause();
        return;
    }

    int id, qty;
    cout << "\n   Enter Product ID: ";
    cin >> id;

    int index = findProductIndex(id);
    if (index == -1) {
        cout << "\n   Product with ID " << id << " not found!" << endl;
        cin.ignore(10000, '\n');
        pause();
        return;
    }

    cout << "   Current Stock of '" << names[index] << "': " << quantities[index] << endl;
    cout << "   Enter quantity to add: ";
    cin >> qty;
    cin.ignore(10000, '\n');

    while (qty <= 0) {
        cout << "   Quantity must be positive. Enter again: ";
        cin >> qty;
        cin.ignore(10000, '\n');
    }

    quantities[index] += qty;
    saveData();

    writeLog("STOCK IN - ID: " + to_string(ids[index]) +
        ", Name: " + names[index] +
        ", Quantity Added: " + to_string(qty) +
        ", New Stock: " + to_string(quantities[index]));

    cout << "\n   Stock updated successfully!" << endl;
    cout << "   New Stock of '" << names[index] << "': " << quantities[index] << endl;
    pause();
}

void stockOut() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      STOCK OUT (Remove Stock)" << endl;
    printLine(50);

    if (productCount == 0) {
        cout << "\n   No products in inventory." << endl;
        pause();
        return;
    }

    int id, qty;
    cout << "\n   Enter Product ID: ";
    cin >> id;

    int index = findProductIndex(id);
    if (index == -1) {
        cout << "\n   Product with ID " << id << " not found!" << endl;
        cin.ignore(10000, '\n');
        pause();
        return;
    }

    cout << "   Current Stock of '" << names[index] << "': " << quantities[index] << endl;
    cout << "   Enter quantity to remove: ";
    cin >> qty;
    cin.ignore(10000, '\n');

    while (qty <= 0) {
        cout << "   Quantity must be positive. Enter again: ";
        cin >> qty;
        cin.ignore(10000, '\n');
    }

    if (qty > quantities[index]) {
        cout << "\n   ERROR: Not enough stock! Available: " << quantities[index] << endl;
        pause();
        return;
    }

    quantities[index] -= qty;
    saveData();

    writeLog("STOCK OUT - ID: " + to_string(ids[index]) +
        ", Name: " + names[index] +
        ", Quantity Removed: " + to_string(qty) +
        ", Remaining: " + to_string(quantities[index]));

    cout << "\n   Stock updated successfully!" << endl;
    cout << "   Remaining Stock of '" << names[index] << "': " << quantities[index] << endl;
    pause();
}

void generateReport() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      GENERATE REPORTS" << endl;
    printLine(50);

    if (productCount == 0) {
        cout << "\n   No products in inventory to generate report." << endl;
        pause();
        return;
    }

    int reportType;
    cout << "\n   Select Report Type:" << endl;
    cout << "   1. Full Inventory Report" << endl;
    cout << "   2. Low Stock Report (Qty < 5)" << endl;
    cout << "   3. Out of Stock Report (Qty = 0)" << endl;
    cout << "   Enter choice: ";
    cin >> reportType;
    cin.ignore(10000, '\n');

    clearScreen();
    cout << "\n";
    printLine(50);

    ofstream reportFile(REPORTS_FILE);

    if (reportType == 1) {
        cout << "      FULL INVENTORY REPORT" << endl;
        printLine(50);

        int totalItems = 0;
        double totalValue = 0;

        for (int i = 0; i < productCount; i++) {
            totalItems += quantities[i];
            totalValue += quantities[i] * sellingPrices[i];
        }

        cout << "\n   Date: " << getDateTime() << endl;
        cout << "   Total Products: " << productCount << endl;
        cout << "   Total Items in Stock: " << totalItems << endl;
        cout << "   Total Inventory Value: Rs." << totalValue << endl;

        if (reportFile.is_open()) {
            reportFile << "==============================================\n";
            reportFile << "        INVENTORY REPORT\n";
            reportFile << "        Generated: " << getDateTime() << "\n";
            reportFile << "==============================================\n\n";
            reportFile << "Total Products: " << productCount << "\n";
            reportFile << "Total Items in Stock: " << totalItems << "\n";
            reportFile << "Total Value: Rs." << totalValue << "\n\n";
            reportFile << "--- ALL PRODUCTS ---\n\n";

            for (int i = 0; i < productCount; i++) {
                reportFile << "ID: " << ids[i]
                           << " | " << names[i]
                           << " | Category: " << categories[i]
                           << " | Qty: " << quantities[i]
                           << " | Purchase: Rs." << purchasePrices[i]
                           << " | Selling: Rs." << sellingPrices[i] << "\n";
            }

            reportFile << "\n==============================================\n";
            reportFile << "         END OF REPORT\n";
            reportFile << "==============================================\n";
            reportFile.close();
        }

        cout << "\n   Products:" << endl;
        cout << "   ID\t\tName\t\t\tCategory\tQty\tPurch.Price\tSell.Price" << endl;
        printLine(85);

        for (int i = 0; i < productCount; i++) {
            cout << "   " << ids[i]
                 << "\t\t" << names[i]
                 << "\t\t" << categories[i]
                 << "\t" << quantities[i]
                 << "\tRs." << purchasePrices[i]
                 << "\t\tRs." << sellingPrices[i] << endl;
        }
        printLine(85);

    } else if (reportType == 2) {
        cout << "      LOW STOCK REPORT (Qty < 5)" << endl;
        printLine(50);
        cout << "\n   Date: " << getDateTime() << endl;

        int lowCount = 0;

        if (reportFile.is_open()) {
            reportFile << "==============================================\n";
            reportFile << "        LOW STOCK REPORT\n";
            reportFile << "        Generated: " << getDateTime() << "\n";
            reportFile << "==============================================\n\n";
        }

        for (int i = 0; i < productCount; i++) {
            if (quantities[i] > 0 && quantities[i] < 5) {
                cout << "   [!] ID: " << ids[i]
                     << " | " << names[i]
                     << " | Qty: " << quantities[i] << endl;
                if (reportFile.is_open()) {
                    reportFile << "ID: " << ids[i]
                               << " | " << names[i]
                               << " | Qty: " << quantities[i] << "\n";
                }
                lowCount++;
            }
        }

        if (lowCount == 0) {
            cout << "\n   No low stock items found." << endl;
        } else {
            cout << "\n   Total Low Stock Items: " << lowCount << endl;
        }

        if (reportFile.is_open()) {
            reportFile << "\nTotal Low Stock Items: " << lowCount << "\n";
            reportFile << "\n==============================================\n";
            reportFile << "         END OF REPORT\n";
            reportFile << "==============================================\n";
            reportFile.close();
        }

    } else if (reportType == 3) {
        cout << "      OUT OF STOCK REPORT (Qty = 0)" << endl;
        printLine(50);
        cout << "\n   Date: " << getDateTime() << endl;

        int outCount = 0;

        if (reportFile.is_open()) {
            reportFile << "==============================================\n";
            reportFile << "        OUT OF STOCK REPORT\n";
            reportFile << "        Generated: " << getDateTime() << "\n";
            reportFile << "==============================================\n\n";
        }

        for (int i = 0; i < productCount; i++) {
            if (quantities[i] == 0) {
                cout << "   [X] ID: " << ids[i]
                     << " | " << names[i]
                     << " | Qty: 0" << endl;
                if (reportFile.is_open()) {
                    reportFile << "ID: " << ids[i]
                               << " | " << names[i]
                               << " | Qty: 0\n";
                }
                outCount++;
            }
        }

        if (outCount == 0) {
            cout << "\n   No out of stock items found." << endl;
        } else {
            cout << "\n   Total Out of Stock Items: " << outCount << endl;
        }

        if (reportFile.is_open()) {
            reportFile << "\nTotal Out of Stock Items: " << outCount << "\n";
            reportFile << "\n==============================================\n";
            reportFile << "         END OF REPORT\n";
            reportFile << "==============================================\n";
            reportFile.close();
        }

    } else {
        cout << "\n   Invalid report type!" << endl;
    }

    writeLog("REPORT GENERATED");
    cout << "\n   Report saved to " << REPORTS_FILE << endl;
    pause();
}

void sortProducts() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      SORT PRODUCTS" << endl;
    printLine(50);

    if (productCount == 0) {
        cout << "\n   No products in inventory to sort." << endl;
        pause();
        return;
    }

    int sortChoice;
    cout << "\n   Sort by:" << endl;
    cout << "   1. Name (A-Z)" << endl;
    cout << "   2. Selling Price (Low to High)" << endl;
    cout << "   3. Quantity (Low to High)" << endl;
    cout << "   Enter choice: ";
    cin >> sortChoice;
    cin.ignore(10000, '\n');

    if (sortChoice == 1) {
        for (int i = 0; i < productCount - 1; i++) {
            for (int j = 0; j < productCount - i - 1; j++) {
                if (names[j] > names[j + 1]) {
                    int t1 = ids[j]; ids[j] = ids[j+1]; ids[j+1] = t1;
                    string t2 = names[j]; names[j] = names[j+1]; names[j+1] = t2;
                    string t3 = categories[j]; categories[j] = categories[j+1]; categories[j+1] = t3;
                    string t4 = suppliers[j]; suppliers[j] = suppliers[j+1]; suppliers[j+1] = t4;
                    int t5 = quantities[j]; quantities[j] = quantities[j+1]; quantities[j+1] = t5;
                    double t6 = purchasePrices[j]; purchasePrices[j] = purchasePrices[j+1]; purchasePrices[j+1] = t6;
                    double t7 = sellingPrices[j]; sellingPrices[j] = sellingPrices[j+1]; sellingPrices[j+1] = t7;
                }
            }
        }
        cout << "\n   Products sorted by Name (A-Z)!" << endl;
        writeLog("PRODUCTS SORTED - By Name");

    } else if (sortChoice == 2) {
        for (int i = 0; i < productCount - 1; i++) {
            for (int j = 0; j < productCount - i - 1; j++) {
                if (sellingPrices[j] > sellingPrices[j + 1]) {
                    int t1 = ids[j]; ids[j] = ids[j+1]; ids[j+1] = t1;
                    string t2 = names[j]; names[j] = names[j+1]; names[j+1] = t2;
                    string t3 = categories[j]; categories[j] = categories[j+1]; categories[j+1] = t3;
                    string t4 = suppliers[j]; suppliers[j] = suppliers[j+1]; suppliers[j+1] = t4;
                    int t5 = quantities[j]; quantities[j] = quantities[j+1]; quantities[j+1] = t5;
                    double t6 = purchasePrices[j]; purchasePrices[j] = purchasePrices[j+1]; purchasePrices[j+1] = t6;
                    double t7 = sellingPrices[j]; sellingPrices[j] = sellingPrices[j+1]; sellingPrices[j+1] = t7;
                }
            }
        }
        cout << "\n   Products sorted by Selling Price (Low to High)!" << endl;
        writeLog("PRODUCTS SORTED - By Price");

    } else if (sortChoice == 3) {
        for (int i = 0; i < productCount - 1; i++) {
            for (int j = 0; j < productCount - i - 1; j++) {
                if (quantities[j] > quantities[j + 1]) {
                    int t1 = ids[j]; ids[j] = ids[j+1]; ids[j+1] = t1;
                    string t2 = names[j]; names[j] = names[j+1]; names[j+1] = t2;
                    string t3 = categories[j]; categories[j] = categories[j+1]; categories[j+1] = t3;
                    string t4 = suppliers[j]; suppliers[j] = suppliers[j+1]; suppliers[j+1] = t4;
                    int t5 = quantities[j]; quantities[j] = quantities[j+1]; quantities[j+1] = t5;
                    double t6 = purchasePrices[j]; purchasePrices[j] = purchasePrices[j+1]; purchasePrices[j+1] = t6;
                    double t7 = sellingPrices[j]; sellingPrices[j] = sellingPrices[j+1]; sellingPrices[j+1] = t7;
                }
            }
        }
        cout << "\n   Products sorted by Quantity (Low to High)!" << endl;
        writeLog("PRODUCTS SORTED - By Quantity");

    } else {
        cout << "\n   Invalid choice!" << endl;
        pause();
        return;
    }

    saveData();

    cout << "\n   Sorted Products:" << endl;
    cout << "   ID\t\tName\t\t\tCategory\tQty\tPurch.Price\tSell.Price" << endl;
    printLine(85);

    for (int i = 0; i < productCount; i++) {
        cout << "   " << ids[i]
             << "\t\t" << names[i]
             << "\t\t" << categories[i]
             << "\t" << quantities[i]
             << "\tRs." << purchasePrices[i]
             << "\t\tRs." << sellingPrices[i] << endl;
    }
    printLine(85);

    pause();
}

void lowStockAlert() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      LOW STOCK ALERT" << endl;
    printLine(50);

    if (productCount == 0) {
        cout << "\n   No products in inventory." << endl;
        pause();
        return;
    }

    int alertCount = 0;

    cout << "\n   Products with LOW STOCK (Quantity < 5):" << endl;
    cout << "\n";

    for (int i = 0; i < productCount; i++) {
        if (quantities[i] < 5) {
            alertCount++;
            if (quantities[i] == 0) {
                cout << "   [OUT OF STOCK] ID: " << ids[i]
                     << " | " << names[i]
                     << " | Qty: " << quantities[i] << endl;
            } else {
                cout << "   [LOW STOCK]    ID: " << ids[i]
                     << " | " << names[i]
                     << " | Qty: " << quantities[i] << endl;
            }
        }
    }

    if (alertCount == 0) {
        cout << "   All products have sufficient stock!" << endl;
    } else {
        cout << "\n   Total Alerts: " << alertCount << " product(s) need attention." << endl;
    }

    pause();
}

void backupData() {
    clearScreen();
    cout << "\n";
    printLine(50);
    cout << "      BACKUP DATA" << endl;
    printLine(50);

    saveData();

    ifstream source(PRODUCTS_FILE);
    if (!source.is_open()) {
        cout << "\n   ERROR: No data file found to backup!" << endl;
        pause();
        return;
    }

    ofstream dest(BACKUP_FILE);
    if (!dest.is_open()) {
        cout << "\n   ERROR: Could not create backup file!" << endl;
        source.close();
        pause();
        return;
    }

    string line;
    while (getline(source, line)) {
        dest << line << endl;
    }

    source.close();
    dest.close();

    writeLog("BACKUP CREATED");

    cout << "\n   Backup created successfully!" << endl;
    cout << "   Data backed up to: " << BACKUP_FILE << endl;
    cout << "   Products Backed Up: " << productCount << endl;
    pause();
}

void showMenu() {
    clearScreen();

    cout << "\n";
    printLine(50);
    cout << "      INVENTORY MANAGEMENT SYSTEM" << endl;
    printLine(50);
    cout << "\n";
    cout << "   1.  Add Product" << endl;
    cout << "   2.  View Products" << endl;
    cout << "   3.  Search Product" << endl;
    cout << "   4.  Update Product" << endl;
    cout << "   5.  Delete Product" << endl;
    cout << "   6.  Stock In" << endl;
    cout << "   7.  Stock Out" << endl;
    cout << "   8.  Generate Reports" << endl;
    cout << "   9.  Sort Products" << endl;
    cout << "  10.  Low Stock Alert" << endl;
    cout << "  11.  Backup Data" << endl;
    cout << "  12.  Exit" << endl;
    cout << "\n";
    printLine(50);
}

int getMenuChoice() {
    int choice;
    cout << "   Enter your choice (1-12): ";
    cin >> choice;

    cin.clear();
    cin.ignore(10000, '\n');

    return choice;
}
