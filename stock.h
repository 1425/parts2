#ifndef STOCK_H
#define STOCK_H

#include "db_util.h"

struct Page_stock_new;

void show(std::ostream&,DB,Page_stock_new const&);

#endif
