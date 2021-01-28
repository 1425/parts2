#include "recent.h"
#include "db.h"
#include "page.h"
#include "latest.h"
#include "main.h"
#include "get_name.h"
#include "map_util.h"

using namespace std;

template<typename T>
string to_table(Table_name table,std::vector<std::vector<T>> const& a){
	stringstream ss;
	ss<<"<table border>";
	ss<<"<tr>";
	ss<<join(mapf(th,column_names(table)));
	ss<<"</tr>";
	for(auto row:a){
		ss<<"<tr>";
		ss<<join(mapf([](auto x){ return td(as_string(x)); },row));
		ss<<"</tr>";
	}
	ss<<"</table>";
	return ss.str();
}

std::vector<Column_name> column_names(Table_name const& table){
	auto e=expected_tables();
	return firsts(e[table]);
}

Typename col_type(Id_table const& table,Column_name const& col){
	auto t=table_types();
	auto this_table=to_map(t[table]);
	if(this_table.count(col)==0){
		PRINT(table);
		PRINT(col);
		PRINT(this_table);
	}
	assert(this_table.count(col));
	return this_table[col];
}

optional<Id_table> foreign_key_table(Id_table const& table,Column_name const& col){
	auto this_col=col_type(table,col);
	#define X(A,B,C) if(this_col==""#B "_id") return Id_table{""#A};
	TABLES(X)
	#undef X
	//could make it a link if it's a URL.
	return {};
}

optional<Id_table> foreign_key_table(Table_name const& table,Column_name const& col){
	try{
		return foreign_key_table(Table_name{table},col);
	}catch(...){
		return std::nullopt;
	}
}

std::string show_td(DB db,Id_table const& table,string const& column,int main){
	PRINT(table);
	PRINT(column);
	auto name=get_name(db,table,main,0);
	return td(name);
}

std::string show_td(
	DB db,
	Id_table const& table,
	Column_name const& col,
	std::optional<std::string> const& s
){
	if(!s){
		return tag("td bgcolor=\"#cccccc\"","");
	}
	auto f=foreign_key_table(table,col);
	if(f){
		auto main=stoi(*s);
		auto name=get_name(db,*f,main,0);
		return td(name);
	}
	auto this_col=col_type(table,col);
	if(
		//this_col=="blob" || this_col=="mediumbolb" || this_col=="longblob"
		this_col=="Upload_data"
	){
		//return td("...");
		return td(thumbnail("blah",s));
	}
	return td(*s);
}

std::string show_td(
	DB db,
	Id_table const& table,
	Column_name const& col,
	std::optional<std::string> const& s,
	std::optional<std::string> const& old
){
	if(!s){
		return tag("td bgcolor=\"#cccccc\"","");
	}
	auto f=foreign_key_table(table,col);
	if(f){
		auto main=stoi(*s);
		auto name=get_name(db,*f,main,0);
		if(s==old){
			return td(name);
		}else{
			return tag("td bgcolor=\"#ccffcc\"",name);
		}
	}
	auto this_col=col_type(table,col);
	if(
		//this_col=="blob" || this_col=="mediumbolb" || this_col=="longblob"
		this_col=="Upload_data"
	){
		//return td("...");
		return td(thumbnail("blah",s));
	}
	if(s==old){
		return td(*s);
	}else{
		return tag("td bgcolor=\"#ccffcc\"",*s);
	}
}

std::string change_table(
	DB db,
	Id_table const& table,
	vector<tuple<Column_name,optional<string>,optional<string>>> const& a
){
	if(a.empty()) return "";
	stringstream ss;
	ss<<"<table border>";
	ss<<tr(th("Column")+th("New")+th("Old"));
	for(auto [name,new_,old]:a){
		ss<<"<tr>";
		ss<<td(name);
		ss<<show_td(db,table,name,new_,old);
		ss<<show_td(db,table,name,old);
		ss<<"</tr>";
	}
	ss<<"</table>";
	return ss.str();
}

auto table_types(Id_table const& a){
	auto e=table_types();
	auto f=e.find(a);
	assert(f!=e.end());
	return f->second;
}

std::string to_table(
	DB db,
	Id_table const& table,
	std::vector<std::optional<std::string>> const& data
){
	(void)db;
	stringstream ss;
	ss<<"<table border>";
	for(auto [col,data]:zip(table_types(table),data)){
		auto [col_name,col_type]=col;
		ss<<"<tr>";
		ss<<th(col_name);
		ss<<show_td(db,table,col_name,data);
		//ss<<td(as_string(data));
		ss<<"</tr>";
	}
	ss<<"</table>";
	return ss.str();
}

std::tuple<Datetime,User,Update_type,string> describe(
	DB db,
	Id_table table,
	std::vector<Column_name> labels,
	std::vector<std::vector<std::optional<std::string>>> const& data
){
	assert(data.size()>=1 && data.size()<=2);
	auto first=data[0];

	if(!first[4]){
		PRINT(table);
		PRINT(labels);
		PRINT(data);
	}
	assert(first[4]);
	auto date=parse((Datetime*)0,*first[4]);

	assert(first[3]);
	User user=parse((User*)0,*first[3]);

	int main=stoi(first[1]);

	if(data.size()==1){
		return make_tuple(
			date,
			user,
			Update_type::INSERT,
			a(Item{table,main,{},{}},"Details")+
			to_table(db,table,skip(5,first))
		);
	}
	auto second=data[1];
	auto valid=parse((bool*)0,*first[2]);
	if(valid){
		return make_tuple(
			date,
			user,
			Update_type::UPDATE,
			[=](){
				auto s=skip(5,zip(labels,first,second));
				auto d=filter(
					[](auto x){ return get<1>(x)!=get<2>(x); },
					s
				);
				return a(
					Item{table,main,{},{}},
					get_name(db,table,main,0)
				)
					+change_table(db,table,d);
			}()
		);
	}
	assert(second.size());
	return make_tuple(
		date,
		user,
		Update_type::DELETE,
		a(
			Item{table,main,{},{}},
			get_name(db,table,main,0)
		)
	);
}

template<typename T>
string linkify(std::pair<std::string,T> const& p){
	auto type=p.first;
	auto value=p.second;
	auto vs=as_string(value);
	if(type=="Datetime" || type=="Change" || type=="Type"){
		return vs;
	}
	if(type=="User"){
		return a("?page=raw&filter_col=edit_user&filter_value="+vs,vs);
		//return a(Page_raw{Filter{"edit_user",value},{}},vs);
	}
	if(type=="Table"){
		return a("?page=latest&table="+vs,vs);
	}
	PRINT(type);
	nyi
}

vector<pair<Datetime,int>> recent_updates(DB db,Id_table const& table,int max){
	return query_to<Datetime,int>(
		db,
		"SELECT edit_date,id FROM "+table+"_info ORDER BY id DESC LIMIT "+as_string(max)
	);
}

int get_main(DB db,Id_table const& table,int id){
	return query_single<int>(
		db,
		"SELECT main FROM "+table+"_info WHERE id="+as_string(id)
	);
}

tuple<Datetime,User,Id_table,Update_type,string> show_update(
	DB db,
	Id_table const& table,
	int id
){
	//query_to<Datetime,User,
	//auto q=query("SELECT * FROM "+table+"_info WHERE ");
	auto main=get_main(db,table,id);
	auto q=query(
		db,
		"SELECT * FROM "+table+"_info WHERE main="+as_string(main)+
		" AND id<="+as_string(id)+
		" ORDER BY id DESC "
		"LIMIT 2"
	);
	if(q.empty()){
		assert(0);
	}
	auto new1=q[0]; //id,main,valid,edit_user,edit_date
	assert(new1.size()>5);

	assert(new1[4]);
	auto date=parse((Datetime*)0,*new1[4]);

	assert(new1[3]);
	auto user=parse((User*)0,*new1[3]);

	assert(new1[2]);
	auto valid=parse((bool*)0,*new1[2]);

	switch(q.size()){
		case 1:
			//insert
			//obviously should become prettier
			return make_tuple(
				date,user,table,Update_type::INSERT,
				a(Item{table,main,{},{}},"Details")+to_table(db,table,skip(5,q[0]))
			);
		case 2:{
			if(!valid){
				//delete
				return make_tuple(
					date,user,table,Update_type::DELETE,
					a(
						Item{table,main,{},{}},
						get_name(db,table,main,0)
					)
				);
			}
			//update
			//auto new_values=skip(5,q[0]);
			//auto old_values=skip(5,q[1]);
			//obviously should become prettier
			auto z=skip(5,zip(column_names(Table_name{table+"_info"}),q[0],q[1]));
			return make_tuple(
				date,user,table,Update_type::UPDATE,
				a(
					Item{table,main,{},{}},
					get_name(db,table,main,0)
				)+change_table(db,table,z)//as_string(new_values)+" "+as_string(old_values)
			);
		}
		default:
			assert(0);
	}

	nyi
}

vector<tuple<Datetime,Id_table,int>> recent_updates(
	DB db,
	int max
){
	vector<tuple<Datetime,Id_table,int>> r;
	for(auto table:options((Id_table*)0)){
		for(auto [date,i]:recent_updates(db,table,max)){
			r|=make_tuple(date,table,i);
		}
	}
	return take(10,reversed(sorted(r)));
}

void show(ostream& o,DB db,Recent const&){
	auto to_show1=recent_updates(db,10);
	auto to_show=mapf(
		[&](auto x){
			auto table=get<1>(x);
			auto id=get<2>(x);
			return show_update(db,table,id);
		},
		to_show1
	);

	stringstream ss;
	ss<<"<table border>";
	ss<<tr(join(mapf(
		th,
		vector<string>{"Date","User","Table","Type","Change"}
	)));
	for(auto x:to_show){
		ss<<"<tr>";
		ss<<join(mapf(
			[](auto y){ return td(linkify(y)); },
			zip({"Datetime","User","Table","Type","Change"},x)
		));
		ss<<"</tr>";
	}
	ss<<"</table>";
	page(
		o,
		"Recent changes",
		ss.str()
	);
}
