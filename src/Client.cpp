#include "Client.hpp"

#include <sstream>
#include <iomanip>
#include <exception>
#include <vector>
#include <unistd.h>      // для isatty()

using namespace std;

Client::Client(Bank& bank)
    : bank_(bank)
{}

void Client::displayHelp(Painter& p) const {
    p.printColoredLine("Available commands:");
    p.printColoredLine("  help                         — show this help message");
    p.printColoredLine("  exit                         — exit the program");
    p.printColoredLine("  transfer <from> <to> <amt>   — transfer amt from <from> to <to>");
    p.printColoredLine("  freeze <id>                  — freeze account <id>");
    p.printColoredLine("  unfreeze <id>                — unfreeze account <id>");
    p.printColoredLine("  mass_update <amt>            — add/subtract <amt> to all accounts");
    p.printColoredLine("  set_limits <id> <min> <max>  — set [min,max] limits on <id>");
    p.printColoredLine("  show_account_list            — list all accounts in a table");
    p.printColoredLine("  show_balance <id>            — show current balance for <id>");
    p.printColoredLine("  show_min <id>                — show minimal limit for <id>");
    p.printColoredLine("  show_max <id>                — show maximal limit for <id>");
}

void Client::run() {
    vector<string> successPatterns = {
        "Welcome", "OK:", "Transferred",
        "Available commands", "Account", "limits", "list"
    };
    vector<string> failPatterns = {
        "Error:", "Usage:", "Unknown command"
    };
    Painter p(cout, successPatterns, failPatterns);

    // Интерактивное приветствие и справка
    if (isatty(fileno(stdin))) {
        p.printColoredLine("Welcome to TBANK client!");
        displayHelp(p);
    }

    string line;
    bool interactive = isatty(fileno(stdin));
    while (true) {
        if (interactive) {
            p.printColoredLine("> ");
        }
        if (!getline(cin, line)) {
            p.printColoredLine("Goodbye!");
            break;
        }
        if (!processCommand(line, p)) {
            p.printColoredLine("Exiting client. Goodbye!");
            break;
        }
    }
}

bool Client::processCommand(const string& line, Painter& p) {
    istringstream iss(line);
    string cmd;
    if (!(iss >> cmd)) return true;

    try {
        if (cmd == "show_account_list") {
            showAccountList(p);
        }
        else if (cmd == "show_balance") {
            int id;
            if (!(iss >> id)) p.printColoredLine("Usage: show_balance <id>");
            else showBalance(id, p);
        }
        else if (cmd == "show_min") {
            int id;
            if (!(iss >> id)) p.printColoredLine("Usage: show_min <id>");
            else showMin(id, p);
        }
        else if (cmd == "show_max") {
            int id;
            if (!(iss >> id)) p.printColoredLine("Usage: show_max <id>");
            else showMax(id, p);
        }
        else if (cmd == "help") {
            displayHelp(p);
        }
        else if (cmd == "exit") {
            return false;
        }
        else if (cmd == "transfer") {
            int from, to; int32_t amt;
            if (!(iss >> from >> to >> amt)) {
                p.printColoredLine("Usage: transfer <from> <to> <amt>");
            } else {
                bank_.transferFunds(from, to, amt);
                p.printColoredLine("OK: Transferred " + to_string(amt) +
                                   " from " + to_string(from) +
                                   " to " + to_string(to));
            }
        }
        else if (cmd == "freeze") {
            int id;
            if (!(iss >> id)) {
                p.printColoredLine("Usage: freeze <id>");
            } else {
                bank_.freezeAccount(id);
                p.printColoredLine("OK: account " + to_string(id) + " frozen");
            }
        }
        else if (cmd == "unfreeze") {
            int id;
            if (!(iss >> id)) {
                p.printColoredLine("Usage: unfreeze <id>");
            } else {
                bank_.unfreezeAccount(id);
                p.printColoredLine("OK: account " + to_string(id) + " unfrozen");
            }
        }
        else if (cmd == "mass_update") {
            int32_t amt;
            if (!(iss >> amt)) {
                p.printColoredLine("Usage: mass_update <amt>");
            } else {
                bank_.massUpdate(amt);
                p.printColoredLine("OK: all balances updated by " + to_string(amt));
            }
        }
        else if (cmd == "set_limits") {
            int id; int32_t mn, mx;
            if (!(iss >> id >> mn >> mx)) {
                p.printColoredLine("Usage: set_limits <id> <min> <max>");
            } else {
                bank_.setLimits(id, mn, mx);
                p.printColoredLine("OK: Limits for account " + to_string(id) +
                                   " set to [" + to_string(mn) + "," + to_string(mx) + "]");
            }
        }
        else {
            p.printColoredLine("Unknown command: " + cmd);
        }
    }
    catch (const exception& ex) {
        p.printColoredLine(string("Error: ") + ex.what());
    }

    return true;
}

// ------------------ реализации новых методов ------------------

void Client::showAccountList(Painter& p) const {
    p.printColoredLine(" ID |   Balance   |    Min    |    Max    | Frozen");
    p.printColoredLine("----+-------------+-----------+-----------+--------");
    size_t N = bank_.getAccountCount();
    for (size_t i = 0; i < N; ++i) {
        const Account& a = bank_.getAccount(i);
        ostringstream oss;
        oss << setw(3) << a.account_id << " | "
            << setw(11) << a.balance     << " | "
            << setw(9)  << a.min_balance << " | "
            << setw(9)  << a.max_balance << " | "
            << (a.frozen ? "true" : "false");
        p.printColoredLine(oss.str());
    }
}

void Client::showBalance(int id, Painter& p) const {
    const Account& a = bank_.getAccount(static_cast<size_t>(id));
    p.printColoredLine("Account " + to_string(id) +
                       " balance: " + to_string(a.balance));
}

void Client::showMin(int id, Painter& p) const {
    const Account& a = bank_.getAccount(static_cast<size_t>(id));
    p.printColoredLine("Account " + to_string(id) +
                       " min balance: " + to_string(a.min_balance));
}

void Client::showMax(int id, Painter& p) const {
    const Account& a = bank_.getAccount(static_cast<size_t>(id));
    p.printColoredLine("Account " + to_string(id) +
                       " max balance: " + to_string(a.max_balance));
}
