#include "types.hpp"
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <functional>
#include <queue>
#include <sys/_types/_int32_t.h>
#include <unordered_map>
#include <vector>


#ifndef TRACE 
#define TRACE 0
#endif

#ifndef DEBUG 
#define DEBUG 0
#endif

#ifndef ORDERBOOK
#define ORDERBOOK

bool constexpr is_bid(int32_t const x) { return int32_t(x) >= 0; }


struct PriceLevel {
  int32_t price;
  int32_t qty;
  std::vector<uint64_t> orders;

  PriceLevel(int32_t _price) {
    price = _price;
    orders = {};
  }
};


struct OrderBook {
  inline static uint64_t latestOid = 0;
  inline static std::unordered_map<uint64_t, Order *> orders;

  std::vector<PriceLevel> sortedBuyPriceLevels;
  std::vector<PriceLevel> sortedSellPriceLevels;

  inline static std::vector<OrderBook*> *orderBooks = new std::vector<OrderBook*>;

  static void addOrder(Order *order) {
    order->oid = latestOid;
#if TRACE
    printf("ADD, %llu, %u, %u, %i\n", order->oid, order->securityId, order->qty,
        order->price);
#endif
    OrderBook *orderBook = orderBooks->at(order->securityId);
#if DEBUG
    printf("ORDERBOOK FOUND: %i\n", order->securityId);
#endif

    orderBook->ADD_ORDER(order);
    latestOid++;
  }

  void ADD_ORDER(Order *order) {
    orders[order->oid] = order;

    std::vector<PriceLevel> *priceLevelsToAddTo =
      is_bid(order->price) ? &sortedBuyPriceLevels : &sortedSellPriceLevels;

    auto insertionPoint =
      priceLevelsToAddTo->end(); // price level to insert new order at
    bool found = false;            // if appropriate price level has been found

    order = MATCH_ORDER(order);

    if (!order) {
      return;
#if DEBUG
      printf("ORDER FILLED INSTANTLY\n");
#endif 
    }

    while (insertionPoint-- != priceLevelsToAddTo->begin()) {
      PriceLevel &toInsertAt = *insertionPoint;
#if DEBUG
      printf("PRICE LEVEL: %i\n", toInsertAt.price);
#endif

      if (toInsertAt.price == order->price) {
#if DEBUG
        printf("FOUND PRICE LEVEL\n");
#endif
        found = true;
        toInsertAt.orders.push_back(order->oid);
        break;
      } else if (order->price > toInsertAt.price) {
      // insertion pt will be ...->end()+1 if price > all prices
        priceLevelsToAddTo->insert(insertionPoint+1, PriceLevel(order->price));
        (*(insertionPoint+1)).orders.push_back(
            order->oid);
#if DEBUG
        printf("CREATED PRICE LEVEL %i | LEVELS: %lu\n", order->price, priceLevelsToAddTo->size());
        printf("%lu,%lu\n", sortedBuyPriceLevels.size(), sortedSellPriceLevels.size());
#endif
        found = true;
        break;
      }
    }

    if (!found) {
      priceLevelsToAddTo->insert(priceLevelsToAddTo->begin(), PriceLevel(order->price));
      (*priceLevelsToAddTo)[0].orders.push_back(
          order->oid);
#if DEBUG
      printf("CREATED PRICE LEVEL %i | LEVELS: %lu\n", order->price, priceLevelsToAddTo->size());
      printf("%lu,%lu\n", sortedBuyPriceLevels.size(), sortedSellPriceLevels.size());
#endif
    }

    orders[order->oid] = order; // add order to orders map
  }

  static void cancelOrder(Order *order) {
#if TRACE
    printf("CANCEL, %llu, %u, %u, %i\n", order->oid, order->securityId, order->qty,
        order->price);
#endif
    OrderBook *orderBook = orderBooks->at(order->securityId);

    orderBook->CANCEL_ORDER(order);
  }

  // TODO: deleting may be slow (worst case is quadratic)
  void CANCEL_ORDER(Order *order) {
    auto found = orders.find(order->oid);
    if (found == orders.end()) {
#if DEBUG
    printf("DID NOT FIND ORDER TO CANCEL\n");
#endif
      return;
    }

#if DEBUG
    printf("FOUND ORDER TO DELETE\n");
#endif

    std::vector<PriceLevel> *priceLevelsToSearch =
      is_bid(order->price) ? &sortedBuyPriceLevels : &sortedSellPriceLevels;

    orders.erase(order->oid);

    auto insertionPoint =
      priceLevelsToSearch->end(); // price level to for order in

    while (insertionPoint-- != priceLevelsToSearch->begin()) {
      PriceLevel &toInsertAt = *insertionPoint;

      // erase order if found price level
      if (toInsertAt.price == order->price) {
        for (int i = 0; i < toInsertAt.orders.size(); i++) {
          if ((toInsertAt.orders[i]) == order->oid) {
            toInsertAt.orders.erase(toInsertAt.orders.begin() + i);
            if((toInsertAt.orders.size() < 1)){
              priceLevelsToSearch->erase(insertionPoint);
            }
          }
        }
      }
    }

  }

  Order *MATCH_ORDER(Order *order) {
    bool isBid = is_bid(order->price);

    std::vector<PriceLevel> *priceLevelstoSearch =
      is_bid(order->price) ? &sortedSellPriceLevels : &sortedBuyPriceLevels;

    // TODO: is there a cleaner way to do this???
    /*
     * A comparator is used here to make the code look cleaner (otherwise, we'd
     * have two while loops)
     */
    auto cmp = is_bid(order->price) ?
      [](int32_t priceSearched, int32_t orderPrice){
        return std::abs(priceSearched) <= std::abs(orderPrice);
      } :
    [](int32_t priceSearched, int32_t orderPrice){
      return std::abs(priceSearched) >= std::abs(orderPrice);
    };

    auto priceLvlIt = priceLevelstoSearch->end();

    while (priceLvlIt-- != priceLevelstoSearch->begin() &&
        cmp((*priceLvlIt).price, order->price)) {
#if DEBUG
      printf("%i, %i\n", (*priceLvlIt).price, order->price);
      printf("%i\n", cmp((*priceLvlIt).price, order->price));
      printf("SEARCHING\n");
#endif
      for (auto oid : (*priceLvlIt).orders) {
        uint32_t fillAmount = orders[oid]->qty;
        if (fillAmount >= order->qty) {
          orders[oid]->qty = orders[oid]->qty - order->qty; // TODO: YUCK! FIX PLZ!
#if DEBUG
          printf("DELETING ORDER\n");
#endif
          // delete order;
          return nullptr;
        } else {
          order->qty = order->qty - orders[oid]->qty; // TODO: YUCK! FIX PLZ!
#if DEBUG
          printf("CANCEL ORDER DURING MATCHING\n");
#endif
          CANCEL_ORDER(orders[oid]);
          // orders.erase(orders.find(oid));
#if DEBUG
          printf("ORDER PARTIALLY FILLED\n");
#endif
        }
      }
    }
    // #if TRACE
    //     printf("MATCHED, %llu, %u, %u, %u\n", order->oid, order->securityId, order->qty,
    //            order->price);
    // #endif

    return order;
  }
};

// std::vector<OrderBook> *OrderBook::orderBooks = new std::vector<OrderBook>;

#endif // !ORDERBOOK
