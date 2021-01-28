#ifndef RECENT_H
#define RECENT_H

#include "db_util.h"

class Table_name;
class Column_name;
struct Recent;

void show(std::ostream&,DB,Recent const&);
std::vector<Column_name> column_names(Table_name const&);

#endif
