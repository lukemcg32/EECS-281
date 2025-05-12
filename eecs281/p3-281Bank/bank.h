// Project Identifier: 292F24D17A4455C1B5133EDD8C7CEAA0C9570A98
#ifndef BANK_H
#define BANK_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>


// --------------------------------------------------------------------------------- //
// ************************* STRUCT/OBJECT DECLARATIONS *************************** //
// ------------------------------------------------------------------------------- //

// Struct to store timestamps
struct Timestamp {
    uint32_t yy, mm, dd, hh, mi, ss;
    uint32_t totalSeconds;

    Timestamp();
    std::string toIntegerString() const;
    void parse(const std::string& s);
    bool operator<(const Timestamp& other) const;
    bool operator==(const Timestamp& other) const;
    // std::string toString() const;
};

// Struct to store user information
struct User {
    std::string userID;
    std::string pin;
    uint32_t balance;
    Timestamp regTimestamp;
    Timestamp last;
    std::unordered_set<std::string> validIPs; // store IP addresses in u_set
    bool isLoggedIn;

    User();
};

// Struct to store transaction information
struct Transaction {
    int id = 0;
    Timestamp timestamp;
    Timestamp execDate;
    std::string ip;
    std::string sender;
    std::string recipient;
    uint32_t amount;
    char feeType; // 'o' or 's'
    bool discount = false;
    uint32_t fee = 0;
    bool operator<(const Transaction& other) const;
    Transaction(const std::string& ts_in, const std::string& ip_in, const std::string& sender_in, 
        const std::string& recipient_in, const uint32_t amount_in, const std::string& exec_date_in, const char o_s_in);
};



// --------------------------------------------------------------------------------- //
// *************************** BANK CLASS DECLARATION ***************************** //
// ------------------------------------------------------------------------------- //

class Bank {
public:
    Bank(bool verbose);
    void readRegistrationFile(const std::string& filename);
    void processCommands();
    void processQueries();

private:
    // variables
    int transactionCounter; // initialized to -1 in ctor
    bool verboseMode = false; // initialized in ctor
    Timestamp lastPlaceTimestamp; // default constructor used


    // data structures
    std::unordered_map<std::string, User> users;
    std::priority_queue<Transaction> transactionPQ;
    std::vector<Transaction> completedTransactions;


    // functions
    void loginUser(const std::string& userID, const std::string& pin, const std::string& ip);
    void logoutUser(const std::string& userID, const std::string& ip);
    void placeTransaction(const std::string& ts, const std::string& ip, const std::string& sender, 
            const std::string& recipient, uint32_t amount, const std::string& exec_date, char o_s);
    void processTransactions(const Timestamp& currentTime);
    void checkBalance(const std::string& userID, const std::string& ip);
    std::string formatTimeInterval(const Timestamp& start, const Timestamp& end);
    void printTransaction(const Transaction& tx);


    // Query processing methods
    void listTransactions(const std::string& x, const std::string& y);
    void bankRevenue(const std::string& x, const std::string& y);
    void customerHistory(const std::string& userID);
    void summarizeDay(const std::string& timestampStr);
};



// --------------------------------------------------------------------------------- //
// *********************** BANK PUBLIC MEMBER FUNCTIONS *************************** //
// ------------------------------------------------------------------------------- //


// Bank class implementation
Bank::Bank(bool verbose) : transactionCounter(-1) {
    if (verbose) verboseMode = true;
}




void Bank::readRegistrationFile(const std::string& filename) {
    std::ifstream infile(filename);

    if (!infile.is_open()) {
        std::cerr << "Registration file failed to open.\n";
        exit(1);
    } 

    std::string line;
    while (std::getline(infile, line)) {

       //  std::cout << "read this line: " << line << "\n";

        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string regTimeStr, userID, pin, balanceStr;

        if (std::getline(iss, regTimeStr, '|') && std::getline(iss, userID, '|') &&
                    std::getline(iss, pin, '|') && std::getline(iss, balanceStr)) {
            
            
            User user;
            user.userID = userID;
            user.pin = pin;
            user.balance = (uint32_t)std::stoi(balanceStr); 
            user.regTimestamp.parse(regTimeStr);
            user.last = user.regTimestamp;

           // std::cout << "Emplacing: " << userID << "\n";

            // emplace into the users hash table @ userID
            // avoids double construction
            users.emplace(userID, user);
        }

    } // while (getline)

    // close file
    infile.close();
    return;
} // readRegistrationFile()




void Bank::processCommands() {
    std::string line;
    Timestamp maxTime;
    maxTime.totalSeconds = UINT32_MAX;
    while (std::getline(std::cin, line)) {

        // if at end of commands, process reamining transactions
        if (line == "$$$") {
            processTransactions(maxTime);
            break;
        }

        // if comment or empty line, keep looping
        if (line.empty() || line[0] == '#') continue;

        // break down the line and read
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command[0] == 'l') { // login

            std::string userID, pin, ip;
            iss >> userID >> pin >> ip;
            loginUser(userID, pin, ip);

        } else if (command[0] == 'o') { // logout

            std::string userID, ip;
            iss >> userID >> ip;
            logoutUser(userID, ip);

        } else if (command[0] == 'b') { // balance

            std::string userID, ip;
            iss >> userID >> ip;
            checkBalance(userID, ip);

        } else if (command[0] == 'p') { // place
           // processTransactions(lastPlaceTimestamp);

            std::string ts, ip, sender, recipient, exec_date;
            char o_s;
            uint32_t amount;
            iss >> ts >> ip >> sender >> recipient >> amount >> exec_date >> o_s;
            placeTransaction(ts, ip, sender, recipient, amount, exec_date, o_s);
           // processTransactions(lastPlaceTimestamp);

        } else { 
            std::cerr << "Error: Reading from cin has failed\n";
            exit(1);
        }
    } // while(getline)

    // Process remaining transactions after all commands are read
    processTransactions(maxTime);

} // processCommands()



// if having errors try using getline
void Bank::processQueries() {
    char mode = '\0';
    while (std::cin >> mode) {

        if (mode == 'l') { // list transactions mode
            std::string start, end;
            std::cin >> start >> end;
            listTransactions(start, end);

        } else if (mode == 'r') { // bank revenue mode
            std::string start, end;
            std::cin >> start >> end;   
            bankRevenue(start, end);        

        } else if (mode == 'h') { // customer history mode
            std::string userID;
            std::cin >> userID;
            customerHistory(userID);

        } else if (mode == 's') { // summarize day mode
            std::string start;
            std::cin >> start;
            summarizeDay(start);

        } else { 
            std::cerr << " Error: Reading from cin has failed\n";
            exit(1);
        }

    } // while

    return;
} // processQueries()




// --------------------------------------------------------------------------------- //
// **************** BANK PRIVATE MEMBER FUNCTION IMPLEMENTATIONS ****************** //
// ------------------------------------------------------------------------------- //

void Bank::loginUser(const std::string& userID, const std::string& pin, const std::string& ip) {
    auto it = users.find(userID);

    if (it != users.end()) {
        User& user = it->second;
        if (user.pin == pin) {
            user.isLoggedIn = true; // user logged in
            user.validIPs.insert(ip); // add IP to User-specific hash table

            if (verboseMode) std::cout << "User " << userID << " logged in.\n";

            return;
        }
    } // if

    if (verboseMode) std::cout << "Login failed for " << userID << ".\n";
    
} // loginUser()



void Bank::logoutUser(const std::string& userID, const std::string& ip) {
    auto it = users.find(userID);
    if (it != users.end()) {
        User& user = it->second;

        // make sure the IP is in our hash table, but can't use contains()...
        if (user.validIPs.find(ip) != user.validIPs.end()) {
            user.validIPs.erase(ip); // delete IP

            if (user.validIPs.empty()) user.isLoggedIn = false; // logged out if no IPs active

            // not usre if I should print this when there are multiple IPs...
            if (verboseMode) std::cout << "User " << userID << " logged out.\n";

            return;
        }
    } // if

    if (verboseMode) std::cout << "Logout failed for " << userID << ".\n";

} // logoutUser()



void Bank::checkBalance(const std::string& userID, const std::string& ip) {
    auto it = users.find(userID);

    if (it == users.end()) {
        if (verboseMode) std::cout << "Account " << userID << " does not exist.\n";
        return;
    }

    User& user = it->second;
    if (!user.isLoggedIn) {
        if (verboseMode) std::cout << "Account " << userID << " is not logged in.\n";
        return;
    }

    // is this neccessary? or should it only be for "place" when fraud is on sender?
    if (user.validIPs.find(ip) == user.validIPs.end()) {
        if (verboseMode) std::cout << "Fraudulent transaction detected, aborting request.\n";
        return;
    }


    std::cout << "As of " << user.last.toIntegerString() << ", " << userID << " has a balance of $"
              << user.balance << ".\n";

} // checkBalance()



void Bank::placeTransaction(const std::string& ts, const std::string& ip, const std::string& sender, 
            const std::string& recipient, const uint32_t amount, const std::string& exec_date, char o_s) {

    // initialize our current transaction...parsing handled by constructor
    Transaction tx(ts, ip, sender, recipient, amount, exec_date, o_s);


    bool send = false; bool rec = false;
    auto senderIt = users.find(tx.sender); // use find ONCE for sender
    auto recipientIt = users.find(tx.recipient); // use find ONCE for recipient

    if (senderIt != users.end()) {
        send = true;
        users[tx.sender].last = tx.timestamp; 
    }
    // Check if recipient exists
    if (recipientIt != users.end()) {
        rec = true;
        users[tx.recipient].last = tx.timestamp;
    }




    // start by checking for errors in the correct order... ---------------------------------------

    if (tx.timestamp < lastPlaceTimestamp) {
        std::cerr << "Invalid decreasing timestamp in 'place' command.\n";
        std::exit(1);
    }

    if (tx.execDate < tx.timestamp) {
        std::cerr << "You cannot have an execution date before the current timestamp.\n";
        std::exit(1);
    }

    // Check if sender and recipient are same
    if (tx.sender == tx.recipient) {
        if (verboseMode) std::cout << "Self transactions are not allowed.\n";
        return;
    }

    // Check if EXEC_DATE - TIMESTAMP is more than three days (already checked that exec is after ts)
    if ((tx.execDate.totalSeconds - tx.timestamp.totalSeconds) > (3 * 24 * 60 * 60)) {
        if (verboseMode) std::cout << "Select a time up to three days in the future.\n";
        return;
    }
    

    // Check if sender exists
    if (!send) {
        if (verboseMode) std::cout << "Sender " << tx.sender << " does not exist.\n";
        return;
    }

    // Check if recipient exists
    if (!rec) {
        if (verboseMode) std::cout << "Recipient " << tx.recipient << " does not exist.\n";
        return;
    }

    // Check if execution date is not earlier than both parties' registration dates
    if (tx.execDate < senderIt->second.regTimestamp || tx.execDate < recipientIt->second.regTimestamp) {
        if (verboseMode) 
            std::cout << "At the time of execution, sender and/or recipient have not registered.\n";
        return;
    }

    // Check if sender has an active session
    if (!senderIt->second.isLoggedIn) {
        if (verboseMode) std::cout << "Sender " << tx.sender << " is not logged in.\n";
        return;
    }

    // Check for fraudulent transaction
    if (senderIt->second.validIPs.find(tx.ip) == senderIt->second.validIPs.end()) {
        if (verboseMode) std::cout << "Fraudulent transaction detected, aborting request.\n";
        return;
    }

    // done with checking conditions, move forward with transaction ------------------------------------------------

    // might need to be <=
    // moved here so it doesn't process if invalid place command
    if (lastPlaceTimestamp < tx.timestamp) {
        processTransactions(tx.timestamp);
    }

    lastPlaceTimestamp = tx.timestamp;



    // Assign transaction ID
    tx.id = ++transactionCounter;

    // Calculate fee:    min = 10, max = 450, fee % = 0.01
    int fee = std::max(10, static_cast<int>(tx.amount * 0.01));
    fee = std::min(fee, 450);

    // Check for discount:   if customer has been with 281Bank > 5 years
    uint32_t fiveYearsLater = senderIt->second.regTimestamp.totalSeconds + (5 * 365 * 24 * 60 * 60);

    tx.discount = (tx.execDate.totalSeconds > fiveYearsLater);
    if (tx.discount) fee = ((fee * 3) / 4); // Apply 25% discount (formula from spec)
    tx.fee = (uint32_t)fee;

    // push tx into transactionPQ
    transactionPQ.push(tx);

    if (verboseMode) {
        std::cout << "Transaction " << tx.id << " placed at " << tx.timestamp.toIntegerString() << ": $"
                  << tx.amount << " from " << tx.sender << " to " << tx.recipient << " at "
                  << tx.execDate.toIntegerString() << ".\n";
    }
    return;
} // placeTransaction()




void Bank::processTransactions(const Timestamp& currentTime) {

    while (!transactionPQ.empty()) {
        Transaction tx = transactionPQ.top();

        // stop completing transactions when exec time exceeds current time
        if (tx.execDate.totalSeconds > currentTime.totalSeconds) break;

        transactionPQ.pop();


        // calculate sender & recipient iterators ONCE
        auto senderIt = users.find(tx.sender);
        auto recipientIt = users.find(tx.recipient);

        // initialize variables to 'o' mode (sender covers fee)
        // check to make sure my calculations of discount is implemented properly
        uint32_t totalFee = tx.fee;
        uint32_t senderFee = totalFee;
        uint32_t recipientFee = 0;

        // change if 's' mode (split)
        if (tx.feeType == 's') {
            // make sure integer division functions properly with unsigned int!!!
            senderFee = (totalFee / 2) + (totalFee % 2); // sender covers remiander if odd number
            recipientFee = totalFee / 2;
        }

        uint32_t senderTotal = tx.amount + senderFee;

        // check that balances are sufficient
        if (senderIt->second.balance < senderTotal) {
            if (verboseMode) std::cout << "Insufficient funds to process transaction " << tx.id << ".\n";
            continue;
        }

        if (tx.feeType == 's' && recipientIt->second.balance < recipientFee) {
            if (verboseMode) std::cout << "Insufficient funds to process transaction " << tx.id << ".\n";
            continue;
        }


        // update users balances
        senderIt->second.balance -= senderTotal;
        recipientIt->second.balance += (tx.amount - recipientFee);

        // push into completed transactions vector
        completedTransactions.push_back(tx);


        if (verboseMode) {
            std::cout << "Transaction " << tx.id << " executed at " << tx.execDate.toIntegerString()
                      << ": $" << tx.amount << " from " << tx.sender << " to " << tx.recipient << ".\n";
        }
    } // while (!PQ.empty)

} // processTransactions()




std::string Bank::formatTimeInterval(const Timestamp& start, const Timestamp& end) {

    int years = (int)end.yy - int(start.yy);
    int months = (int)end.mm - (int)start.mm;
    int days = (int)end.dd - (int)start.dd;
    int hours = (int)end.hh - (int)start.hh;
    int minutes = (int)end.mi - (int)start.mi;
    int seconds = (int)end.ss - (int)start.ss;
    std::vector<int> vec = {years, months, days, hours, minutes, seconds};
    std::ostringstream oss;

    // iterate and adjust all numbers
    for (size_t i = 1; i < 6; i++) {
        if (vec[i] < 0) {
            vec[i - 1]--;
            vec[i] += 100;
        }
    }

    // lambda helper function to check if value = 0. append an s if value > 1
    auto appendTime = [&](int value, const std::string& unit) {
        if (value > 0) {
            oss << value << " " << unit << (value == 1 ? " " : "s ");
        }
    };

    appendTime(vec[0], "year");
    appendTime(vec[1], "month");
    appendTime(vec[2], "day");
    appendTime(vec[3], "hour");
    appendTime(vec[4], "minute");
    appendTime(vec[5], "second");

    std::string result = oss.str();
    if (result.empty()) result = "0 seconds";
    else result.pop_back();
    return result;
} // formatTimeIntervals()




void Bank::printTransaction(const Transaction& tx) {
    std::cout << tx.id << ": " << tx.sender << " sent ";
                                // ternary operator
    std::cout << tx.amount << (tx.amount == 1 ? " dollar" : " dollars") << " to "
              << tx.recipient << " at " << tx.execDate.toIntegerString() << ".\n";

} // printTransaction()




// --------------------------------------------------------------------------------- //
// **************** STRUCT/OBJECT MEMBER FUNCTION IMPLEMENTATIONS ***************** //
// ------------------------------------------------------------------------------- //

User::User() : balance(0), isLoggedIn(false) {}

Timestamp::Timestamp() : yy(0), mm(0), dd(0), hh(0), mi(0), ss(0), totalSeconds(0) {}

void Timestamp::parse(const std::string& s) {
    yy = (uint32_t)std::stoi(s.substr(0, 2));
    mm = (uint32_t)std::stoi(s.substr(3, 2));
    dd = (uint32_t)std::stoi(s.substr(6, 2));
    hh = (uint32_t)std::stoi(s.substr(9, 2));
    mi = (uint32_t)std::stoi(s.substr(12, 2));
    ss = (uint32_t)std::stoi(s.substr(15, 2));
    totalSeconds = (yy * 12 * 31 * 24 * 60 * 60) +
                   (mm * 31 * 24 * 60 * 60) +
                   (dd * 24 * 60 * 60) +
                   (hh * 60 * 60) +
                   (mi * 60) +
                   ss;
    return;
} // parse

bool Timestamp::operator<(const Timestamp& other) const {
    return totalSeconds < other.totalSeconds;
} // <

bool Timestamp::operator==(const Timestamp& other) const {
    return totalSeconds == other.totalSeconds;
} // ==


std::string Timestamp::toIntegerString() const {
    std::deque<uint32_t> store = {yy, mm, dd, hh, mi, ss};
    bool first = false;
    std::string returning = "";

    while(!store.empty()) {
        auto curr = store.front();
        store.pop_front();
        if (curr > 0 && !first) {
            returning += std::to_string(curr);
            first = true;
        } else if (first) {
            returning += (curr < 10 ? "0" : "") + std::to_string(curr);
            continue;
        } else {
            continue;
        }
    } // while
    if (returning.empty()) returning = "0";

    return returning;
} // toIntergerString()



// Transaction implementation
bool Transaction::operator<(const Transaction& other) const {
    if (execDate == other.execDate)
        return other.id < id; // earlier id has higher priority
    return other.execDate < execDate;
} // < (transaction)


Transaction::Transaction(const std::string& ts_in, const std::string& ip_in, const std::string& sender_in, 
        const std::string& recipient_in, const uint32_t amount_in, const std::string& exec_date_in, const char o_s_in) :
            ip(ip_in), sender(sender_in), recipient(recipient_in), amount(amount_in), feeType(o_s_in) {
                execDate.parse(exec_date_in);
                timestamp.parse(ts_in);
} // Transaction()



// --------------------------------------------------------------------------------- //
// ******************* QUERY PROCESSING METHODS IMPLEMENTATIONS ******************* //
// ------------------------------------------------------------------------------- //

// looping through the vectors each function might get expensive

void Bank::listTransactions(const std::string& s, const std::string& e) {
    Timestamp start; start.parse(s);
    Timestamp end; end.parse(e);

    if (!(end.totalSeconds > start.totalSeconds)) {
        std::cout << "List Transactions requires a non-empty time interval.\n";
        return;
    }

    std::vector<Transaction> txs;
    for (const auto& tx : completedTransactions) {
        if (tx.execDate.totalSeconds >= start.totalSeconds && tx.execDate < end) {
            txs.push_back(tx);
        }
    } //

    for (const auto& tx : txs) {
        printTransaction(tx);
    }

    if (txs.size() == 1) {
        std::cout << "There was 1 transaction that was placed between time " << start.toIntegerString() << " to " << end.toIntegerString() << ".\n";
    } else {
        std::cout << "There were " << txs.size() << " transactions that were placed between time " << start.toIntegerString() << " to " << end.toIntegerString() << ".\n";
    }
    return;
} // listTransactions()


void Bank::bankRevenue(const std::string& s, const std::string& e) {
    Timestamp start; start.parse(s);
    Timestamp end; end.parse(e);

    if (!(end.totalSeconds > start.totalSeconds)) {
        std::cout << "Bank Revenue requires a non-empty time interval.\n";
        return;
    }

    uint32_t totalRevenue = 0;
    for (const auto& tx : completedTransactions) {
        if (tx.execDate.totalSeconds >= start.totalSeconds && tx.execDate < end) {
            totalRevenue += tx.fee;
        }
    }

    std::string interval = formatTimeInterval(start, end);
    std::cout << "281Bank has collected " << totalRevenue << " dollars in fees over " << interval << ".\n";

    return;
} // bankRevenue()

void Bank::customerHistory(const std::string& userID) {
    auto it = users.find(userID);

    if (it == users.end()) {
        std::cout << "User " << userID << " does not exist.\n";
        return;
    }

    const User& user = it->second;
    std::vector<Transaction> incoming;
    std::vector<Transaction> outgoing;

    for (const auto& tx : completedTransactions) {
        if (tx.sender == userID) {
            outgoing.push_back(tx);
        } else if (tx.recipient == userID) {
            incoming.push_back(tx);
        } else { continue; }
    } // for


    std::cout << "Customer " << userID << " account summary:\n";
    std::cout << "Balance: $" << user.balance << "\n";
    std::cout << "Total # of transactions: " << incoming.size() + outgoing.size() << "\n";

    std::cout << "Incoming " << incoming.size() << ":\n";

    // if > 10 transactions, show the latest 10 FOR EACH CATEGORY
    if (incoming.size() > 10) {
        incoming.erase(incoming.begin(), incoming.end() - 10);
    }

    for (const auto& tx : incoming) {
        printTransaction(tx);
    }

    std::cout << "Outgoing " << outgoing.size() << ":\n";

    if (outgoing.size() > 10) {
        outgoing.erase(outgoing.begin(), outgoing.end() - 10);
    }

    for (const auto& tx : outgoing) {
        printTransaction(tx);
    }

    return;
} // customerHistory()

void Bank::summarizeDay(const std::string& s) {
    Timestamp start; start.parse(s); 
    Timestamp end;
    end.yy = start.yy;
    end.mm = start.mm;
    end.dd = start.dd + 1; // day + 1
    end.hh = 0; start.hh = 0;
    end.mi = 0; start.mi = 0;
    end.ss = 0; start.ss = 0;
    start.totalSeconds = (end.yy * 365 * 31 * 24 * 60 * 60) + (end.mm * 31 * 24 * 60 * 60) + (end.dd * 24 * 60 * 60);
    end.totalSeconds = start.totalSeconds + (24 * 60 * 60); // start + 1 day

    uint32_t day = start.dd;

    std::vector<Transaction> txs;
    uint32_t totalFees = 0;

    for (const auto& tx : completedTransactions) {
        if (tx.execDate.dd == day) {
            txs.push_back(tx);
            totalFees += tx.fee;
        }
    } // for

    std::cout << "Summary of [" << start.toIntegerString() << ", " << end.toIntegerString() << "):\n";
    for (const auto& tx : txs) {
        printTransaction(tx);
    }

    if (txs.size() == 1) {
        std::cout << "There was a total of 1 transaction, 281Bank has collected " << totalFees << " dollars in fees.\n";
    } else {
        std::cout << "There were a total of " << txs.size() << " transactions, 281Bank has collected " << totalFees << " dollars in fees.\n";
    }

    return;
} // summarizeDay()


#endif // BANK_H
