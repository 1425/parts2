#ifndef GET_NAME_H
#define GET_NAME_H

#include "db.h"
#include "page.h"

std::string get_name(DB,Id_table const&,int main,bool show_table=0);

#define X(A,B,C) std::string get_name(DB,B##_id);
TABLES(X)
#undef X

template<typename T>
std::string get_name(DB db,std::optional<T> const& a){
	if(a) return get_name(db,*a);
	return "NULL";
}

std::string a(Id_table const&);
std::string a(Id_table const&,std::string const&);

#define X(A,B,C) std::string a(DB,B##_id);
TABLES(X)
#undef X

std::string a(Page const&,std::string const&);
std::string a(Page const&);
std::string a(DB,Page const&);

#define X(A,B,C) \
	std::string a(B##_id id,std::string const& body);\
	std::string a(std::optional<B##_id> const& id,std::string const& body);
TABLES(X)
#undef X

template<typename T>
std::string a(DB db,std::optional<T> const& dest){
	if(!dest) return "";
	return a(db,*dest);
}

template<typename T>
std::string a(std::optional<T> const& t,std::string const& s){
	if(!t) return "";
	return a(*t,s);
}

std::string a(URL const&,std::string const&);

#endif
