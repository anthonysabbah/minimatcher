#include "catch.hpp"
#include "OrderBook.hpp"

TEST_CASE( "Testing OrderBook Methods", "[minimatcher]" ) {
  Order order1;
  order1.securityId = 0;
  order1.price = 1000000;
  order1.qty = 10;

  Order order2;
  order2.securityId = 0;
  order2.price = -1010000;
  order2.qty = 10;

  Order order3;
  order3.securityId = 0;
  order3.price = 1000000;
  order3.qty = 5;

  Order order4;
  order4.securityId = 0;
  order4.price = -1010000;
  order4.qty = 5;

  Order order5;
  order5.securityId = 0;
  order5.price = -1015000;
  order5.qty = 10;

  Order order6;
  order6.securityId = 0;
  order6.price = 990000;
  order6.qty = 20;

  Order order7;
  order7.securityId = 0;
  order7.price = -1012500;
  order7.qty = 10;

  Order order8;
  order8.securityId = 0;
  order8.price = 995000;
  order8.qty = 10;

  Order order9;
  order9.securityId = 0;
  order9.price = 992500;
  order9.qty = 10;


  Order order10;
  order10.securityId = 0;
  order10.price = -990000;
  order10.qty = 20;

  Order order11;
  order11.securityId = 0;
  order11.price = -990000;
  order11.qty = 10;


  OrderBook::orderBooks = new std::vector<OrderBook*>;
  OrderBook orderbk = OrderBook();
  OrderBook::orderBooks->push_back(&orderbk);
  
  SECTION("order should be added when there are no orders to be matched with"){
    OrderBook::addOrder(&order1);
    OrderBook *orderBook0 = OrderBook::orderBooks->at(0);

    REQUIRE( orderBook0->sortedBuyPriceLevels.size() == 1);
  }

  SECTION("order should be canceled"){
    OrderBook *orderBook0 = OrderBook::orderBooks->at(0);

    OrderBook::cancelOrder(&order1);
    REQUIRE( orderBook0->sortedBuyPriceLevels.size() == 0);
  }

  SECTION( "the order book should match orders correctly" ) {
    OrderBook::addOrder(&order1);
    OrderBook::addOrder(&order2);
    OrderBook::addOrder(&order3);
    OrderBook::addOrder(&order4);
    OrderBook::addOrder(&order5);
    OrderBook::addOrder(&order6);
    OrderBook::addOrder(&order7);
    OrderBook::addOrder(&order8);
    OrderBook::addOrder(&order9);

    OrderBook *orderBook0 = OrderBook::orderBooks->at(0);

    REQUIRE( orderBook0->sortedBuyPriceLevels.size() == 4);

    OrderBook::addOrder(&order10);

    REQUIRE( orderBook0->sortedBuyPriceLevels.size() == 3);

    OrderBook::addOrder(&order11);

    REQUIRE( orderBook0->sortedBuyPriceLevels.size() == 2);

    REQUIRE( OrderBook::orderBooks->size() == 1 );
    REQUIRE( orderBook0->sortedSellPriceLevels.size() == 3);

    WARN("CHECKING PRICE LEVEL ORDER");
    for(auto priceLevel: orderBook0->sortedBuyPriceLevels){
      WARN("" << int32_t(priceLevel.price));
    }
    WARN("-----");
    for(auto priceLevel: orderBook0->sortedSellPriceLevels){
      WARN("" << int32_t(priceLevel.price));
    }
  }

}


// TEST_CASE( "Test Orderbook", "[minimatcher]" ) {
//   Order order;
//   order.oid = oid_t(69);
//   order.securityId = scrty_t(0);
//   order.price = sprice_t(4206969);
//   order.qty = qty_t(42);

//   OrderBook orderbk = OrderBook();
//   OrderBook::orderBooks->push_back(orderbk);
//   OrderBook::addOrder(&order);
// }
