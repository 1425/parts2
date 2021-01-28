#ifndef ITEM_H
#define ITEM_H

#include "page.h"

void show(std::ostream&,DB,Item const&);

unsigned available(DB,Stock_id);

#define X(A,B,C) Page page(B##_id);
TABLES(X)
#undef X

#define X(A,B,C) Page_normal page_normal(B##_id);
TABLES(X)
#undef X

void show_table(std::ostream&,DB,Page_normal const&,Table_name const&);

#endif
