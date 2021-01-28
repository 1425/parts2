#ifndef INSERT_H
#define INSERT_H

#include "db.h"

struct Page_insert;

void show(std::ostream&,DB,Page_insert const&);
std::set<Id_table> normal_name_tables();

void invalid_data(std::ostream&,std::string const&,Get_cgi_var::Errors const&);

std::optional<std::pair<std::set<Column_name>,std::set<Column_name>>> check_column_names(
	Id_table const&,
	std::set<Column_name> const&
);

#endif
