#ifndef DB_UTIL_H
#define DB_UTIL_H

#include<optional>
#include<vector>
#include<string>
#include<mariadb/mysql.h>

using DB=MYSQL*;

void run_cmd(DB db,std::string const& cmd);
std::vector<std::vector<std::optional<std::string>>> query(DB db,std::string const& query);
void insert(DB,std::string const& table,std::vector<std::pair<std::string,std::string>> const&);

#endif
