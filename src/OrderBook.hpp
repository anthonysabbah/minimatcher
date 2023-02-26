#include <vector>
#include <cassert>
#include "types.hpp"


// this is a neat trick to replace how casts are normally done
#define MKPRIMITIVE(__x) ((std::underlying_type<decltype(__x)>::type)__x)

#define TRACE 1

enum class sprice_t : int32_t {};
bool constexpr is_bid(sprice_t const x) { return int32_t(x) >= 0; }

// TODO: why are these here?
#define MEMORY_DEFS    \
  using size_t__ = typename std::underlying_type<ptr_t>::type;
//  using __ptr = ptr_t; 


enum class book_id_t : uint16_t {};
enum class level_id_t : uint32_t {};
enum class order_id_t : uint32_t {};

/* 
  An implementation of a pool allocator (see https://en.wikipedia.org/wiki/Memory_pool)
  We use vectors for both the mem. pool and the "free block list"
*/
template <class T, typename ptr_t, size_t INIT_SIZE>
class PoolAllocator{
public:
  MEMORY_DEFS;
  std::vector<T> mem_allocated;
  std::vector<ptr_t> mem_free;

  PoolAllocator(){
    mem_allocated.reserve(INIT_SIZE);
  }

  PoolAllocator(size_t reserve_size){
    mem_allocated.reserve(reserve_size);
  }

  T *get(ptr_t idx) { 
    return &mem_allocated[size_t__(idx)]; 
  }

  T &operator[](ptr_t idx) { 
    return mem_allocated[size_t__(idx)]; 
  } 

  ptr_t alloc(){
    // if there is a pointer in mem_free that points to a free block
    if(!mem_free.empty()){
      auto ret = mem_free.back();
      mem_free.pop_back();
      return ret;
    }


    // resize mem_allocated if there isn't and a pointer to the newly allocated block
    auto ret = ptr_t(mem_allocated.size());
    mem_allocated.push_back(T());
    return ret;
  }

  void free(ptr_t idx) { 
    mem_free.push_back(idx); 
  }

};

class Level
{
 public:
  sprice_t m_price;
  qty_t m_qty;
  Level(sprice_t __price, qty_t __qty) : m_price(__price), m_qty(__qty) {}
  Level() {}
};

/*
  Originally, a struct was used to represent the order, 
  but now we use a class (see OrderBook::order_vector in OrderBook for more info)
*/

class Order {
  public:
    Order() {}
    Order(qty_t __qty, level_id_t __level_idx, book_id_t __book_idx) {
    m_qty = __qty;
    level_idx = __level_idx;
    book_idx = __book_idx;
    }
    qty_t m_qty;
    level_id_t level_idx;
    book_id_t book_idx;
};

class PriceLevel
{
 public:
  PriceLevel() {}
  PriceLevel(sprice_t __price, level_id_t __ptr)
      : m_price(__price), m_ptr(__ptr)
  {
  }
  sprice_t m_price;
  level_id_t m_ptr;
};

// operators for custom defined types
bool operator>(PriceLevel a, PriceLevel b)
{
  return int32_t(a.m_price) > int32_t(b.m_price);
}

struct order_id_hash {
  size_t operator()(order_id_t const id) const { return size_t(id); }
};

qty_t operator+(qty_t const a, qty_t const b)
{
  return qty_t(MKPRIMITIVE(a) + MKPRIMITIVE(b));
}


class OrderBook
{
 public:
  static constexpr size_t MAX_BOOKS = 1 << 14;
  static constexpr size_t NUM_LEVELS = 1 << 20;
  static constexpr size_t NUM_ORDERS = 1 << 20;
  static OrderBook *s_books;  // can we allocate this on the stack?
  /*
    since we need dynamic mem allocation bcos static mem is wasteful and limiting for order id, 
    we use a pool allocator instead of a vector with no resizes.
  */ 
  using order_vector = PoolAllocator<Order, order_id_t, NUM_ORDERS>;
  using level_vector = PoolAllocator<Level, level_id_t, NUM_LEVELS>;
  using sorted_levels_t = std::vector<PriceLevel>;
  // A global allocator for all the price levels allocated by all the books.
  static order_vector oid_map;
  static level_vector s_levels;
  sorted_levels_t m_bids;
  sorted_levels_t m_offers;
  // TODO: I replaced the line below with the one following it, how does the next line actually work?:
  // using level_ptr_t = level_vector::__ptr;
  using level_ptr_t = level_id_t;

  static void add_order(book_id_t const book_idx,
                        sprice_t const price, qty_t const qty)
  {
    // oid_map.reserve(oid);
    order_id_t oid = oid_map.alloc();
    Order *order = oid_map.get(oid);
    order->m_qty = qty;
    order->book_idx = book_idx;

#if TRACE
    printf("ADD %lu, %u, %d, %u", oid, book_idx, price, qty);
#endif  // TRACE

    s_books[size_t(book_idx)].ADD_ORDER(order, price, qty);
#if TRACE
    auto lvl = oid_map[oid].level_idx;
    printf(", %u, %u \n", lvl, s_books[size_t(book_idx)].s_levels[lvl].m_qty);
#endif  // TRACE
  }
  void ADD_ORDER(Order *order, sprice_t const price, qty_t const qty)
  {
    sorted_levels_t *sorted_levels = is_bid(price) ? &m_bids : &m_offers;
    // search descending for the price
    auto insertion_point = sorted_levels->end();
    bool found = false;
    while (insertion_point-- != sorted_levels->begin()) {
      PriceLevel &curprice = *insertion_point;
      if (curprice.m_price == price) {
        order->level_idx = curprice.m_ptr;
        found = true;
        break;
      } else if (price > curprice.m_price) {
        // insertion pt will be -1 if price < all prices
        break;
      }
    }
    if (!found) {
      order->level_idx = s_levels.alloc();
      s_levels[order->level_idx].m_qty = qty_t(0);
      s_levels[order->level_idx].m_price = price;
      PriceLevel const px(price, order->level_idx);
      ++insertion_point;
      sorted_levels->insert(insertion_point, px);
    }
    s_levels[order->level_idx].m_qty = s_levels[order->level_idx].m_qty + qty;
  }
  static void delete_order(order_id_t const oid)
  {
#if TRACE
    printf("DELETE %lu\n", oid);
#endif  // TRACE
    Order *order = oid_map.get(oid);
    s_books[size_t(order->book_idx)].DELETE_ORDER(order, oid);
  }
  static void cancel_order(order_id_t const oid, qty_t const qty)
  {
#if TRACE
    printf("REDUCE %lu, %u\n", oid, qty);
#endif  // TRACE
    Order *order = oid_map.get(oid);
    s_books[size_t(order->book_idx)].REDUCE_ORDER(order, qty);
  }
  // shared between cancel(aka partial cancel aka reduce) and execute
  void REDUCE_ORDER(Order *order, qty_t const qty)
  {
    auto tmp = MKPRIMITIVE(s_levels[order->level_idx].m_qty);
    tmp -= MKPRIMITIVE(qty);
    s_levels[order->level_idx].m_qty = qty_t(tmp);

    tmp = MKPRIMITIVE(order->m_qty);
    tmp -= MKPRIMITIVE(qty);
    order->m_qty = qty_t(tmp);
  }
  // shared between delete and execute
  void DELETE_ORDER(Order *order, order_id_t oid)
  {
    // TODO: is this the best way to do this?
    assert(MKPRIMITIVE(s_levels[order->level_idx].m_qty) >=
           MKPRIMITIVE(order->m_qty));
    auto tmp = MKPRIMITIVE(s_levels[order->level_idx].m_qty);
    tmp -= MKPRIMITIVE(order->m_qty);
    s_levels[order->level_idx].m_qty = qty_t(tmp);
    if (qty_t(0) == s_levels[order->level_idx].m_qty) {
      // DELETE_SORTED([order->level_idx].price);
      sprice_t price = s_levels[order->level_idx].m_price;
      sorted_levels_t *sorted_levels = is_bid(price) ? &m_bids : &m_offers;
      auto it = sorted_levels->end();
      while (it-- != sorted_levels->begin()) {
        if (it->m_price == price) {
          sorted_levels->erase(it);
          break;
        }
      }
      s_levels.free(order->level_idx);
    }
    oid_map.free(oid);
  }
  static void execute_order(order_id_t const oid, qty_t const qty)
  {
#if TRACE
    printf("EXECUTE %lu %u\n", oid, qty);
#endif  // TRACE
    Order *order = oid_map.get(oid);
    OrderBook *book = &s_books[MKPRIMITIVE(order->book_idx)];

    if (qty == order->m_qty) {
      book->DELETE_ORDER(order, oid);
    } else {
      book->REDUCE_ORDER(order, qty);
    }
  }
  static void replace_order(order_id_t const oid,
                            qty_t const new_qty, sprice_t new_price)
  {
#if TRACE
    printf("REPLACE %lu %lu %d %u\n", oid, new_price, new_qty);
#endif  // TRACE
    Order *order = oid_map.get(oid);
    OrderBook *book = &s_books[MKPRIMITIVE(order->book_idx)];
    bool const bid = is_bid(book->s_levels[order->level_idx].m_price);
    book->DELETE_ORDER(order, oid);
    if (!bid) {
      new_price = sprice_t(-1 * MKPRIMITIVE(new_price));
    }
    book->add_order(order->book_idx, new_price, new_qty);
  }
};

OrderBook::order_vector OrderBook::oid_map = order_vector();
OrderBook *OrderBook::s_books = new OrderBook[OrderBook::MAX_BOOKS];
OrderBook::level_vector OrderBook::s_levels = level_vector();