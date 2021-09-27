#include "search.h"
#include "input.h"
#include "recent.h"
#include "status.h"
#include "extra.h"
#include "latest.h"
#include "type.h"
#include "main.h"
#include "get_name.h"

using namespace std;

Row_id to_row(Id_table const& table,int i){
	#define X(A,B,C) if(table==""#A) return B##_id{i};
	TABLES(X)
	#undef X
	assert(0);
}

char toupper1(char c){
	if(c=='_') return ' ';
	return toupper(c);
}

//could probably move to util
bool contains_i(string const& haystack,string const& needle){
	//case-insensitive string search
	auto x=std::search(
		begin(haystack),end(haystack),
		begin(needle),end(needle),
		[](auto a,auto b){
			return toupper1(a)==toupper1(b);
		}
	);
	return x!=end(haystack);
}

using Search_result=tuple<Page,Column_name,string>;

Latest latest(Id_table table){
	return Latest{table,{},{}};
}

/*Id_table table(Row_id a){
	#define X(A,B,C) if(holds_alternative<B##_id>(a)) return Id_table{""#A};
	TABLES(X)
	#undef X
	assert(0);
}*/

int stoi(Row_id);

Page page(Row_id a){
	#define X(A,B,C) return Item{table(a),stoi(a),{},{}};
	TABLES(X)
	#undef X
	assert(0);
}

vector<Search_result> search_table(
	DB db,
	Id_table const& table,
	std::string target,
	unsigned max_len
){
	vector<Search_result> r;
	//Obviouly this could be done much faster.  In particular:
	//1) If would make sense not to search through the binaries of images, etc.
	//2) The searching could be done mostly in the db itself
	//3) Even if the matching is done here, could do it with a lot fewer queries by asking for whole rows at a time.
	for(auto col:column_names(Table_name{table+"_info"})){
		auto q=query_to<int,optional<string>>(db,"SELECT main,`"+col+"`"+current(table));
		for(auto [i,elem]:q){
			if(elem && contains_i(as_string(elem),target)){
				r|=Search_result{page(to_row(table,i)),col,*elem};
				if(r.size()>=max_len){
					return r;
				}
			}
		}
	}
	return r;
}

std::string show(DB db,Row_id id){
	return std::visit([&](auto x){ return show(db,x); },id);
}

vector<Id_table> tables(){
	vector<Id_table> r;
	#define X(A,B,C) r|=Id_table(""#A);
	TABLES(X)
	#undef X
	return r;
}

string abbreviate(unsigned max_len,string s){
	if(s.size()>max_len){
		return s.substr(0,max_len)+"...";
	}
	return s;
}

template<typename T>
T base(T const*){
	return T{};
}

Item base(Item const* x){ return rand(x); }

vector<Search_result> search_page(std::string const& query,unsigned max_len){
	vector<Search_result> found;
	for(auto table:tables()){
		if(contains_i(table,query)){
			found|=Search_result(latest(table),"name",as_string(table));
			if(found.size()>=max_len) return found;
		}
		auto d=table_description(table);
		if(contains_i(d,query)){
			found|=Search_result(latest(table),"description",d);
			if(found.size()>=max_len) return found;
		}
	}

	#define X(A,B) if(contains_i(""#A,query)){\
		found|=Search_result(base((A*)0),"page name",string{""#A});\
		if(found.size()>=max_len) return found;\
	}
	NORMAL_PAGES(X)
	#undef X

	for(auto table:tables()){
		for(auto column_name:column_names(Table_name{table+"_info"})){
			if(contains_i(column_name,query)){
				found|=Search_result(
					latest(table),
					Column_name{"column"},
					as_string(column_name)
				);
				if(found.size()>=max_len) return found;
			}
		}
	}

	if(contains_i(tutorial(),query)){
		found|=Search_result(Home{},"Tutorial",tutorial());
	}

	return found;
}

template<typename T>
string show(DB,T t){
	return as_string(t);
}

string show(DB db,Page const& p){
	return a(db,p);
}

void show(std::ostream& o,DB db,Search const& page1){
	(void)o;
	vector<Search_result> found;
	unsigned max_len=30;
	for(auto table:tables()){
		found|=search_table(db,table,page1.query,max_len-found.size());
		if(found.size()>=max_len){
			break;
		}
	}
	found|=search_page(page1.query,max_len-found.size());

	auto title="Search:"+html_escape(page1.query);
	page(
		o,
		title,
		title,
		[&](){
			stringstream ss;
			ss<<"<table border>";
			ss<<tr(th("Item")+th("Column")+th("Value"));
			for(auto [item,col,value]:found){
				ss<<tr(
					td(show(db,item))+td(col)+
					td(
						html_escape(abbreviate(100,as_string(value)))
					)
				);
			}
			ss<<"</table>";
			return ss.str();
		}(),
		page1.query
	);
}
