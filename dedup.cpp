#include "dedup.h"
#include<string.h>
#include "db.h"
#include "input.h"
#include "status.h"
#include "map_util.h"
#include "recent.h"
#include "variant_util.h"
#include "page.h"

using namespace std;

bool operator==(string const& a,Id_table const& b){
	return a==b.get();
}

bool substring(string haystack,string needle){
	return !!strstr(haystack.c_str(),needle.c_str());
}

bool operator==(Column_name a,Id_table b){
	return string{a}==b;
}

string type_name(Id_table const& a){
	return char(toupper(a.get()[0]))+a.get().substr(1,a.get().size())+"_id";
}

vector<pair<Id_table,vector<Column_name>>> referred_by(Id_table const& target){
	vector<pair<Id_table,vector<Column_name>>> found;

	for(auto [table,cols]:table_types()){
		vector<Column_name> cols_found;
		for(auto [col_name,col_type]:cols){
			if(substring(as_string(col_type),type_name(target))){
				cols_found|=col_name;
			}
		}
		if(cols_found.size()){
			found|=make_pair(table,cols_found);
		}
	}
	return found;
}

Row_id cast(Id_table const& table,int id){
	(void)table;
	(void)id;
	#define X(A,B,C) if(table==""#A) return Row_id{B##_id{id}};
	TABLES(X)
	#undef X
	assert(0);
}

vector<Row_id> cast(Id_table const& table,vector<int> ids){
	return mapf([&](auto x){ return cast(table,x); },ids);
}

int as_int(Row_id a){
	#define X(A,B,C) if(holds_alternative<B##_id>(a)) return std::get<B##_id>(a).data;
	TABLES(X)
	#undef X
	assert(0);
}

string sql_escape(DB,Row_id a){
	/*#define X(A,B,C) if(holds_alternative<B##_id>(a)) return sql_escape(db,std::get<B##_id>(a));
	TABLES(X)
	#undef X
	assert(0);*/
	return as_string(as_int(a));
}

void delete_row(DB db,Row_id row){
	auto base_table=table(row);
	auto x=query(db,"SELECT * "+current(base_table)+" AND main="+as_string(as_int(row)));
	if(x.empty()){
		//If it doesn't seem to exist to begin with, then nothing needs to be done
		return;
	}
	assert(x.size()==1);
	
	run_cmd(
		db,
		"INSERT INTO "+base_table+"_info("+
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
				skip(5,column_names(Table_name{base_table.get()+"_info"}))
			)
		)
		+") VALUES ("+
		join(
			",",
			vector<string>{as_string(as_int(row)),"0",sql_escape(db,current_user()),"now()"}+
			mapf([&](auto x){ return sql_escape(db,x); },skip(5,x[0]))
		)
		+")"
	);
}

void delete_rows(DB db,vector<Row_id> v){
	mapv([db](auto x){ delete_row(db,x); },v);
}

void check_dup(DB db,Id_table const& dedup_table,std::vector<std::string> const& items){
	//todo: at some point, put in a check to see that all of the "main" items are pointing to things that actually exist...

	auto s=query(
		db,
		"SELECT main,"+join(",",items)+current(dedup_table)
	);
	auto g=GROUP(tail,s);

	bool header=0;
	auto show_header=[&](){
		if(header) return;
		header=1;
		PRINT(dedup_table);
		PRINT(items);
	};

	for(auto [k,v]:g){
		auto v1=[&](){
			vector<Row_id> r;
			for(auto row:v){
				if(!row[0]) continue;
				r|=cast(dedup_table,stoi(*row[0]));
			}
			return r;
		}();

		if(v1.size()<2) continue;
		show_header();
		PRINT(k);
		cout<<v1<<"\n";

		//find all of the things that reference that, and redirect them

		auto r=referred_by(dedup_table);
		PRINT(r);

		auto key_keeping=v1[0];
		auto keys_to_rm=tail(v1);

		for(auto [edit_table,cols]:r){
			for(auto column_name:cols){
				//PRINT(to_edit);
				for(auto key_to_remove:keys_to_rm){
					auto q=query_to<int>(
						db,
						"SELECT main"+current(edit_table)+"AND "+column_name+"="+sql_escape(db,key_to_remove)
					);
					for(auto edit_row:cast(edit_table,q)){
						cout<<"Edit:"<<edit_row<<": "<<column_name<<"="<<key_keeping<<"\n";
						edit_single(db,edit_row,column_name,as_int(key_keeping));
						reconnect(db);
						return check_dup(db,dedup_table,items);
					}
				}
			}
		}
		//then delete the extra entries

		delete_rows(db,keys_to_rm);
	}
}

int duplicates(){
	//try to find duplicate items... and them maybe try to dedup them
	auto db=connect_db();

	#define Y(A,B) v|=""#B;
	#define X(A,B,C) {\
		std::vector<std::string> v;\
		C(Y)\
		check_dup(db.db,Id_table{""#A},v);\
	}
	TABLES(X)
	#undef X
	#undef Y
	return 0;
}
