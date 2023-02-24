#include <iostream>
#include <cstdint>
#include <endian.h>


using namespace std;

struct Order{
  uint32_t timestamp;
  uint16_t tradeDate;
  uint64_t contractNumber;
  // uint16_t oid;
};

int main()
{
    const char msg[] = {
        'A',
        '\xFF', '\xEE', '\xDD', '\xCC',                                 // timestamp: 4293844428
        '\xFF', '\xEE',                                                 // tradeDate: 65518
        '\xFF', '\xEE', '\xDD', '\xCC',                                 // contractNumber: 4293844428
        // 'B',                                                            // side: Buy
        // '\xFF', '\xEE', '\xDD', '\xCC', '\xBB', '\xAA', '\x99', '\x88', // orderNumber: 18441921395520346504
        // '\xFF', '\xEE', '\xDD', '\xCC',                                 // orderBookPriority: 4293844428
        // '\xFF', '\xEE', '\xDD', '\xCC',                                 // quantity: 4293844428
        // '\xFF', '\xEE', '\xDD', '\xCC'                                  // price: -1122868
    };
    
    Order o{*reinterpret_cast<const Order*>(msg)};
    o.timestamp = be32toh(o.timestamp);
    cout << o.timestamp << endl;

    return 0;
}
