#include "OrderBook.hpp"
#include <iostream>
#include <algorithm>

// 1. Destructor: Safely wipes out all allocated memory when the program closes
OrderBook::~OrderBook() {
    for (auto& pair : orderMap) delete pair.second;
    for (auto& pair : bidBook) delete pair.second;
    for (auto& pair : askBook) delete pair.second;
}

// 2. Submit Order Logic
void OrderBook::limitOrder(const std::string& id, Side side, double price, int qty) {
    // If the order ID already exists, ignore it to prevent duplicates
    if (orderMap.find(id) != orderMap.end()) return;

    // Create a new order receipt in memory
    Order* newOrder = new Order(id, side, price, qty);
    orderMap[id] = newOrder;

    // Route the order to the correct side (Buy or Sell)
    if (side == Side::BUY) {
        // If a price bench for this amount doesn't exist yet, create one
        if (bidBook.find(price) == bidBook.end()) {
            bidBook[price] = new Limit(price);
        }
        bidBook[price]->appendOrder(newOrder); // Sit at the back of the line
    } else {
        if (askBook.find(price) == askBook.end()) {
            askBook[price] = new Limit(price);
        }
        askBook[price]->appendOrder(newOrder); // Sit at the back of the line
    }

    // Every time a new order arrives, immediately check if a trade can happen!
    matchOrders();
}

// 3. Cancel Order Logic
void OrderBook::cancelOrder(const std::string& id) {
    auto it = orderMap.find(id);
    if (it == orderMap.end()) return; // Order not found, nothing to do

    Order* order = it->second;
    Limit* limit = order->parentLimit;
    
    if (limit) {
        limit->removeOrder(order); // Step out of the bench line
        
        // If the price bench is now completely empty, burn it to save space
        if (limit->orderCount == 0) {
            if (order->side == Side::BUY) bidBook.erase(limit->price);
            else askBook.erase(limit->price);
            delete limit;
        }
    }
    orderMap.erase(it); // Erase from master registry
    delete order;       // Throw away the physical order receipt memory
}

// 4. The Matching Loop (The Trade Engine)
void OrderBook::matchOrders() {
    // Keep trading as long as there are both buyers and sellers available
    while (!bidBook.empty() && !askBook.empty()) {
        auto bestBidIt = bidBook.begin(); // Highest Buyer
        auto bestAskIt = askBook.begin(); // Cheapest Seller

        // If the buyer is willing to pay equal to or more than what the seller wants
        if (bestBidIt->first >= bestAskIt->first) {
            Limit* bidLimit = bestBidIt->second;
            Limit* askLimit = bestAskIt->second;
            
            Order* bidOrder = bidLimit->head; // First buyer in line
            Order* askOrder = askLimit->head; // First seller in line

            // Match orders at these price benches until one bench runs out of people
            while (bidOrder && askOrder) {
                // Find out how many units can be exchanged (whichever is smaller)
                int matchQty = std::min(bidOrder->qty, askOrder->qty);
                
                std::cout << "[TRADE MATCH] " << matchQty << " units matched @ ₹" 
                          << askLimit->price << " (" << bidOrder->id << " <-> " << askOrder->id << ")\n";

                // Deduct the matched volume from the orders and benches
                bidOrder->qty -= matchQty;
                askOrder->qty -= matchQty;
                bidLimit->totalVolume -= matchQty;
                askLimit->totalVolume -= matchQty;

                // If the buyer got all their apples, remove them from the book
                if (bidOrder->qty == 0) {
                    Order* nextBid = bidOrder->next;
                    bidLimit->removeOrder(bidOrder);
                    orderMap.erase(bidOrder->id);
                    delete bidOrder;
                    bidOrder = nextBid; // Move to the next buyer in line
                }
                
                // If the seller sold all their apples, remove them from the book
                if (askOrder->qty == 0) {
                    Order* nextAsk = askOrder->next;
                    askLimit->removeOrder(askOrder);
                    orderMap.erase(askOrder->id);
                    delete askOrder;
                    askOrder = nextAsk; // Move to the next seller in line
                }
            }

            // Clean up the price benches if they are completely empty
            if (bidLimit->orderCount == 0) { bidBook.erase(bestBidIt); delete bidLimit; }
            if (askLimit->orderCount == 0) { askBook.erase(bestAskIt); delete askLimit; }
        } else {
            // The prices do not cross (e.g., Best Bid is ₹100, Best Ask is ₹105). Stop matching.
            break;
        }
    }
}

// 5. Visualizer to print out the book state
void OrderBook::printBook() const {
    std::cout << "\n--- CURRENT ORDER BOOK STATUS ---\n";
    std::cout << "--- ASKS (Sellers) ---\n";
    for (auto it = askBook.rbegin(); it != askBook.rend(); ++it) {
        std::cout << "  Price: ₹" << it->first << " | Volume: " << it->second->totalVolume << "\n";
    }
    std::cout << "---------------------------------\n";
    std::cout << "--- BIDS (Buyers) ---\n";
    for (auto const& [price, limit] : bidBook) {
        std::cout << "  Price: ₹" << price << " | Volume: " << limit->totalVolume << "\n";
    }
    std::cout << "---------------------------------\n\n";
}
