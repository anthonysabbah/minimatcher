//
// blocking_udp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "asio.hpp"

using asio::ip::udp;

enum { max_length = 36 };

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: blocking_udp_echo_client <host> <port>\n";
      return 1;
    }

    asio::io_context io_context;

    udp::socket s(io_context, udp::endpoint(udp::v4(), 0));

    udp::resolver resolver(io_context);
    udp::resolver::results_type endpoints =
      resolver.resolve(udp::v4(), argv[1], argv[2]);

    // std::cout << "Enter message: ";
    // char request[max_length];
    // std::cin.getline(request, max_length);
    // test message
    char request[max_length] = {
    'A',                                                             // messageType: OrderAdded
    '\x00', '\x45',                                                  // order book id: 69
    '\x00', '\x00',                                                  // tracking number: left empty cos not read by the matching engine at the moment
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x42', // timestamp: 566
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x47', // orderNumber: 71
    'B',                                                            // side: Buy
    '\x00', '\x00', '\x00', '\xCC',                                 // quantity: 204
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x42', // stock ticker: this isn't parsed by the engine either
    '\x00', '\x00', '\x00', '\x46'                                  // price: 70
    };
    std::cout << request << std::endl;
    size_t request_length = std::strlen(request);
    s.send_to(asio::buffer(request, request_length), *endpoints.begin());

    char reply[max_length];
    udp::endpoint sender_endpoint;
    size_t reply_length = s.receive_from(
        asio::buffer(reply, max_length), sender_endpoint);
    std::cout << "Reply is: ";

    std::ostringstream os;
    for(auto c:reply){
      os << std::hex << c;
    }
    std::string mystring = os.str();

    std::cout.write(mystring.c_str(), reply_length);
    std::cout << "\n";
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}