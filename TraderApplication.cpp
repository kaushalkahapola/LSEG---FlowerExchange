#include "TraderApplication.h"

// Definitions for headers
string exec_header = "Order ID,Cl. Ord. ID.,Instrument,Side,Status,Quantity,Price,Reason,TransactionTime";
string order_header = "Order ID,Cl. Ord. ID.,Instrument,Side,Quantity,Price";

// Function to split a string into lines
vector<string> split(const string &str) {
    vector<string> tokens;
    istringstream tokenStream(str);
    string token;

    while (getline(tokenStream, token)) {
        tokens.push_back(token);
    }

    return tokens;
}

// Function to convert a string to lowercase
string toLowercase(const string &str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return tolower(c);
    });
    return result;
}

// Function to print executions
void print_execs() {
    cout << "Executions :" << endl;
    cout << exec_header << endl;
    for (const Execution &element : executions) {
        cout << element.get_details() << endl;
    }
}

// Function to print orders
void print_orders() {
    cout << "Orders :" << endl;
    cout << order_header << endl;
    for (const Order &element : orders) {
        cout << element.get_details() << endl;
    }
}

// Function to write executions to a CSV file
void writeCSV(const string fileName) {
    ofstream file(fileName);

    if (!file.is_open()) {
        cerr << "Error writing the file." << endl;
        return;
    }

    // Write the header
    file << exec_header << endl;

    // Write the lines
    for (const Execution &exec : executions) {
        file << exec.get_details() << endl;
    }

    file.close();
}

// Function to read CSV lines from a file
pair<string, vector<string>> readCSVLines(const string &fileName) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cerr << "Error opening the file." << endl;
        throw runtime_error("Error opening file");
        return {};
    }

    pair<string, vector<string>> result;
    string line;

    // Read the first line separately
    if (getline(file, line)) {
        result.first = line;
    }

    // Read the remaining lines
    while (getline(file, line)) {
        result.second.push_back(line);
    }

    file.close();

    return result;
}

