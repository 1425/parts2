#ifndef EXPORT_H
#define EXPORT_H

#include<string>
#include "db_util.h"

struct Export;

void show(std::ostream&,DB,Export const&);
bool endswith(std::string const&,std::string const&);

#endif
