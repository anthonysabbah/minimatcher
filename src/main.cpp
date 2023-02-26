//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
// #include <boost/array.hpp>
#include <asio.hpp>

#include "OrderBook.hpp"

// std::string make_daytime_string()
// {
//   using namespace std; // For time_t, time and ctime;
//   time_t now = time(0);
//   return ctime(&now);
// }

// template<itch_t __code>
// itch_message<__code> parse(char* message){
//   auto ret = itch_message<__code>::parse(message);
//   return ret;
// }

template <itch_t __code>
class PROCESS
{
 public:
  static itch_message<__code> parse(char* msg)
  {
    itch_message<__code> ret = itch_message<__code>::parse(msg);
    // __buf->advance(netlen<__code>);
    return ret;
  }
};


static sprice_t mksigned(price_t price, BUY_SELL buy)
{
  assert(MKPRIMITIVE(price) < std::numeric_limits<int32_t>::max());
  auto ret = MKPRIMITIVE(price);
  if (BUY_SELL::SELL == buy) ret = -ret;
  return sprice_t(ret);
}

#define DO_CASE(__itch_t)               \
  case (__itch_t): {                    \
    PROCESS<__itch_t>::parse(recv_buf); \
    break;                              \
  }

using asio::ip::udp;

int main()
{

#define BUILD_BOOK 1
#if !BUILD_BOOK
  size_t nadds(0);
  uint64_t maxoid(0);
#else
  // OrderBook::oid_map.max_load_factor(0.5);
  OrderBook::oid_map.reserve(order_id_t(1000 * 2));  // the first number
                                                          // is the empirically
                                                          // largest oid seen.
                                                          // multiply by 2 for
                                                          // good measure
#endif

  try
  {


    asio::io_context io_context;

    udp::socket socket(io_context, udp::endpoint(udp::v4(), 69));

    for (;;)
    {


      char recv_buf[64];
      udp::endpoint remote_endpoint;
      socket.receive_from(asio::buffer(recv_buf), remote_endpoint);
      for(auto c:recv_buf){
        std::cout << c << " ";
      }
      std::cout << std::endl;

      // get msg type (see types.hpp)
      itch_t const msg_type = itch_t(recv_buf[0]);

      switch(msg_type){
        // DO_CASE(itch_t::ADD_ORDER);
        case (itch_t::ADD_ORDER): {
          // if (!npkts) {
          //   start = std::chrono::steady_clock::now();
          //   ++npkts;
          // }
          auto const pkt = PROCESS<itch_t::ADD_ORDER>::parse(recv_buf);
          assert(uint64_t(pkt.oid) <
                uint64_t(std::numeric_limits<int32_t>::max()));
  #if BUILD_BOOK
          OrderBook::add_order(order_id_t(pkt.oid), book_id_t(pkt.stock_locate),
                                mksigned(pkt.price, pkt.buy), pkt.qty);
  #else
          int64_t oid = int64_t(pkt.oid);
          maxoid = maxoid > uint64_t(pkt.oid) ? maxoid : uint64_t(pkt.oid);
          // printf("oid:%lu, nadds:%lu, npkts:%lu, %lu, %lu, %f, %f\n", oid,
          // nadds, npkts, maxoid, oid - (int64_t)npkts, oid / (double)nadds, oid
          // / (double)npkts);
          // ++nadds;
  #endif
          break;
        }
        // DO_CASE(itch_t::DELETE_ORDER);
        default:{
          printf("woopsy");
        }
      }

      // I'll add checks for sender later
      // process

      asio::error_code ignored_error;
      socket.send_to(asio::buffer(recv_buf),
          remote_endpoint, 0, ignored_error);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}