#include "input.h"
#include "status.h"
#include "base64.h"
#include "get_name.h"
#include "page.h"
#include "type.h"
#include "map_util.h"
#include "set_util.h"

using namespace std;

//start utility section

string hidden(Column_name const& a,string const& b){
	return hidden(a.get(),b);
}

bool nonempty(string s){ return !s.empty(); }

template<typename T>
bool is_null(T){ return 0; }

template<typename T>
bool is_null(std::optional<T> const& a){
	if(!a) return 1;
	return is_null(*a);
}

string html_escape(char c){
	//this is for debugging purposes only.
	if(c=='>') return "&gt;";
	if(c=='<') return "&lt;";
	if(c=='"') return "&quot;";//this is kinda ugly.
	return string()+c;
}

string html_escape(string const& s){
	return join(MAP(html_escape,s));
}

template<typename T>
std::optional<string> as_opt_string(T const& t){
	return as_string(t);
}

template<typename T>
std::optional<std::string> as_opt_string(std::optional<T> const& t){
	if(t) return as_string(t);
	return std::nullopt;
}

template<typename K>
map<std::optional<K>,std::string> add_null(map<K,string> m){
	map<optional<K>,string> r;
	r[optional<K>{}]="NULL";
	for(auto [k,v]:m){
		r[k]=v;
	}
	return r;
}

template<typename T>
std::optional<T> null(T const*){
	return std::nullopt;
}

template<typename T>
std::optional<T> null(std::optional<T> const*){
	return std::nullopt;
}

//start calculation section

#define X(A,B,C) Id_table table_name(B##_id const*){\
	return Id_table{""#A};\
}
TABLES(X)
#undef X

//this should probably go with the other "current" functions
#define X(A,B,C) string current(B##_id a){ \
	return current(Id_table{""#A})+" AND main="+escape(a)+" "; \
}
TABLES(X)
#undef X

string current(Row_id a){
	return visit([](auto x){ return current(x); },a);
}

Part_design_id design(DB db,Part_id a){
	return query_single<Part_design_id>(
		db,
		"SELECT part_design "+current(a)
	);
}

optional<Assembly_design_id> design(DB db,Assembly_id a){
	return query_optional<Assembly_design_id>(
		db,
		"SELECT assembly_design"+current(a)
	);
}

map<Manufacture_step_id,std::string> steps(DB db,Part_design_id a){
	auto q=query_to<Manufacture_step_id>(
		db,
		"SELECT main "+current(Id_table{"manufacture_step"})+" AND part_design="+escape(a)
	);
	return to_map(mapf(
		[&](auto x){ return make_pair(x,get_name(db,x)); },
		q
	));
}

vector<Assembly_design_id> parent_types(DB db,Part_design_id a){
	return query_to<Assembly_design_id>(
		db,
		"SELECT parent"+current(Id_table{"assembly_design_part"})+" AND child="+escape(a)
	);
}

vector<Assembly_design_id> parent_types(DB db,std::optional<Part_design_id> const& a){
	if(!a) return {};
	return parent_types(db,*a);
}

vector<Assembly_design_id> parent_types(DB db,Assembly_design_id a){
	return query_to<Assembly_design_id>(
		db,
		"SELECT parent"+current(Id_table{"assembly_design_assembly"})+"AND child="+escape(a)
	);
}

vector<Assembly_design_id> parent_types(DB db,optional<Assembly_design_id> a){
	if(!a) return {};
	return parent_types(db,*a);
}

set<Assembly_id> instances(DB db,Assembly_design_id a){
	return to_set(query_to<Assembly_id>(
		db,
		"SELECT main "+current(Id_table{"assembly"})+"AND assembly_design="+escape(a)
	));
}

map<Assembly_id,std::string> assemblies(DB db,vector<Assembly_design_id> const& v){
	//Clearly, this could be done in a lot fewer fetches from the database
	set<Assembly_id> f=flatten(mapf(
		[&](auto x){ return instances(db,x); },
		v
	));
	return to_map(mapf(
		[&](auto x){ return make_pair(x,get_name(db,x)); },
		f
	));
}

set<Assembly_id> assemblies_of_design(DB db,vector<Assembly_design_id> v){
	return flatten(mapf([&](auto x){ return instances(db,x); },v));
}

optional<Assembly_state> state(DB db,Assembly_id a){
	return query_optional<Assembly_state>(db,
		"SELECT state"+current(a)
	);
}

Stock_id stock(DB db,Stock_install_id a){
	return query_single<Stock_id>(db,"SELECT stock"+current(a));
}

vector<Assembly_design_id> assembly_designs_with(DB db,Stock_id a){
	return query_to<Assembly_design_id>(
		db,
		"SELECT parent"+current(Id_table{"assembly_design_stock"})+"AND child="+escape(a)
	);
}

template<typename T>
map<T,std::string> get_names(DB db,vector<T> const& v){
	return to_map(mapf(
		[&](auto x){ return make_pair(x,get_name(db,x)); },
		v
	));
}

template<typename T>
map<T,std::string> get_names(DB db,set<T> const& v){
	return get_names(db,to_vec(v));
}

map<Stock_id,std::string> stock_types_used(DB db,Assembly_design_id a){
	auto q=query_to<Stock_id>(db,"SELECT child"+current(Id_table{"assembly_design_stock"})+"AND parent="+escape(a));
	return get_names(db,q);
}

map<Stock_id,std::string> stock_types_used(DB db,optional<Assembly_design_id> a){
	if(!a) return {};
	return stock_types_used(db,*a);
}

Assembly_id assembly(DB db,Stock_install_id a){
	return query_single<Assembly_id>(db,"SELECT assembly"+current(a));
}

std::map<int,std::string> current_options_inner(DB db,Id_table const& table){
	auto q=query_to<int>(db,"SELECT main FROM "+table+"_info WHERE " 
		"id IN ("
			"SELECT MAX(id) FROM "+table+"_info GROUP BY main"
		") AND valid"
	);
	return to_map(mapf(
		[&](auto x){
			return make_pair(x,get_name(db,table,x,0));
		},
		q
	));
}

std::map<int,std::string> current_options(DB db,Id_table const& table){
	using R=std::map<int,std::string>;
	static std::map<Table_name,R> cache;
	auto f=cache.find(table);
	if(f==cache.end()){
		cache[table]=current_options_inner(db,table);
		return cache[table];
	}
	return f->second;
}

template<typename T>
std::map<T,std::string> current_options(DB db,T const* x){
	map<T,std::string> r;
	for(auto [k,v]:current_options(db,table_name(x))){
		r[T{k}]=v;
	}
	return r;
}

template<typename T>
std::map<optional<T>,std::string> current_options(DB db,std::optional<T> const*){
	map<optional<T>,string> r;
	r[std::nullopt]="NULL";
	for(auto [k,v]:current_options(db,(T*)0)){
		r[k]=v;
	}
	return r;
}

//start output section

string thumbnail(string name,string const& value){
	return a(
		View{},
		"<img src=\"data:image/jpeg;base64,"+base64_encode(value)+"\" alt=\""+name+"\" width=100>"
	);
}

string thumbnail(string name,Upload_data const& value){
	(void)name;
	(void)value;
	return "Not changable.";
}

string thumbnail(string const&,int)nyi

string thumbnail(string const& name,optional<int> const& i){
	if(i)nyi
	return name+": NULL ...";
}

string thumbnail(string const&,optional<int>,Upload_id)nyi

string thumbnail(string const& name,optional<string> const& value,Upload_id const& id){
	if(!value){
		return "NULL";
	}
	return a(
		View{id},
		"<img src=\"data:image/jpeg;base64,"+base64_encode(*value)+"\" alt=\""+name+"\" width=100>"
	);
}

string thumbnail(DB db,Upload_id const& id){
	auto q=query_to<Nonempty_string>(
		db,
		"SELECT datatype"+current(Id_table{"upload"})+"AND main="+escape(id)
	);
	if(q.empty()) return "(invalid)";
	assert(q.size()==1);
	auto type=q[0];
	if(prefix("image/",type.get())){
		return "<img src=\"?page=view&main="+as_string(id.data)+"\" width=100>";
	}
	return ("(type="+type+")").get();
}

string thumbnail(string const& name,optional<string> const& a){
	if(a) return thumbnail(name,*a);
	return name+": NULL";
}

template<typename T>
std::string input_text(std::string const& name,std::optional<T> const& value){
	std::stringstream ss;
	ss<<"<input type=\"text\" name=\""+name+"\"";
	if(value){
		ss<<" value=\""<<html_escape(*value)<<"\"";
	}
	ss<<" onclick=\"style.background='yellow';\">";
	return ss.str();
}

using Display=std::string;

template<typename Value>
std::string drop_down(
	std::string const& name,
	std::optional<Value> const& current,
	std::map<Value,Display> const& options
){
	std::stringstream ss;
	//ss<<"<select name=\""<<name<<"\" onclick=\"document.body.style.background='red';\">";
	ss<<"<select name=\""<<name<<"\" onclick=\"style.background='yellow';\">";
	bool found=0;
	for(auto elem:options){
		ss<<"<option value="<<escape(elem.first);
		if(elem.first==current){
			ss<<" selected=selected";
			found=1;
		}
		ss<<">"<<elem.second<<"</option>\n";
	}
	if(!is_null(current) && !found){
		ss<<"<option value="<<escape(*current)<<" selected=selected>";
		ss<<"(invalid:"<<current<<")";
		ss<<"</option>";
	}
	ss<<"</select>";
	return ss.str();
}

template<typename T>
std::string drop_down(
	std::string const& name,
	std::optional<T> const& current,
	std::vector<T> const& options
){
	return drop_down(
		name,
		current,
		to_map(mapf(
			[](auto x){ return make_tuple(x,as_string(x)); },
			options
		))
	);
}

template<typename T>
std::string drop_down(
	std::string const& name,
	T const& t,
	std::vector<T> const& options
){
	return drop_down(name,std::optional<T>{t},options);
}

template<typename T>
std::string drop_down(
	std::string const& name,
	T const& value,
	std::map<T,std::string> const& options
){
	return drop_down(name,std::optional<T>(value),options);
}

string input_form(
	DB db,
	Column_name const& name,
	std::optional<Manufacture_step_id> current,
	Part_id main
){
	auto p=design(db,main);
	auto d=steps(db,p);
	return drop_down(name,current,add_null(d));
}

string input_form(
	DB db,
	Column_name const& name,
	std::optional<Assembly_id> const& current,
	Part_id main
){
	auto d=design(db,main);
	auto pt=parent_types(db,d);
	auto asms=assemblies(db,pt);
	return drop_down(name,current,add_null(asms));
}

string input_form(
	DB db,
	Column_name const& name,
	std::optional<Assembly_id> current,
	Assembly_id main
){
	auto d=design(db,main);//figure out the assembly's design that it follows
	auto pt=parent_types(db,d); //figure out the types that use things of this type
	auto asms=assemblies(db,pt); //assemblies that are one of those types
	return drop_down(name,current,add_null(asms));
}

std::string input_form(DB db,std::string const& name,std::optional<Decimal> const& a);

string input_form(
	DB db,
	Column_name const& name,
	optional<Decimal> current,
	Assembly_id main
){
	if(name!="time_left"){
		//return show_input(db,name,current).form;
		return input_form(db,name,current);//show_input(db,name,current).form;
	}
	auto s=state(db,main);
	if(s==Assembly_state::in_design || s==Assembly_state::done){
		return "Not applicable in current state"+hidden(name,"NULL");
	}
	//return show_input(db,name,current).form;
	return input_form(db,name,current);
}

string input_form(
	DB db,
	Column_name const& name,
	Assembly_id current,
	Stock_install_id main
){
	//just show assemblies that make sense for the chosen stock type
	auto s=stock(db,main);
	auto vad=assembly_designs_with(db,s);
	auto va=assemblies_of_design(db,vad);
	return drop_down(name,current,get_names(db,va));
}

string input_form(
	DB db,
	Column_name const& name,
	Stock_id current,
	Stock_install_id main
){
	auto a=assembly(db,main);
	auto d=design(db,a);
	auto opt=stock_types_used(db,d);
	return drop_down(name,current,opt);
}

string input_form(DB db,string const& name,Stock_category_id a){
	return drop_down(name,a,current_options(db,(Stock_category_id*)0));
}

string input_form(DB db,string const& name,std::optional<Stock_category_id> const& a){
	return drop_down(
		name,
		a,
		add_null(current_options(db,(Stock_category_id*)0))
	);
}

template<typename COL,typename MAIN>
string input_form(DB db,Column_name name,COL col,MAIN main){
	(void)main;
	return input_form(db,name,col);
}

template<typename T>
string input_form(DB db,Column_name const& name,Column_value const& value,T main){
	return visit([&](auto x){ return input_form(db,name,x,main); },value);
}

string input_form(DB db,Column_name const& name,Column_value const& value,Row_id main){
	return visit([&](auto x){ return input_form(db,name,value,x); },main);
}

string input_form(DB,std::string const&,Availability_id);
string input_form(DB,std::string const&,Assembly_design_stock_id);

std::string input_form(DB,std::string const& name,std::string const& a){
	return input_text(name,optional<string>(a));
}

template<typename Num>
string input_num(
	std::string const& name,
	optional<Num> min,
	optional<Num> max,
	optional<Num> step,
	optional<Num> value
){
	stringstream ss;
	ss<<"<input type=\"number\" name=\""+name+"\"";
	if(min) ss<<" min="+escape(min);
	if(max) ss<<" max="+escape(max);
	if(step) ss<<" step="+escape(step);
	if(value) ss<<" value="+escape(value);
	ss<<" onclick=\"style.background='yellow';\">";
	return ss.str();
}

std::string input_form(DB,std::string const& name,Positive_int const& a){
	return input_num<Positive_int>(name,Positive_int{1},Positive_int{1000},{},a);
}

std::string input_form(DB,std::string const& name,std::optional<Positive_int> const& a){
	return input_num<Positive_int>(name,Positive_int{1},Positive_int{1000},{},a);
}

std::string input_form(DB,std::string const& name,unsigned a){
	return input_num<unsigned>(name,0,{},{},a);
}

std::string input_form(DB,std::string const& name,std::optional<unsigned> const& a){
	return input_num<unsigned>(name,0,{},{},a);
}

std::string input_form(DB,std::string const& name,Decimal a){
	return input_num<Decimal>(name,{},{},Decimal{.001},a);
}

string input_form(DB,string name,Positive_decimal a){
	auto step=Positive_decimal{Decimal{0.001}};
	return input_num<Positive_decimal>(name,step,{},step,a);
}

string input_form(DB,string name,optional<Positive_decimal> a){
	auto step=Positive_decimal{Decimal{.001}};
	return input_num<Positive_decimal>(name,step,{},step,a);
}

std::string input_form(DB,std::string const& name,std::optional<Decimal> const& a){
	auto step=Decimal{.001};
	return input_num<Decimal>(name,{},{},step,a);
}

std::string input_form(DB,std::string const& name,std::optional<Weight> const& a){
	auto step=Weight{Decimal{.001}};
	return input_num<Weight>(name,{},{},step,a);
}

#define X(A) std::string input_form(DB,std::string const& name,A const& a){\
	return input_text(name,as_opt_string(a));\
}
X(int)
X(Nonempty_string)
X(Part_number)
X(Part_number_local)
X(std::optional<Part_number_local>)

X(std::optional<User>)
X(optional<Part_number>)
X(optional<Subsystem_prefix>)
X(optional<Nonempty_string>)
X(optional<Upload_data>)
#undef X

std::string input_form(DB db,std::string const& name,std::optional<Date> const& a){
	if(a) return show_input(db,name,*a).form;
	return show_input(db,name,(Date*)0).form;
}

#define X(A) std::string input_form(DB db,std::string const& name,A##_id value){\
	return drop_down(name,value,current_options(db,&value));\
}\
std::string input_form(DB db,std::string const& name,std::optional<A##_id> const& value){\
	return drop_down(name,value,current_options(db,&value));\
}
X(Supplier)
X(Location)
X(Part_design)
X(Assembly_design)
X(Assembly)
X(Stock)
X(Machine)
X(Manufacture_step)
X(Meeting)
X(Order)
X(Part)
X(Assembly_design_part)
X(Availability)
X(Assembly_design_stock)
X(Assembly_design_assembly)
X(Order_member)
X(Stock_install)
#define SND(A,B,C) X(B)
UPLOAD_LINK_TABLES(SND)
#undef SND
#undef X

std::string input_form(DB db,std::string const& name,Upload_id value){
	return a(value,thumbnail(db,value))
		+drop_down(name,value,current_options(db,&value));
}

std::string input_form(DB db,std::string const& name,std::optional<Upload_id> value){
	return [=]()->string{
		if(value){
			return a(*value,thumbnail(db,*value));
		}
		return "";
	}()+drop_down(name,value,current_options(db,&value));
}

std::string input_form(DB,std::string const& name,optional<int>);
#define X(A) std::string input_form(DB,std::string const&/* name*/,optional<A>)nyi
X(int)
#undef X

std::string input_form(DB,std::string const& name,Part_state value){
	return drop_down(
		name,
		value,
		options((Part_state*)0)
	);
}

std::string input_form(DB db,std::string const& name,optional<Part_state> value){
	if(value) return input_form(db,name,*value);
	return drop_down(
		name,
		std::optional<Part_state>{},
		options((Part_state*)0)
	);
}

std::string input_form(DB db,std::string const& name,Date value){
	return show_input(db,name,value).form;
}

std::string input_form(DB,std::string const& name,bool value){
	std::stringstream ss;
	string id="r"+as_string(rand());

	ss<<"<table width=\"100%\"><tr><td align=center id=\""+id+"\">";
	ss<<"<input type=\"checkbox\" name=\""<<name<<"\"";
	if(value) ss<<" checked=on";
	ss<<" onclick=\""+id+".style.background='yellow';\"";
	ss<<">";
	ss<<"</td></tr></table>";
	//ss<<" onclick=\"style.background-color='yellow';\"";
	//ss<<" onclick=\"document.write('hello!!!');\"";
	//ss<<" onclick=\"document.getElementById('"+id+"').style.border='thick solid yellow';\"";
	//ss<<" onclick=\"document.getElementById('"+id+"').innerHTML='hello';\"";
	return ss.str();
}

std::string input_form(DB,std::string const& name,std::optional<Assembly_state> value){
	return drop_down(name,value,options((Assembly_state*)0));
}

std::string input_form(DB,string const& name,Assembly_state value){
	return drop_down(name,value,options((Assembly_state*)0));
}

std::string input_form(DB,std::string const& name,Part_design_state value){
	return drop_down(name,value,options((Part_design_state*)0));
}

std::string input_form(DB,std::string const& name,std::optional<Part_design_state> value){
	return drop_down(name,value,options((Part_design_state*)0));
}

std::string input_form(DB,std::string const& name,std::optional<URL> const& a){
	return "<input type=\"url\" name=\""+name+"\""+
	[&]()->string{
		if(!a) return "";
		return " value="+escape(*a);
	}()
	+" onclick=\"style.background='yellow';\""
	+">";
}

std::string input_form(DB db,std::string const& name,URL const& value){
	std::stringstream ss;
	if(nonempty(value.data)) ss<<a(as_string(value),"View");
	ss<<input_form(db,name,optional<URL>{value});
	return ss.str();
}

std::string input_form(DB,std::string const& name,Subsystem_prefix value){
	std::stringstream ss;
	ss<<"<input name=\""+name+"\" maxlength=\"2\"";
	ss<<" value=\""<<value<<"\"";
	ss<<" onclick=\"style.background='yellow';\">";
	return ss.str();
}

std::string input_form(DB,std::string const& name,Upload_data const& value){
	return thumbnail(name,value);
}

template<int A,int B>
string input_form(DB,string const& name,Int_limited<A,B> a){
	return input_num<Int_limited<A,B>>(
		name,
		Int_limited<A,B>(A),
		Int_limited<A,B>(B),
		{},
		a
	);
}

template<int A,int B>
string input_form(DB,string const& name,std::optional<Int_limited<A,B>> a){
	return input_num<Int_limited<A,B>>(
		name,
		Int_limited<A,B>(A),
		Int_limited<A,B>(B),
		{},
		a
	);
}

template<typename A,A B>
std::string input_form(DB db,std::string const& name,Defaulted<A,B> const& a){
	return input_form(db,name,a.t);
}

std::string input_form(DB db,std::string const& name,Column_value const& value){
	return visit([&](auto x){ return input_form(db,name,x); },value);
}

string input_form(DB db,string const& name,optional<bool> const& a){
	return input_form(
		db,
		name,
		[=](){
			if(a) return *a;
			return false;
		}()
	);
}

string input_form(DB,string const& name,Color const& a){
	return "<input type=\"color\" name=\""+name+"\" value="+escape(a)+
	" onclick=\"style.background='yellow';\">";
}

string input_form(DB db,string const& name,optional<Color> const& a){
	if(a) return input_form(db,name,*a);
	return "<input type=\"color\" name=\""+name+"\""+
		" onclick=\"style.background='yellow';\">";
}

string input_form(DB,string const& name,std::optional<string> s){
	return input_text(name,s);
}

template<typename A,A B>
string input_form(DB db,string const& name,optional<Defaulted<A,B>> const& a){
	if(a) return input_form(db,name,a->t);
	return input_form(db,name,B);
}

template<typename T>
string input_form(DB,string const& name,optional<T> const& a){
	PRINT(name);
	PRINT(type(a));
	nyi
}

string input_form(DB db,string const& name,Typename const& type){
	#define X(A) if(type==Typename{""#A}){\
		return input_form(db,name,null((A*)0));\
	}
	STANDARD_COL_TYPES(X)
	X(Meeting_length)
	X(Color)
	X(bool)
	X(std::optional<unsigned>)
	X(Assembly_id)
	X(Dimension)
	#undef X
	PRINT(type)
	nyi
}
