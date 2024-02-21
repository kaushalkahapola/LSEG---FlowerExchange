#ifndef TRADER_APPLICATION_H
#define TRADER_APPLICATION_H

#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <cctype>
#include <fstream>
#include <algorithm>
#include "ExchangeApplication.h"

using namespace std;

// Definitions for headers
extern string exec_header;
extern string order_header;

// Function to split a string into lines
vector<string> split(const string &str);

// Function to convert a string to lowercase
string toLowercase(const string &str);

// Function to print executions
void print_execs();

// Function to print orders
void print_orders();

// Function to write executions to a CSV file
void writeCSV(const string fileName);

// Function to read CSV lines from a file
pair<string, vector<string>> readCSVLines(const string &fileName);

// Main function for the trader application
int main();

#endif // TRADER_APPLICATION_H
