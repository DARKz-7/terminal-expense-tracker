# Terminal Expense Tracker

A command-line expense tracking tool built in **C++** to log daily expenses by category, set monthly budgets, and get real-time overspending alerts — all from the terminal with no internet or account required.

## Features

- **Add expenses** with date, category, description, and amount
- **View expenses** with optional month filter
- **Monthly summary** — category-wise spending vs budget with ASCII progress bars
- **Budget alerts** — warns at 80% usage, alerts on exceeding budget
- **Top spending categories** — ranked by amount for the current month
- **Delete expenses** by ID
- **Persistent storage** — all data saved to local CSV files, survives across sessions

## Demo

```
╔════════════════════════════════════╗
║    Terminal Expense Tracker CLI    ║
╚════════════════════════════════════╝

====== Summary: 2025-06 ======
Category        Spent       Budget      Status
--------------------------------------------------
Bills           Rs. 1200    Rs. 1500    OK    [########..]
Food            Rs. 3100    Rs. 3000    OVER  [##########]
Shopping        Rs. 800     Rs. 2000    OK    [####......]
Travel          Rs. 1750    Rs. 2000    WARN  [########..]
--------------------------------------------------
Total Spent: Rs. 6850.00

⚠  ALERT: Budget EXCEEDED for Food! Spent Rs. 3100.00 / Rs. 3000.00
```

## Build & Run

**Requirements:** g++ with C++17 support

```bash
# Compile
g++ -std=c++17 -o expense_tracker expense_tracker.cpp

# Run
./expense_tracker
```

## Concepts Used

| Concept | Usage |
|---|---|
| OOP (Classes) | `ExpenseTracker` class encapsulates all data and logic |
| STL `map` | Stores per-category budgets and monthly totals for O(log n) lookup |
| STL `vector` | Stores the list of all expense entries in memory |
| STL `sort` + Lambda | Sorts categories by spending amount for the top-spending view |
| `find_if` + Lambda | Searches expenses by ID for deletion |
| File I/O (`fstream`) | Persists expenses and budgets to CSV files across sessions |
| `struct` | Groups expense fields (id, date, category, description, amount) |
| `ctime` | Fetches today's date automatically as default for new entries |

## File Structure

```
terminal-expense-tracker/
├── expense_tracker.cpp   # Main source file
├── expenses.csv          # Auto-generated: stores all expense entries
└── budgets.csv           # Auto-generated: stores category budgets
```

## Categories

Default categories: `Food`, `Travel`, `Shopping`, `Bills`, `Other`

You can enter any custom category name — the tracker handles it automatically.

## Why I Built This

Managing daily expenses as a hostel student at IIT Kharagpur without any app or account. This tool gives a clean local solution — add an expense in under 10 seconds, check your monthly burn rate at a glance, and get warned before you blow your budget.

---

*Built by Eklavya Joshi | IIT Kharagpur*
