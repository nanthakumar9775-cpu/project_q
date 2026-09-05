#ifndef ORDER_HPP
#define ORDER_HPP

#include <string>

enum class side{
    BUY,
    SELL
};

struct order{
    std::string order_id;
    
    side side;
    double price;
    int qty;

    order* prev = nullptr;
    order* next = nullptr;

    order(std::string order_id, side side, double price, int qty)
        : order_id(order_id), side(side), price(price), qty(qty) {}
}

#endif // ORDER_HPP
