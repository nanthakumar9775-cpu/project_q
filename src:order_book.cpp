#ifdef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include "limit.hpp"
#include <map>
#include <string>
#include "order.hpp"
#include <unordered_map>


class orderbook{
    std::map<double,limit*> askbook;
    std::map<double,limit*,std::greater<double>> bidbook;
    std::unordered_map<std::string,order*> order_map;
    void matchorder();

public:
    ~orderbook();
    void limitOrder(const std::string& id, side side, double price, int qty);
    void cancelOrder(const std::string& id);
    void printBook() const;


}
#endif
