#ifndef LIMIT_HPP
#define LIMIT_HPP

#include "order.hpp"


struct limit{
    double price;
    int totalvolume = 0;
    int ordercount = 0;
    order* head = nullptr;
    order* tail = nullptr;
    limit(double price): price(price) {}

    void append_order(order* new_order){
        if(head == nullptr){
            head = new_order;
            tail = new_order;
        }else{
            tail->next = new_order;
            new_order->prev = tail;
            tail = new_order;
        }
        totalvolume += new_order->qty;
    ordercount++;
    }
   
    void remove_order(order* order_to_remove){
        if(order_to_remove == head){
            head = order_to_remove->next;
            if(head != nullptr){
                head->prev = nullptr;
            }
        }else if(order_to_remove == tail){
            tail = order_to_remove->prev;
            if(tail != nullptr){
                tail->next = nullptr;
            }
        }else{
            order_to_remove->prev->next = order_to_remove->next;
            order_to_remove->next->prev = order_to_remove->prev;
        }
        totalvolume -= order_to_remove->qty;
        ordercount--;
    }
}
