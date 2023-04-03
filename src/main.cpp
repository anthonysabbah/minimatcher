#include <_types/_uint32_t.h>
#include <ctime>
#include <iostream>
#include <string>
#include <asio.hpp>
#include <sys/_types/_int32_t.h>

#define TRACE 0
#define DEBUG 0
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


// static int32_t mksigned(uint32_t price, BUY_SELL side)
// {
//   assert(MKPRIMITIVE(price) < std::numeric_limits<int32_t>::max());
//   auto ret = MKPRIMITIVE(price);
//   if (BUY_SELL::SELL == side) ret = -ret;
//   return int32_t(ret);
// }

#define DO_CASE(__itch_t)               \
  case (__itch_t): {                    \
    PROCESS<__itch_t>::parse(recv_buf); \
    break;                              \
  }
// using asio::ip::udp;

int main(){

  Order order;
  order.oid = 69;
  order.securityId = 0;
  order.price = 4206969;
  order.qty = 42;

#if DEBUG
  printf("starting...\n");
#endif // DEBUG
  
  OrderBook orderbk = OrderBook();
  OrderBook::orderBooks = new std::vector<OrderBook*>;

  OrderBook::orderBooks->push_back(&orderbk);
  OrderBook::addOrder(&order);
}
