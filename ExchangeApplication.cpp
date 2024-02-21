#include "ExchangeApplication.h"

// Function to get the current date and time formatted
string getCurrentDateTimeFormatted() {
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);

    struct tm *timeinfo = localtime(&currentTime);

    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    stringstream formattedTime;
    formattedTime << put_time(timeinfo, "%Y/%m/%d-%H.%M.%S") << '.' << setfill('0') << setw(3) << milliseconds;

    return formattedTime.str();
}

// Order class methods
Order::Order(string client_order_id, string order_id, string instrument, int side, float price, int quantity, bool auto_gen)
    : client_order_id(client_order_id), order_id(order_id), instrument(instrument), side(side),
      price(price), quantity(quantity), auto_gen(auto_gen) {}

Order::Order(const Order &other)
    : client_order_id(other.client_order_id), order_id(other.order_id), instrument(other.instrument),
      side(other.side), price(other.price), quantity(other.quantity), auto_gen(other.auto_gen) {}

Order::Order() : client_order_id(""), order_id(""), instrument(""), side(0), quantity(0), price(0), auto_gen(false) {}

string Order::get_details() const {
    ostringstream priceFormatted;
    priceFormatted << fixed << setprecision(2) << price;

    return order_id + "," + client_order_id + "," + instrument + "," + to_string(side) + ","
        + to_string(quantity) + "," + priceFormatted.str();
}

bool Order::is_null() {
    return order_id == "";
}

// Execution class methods
Execution::Execution(string order_id, string client_order_id, string instrument, int side,
                     float price, int quantity, int status, string reason)
    : order_id(order_id), client_order_id(client_order_id), instrument(instrument), side(side),
      price(price), quantity(quantity), status(status), reason(reason),
      transaction_time(getCurrentDateTimeFormatted()) {}

Execution::Execution(Order order, int status, string reason)
    : order_id(order.order_id), client_order_id(order.client_order_id), instrument(order.instrument),
      side(order.side), price(order.price), quantity(order.quantity), status(status), reason(reason),
      transaction_time(getCurrentDateTimeFormatted()) {}

string Execution::get_details() const {
    ostringstream priceFormatted;
    priceFormatted << fixed << setprecision(2) << price;

    return order_id + "," + client_order_id + "," + instrument + "," + to_string(side) + ","
        + statusMap[status] + "," + to_string(quantity) + "," + priceFormatted.str() + ","
        + reason + "," + transaction_time;
}

// OrderBook class methods
int OrderBook::set_position_of_order(Order order) {
    if (order.side == 1) {
        for (size_t i = 0; i < buy_orders.size(); i++) {
            if (buy_orders[i].price < order.price) {
                return i;
            }
        }
        return buy_orders.size();
    } else {
        for (size_t i = 0; i < sell_orders.size(); i++) {
            if (sell_orders[i].price > order.price) {
                return i;
            }
        }
        return sell_orders.size();
    }
}

void OrderBook::process_order(Order &order) {
    if (order.side == 1) { // buy order
        if (sell_orders.size() > 0) {
            float min_ask = sell_orders[0].price;
            if (min_ask <= order.price) {
                int status = execute_trade(order, sell_orders[0]);
                if (status == 22) {
                    order.price = sell_orders[0].price;
                    executions.push_back(Execution(order, 2));
                    executions.push_back(Execution(sell_orders[0], 2));
                    sell_orders.erase(sell_orders.begin());
                } else if (status == 23) {
                    order.price = sell_orders[0].price;
                    executions.push_back(Execution(order, 2));
                    executions.push_back(Execution(sell_orders[0], 3));
                    sell_orders[0].quantity -= order.quantity;
                } else {
                    Order order_done = Order(order);
                    order_done.quantity = sell_orders[0].quantity;
                    order_done.price = sell_orders[0].price;
                    executions.push_back(Execution(order_done, 3));
                    executions.push_back(Execution(sell_orders[0], 2));
                    order.quantity -= sell_orders[0].quantity;
                    sell_orders.erase(sell_orders.begin());
                    order.auto_gen = true;
                    process_order(order);
                }
            } else {
                int insert_index = set_position_of_order(order);
                buy_orders.insert(buy_orders.begin() + insert_index, order);
                if (!order.auto_gen) {
                    executions.push_back(Execution(order, 0));
                }
            }
        } else {
            int insert_index = set_position_of_order(order);
            buy_orders.insert(buy_orders.begin() + insert_index, order);
            if (!order.auto_gen) {
                executions.push_back(Execution(order, 0));
            }
        }
    } else { // sell order
        if (buy_orders.size() > 0) {
            float max_bid = buy_orders[0].price;
            if (max_bid >= order.price) {
                int status = execute_trade(order, buy_orders[0]);
                if (status == 22) {
                    order.price = buy_orders[0].price;
                    executions.push_back(Execution(order, 2));
                    executions.push_back(Execution(buy_orders[0], 2));
                    buy_orders.erase(buy_orders.begin());
                } else if (status == 23) {
                    order.price = buy_orders[0].price;
                    executions.push_back(Execution(order, 2));
                    executions.push_back(Execution(buy_orders[0], 3));
                    buy_orders[0].quantity -= order.quantity;
                } else {
                    Order order_done = Order(order);
                    order_done.quantity = buy_orders[0].quantity;
                    order_done.price = buy_orders[0].price;
                    executions.push_back(Execution(order_done, 3));
                    executions.push_back(Execution(buy_orders[0], 2));
                    order.quantity -= buy_orders[0].quantity;
                    buy_orders.erase(buy_orders.begin());
                    order.auto_gen = true;
                    process_order(order);
                }
            } else {
                int insert_index = set_position_of_order(order);
                sell_orders.insert(sell_orders.begin() + insert_index, order);
                if (!order.auto_gen) {
                    executions.push_back(Execution(order, 0));
                }
            }
        } else {
            int insert_index = set_position_of_order(order);
            sell_orders.insert(sell_orders.begin() + insert_index, order);
            if (!order.auto_gen) {
                executions.push_back(Execution(order, 0));
            }
        }
    }
}

int OrderBook::execute_trade(Order order_1, Order order_2) {
    if (order_1.quantity == order_2.quantity) {
        return 22;
    }
    int amount_to_trade = min(order_1.quantity, order_2.quantity);

    if (amount_to_trade == order_1.quantity) {
        return 23;
    } else {
        return 32;
    }
}

void OrderBook::get_details() {
    std::cout << "Buy orders: \n";
    for (auto &it : buy_orders) {
        cout << it.get_details() << endl;
    }
    cout << endl;
    std::cout << "\nSell orders:\n";
    for (auto &it : sell_orders) {
        cout << it.get_details() << endl;
    }
}

// Function to validate and create an Order object
Order validate_order(string order_id, string line) {
    string client_order_id;
    string instrument;
    int side;
    float price;
    int quantity;
    int status = 0;
    string reason;
    string transaction_time;

    istringstream ss(line);
    string s;
    int i = 0;
    while (getline(ss, s, ',')) {
        switch (i++) {
        case 0: // client_order_id
            client_order_id = s;
            break;
        case 1: // instrument
            if (!(instruments.find(s) != instruments.end())) {
                reason += "Invalid instrument ";
                status = 1;
            }
            instrument = s;
            break;
        case 2: // side
            try {
                side = stoi(s);
                if (!(side == 1 || side == 2))
                    throw runtime_error("Invalid value ");
            } catch (const exception &e) {
                reason += "Invalid side ";
                status = 1;
            }
            break;
        case 3: // quantity
            try {
                quantity = stoi(s);
                if (!(9 < quantity && quantity < 1000 && quantity % 10 == 0))
                    throw runtime_error("Invalid value ");
            } catch (const exception &e) {
                reason += "Invalid size ";
                status = 1;
            }
            break;
        case 4: // price
            try {
                price = stod(s);
                if (price <= 0)
                    throw runtime_error("Invalid value ");
            } catch (const exception &e) {
                reason += "Invalid price ";
                status = 1;
            }
            break;
        default:
            break;
        }
    }

    Order order = Order(client_order_id, order_id, instrument, side, price, quantity);
    orders.push_back(order);

    if (status != 1) {
        return order;
    } else {
        Execution execution = Execution(order, status, reason);
        executions.push_back(execution);
        return Order();
    }
}

// Function to add orders to the order books
void add_orders(vector<string> order_csv_s) {
    for (string ins : instruments) {
        orderbooks[ins] = OrderBook();
    }
    int order_i = 1;
    string order_id;
    OrderBook orderBook;
    for (auto &line : order_csv_s) {
        order_id = "ord" + to_string(order_i);

        try {
            Order order = validate_order(order_id, line);
            if (!order.is_null()) {
                order.auto_gen = false;
                orderbooks[order.instrument].process_order(order);
            }
        } catch (const exception &e) {
            cerr << "Error processing order: " << e.what() << endl;
            // Handle the error, maybe log it, and continue processing other orders
        }

        order_i++;
    }
}

// Global data structures (to be defined in the corresponding source file)
set<string> instruments = {"rose", "lavender", "lotus", "tulip", "orchid"};
unordered_map<int, string> statusMap = {
    {0, "New"},
    {1, "Rejected"},
    {2, "Fill"},
    {3, "PFill"}
};

vector<Execution> executions;
vector<Order> orders;


unordered_map<string, OrderBook> orderbooks;

