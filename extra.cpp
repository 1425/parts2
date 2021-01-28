#include "extra.h"
#include<functional>
#include "page.h"
#include "status.h"
#include "type.h"
#include "recent.h"
#include "main.h"
#include "get_name.h"
#include "set_util.h"

using namespace std;

#define X(A,B,C) Id_table table(B##_id const*){ return Id_table{""#A}; }
TABLES(X)
#undef X

#define X(A,B,C) Id_table table(B##_id){ return Id_table{""#A}; }
TABLES(X)
#undef X

#define X(A,B,C) set<B##_id> current_options(DB db,B##_id const* x){\
	return to_set(query_to<B##_id>(db,"SELECT main"+current(table(x))));\
}
TABLES(X)
#undef X

#define X(A,B,C) set<optional<B##_id>> current_options(DB db,std::optional<B##_id> const*){\
	return optional<B##_id>{}|current_options(db,(B##_id*)0);\
}
TABLES(X)
#undef X

vector<Column_name> info_columns(Id_table const& table){
	return column_names(Table_name{table.get()+"_info"});
}

void set_null(DB db,Id_table const& table,Column_name const& col,int main){
	auto x=query(db,"SELECT * "+current(table)+" AND main="+as_string(main));
	if(x.empty())nyi
	assert(x.size()==1);
	auto cmd=(
		"INSERT INTO "+table+"_info("+
		join(
			",",
			mapf(
				[](auto x){ return "`"+x+"`"; },
				vector<Column_name>{
					Column_name{"main"},
					Column_name{"valid"},
					Column_name{"edit_user"},
					Column_name{"edit_date"}
				}+
				skip(5,column_names(Table_name{table.get()+"_info"}))
			)
		)+")VALUES("+
		join(
			",",
			vector<string>{as_string(main),"1",sql_escape(db,current_user()),"now()"}+
			mapf(
				[&](auto x)->string{
					auto [column_name,value]=x;
					if(column_name==col){
						return "NULL";
					}else{
						return sql_escape(db,value);
					}
				},
				skip(
					5,
					zip(
						info_columns(table),
						x[0]
					)
				)
			)
		)+
		")"
	);
	run_cmd(db,cmd);
}

template<typename T>
vector<string> check_type(DB db,Id_table const& table,Column_name const& name,optional<T> const* t){
	//could combine the next two lines into a single query
	auto opt=current_options(db,t);
	auto used=query_to<int,std::optional<T>>(db,"SELECT main,`"+name+"`"+current(table));

	auto f=filter([&](auto x){ return opt.count(x.second)==0; },used);

	return mapf(
		[&](auto x)->string{
			auto [main,value]=x;
			set_null(db,table,name,main);
			return a(
				Item{table,main,{},{}},
				"Auto nulled:"+table+" "+name+" "+type(t)+":"+as_string(x)
			);
		},
		f
	);
}

template<typename T>
vector<string> check_type(DB db,Id_table const& table,Column_name const& name,T const* t){
	//could combine the next two lines into a single query
	auto opt=current_options(db,t);
	auto used=query_to<int,T>(db,"SELECT main,`"+name+"`"+current(table));

	auto f=filter([&](auto x){ return opt.count(x.second)==0; },used);

	return mapf(
		[&](auto x)->string{
			auto [main,value]=x;
			return a(
				Item{table,main,{},{}},
				table+" "+name+" "+type(t)+":"+as_string(x)
			);
		},
		f
	);
}

vector<string> check_type(DB db,Id_table const& table,Column_name const& name,Typename const& type){
	(void)table;
	if(type=="std::string") return {};
	#define X(A,B,C) \
		if(type==""#B "_id") return check_type(db,table,name,(B##_id*)0);\
		if(type=="std::optional<"#B "_id>"){\
			return check_type(db,table,name,(optional<B##_id>*)0);\
		}
	TABLES(X)
	#undef X
	return {};
	(void)name;
	//auto q=query_to<int>(db,"SELECT "+name+current(table));
	PRINT(type);
	nyi
}

vector<pair<Assembly_design_id,Assembly_design_id>> assembly_design_assembly(DB db){
	return query_to<Assembly_design_id,Assembly_design_id>(
		db,
		"SELECT parent,child"+current(Id_table("assembly_design_assembly"))
	);
}

using Node=Assembly_design_id;
using Edge=pair<Node,Node>;//first is where from, second is where to
using Path=vector<Node>;

optional<Path> find_loops(vector<Edge> const& v){
	map<Node,set<Node>> accessible;
	for(auto [a,b]:v){
		accessible[a]|=b;
	}

	//map<Node,set<Node>> complete;

	auto nodes=firsts(v)|seconds(v);

	using Return=std::optional<Path>;
	std::function<Return(Path)> dfs;

	dfs=[&](Path path)->Return{
		assert(path.size());
		for(auto x:accessible[path[path.size()-1]]){
			if(contains(path,x)){
				return path|x;
			}
			auto d=dfs(path|x);
			if(d) return d;
		}
		return std::nullopt;
	};

	for(auto node:nodes){
		auto d=dfs({node});
		if(d) return d;
	}
	return std::nullopt;
}

std::optional<std::string> loop_check(DB db){
	auto f=find_loops(assembly_design_assembly(db));
	if(!f) return std::nullopt;
	return h3("Unexpected loops")+ol(join(mapf(
		[&](auto x){ return li(a(x,get_name(db,x))); },
		*f
	)));
}

string consistency_check(DB db){
	//for each table, see anything that is a link goes to something current
	map<Id_table,set<int>> available;
	for(auto table:options((Id_table*)0)){
		available[table]=to_set(query_to<int>(db,"SELECT main"+current(table)));
	}
	vector<string> r;
	for(auto [table,type]:table_types()){
		for(auto [col_name,col_type]:type){
			//PRINT(col_name);
			//PRINT(col_type);
			//nyi
			r|=check_type(db,table,col_name,col_type);
		}
	}
	return h2("Data oddities")+
		h3("References to deleted items")+join("<br>",r)+[&]()->string{
			auto x=loop_check(db);
			if(!x) return "";
			return *x;
		}();
		/*h3("Unexpected loops")+[&]()->string{
			auto f=find_loops(assembly_design_assembly(db));
			if(!f) return "";
			return ol(join(mapf(
				[&](auto x){ return li(a(x,get_name(db,x))); },
				*f
			)));
		}();*/
}

void show(ostream& o,DB db,Extra const&){
	page(
		o,
		"Extra",
		"This page exists to give a way to get to some of the less-commonly used pages."+
		p(a(Page_raw{},"Raw data"))+
		p(a(Export{},"Export"))+
		p(a(Page_upload{},"Upload"))+
		consistency_check(db)
	);
}
