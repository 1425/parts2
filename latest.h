#ifndef LATEST_H
#define LATEST_H

#include "page.h"

void show(std::ostream&,DB,Latest const&);

enum class Category{
	IGNORE,HIDE,SHOW
};

struct Form_item{
	Category category;
	Column_name name;
	Typename type;
};

void show_row(
	std::ostream &ss,
	Id_table const& table,
	Page_normal const& page,
	std::vector<Form_item> const& cols,
	std::vector<std::optional<std::string>> const& row,
	DB db,
	bool
);

std::vector<Form_item> form_columns(Id_table const&);

std::string thumbnail(std::string const&,std::optional<std::string> const&);
std::string redirect_items(Page_normal const&);

std::map<Order_id,std::string> order_labels(DB);

bool parse_bool(std::string const&);
bool parse_bool(int);

std::map<int,std::string> current_options(DB,Id_table const&);
Upload_id parse(Upload_id const*,int);

std::string html_escape(char);
std::string html_escape(std::string const&);

template<typename T>
int stoi(std::optional<T> const& a){
	if(a) return stoi(*a);
	nyi//return 0;
}

#define X(A,B,C) int stoi(B##_id);
TABLES(X)
#undef X

template<typename T>
T parse(T const*,std::optional<T> const& t){
	if(!t) throw "was null";
	return *t;
}

template<typename T>
std::string thumbnail(std::string,std::optional<T>,Upload_id)nyi

Part_state parse(Part_state const*,Assembly_state);
Part_design_state parse(Part_design_state const*,Assembly_state);
std::string escape(Column_value const&);
std::string link_type(DB,Typename const&,std::string const&);

#define X(A,B,C) std::string show(DB,B##_id);
TABLES(X)
#undef X

Column_value to_val(Typename,std::optional<std::string>);
std::optional<Sort> sort(Page_normal const&);

#endif
