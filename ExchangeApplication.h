#ifndef EXCHANGE_APPLICATION_H
#define EXCHANGE_APPLICATION_H

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <set>
#include <vector>
#include <unordered_map>

using namespace std;

// Function to get the current date and time formatted
string getCurrentDateTimeFormatted();

// Order class definition
class Order {
public:
    string client_order_id;
    string order_id;
    string instrument;
    int side;
    float price;
    int quantity;
    bool auto_gen;

    Order(string client_order_id, string order_id, string instrument, int side, float price, int quantity, bool auto_gen = true);
    Order(const Order &other);
    Order();

    string get_details() const;
    bool is_null();
};

// Execution class definition
class Execution {
public:
    string client_order_id;
    string order_id;
    string instrument;
    int side;
    float price;
    int quantity;
    int status;
    string reason;
    string transaction_time;

    Execution(string order_id, string client_order_id, string instrument, int side, float price, int quantity, int status, string reason = "");
    Execution(Order order, int status, string reason = "Success");

    string get_details () const;
};

// OrderBook class definition
class OrderBook {
public:
    vector<Order> buy_orders;
    vector<Order> sell_orders;

    int set_position_of_order(Order order);
    void process_order(Order &order);
    int execute_trade(Order order_1, Order order_2);
    void get_details();
};

// Function to validate and create an Order object
Order validate_order(string order_id, string line);

// Function to add orders to the order books
void add_orders(vector<string> order_csv_s);

void allocate_vector();

// Global data structures
extern set<string> instruments;
extern unordered_map<int, string> statusMap;
extern vector<Execution> executions;
extern vector<Order> orders;
extern unordered_map<string, OrderBook> orderbooks;

#endif // EXCHANGE_APPLICATION_H
