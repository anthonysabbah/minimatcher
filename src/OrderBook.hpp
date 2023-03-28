#include <functional>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cassert>
#include <cmath>
#include "types.hpp"

#ifndef ORDERBOOK
#define ORDERBOOK

// this is a neat trick to replace how casts are normally done
#define MKPRIMITIVE(__x) ((std::underlying_type<decltype(__x)>::type)__x)

// template <class T, typename ptr_t, size_t INIT_SIZE>

// enum OrderSide{
//   Buy,
//   Sell
// };


// TODO: is this the best way to do this?
// bool operator>(const Order& a, const Order& b){
//   return a.qty > b.qty;
// }

bool constexpr is_bid(sprice_t const x) { return int32_t(x) >= 0; }

class PriceLevel{
  public:
    sprice_t price;
    std::vector<oid_t> orders;

    PriceLevel(sprice_t _price){
      price = _price;
      orders = {};
    }
};

class OrderBook{
  private:
  inline static oid_t latestOid;
  inline static std::unordered_map<oid_t, Order*> orders;

  std::vector<PriceLevel> sortedBuyPriceLevels;
  std::vector<PriceLevel> sortedSellPriceLevels;


  public:
  static std::vector<OrderBook>* orderBooks;


  static oid_t getLatestOid(){
    return latestOid;
  }

  static void addOrder(Order* order){
#if TRACE
    printf("ADD, %llu, %u, %u, %u\n", order->oid, order->securityId, order->qty, order->price);
#endif
    OrderBook orderBook = orderBooks->at(MKPRIMITIVE(order->securityId));
#if DEBUG
    printf("ORDERBOOK FOUND\n");
#endif

    orderBook.ADD_ORDER(order);
  }

  void ADD_ORDER(Order* order){
    orders[order->oid] = order;

    std::vector<PriceLevel> *priceLevelsToAddTo = is_bid(order->price) ? 
      &sortedBuyPriceLevels : 
      &sortedSellPriceLevels;

    auto insertionPoint = priceLevelsToAddTo->end(); // price level to insert new order at
    bool found = false; // if appropriate price level has been found

    order = MATCH_ORDER(order);

    if(!order){
      return;
    }

    while(insertionPoint != priceLevelsToAddTo->begin()){
      PriceLevel &toInsertAt = *insertionPoint;

      if(toInsertAt.price == order->price){
#if DEBUG
        printf("FOUND PRICE LEVEL\n");
#endif // TRACE
        found = true;
        toInsertAt.orders.push_back(order->oid);
      }

    }

    if(!found){
      priceLevelsToAddTo->push_back(PriceLevel(order->price));
      (*priceLevelsToAddTo)[priceLevelsToAddTo->size() - 1].orders.push_back(order->oid);
    }


    orders[order->oid] = order; // add order to orders map

  }

  static void cancelOrder(Order* order){
    OrderBook orderBook = orderBooks->at(MKPRIMITIVE(order->securityId));

    orderBook.CANCEL_ORDER(order);
  }

  // TODO: deleting may be slow (worst case is quadratic)
  void CANCEL_ORDER(Order* order){
    auto found = orders.find(order->oid);
    if(found == orders.end()){
      return;
    }

    std::vector<PriceLevel> *priceLevelsToSearch = is_bid(order->price) ? 
      &sortedSellPriceLevels :
      &sortedBuyPriceLevels;

    orders.erase(order->oid);


    auto insertionPoint = priceLevelsToSearch->end(); // price level to insert new order at

    while(insertionPoint != priceLevelsToSearch->begin()){
      PriceLevel &toInsertAt = *insertionPoint;

      // erase orders if found price level
      if(toInsertAt.price == order->price){
        for(int i=0; i<toInsertAt.orders.size(); i++){
          if((toInsertAt.orders[i]) == order->oid){
            toInsertAt.orders.erase(toInsertAt.orders.begin() + i);
          }
        }
      }

    }

  }


  Order* MATCH_ORDER(Order* order){

    bool isBid = is_bid(order->price);

    std::vector<PriceLevel> *priceLevelstoSearch =  is_bid(order->price)? 
      &sortedBuyPriceLevels : 
      &sortedSellPriceLevels;
    
    // TODO: is there a cleaner way to do this???
    /*
     * A comparator is used here to make the code look cleaner (otherwise, we'd have two while loops)
     */
    auto cmp = is_bid(order->price) ? 
      [](sprice_t priceSearched, sprice_t orderPrice){
        return std::abs(MKPRIMITIVE(priceSearched)) <= std::abs(MKPRIMITIVE(orderPrice));
      } :
      [](sprice_t priceSearched, sprice_t orderPrice){
        return std::abs(MKPRIMITIVE(priceSearched)) >= std::abs(MKPRIMITIVE(orderPrice));
      };

    auto priceLvlIt = priceLevelstoSearch->end();
    
    while(priceLvlIt-- != priceLevelstoSearch->begin() && cmp((*priceLvlIt).price, order->price)){

      for (auto oid: (*priceLvlIt).orders){
        qty_t fillAmount = orders[oid]->qty;
        if(fillAmount >= order->qty){
          orders[oid]->qty = (qty_t)(MKPRIMITIVE(orders[oid]->qty) - MKPRIMITIVE(order->qty)); // TODO: YUCK! FIX PLZ!
          delete order;
          return nullptr;
        } else{
          order->qty = (qty_t)(MKPRIMITIVE(order->qty) - MKPRIMITIVE(orders[oid]->qty)); // TODO: YUCK! FIX PLZ!
          orders.erase(orders.find(oid));
        }
      }
    }

    return order;
  }

};


std::vector<OrderBook>* OrderBook::orderBooks = new std::vector<OrderBook>;

#endif // !ORDERBOOK
