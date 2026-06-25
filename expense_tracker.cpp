/*
 * Terminal Expense Tracker
 * Author: Eklavya Joshi
 *
 * Track daily expenses by category, set monthly budgets,
 * and get alerts when overspending.
 *
 * Concepts: STL (map, vector), OOP, File I/O, structs, sorting
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <ctime>
using namespace std;

// ─── Data Structures ─────────────────────────────────────────────────────────

struct Expense {
    int    id;
    string date;       // format: YYYY-MM-DD
    string category;   // e.g. Food, Travel, Shopping
    string description;
    double amount;
};

// ─── Utility ─────────────────────────────────────────────────────────────────

string todayDate() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", now);
    return string(buf);
}

string currentMonth() {
    return todayDate().substr(0, 7); // "YYYY-MM"
}

void printLine(int w = 60) {
    cout << string(w, '-') << "\n";
}

// ─── ExpenseTracker Class ─────────────────────────────────────────────────────

class ExpenseTracker {
private:
    vector<Expense>        expenses;
    map<string, double>    budgets;    // category -> monthly budget
    int                    nextId;
    const string           EXPENSE_FILE = "expenses.csv";
    const string           BUDGET_FILE  = "budgets.csv";

    void saveExpenses() {
        ofstream f(EXPENSE_FILE);
        f << "id,date,category,description,amount\n";
        for (const auto& e : expenses)
            f << e.id << "," << e.date << "," << e.category << ","
              << "\"" << e.description << "\"," << fixed << setprecision(2) << e.amount << "\n";
        f.close();
    }

    void loadExpenses() {
        ifstream f(EXPENSE_FILE);
        if (!f.is_open()) return;
        string line;
        getline(f, line); // skip header
        while (getline(f, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            string tok;
            Expense e;
            getline(ss, tok, ','); e.id       = stoi(tok);
            getline(ss, tok, ','); e.date      = tok;
            getline(ss, tok, ','); e.category  = tok;
            getline(ss, tok, ',');
            if (!tok.empty() && tok.front() == '"') {
                // Handle quoted description
                string rest;
                if (tok.back() != '"') {
                    getline(ss, rest, '"');
                    tok += "," + rest + "\"";
                    getline(ss, rest, ','); // consume comma after closing quote
                }
                tok = tok.substr(1, tok.size() - 2);
            }
            e.description = tok;
            getline(ss, tok); e.amount = stod(tok);
            expenses.push_back(e);
            nextId = max(nextId, e.id + 1);
        }
        f.close();
    }

    void saveBudgets() {
        ofstream f(BUDGET_FILE);
        f << "category,amount\n";
        for (const auto& b : budgets)
            f << b.first << "," << fixed << setprecision(2) << b.second << "\n";
        f.close();
    }

    void loadBudgets() {
        ifstream f(BUDGET_FILE);
        if (!f.is_open()) return;
        string line;
        getline(f, line); // skip header
        while (getline(f, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            string cat, amt;
            getline(ss, cat, ',');
            getline(ss, amt);
            budgets[cat] = stod(amt);
        }
        f.close();
    }

    // Returns total spent in a category for a given month (YYYY-MM)
    double spentInMonth(const string& category, const string& month) {
        double total = 0;
        for (const auto& e : expenses)
            if (e.category == category && e.date.substr(0, 7) == month)
                total += e.amount;
        return total;
    }

    void checkBudgetAlert(const string& category) {
        if (budgets.find(category) == budgets.end()) return;
        double budget = budgets[category];
        double spent  = spentInMonth(category, currentMonth());
        double pct    = (spent / budget) * 100.0;

        if (pct >= 100)
            cout << "\n  ⚠  ALERT: Budget EXCEEDED for " << category
                 << "! Spent Rs. " << fixed << setprecision(2) << spent
                 << " / Rs. " << budget << "\n";
        else if (pct >= 80)
            cout << "\n  ⚠  WARNING: " << fixed << setprecision(1) << pct
                 << "% of " << category << " budget used this month.\n";
    }

public:
    ExpenseTracker() : nextId(1) {
        loadExpenses();
        loadBudgets();
    }

    // ── Add Expense ───────────────────────────────────────────────────────
    void addExpense() {
        Expense e;
        e.id = nextId++;

        cin.ignore();
        cout << "\nDate (YYYY-MM-DD) [Enter for today]: ";
        getline(cin, e.date);
        if (e.date.empty()) e.date = todayDate();

        cout << "Category (Food / Travel / Shopping / Bills / Other): ";
        getline(cin, e.category);

        cout << "Description: ";
        getline(cin, e.description);

        cout << "Amount (Rs.): ";
        cin >> e.amount;

        expenses.push_back(e);
        saveExpenses();

        cout << "\n[+] Expense added: Rs. " << fixed << setprecision(2)
             << e.amount << " in " << e.category << "\n";

        checkBudgetAlert(e.category);
    }

    // ── View All Expenses (optional month filter) ─────────────────────────
    void viewExpenses() {
        if (expenses.empty()) { cout << "\nNo expenses recorded yet.\n"; return; }

        cin.ignore();
        cout << "\nFilter by month? (YYYY-MM) [Enter for all]: ";
        string filter; getline(cin, filter);

        cout << "\n" << left
             << setw(5)  << "ID"
             << setw(13) << "Date"
             << setw(14) << "Category"
             << setw(22) << "Description"
             << setw(10) << "Amount"
             << "\n";
        printLine(64);

        double total = 0;
        int count = 0;
        for (const auto& e : expenses) {
            if (!filter.empty() && e.date.substr(0, 7) != filter) continue;
            cout << left
                 << setw(5)  << e.id
                 << setw(13) << e.date
                 << setw(14) << e.category
                 << setw(22) << e.description.substr(0, 20)
                 << "Rs. " << fixed << setprecision(2) << e.amount << "\n";
            total += e.amount;
            count++;
        }
        printLine(64);
        cout << "  " << count << " expense(s)   Total: Rs. "
             << fixed << setprecision(2) << total << "\n";
    }

    // ── Monthly Summary with budget comparison ────────────────────────────
    void monthlySummary() {
        cin.ignore();
        cout << "\nEnter month (YYYY-MM) [Enter for current]: ";
        string month; getline(cin, month);
        if (month.empty()) month = currentMonth();

        // Aggregate by category
        map<string, double> catTotals;
        double grandTotal = 0;
        for (const auto& e : expenses) {
            if (e.date.substr(0, 7) != month) continue;
            catTotals[e.category] += e.amount;
            grandTotal += e.amount;
        }

        if (catTotals.empty()) { cout << "\nNo expenses for " << month << ".\n"; return; }

        cout << "\n====== Summary: " << month << " ======\n";
        cout << left << setw(16) << "Category"
             << setw(12) << "Spent"
             << setw(12) << "Budget"
             << setw(10) << "Status"
             << "\n";
        printLine(50);

        for (const auto& entry : catTotals) {
            string cat   = entry.first;
            double spent = entry.second;
            string budgetStr = "-";
            string status    = "";

            if (budgets.count(cat)) {
                double bud = budgets[cat];
                double pct = (spent / bud) * 100.0;
                budgetStr  = "Rs. " + to_string((int)bud);
                status     = (pct >= 100) ? "OVER" : (pct >= 80 ? "WARN" : "OK");
            }

            // ASCII bar (10 chars)
            int barLen = budgets.count(cat) ? min(10, (int)(10 * spent / budgets[cat])) : 5;
            string bar = "[" + string(barLen, '#') + string(10 - barLen, '.') + "]";

            cout << left << setw(16) << cat
                 << "Rs. " << setw(8) << fixed << setprecision(2) << spent
                 << setw(12) << budgetStr
                 << setw(6)  << status
                 << bar << "\n";
        }
        printLine(50);
        cout << "Total Spent: Rs. " << fixed << setprecision(2) << grandTotal << "\n";
    }

    // ── Set Budget ────────────────────────────────────────────────────────
    void setBudget() {
        cin.ignore();
        cout << "\nCategory: ";
        string cat; getline(cin, cat);
        cout << "Monthly Budget (Rs.): ";
        double amt; cin >> amt;
        budgets[cat] = amt;
        saveBudgets();
        cout << "[+] Budget set: Rs. " << fixed << setprecision(2)
             << amt << " / month for " << cat << "\n";
    }

    // ── Delete Expense ────────────────────────────────────────────────────
    void deleteExpense() {
        cout << "\nEnter Expense ID to delete: ";
        int id; cin >> id;
        auto it = find_if(expenses.begin(), expenses.end(),
                          [id](const Expense& e){ return e.id == id; });
        if (it == expenses.end()) { cout << "[!] ID not found.\n"; return; }
        cout << "[-] Deleted: " << it->description << " (Rs. "
             << fixed << setprecision(2) << it->amount << ")\n";
        expenses.erase(it);
        saveExpenses();
    }

    // ── Top Spending Categories this month ────────────────────────────────
    void topCategories() {
        map<string, double> totals;
        for (const auto& e : expenses)
            if (e.date.substr(0, 7) == currentMonth())
                totals[e.category] += e.amount;

        if (totals.empty()) { cout << "\nNo expenses this month yet.\n"; return; }

        // Sort by amount descending
        vector<pair<string, double>> sorted(totals.begin(), totals.end());
        sort(sorted.begin(), sorted.end(),
             [](const pair<string,double>& a, const pair<string,double>& b){ return a.second > b.second; });

        cout << "\n-- Top Spending This Month --\n";
        int rank = 1;
        for (const auto& p : sorted)
            cout << rank++ << ". " << left << setw(16) << p.first
                 << "Rs. " << fixed << setprecision(2) << p.second << "\n";
    }
};

// ─── Main Menu ────────────────────────────────────────────────────────────────

int main() {
    ExpenseTracker tracker;
    int choice = 0;

    cout << "╔════════════════════════════════════╗\n";
    cout << "║    Terminal Expense Tracker CLI    ║\n";
    cout << "╚════════════════════════════════════╝\n";

    while (true) {
        cout << "\n1. Add Expense\n"
             << "2. View Expenses\n"
             << "3. Monthly Summary\n"
             << "4. Set Monthly Budget\n"
             << "5. Top Spending Categories\n"
             << "6. Delete Expense\n"
             << "0. Exit\n"
             << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: tracker.addExpense();      break;
            case 2: tracker.viewExpenses();    break;
            case 3: tracker.monthlySummary();  break;
            case 4: tracker.setBudget();       break;
            case 5: tracker.topCategories();   break;
            case 6: tracker.deleteExpense();   break;
            case 0: cout << "Goodbye!\n"; return 0;
            default: cout << "[!] Invalid choice.\n";
        }
    }
}
