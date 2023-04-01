#include "catch.hpp"
#include "OrderBook.hpp"

TEST_CASE( "Testing OrderBook Methods", "[minimatcher]" ) {
    Order order1;
    order1.securityId = scrty_t(0);
    order1.price = sprice_t(1000000);
    order1.qty = qty_t(10);

    Order order2;
    order2.securityId = scrty_t(0);
    order2.price = sprice_t(-1010000);
    order2.qty = qty_t(10);

    Order order3;
    order3.securityId = scrty_t(0);
    order3.price = sprice_t(1000000);
    order3.qty = qty_t(5);

    Order order4;
    order4.securityId = scrty_t(0);
    order4.price = sprice_t(-1010000);
    order4.qty = qty_t(5);

    Order order5;
    order5.securityId = scrty_t(0);
    order5.price = sprice_t(-1015000);
    order5.qty = qty_t(10);

    Order order6;
    order6.securityId = scrty_t(0);
    order6.price = sprice_t(990000);
    order6.qty = qty_t(20);

    OrderBook orderbk = OrderBook();

    OrderBook::orderBooks->push_back(&orderbk);

    OrderBook::addOrder(&order1);
    OrderBook::addOrder(&order2);
    OrderBook::addOrder(&order3);
    OrderBook::addOrder(&order4);
    OrderBook::addOrder(&order5);
    OrderBook::addOrder(&order6);


    OrderBook *orderBook0 = OrderBook::orderBooks->at(0);

    REQUIRE( OrderBook::orderBooks->size() == 1 );
    REQUIRE( orderBook0->getBuyPriceLevelsSize() == 2);
    REQUIRE( orderBook0->getSellPriceLevelsSize() == 2);

    WARN("CHECKING PRICE LEVEL ORDER");
    for(auto priceLevel: orderBook0->getSortedBuyPriceLevels()){
      WARN("" << int32_t(priceLevel.price));
    }
    WARN("-----");
    for(auto priceLevel: orderBook0->getSortedSellPriceLevels()){
      WARN("" << int32_t(priceLevel.price));
    }


    // SECTION( "resizing bigger changes size and capacity" ) {
    // }
    // SECTION( "resizing smaller changes size but not capacity" ) {
    // }
    // SECTION( "reserving bigger changes capacity but not size" ) {
    // }
    // SECTION( "reserving smaller does not change size or capacity" ) {
    // }
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
