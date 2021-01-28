#include "insert.h"
#include "db.h"
#include "status.h"
#include "latest.h"
#include "extra.h"
#include "item.h"
#include "main.h"
#include "get_name.h"
#include "orders.h"
#include "variant_util.h"
#include "map_util.h"
#include "set_util.h"

using namespace std;

optional<pair<set<Column_name>,set<Column_name>>> check_column_names(
	Id_table const& table,
	set<Column_name> const& given_cols1
){
	auto given_cols=to_set(MAP(Column_name,given_cols1));

	auto e=table_types();
	auto this_table=e[table];
	auto expected_names=to_set(firsts(this_table));
	if(expected_names!=given_cols){
		auto extra=given_cols-expected_names;
		auto missing=expected_names-given_cols;
		return std::make_pair(extra,missing);
	}
	return std::nullopt;
}

void invalid_data(std::ostream& o,std::string const& operation,Get_cgi_var::Errors const& errors){
	return page(
		o,
		"Error - invalid data for "+operation,
		tag(
			"table border",
			tr(th("Column")+th("Type")+th("Found Value")+th("Example valid value"))+
			join(mapf(
				[](auto e){
					return tr(td(e.col.get())+td(e.type.data)+td(e.value)+td(e.example));
				},
				errors
			))
		)+
		"<button onclick=\"window.history.back();\">Go back</button>"
	);
}

set<Id_table> normal_name_tables(){
	return set<Id_table>{
		Id_table{"stock_category"},
		Id_table{"part_design"},
		Id_table{"assembly_design"},
		Id_table{"assembly"},
		Id_table{"location"},
		Id_table{"machine"},
		Id_table{"supplier"},
		//image's is not normal because no reason can't have the same filename
	};
}

map<string,Id_table> existing_names(DB db){
	map<string,Id_table> r;
	for(auto table:normal_name_tables()){
		auto q=query_to<std::string>(db,"SELECT name "+current(table));
		for(auto elem:q){
			r.insert_or_assign(elem,table);
		}
	}
	return r;
}

#define X(A,B,C) optional<string> check_valid(DB db,Column_name const&,B##_id a){\
	if(!keys(current_options(db,Id_table{""#A})).count(a.data)){\
		return "Not a valid id for "+as_string(a);\
	}\
	return std::nullopt;\
}
TABLES(X)
#undef X

optional<string> check_valid(DB,Column_name const& name,Date const& date){
	if(name=="expected_arrival" && in_past(date)){
		return "Not allowed to set expected arrival to a date in the past";
	}
	return std::nullopt;
}

template<typename T>
optional<string> check_valid(DB,Column_name const&,T const&){
	return std::nullopt;
}

optional<string> check_valid(DB db,Column_name const& name,Column_value const& a){
	return visit([&](auto x){ return check_valid(db,name,x); },a);
}

optional<string> check_redundant(DB db,Id_table const& table,map<Column_name,Column_value> const& values,std::optional<std::string> const& existing){
	//if this table has a name column, then make sure that the name is unique.
	if(normal_name_tables().count(table)){
		auto f1=values.find(Column_name{"name"});
		assert(f1!=values.end());
		auto value=f1->second;

		string s_name;
		if(holds_alternative<std::string>(value)){
			s_name=get<string>(value);
		}else if(holds_alternative<Nonempty_string>(value)){
			s_name=get<Nonempty_string>(value).get();
		}else{
			PRINT(value.index());
			nyi
		}
		if(s_name==existing) return std::nullopt;

		auto x=existing_names(db);
		auto f=x.find(s_name);
		if(f!=x.end() && s_name!=""){ //don't keep there from being multiple things with blank names.
			Id_table t{f->second.get()};
			return "Name \""+s_name+"\" was already used:"+a(t);
		}
	}

	//if a reference to another table, then see that it actually exists.
	for(auto [name,value]:values){
		auto c=check_valid(db,name,value);
		if(c) return c;
	}

	if(table==Id_table{"stock"}){
		auto f=values.find(Column_name{"category"});
		if(f==values.end()){
			return "Could not find category";
		}
		if(!holds_alternative<Stock_category_id>(f->second)){
			return "Wrong type of category";
		}
		auto cat=get<Stock_category_id>(f->second);
		auto expected_dimensions=dimensions(db,cat);

		int found=0;
		bool invalid=0;
		auto g=[&](auto name){
			auto f=values.find(Column_name{name});
			if(f==values.end()){
				invalid=1;
				return;
			}
			if(!holds_alternative<Dimension>(f->second)){
				invalid=1;
				return;
			}
			auto d=get<Dimension>(f->second);
			if(d) found++;
		};
		g("length");
		g("width");
		g("thickness");
		if(invalid){
			return "Dimensions garbled";
		}
		if(found!=expected_dimensions){
			return "Wrong number of dimensions, expected "+as_string(expected_dimensions)+" but found "+as_string(found);
		}
	}

	return std::nullopt;
}

template<typename T>
bool check_link(DB,T const&){
	return 0;
}

#define X(A,B,C) bool check_link(DB db,B##_id value){\
	return !current_options(db,&value).count(value);\
}\
bool check_link(DB db,std::optional<B##_id> const& value){\
	if(!value) return 0;\
	return check_link(db,*value);\
}
TABLES(X)
#undef X

vector<string> check_links(DB db,Column_name const& name,Column_value const& value){
	(void)name;
	auto b=visit([&](auto x){ return check_link(db,x); },value);
	if(!b) return {};
	return vector<string>{name+" "+as_string(value)};
}

optional<string> check_links(DB db,map<Column_name,Column_value> const& values){
	auto issues=flatten(mapf([&](auto p){ return check_links(db,p.first,p.second); },values));
	if(issues.empty()) return std::nullopt;
	return join("<br>",issues);
}

void show(ostream& o,DB db,Page_insert const& vars){
	if(holds_alternative<Get_cgi_var::Errors>(vars.values)){
		auto errors=std::get<Get_cgi_var::Errors>(vars.values);
		invalid_data(o,"insert",errors);
		return;
	}
	auto m=std::get<Get_cgi_var::Success>(vars.values);

	{
		auto c=check_column_names(vars.table,keys(m));
		if(c){
			page(o,"Insert","Column mismatch");
			return;
		}
	}

	{
		auto c=check_redundant(db,vars.table,m,std::nullopt);
		if(c){
			page(o,"insert",*c);
			return;
		}
	}

	{
		auto c=check_links(db,m);
		if(c){
			page(o,"insert",*c);
			return;
		}
	}


	auto id=new_item(db,vars.table);
	
	run_cmd(
		db,
		"INSERT INTO "+vars.table+"_info ("+
		join(
			",",
			mapf(
				[](auto x){ return "`"+x+"`"; },
				vector<Column_name>{
					Column_name{"main"},
					Column_name{"valid"},
					Column_name{"edit_user"},
					Column_name{"edit_date"}
				}+keys(m)
			)
		)+
		") VALUES ("+
		join(
			",",
			vector<string>{as_string(id),"1",sql_escape(db,current_user()),"now()"}+
			mapf(
				[&](auto x){ return sql_escape(db,x); },
				values(m)
			)
		)+
		")"
	);
	redirect_page(o,"Insert",vars.redirect);
}
