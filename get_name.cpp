#include "get_name.h"
#include "latest.h"
#include "status.h"
#include "extra.h"
#include "page.h"
#include "item.h"

using namespace std;

template<typename T>
string title(DB,T const& t){
	return as_string(t);
}

string title(DB db,Item const& a){
	return get_name(db,a.table,a.main);
}

template<typename... T>
string title(DB db,std::variant<T...> const& p){
	return visit([&](auto x){ return title(db,x); },p);
}

bool empty(URL url){
	string s=url.data;
	return s.empty();
}

#define X(A,B,C) std::string a(DB db,B##_id id){\
	return a(page(id),get_name(db,id));\
}
TABLES(X)
#undef X

string no_data(Id_table const& table,int main){
	return table+" #"+as_string(main);
}

#define X(A,B,C) string no_data(B##_id a){ return no_data(Id_table{""#A},a.data); }
TABLES(X)
#undef X

string get_name(DB db,Availability_id a,bool){
	auto q=query_to<Supplier_id,Part_number>(
		db,
		"SELECT supplier,part_number"+current(a)
	);
	if(q.empty()) return no_data(a);
	auto [supplier,pn]=q[0];
	return get_name(db,supplier)+" "+as_string(pn);
}

string get_name(DB db,Stock_id a,bool){
	auto q=query_to<Stock_category_id,Dimension,Dimension,Dimension>(
		db,
		"SELECT category,length,width,thickness"+current(a)
	);
	if(q.empty()){
		return no_data(a);
	}
	if(q.size()>1){
		assert(0);
	}
	auto row=q[0];
	vector<Positive_decimal> dims;
	auto f=[&](auto x){
		if(x) dims|=*x;
	};
	f(get<1>(row));
	f(get<2>(row));
	f(get<3>(row));
	auto dim_section=[&]()->string{
		if(dims.empty()) return "";
		return " ("+join("x",mapf(
			[](auto x){ return as_string(x)+"\""; },
			dims
		))+")";
	}();
	return get_name(db,get<0>(row))+" "+dim_section;
}

string get_name(DB db,Order_id a,bool){
	auto names=order_labels(db);
	auto f=names.find(a);
	if(f==names.end()) return no_data(a);
	return f->second;
}

string get_name(DB db,Meeting_id id,bool b){
	auto q=query_to<Date,Location_id>(db,
		"SELECT date,location FROM meeting_info "
		"WHERE main="+escape(id.data)+
		" ORDER BY id DESC LIMIT 1"
	);
	if(q.size()){
		auto row=q[0];
		return as_string(row.first)+" "+get_name(db,row.second)+[&]()->string{
			if(!b) return "";
			return " "+a(Id_table("meeting"));
		}();
	}
	return no_data(id);
}

string get_name(DB db,Manufacture_step_id id,bool b){
	/*auto q=query_to<Part_design_id,optional<Machine_id>>(db,
		"SELECT part_design,machine FROM manufacture_step_info "
		"WHERE main="+escape(id)+
		" ORDER BY id DESC LIMIT 1"
	);*/
	auto q=query_to<Part_design_id,optional<Machine_id>>(
		db,
		"SELECT part_design,machine FROM manufacture_step_current "
		"WHERE main="+escape(id)
	);
	if(q.size()){
		auto row=q[0];
		stringstream ss;
		ss<<get_name(db,row.first)<<" ";
		if(row.second){
			ss<<get_name(db,*row.second);
		}else{
			ss<<"NULL";
		}
		if(b){
			ss<<" ("<<a(Id_table("manufacture_step"))<<")";
		}
		return ss.str(); 
	}
	return no_data(id);
}

string get_name(DB db,Part_id a,bool){
	auto q=query_to<Part_design_id>(db,
		"SELECT part_design "+current(Id_table{"part"})+" AND main="+as_string(a.data)+" AND part_design IS NOT NULL"
	);
	if(q.size()){
		return "part #"+as_string(a.data)+" ("+get_name(db,q[0])+")";
	}
	return no_data(a);
}

string table_show(Id_table const& table,bool show_table){
	if(!show_table) return "";
	return " "+a(Latest{table,{},{}},"("+table+")");
}

string get_name(DB db,Assembly_design_id a,bool show_table){
	auto q=query_to<Part_number_local,std::string>(db,
		"SELECT part_number,name "+current(Id_table{"assembly_design"})+
		" AND main="+as_string(a.data)
	);
	assert(q.size()<=1);
	if(q.size()==1){
		auto row=q[0];
		return as_string(row.first)+" "+row.second+table_show(Id_table("assembly_design"),show_table);
	}
	return no_data(a);
}

string get_name(DB db,Assembly_id a,bool show_table){
	auto q=query_to<string,Assembly_design_id>(db,
		"SELECT name,assembly_design"+current(Id_table{"assembly"})+" AND main="+escape(a)
	);
	if(q.size()){
		auto [name,design]=q[0];
		if(name.size()){
			return name+table_show(Id_table{"assembly"},show_table);
		}
		return get_name(db,design)+" #"+escape(a)+table_show(Id_table{"assembly"},show_table);
	}
	return no_data(a);
}

string get_name_inner(DB db,Part_design_id id,bool show_table){
	auto q=query_to<string,string>(
		db,
		"SELECT part_number,name"+current(Id_table{"part_design"})+" AND main="+escape(id)
	);
	if(q.size()==1){
		auto [pn,name]=q[0];
		if(pn.empty() && name.empty())nyi
		return pn+" "+name+[&]()->string{
			if(show_table) return " ("+a(Id_table{"part_design"})+")";
			return "";
		}();
	}
	return "(deleted)";
}

string get_name(DB db,Part_design_id id,bool show_table){
	static map<pair<Part_design_id,bool>,string> cache;
	auto k=make_pair(id,show_table);
	auto f=cache.find(k);
	if(f!=cache.end()){
		return f->second;
	}
	auto r=get_name_inner(db,id,show_table);
	cache[k]=r;
	return r;
}

string get_name_none_inner(DB db,Id_table const& table,int main,bool show_table){
	auto e=expected_tables();
	auto this_table=e[Table_name{table+"_info"}];
	const auto no_data=table+" #"+as_string(main);
	for(auto [name,type]:this_table){
		if(name=="name"){
			/*auto q=query_to<optional<string>>(db,
				"SELECT name FROM "+table+"_info "
				"WHERE main="+as_string(main)+" AND valid "
				"ORDER BY id DESC LIMIT 1"
			);*/
			auto q=query_to<optional<string>>(
				db,
				"SELECT name FROM "+table+"_current "
				"WHERE main="+as_string(main)
			);
			if(q.empty()){
				//you get here if it seems like the thing has never existed
				return no_data;
			}
			auto elem=q[0];
			if(!elem){
				return "NULL";
			}
			if(elem=="") return no_data;
			//assert(elem);
			return *elem+table_show(table,show_table);
		}
	}
	return no_data;
}

string get_name_none(DB db,Id_table const& table,int main,bool show_table){
	using K=tuple<Id_table,int,bool>;
	auto k=make_tuple(table,main,show_table);

	static map<K,string> cache;
	{
		auto f=cache.find(k);
		if(f!=cache.end()){
			return f->second;
		}
	}
	auto s=get_name_none_inner(db,table,main,show_table);
	cache[k]=s;
	return s;
}

template<typename T>
string get_name(DB db,T const& t,bool show_table){
	return get_name_none(db,table(&t),t.data,show_table);
}

string get_name(DB db,Id_table const& table,int main,bool show_table){
	#define X(A,B,C) if(table==""#A) return get_name(db,B##_id{main},show_table);
	TABLES(X)
	#undef X
	assert(0);
}

#define X(A,B,C) string get_name(DB db,B##_id a){\
	return get_name(db,a,0); \
}
TABLES(X)
#undef X

string a(Id_table const& table){
	return a(Latest{table,{},{}},string{table});
}

string a(Page const& p){
	return a(p,as_string(p));
}

string a(URL const& url,string const& s){
	if(empty(url)){
		return "";
	}
	return "<a href="+escape(url)+">"+s+"</a>";
}

string a(Page const& p,string const& body){
	return a(to_url(p),body);
}

#define X(A,B,C) std::string a(B##_id id,std::string const& body){\
	if(string{""#B}=="Image"){\
		return a(View{Upload_id{id.data}},body);\
	}\
	return a(Item{Id_table{""#A},id.data,{},{}},body);\
}\
std::string a(std::optional<B##_id> const& id,std::string const& body){\
	if(id) return a(*id,body);\
	return body;\
}
TABLES(X)
#undef X

string a(Id_table const& table,string const& s){
	return a(Latest{table,{},{}},s);
}

string a(Id_table const& table,string s){
	return a(Latest{table,{},{}},s);
}

string a(Latest x,string s){
	return a(Page{x},s);
}

#define X(A,B,C) Page item(B##_id a){ return Item{Id_table{""#A},a.data,{},{}}; }
TABLES(X)
#undef X

template<typename T>
string a(T t,string b){
	return a(item(t),b);
}

string a(DB db,Page const& p){
	return a(p,title(db,p));
}

