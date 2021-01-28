#ifndef ORDERS_H
#define ORDERS_H

#include "db.h"
#include "page.h"
#include "status.h"
#include "input.h"

struct Orders;
struct Page_arrived;
struct Page_order;

void show(std::ostream&,DB,Orders const&);
void show(std::ostream&,DB,Page_arrived const&);
void show(std::ostream&,DB,Page_order const&);

std::multiset<Stock_id> wanted(DB);
std::string all_arrived(Page_normal const&,Order_id);
std::vector<Order_member_id> order_members(DB,Order_id);
std::multiset<Stock_id> stock_needed(All_work const&);
std::multiset<Stock_id> wishlist(DB);
std::string suppliers(DB,Stock_id);
Dimensions dimensions(DB,Stock_category_id);
bool exists(DB,Row_id const&);

#endif
