#include <ctime>
#include <iostream>
#include <string>
#include <asio.hpp>

#define TRACE 1
#define DEBUG 1
#include "OrderBook.hpp"

template <itch_t __code>
class PROCESS
{
 public:
  static itch_message<__code> parse(char* msg)
  {
    itch_message<__code> ret = itch_message<__code>::parse(msg);
    return ret;
  }
};


static sprice_t mksigned(price_t price, BUY_SELL side)
{
  assert(MKPRIMITIVE(price) < std::numeric_limits<int32_t>::max());
  auto ret = MKPRIMITIVE(price);
  if (BUY_SELL::SELL == side) ret = -ret;
  return sprice_t(ret);
}

#define DO_CASE(__itch_t)               \
  case (__itch_t): {                    \
    PROCESS<__itch_t>::parse(recv_buf); \
    break;                              \
  }
// using asio::ip::udp;

int main(){

  Order order;
  order.oid = oid_t(69);
  order.securityId = scrty_t(0);
  order.price = sprice_t(4206969);
  order.qty = qty_t(42);

#if DEBUG
  printf("starting...\n");
#endif // DEBUG
  
  OrderBook orderbk = OrderBook();
  OrderBook::orderBooks->push_back(&orderbk);
  OrderBook::addOrder(&order);
}
