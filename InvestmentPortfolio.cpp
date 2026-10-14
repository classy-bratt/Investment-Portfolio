#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <cctype>

using namespace std;

// ---------- HELPER FUNCTIONS ----------

string lowerText(string text) {
    for (int i = 0; i < (int)text.size(); i++)
        text[i] = (char)tolower((unsigned char)text[i]);
    return text;
}

string readText(string prompt) {
    string value;
    cout << prompt;
    getline(cin, value);

    if (value.empty())
        throw invalid_argument("Input cannot be empty.");

    return value;
}

int readInt(string prompt) {
    int value;
    cout << prompt;

    if (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Enter a valid whole number.");
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

double readNumber(string prompt) {
    double value;
    cout << prompt;

    if (!(cin >> value) || value <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Enter a number greater than zero.");
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

void line(char symbol = '=', int length = 100) {
    for (int i = 0; i < length; i++)
        cout << symbol;
    cout << '\n';
}

// ---------- TRANSACTION CLASS ----------

class Transaction {
private:
    int transactionId;
    int investmentId;
    string action;
    string investmentName;
    double quantity;
    double price;
    double total;

public:
    Transaction(int tid, int iid, string act, string name,
                double qty, double p) {
        transactionId = tid;
        investmentId = iid;
        action = act;
        investmentName = name;
        quantity = qty;
        price = p;
        total = qty * p;
    }

    void display() const {
        cout << left
             << setw(8) << transactionId
             << setw(8) << investmentId
             << setw(10) << action
             << setw(22) << investmentName
             << right << setw(10) << fixed << setprecision(2) << quantity
             << setw(15) << price
             << setw(17) << total << '\n';
    }
};

// ---------- BASE CLASS ----------

class AssetRecord {
protected:
    int id;
    string name;
    string type;
    double quantity;
    double averagePrice;
    double currentPrice;

public:
    AssetRecord(int i, string n, string t, double q,
                double avg, double current) {
        id = i;
        name = n;
        type = t;
        quantity = q;
        averagePrice = avg;
        currentPrice = current;
    }

    virtual void display() const = 0;
    virtual ~AssetRecord() {}

    int getId() const { return id; }
    string getName() const { return name; }
    string getType() const { return type; }
    double getQuantity() const { return quantity; }
    double getAveragePrice() const { return averagePrice; }
    double getCurrentPrice() const { return currentPrice; }

    double investedValue() const {
        return quantity * averagePrice;
    }

    double currentValue() const {
        return quantity * currentPrice;
    }

    double profitLoss() const {
        return currentValue() - investedValue();
    }

    double returnPercentage() const {
        if (investedValue() == 0)
            return 0;
        return profitLoss() * 100.0 / investedValue();
    }
};

// ---------- DERIVED CLASS ----------

class Investment : public AssetRecord {
public:
    Investment(int i, string n, string t, double q,
               double avg, double current)
        : AssetRecord(i, n, t, q, avg, current) {}

    void display() const override {
        cout << left
             << setw(6) << id
             << setw(21) << name
             << setw(15) << type
             << right << setw(10) << fixed << setprecision(2) << quantity
             << setw(14) << averagePrice
             << setw(14) << currentPrice
             << setw(16) << currentValue()
             << setw(14) << profitLoss() << '\n';
    }

    void buy(double qty, double price) {
        double oldCost = quantity * averagePrice;
        double newCost = qty * price;
        quantity += qty;
        averagePrice = (oldCost + newCost) / quantity;
    }

    void sell(double qty) {
        if (qty <= 0)
            throw invalid_argument("Quantity must be greater than zero.");
        if (qty > quantity)
            throw invalid_argument("You cannot sell more units than you own.");
        quantity -= qty;
    }

    void updatePrice(double price) {
        if (price <= 0)
            throw invalid_argument("Price must be greater than zero.");
        currentPrice = price;
    }
};

// ---------- PORTFOLIO MANAGEMENT CLASS ----------

class PortfolioManager {
private:
    vector<Investment> investments;
    vector<Transaction> transactions;
    int nextInvestmentId;
    int nextTransactionId;

    int findInvestment(int id) const {
        for (int i = 0; i < (int)investments.size(); i++) {
            if (investments[i].getId() == id)
                return i;
        }
        return -1;
    }

    void printInvestmentHeader() const {
        line('-', 125);
        cout << left
             << setw(6) << "ID"
             << setw(21) << "NAME"
             << setw(15) << "ASSET TYPE"
             << right << setw(10) << "UNITS"
             << setw(14) << "AVG COST"
             << setw(14) << "MARKET PRICE"
             << setw(16) << "VALUE"
             << setw(14) << "P/L" << '\n';
        line('-', 125);
    }

    string chooseAssetType() {
        cout << "\nAsset Types\n";
        cout << "  1. Stock\n";
        cout << "  2. ETF\n";
        cout << "  3. Mutual Fund\n";
        cout << "  4. Bond\n";
        cout << "  5. Gold\n";
        cout << "  6. Cryptocurrency\n";
        cout << "  7. Other\n";

        int choice = readInt("Choose asset type: ");

        switch (choice) {
            case 1: return "Stock";
            case 2: return "ETF";
            case 3: return "Mutual Fund";
            case 4: return "Bond";
            case 5: return "Gold";
            case 6: return "Crypto";
            case 7: return "Other";
            default:
                throw invalid_argument("Choose an asset type from 1 to 7.");
        }
    }

public:
    PortfolioManager() {
        nextInvestmentId = 1001;
        nextTransactionId = 1;
    }

    void addInvestment() {
        cout << "\n========== ADD INVESTMENT ==========\n";
        string name = readText("Investment name or symbol: ");
        string type = chooseAssetType();
        double qty = readNumber("Quantity / units: ");
        double price = readNumber("Purchase price per unit (INR): ");

        Investment item(nextInvestmentId, name, type, qty, price, price);
        investments.push_back(item);
        transactions.push_back(Transaction(nextTransactionId, nextInvestmentId,
                                            "BUY", name, qty, price));

        cout << "\nInvestment added successfully!\n";
        cout << "Investment ID: " << nextInvestmentId << '\n';
        cout << "Initial value: INR " << fixed << setprecision(2)
             << qty * price << '\n';

        nextInvestmentId++;
        nextTransactionId++;
    }

    void buyInvestment() {
        cout << "\n========== BUY INVESTMENT ==========\n";
        if (investments.empty()) {
            cout << "No investments found. Add an investment first.\n";
            return;
        }

        int id = readInt("Enter investment ID: ");
        int index = findInvestment(id);
        if (index == -1) {
            cout << "Investment ID not found.\n";
            return;
        }

        double qty = readNumber("Additional quantity: ");
        double price = readNumber("Purchase price per unit (INR): ");
        investments[index].buy(qty, price);

        transactions.push_back(Transaction(nextTransactionId, id, "BUY",
            investments[index].getName(), qty, price));

        cout << "\nPurchase completed successfully.\n";
        cout << "Updated quantity: " << fixed << setprecision(2)
             << investments[index].getQuantity() << '\n';
        cout << "Updated average purchase price: INR "
             << investments[index].getAveragePrice() << '\n';

        nextTransactionId++;
    }

    void sellInvestment() {
        cout << "\n========== SELL INVESTMENT ==========\n";
        if (investments.empty()) {
            cout << "No investments available to sell.\n";
            return;
        }

        int id = readInt("Enter investment ID: ");
        int index = findInvestment(id);
        if (index == -1) {
            cout << "Investment ID not found.\n";
            return;
        }

        cout << "Investment: " << investments[index].getName() << '\n';
        cout << "Available units: " << fixed << setprecision(2)
             << investments[index].getQuantity() << '\n';

        double qty = readNumber("Quantity to sell: ");
        double price = readNumber("Selling price per unit (INR): ");

        if (qty > investments[index].getQuantity()) {
            cout << "Error: Insufficient units. Sale cancelled.\n";
            return;
        }

        string name = investments[index].getName();
        investments[index].sell(qty);
        transactions.push_back(Transaction(nextTransactionId, id, "SELL",
            name, qty, price));

        cout << "\nSale completed successfully.\n";
        cout << "Sale proceeds: INR " << fixed << setprecision(2)
             << qty * price << '\n';
        cout << "Remaining units: " << investments[index].getQuantity() << '\n';

        nextTransactionId++;
    }

    void viewInvestments() const {
        cout << "\n========== ALL INVESTMENTS ==========\n";
        if (investments.empty()) {
            cout << "Your portfolio is empty. Add your first investment.\n";
            return;
        }

        printInvestmentHeader();
        double invested = 0, current = 0;

        for (int i = 0; i < (int)investments.size(); i++) {
            const AssetRecord& record = investments[i];
            record.display();
            invested += investments[i].investedValue();
            current += investments[i].currentValue();
        }

        line('-', 125);
        cout << "Total invested: INR " << fixed << setprecision(2) << invested << '\n';
        cout << "Current portfolio value: INR " << current << '\n';
        cout << "Total P/L: INR " << current - invested << '\n';
    }

    void portfolioSummary() const {
        cout << "\n========== PORTFOLIO SUMMARY ==========\n";
        double invested = 0, current = 0;
        int profitable = 0, lossMaking = 0;

        for (int i = 0; i < (int)investments.size(); i++) {
            invested += investments[i].investedValue();
            current += investments[i].currentValue();

            if (investments[i].profitLoss() > 0)
                profitable++;
            else if (investments[i].profitLoss() < 0)
                lossMaking++;
        }

        double pl = current - invested;
        double returnPercent = invested > 0 ? pl * 100.0 / invested : 0;

        line();
        cout << "  Number of holdings : " << investments.size() << '\n';
        cout << "  Total invested     : INR " << fixed << setprecision(2) << invested << '\n';
        cout << "  Current value      : INR " << current << '\n';
        cout << "  Unrealized P/L     : INR " << pl << '\n';
        cout << "  Return             : " << returnPercent << "%\n";
        cout << "  Profitable holdings: " << profitable << '\n';
        cout << "  Loss-making        : " << lossMaking << '\n';
        line();
        cout << "Note: Values exclude fees, taxes and dividends.\n";
    }

    void searchInvestments() const {
        cout << "\n========== SEARCH INVESTMENTS ==========\n";
        string query = lowerText(readText("Search by ID, name, or asset type: "));
        bool found = false;

        for (int i = 0; i < (int)investments.size(); i++) {
            string idText = to_string(investments[i].getId());

            if (lowerText(investments[i].getName()).find(query) != string::npos ||
                lowerText(investments[i].getType()).find(query) != string::npos ||
                idText.find(query) != string::npos) {
                if (!found)
                    printInvestmentHeader();
                const AssetRecord& record = investments[i];
                record.display();
                found = true;
            }
        }

        if (!found)
            cout << "No matching investments found.\n";
    }

    void updatePrice() {
        cout << "\n========== UPDATE MARKET PRICE ==========\n";
        int id = readInt("Enter investment ID: ");
        int index = findInvestment(id);

        if (index == -1) {
            cout << "Investment ID not found.\n";
            return;
        }

        cout << "Investment: " << investments[index].getName() << '\n';
        cout << "Old price: INR " << fixed << setprecision(2)
             << investments[index].getCurrentPrice() << '\n';

        double price = readNumber("New market price per unit (INR): ");
        investments[index].updatePrice(price);

        cout << "Market price updated successfully.\n";
        cout << "New holding value: INR " << investments[index].currentValue() << '\n';
        cout << "Updated profit/loss: INR " << investments[index].profitLoss() << '\n';
    }

    void assetWiseReport() const {
        cout << "\n========== ASSET-WISE REPORT ==========\n";
        if (investments.empty()) {
            cout << "No investments available for a report.\n";
            return;
        }

        vector<string> types;
        vector<double> values;
        double grandTotal = 0;

        for (int i = 0; i < (int)investments.size(); i++) {
            string type = investments[i].getType();
            double value = investments[i].currentValue();
            grandTotal += value;

            bool found = false;
            for (int j = 0; j < (int)types.size(); j++) {
                if (types[j] == type) {
                    values[j] += value;
                    found = true;
                    break;
                }
            }

            if (!found) {
                types.push_back(type);
                values.push_back(value);
            }
        }

        for (int i = 0; i < (int)values.size(); i++) {
            for (int j = i + 1; j < (int)values.size(); j++) {
                if (values[j] > values[i]) {
                    double tempValue = values[i];
                    values[i] = values[j];
                    values[j] = tempValue;

                    string tempType = types[i];
                    types[i] = types[j];
                    types[j] = tempType;
                }
            }
        }

        cout << left << setw(25) << "ASSET TYPE"
             << right << setw(22) << "CURRENT VALUE"
             << setw(17) << "ALLOCATION" << '\n';
        line('-', 64);

        for (int i = 0; i < (int)types.size(); i++) {
            double percent = grandTotal > 0 ? values[i] * 100.0 / grandTotal : 0;
            cout << left << setw(25) << types[i]
                 << right << setw(22) << fixed << setprecision(2) << values[i]
                 << setw(16) << percent << "%\n";
        }

        line('-', 64);
        cout << left << setw(25) << "TOTAL"
             << right << setw(22) << fixed << setprecision(2) << grandTotal
             << setw(16) << "100.00%" << '\n';
    }

    void transactionHistory() const {
        cout << "\n========== TRANSACTION HISTORY ==========\n";
        if (transactions.empty()) {
            cout << "No transactions recorded.\n";
            return;
        }

        line('-', 90);
        cout << left
             << setw(8) << "TXN ID"
             << setw(8) << "ASSET"
             << setw(10) << "ACTION"
             << setw(22) << "NAME"
             << right << setw(10) << "QUANTITY"
             << setw(15) << "PRICE"
             << setw(17) << "TOTAL" << '\n';
        line('-', 90);

        for (int i = 0; i < (int)transactions.size(); i++)
            transactions[i].display();

        line('-', 90);
        cout << "Total transactions: " << transactions.size() << '\n';
    }

    void run() {
        while (true) {
            cout << "\n\n";
            line('=', 68);
            cout << "             INVESTMENT PORTFOLIO MANAGER\n";
            cout << "                PERSONAL WEALTH TRACKER\n";
            line('=', 68);
            cout << "  1. Add Investment\n";
            cout << "  2. View All Investments\n";
            cout << "  3. Buy Investment\n";
            cout << "  4. Sell Investment\n";
            cout << "  5. Portfolio Summary\n";
            cout << "  6. Search Investments\n";
            cout << "  7. Update Current Price\n";
            cout << "  8. Asset-wise Report\n";
            cout << "  9. Transaction History\n";
            cout << "  0. Exit\n";
            line('=', 68);

            try {
                int choice = readInt("Choose an option: ");

                switch (choice) {
                    case 1: addInvestment(); break;
                    case 2: viewInvestments(); break;
                    case 3: buyInvestment(); break;
                    case 4: sellInvestment(); break;
                    case 5: portfolioSummary(); break;
                    case 6: searchInvestments(); break;
                    case 7: updatePrice(); break;
                    case 8: assetWiseReport(); break;
                    case 9: transactionHistory(); break;
                    case 0:
                        cout << "\nThank you for using Investment Portfolio Manager!\n";
                        cout << "Keep learning and invest responsibly.\n";
                        return;
                    default:
                        cout << "Invalid option. Please choose from 0 to 9.\n";
                }
            }
            catch (const exception& e) {
                cout << "Error: " << e.what() << '\n';
            }
        }
    }
};

int main() {
    try {
        PortfolioManager portfolio;
        portfolio.run();
    }
    catch (const exception& e) {
        cout << "Fatal error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
