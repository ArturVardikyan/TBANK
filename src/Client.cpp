#include "Client.hpp"
#include "Bank.hpp"

#include <colorprint.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <exception>

using namespace std;

Client::Client(Bank& bank)
    : bank_(bank)
{}

void Client::displayHelp() const {
    cout << "Available commands:\n"
         << "  help                         — show this help message\n"
         << "  exit                         — exit the program\n"
         << "  transfer <from> <to> <amt>   — transfer amt from account <from> to <to>\n"
         << "  freeze <id>                  — freeze account <id>\n"
         << "  unfreeze <id>                — unfreeze account <id>\n"
         << "  mass_update <amt>            — add (or subtract) <amt> to all accounts\n"
         << "  set_limits <id> <min> <max>  — set new [min,max] limits on account <id>\n"
         << endl;
}

void Client::run() {
    vector<string> successPatterns = {
        "Transferred", "OK:", "All accounts", "Limits for account"
    };
    vector<string> failPatterns = {
        "Error:", "Usage:", "Unknown command"
    };
    Painter p(cout, successPatterns, failPatterns);

    p.printColoredLine("Welcome to TBANK client!");
    displayHelp();

    string line;
    while (true) {
        p.printColoredLine("> "); 
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
        if (cmd == "help") {
            displayHelp();
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
                p.printColoredLine("OK: all accounts updated by " + to_string(amt));
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
