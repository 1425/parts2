#include "page.h"
#include "data_types.h"
#include "rand.h"
#include "db.h"
#include "main.h"
#include "status.h"
#include "item.h"
#include "latest.h"
#include "extra.h"
#include "input.h"
#include "vars.h"
#include "get_name.h"
#include "type.h"
#include "variant_util.h"
#include "map_util.h"
#include "set_util.h"

using namespace std;

template<typename... T>
string escape(std::variant<T...> const& a){
	return visit([](auto x){ return escape(x); },a);
}

#define PRINT_ITEMS(A,B) o<<""#B<<":"<<a.B<<" ";

#define COMPARE_ITEMS(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;

#define X(A,B) \
	std::ostream& operator<<(std::ostream& o,A const& a){\
		(void)a;\
		o<<""#A<<"( ";\
		B(PRINT_ITEMS)\
		return o<<")";\
	}\
	bool operator<(A const& a,A const& b){\
		(void)a;(void)b;\
		B(COMPARE_ITEMS)\
		return 0;\
	}
PAGES(X)
#undef X

struct Var_reader{
	virtual std::optional<std::string> operator()(std::string const&)const=0;
};

struct Cgicc_reader:Var_reader{
	cgicc::Cgicc cgi;

	Cgicc_reader(cgicc::Cgicc a):cgi(a){}

	std::optional<std::string> operator()(std::string const& col)const{
		return maybe_get(cgi,col);
	}
};

std::ostream& operator<<(std::ostream& o,Cgicc_reader const&){
	return o<<"Cgicc_reader(...)";
}

class Next_reader:public Var_reader{
	Var_reader const& inner;

	public:
	Next_reader(Var_reader const& a):inner(a){}

	std::optional<std::string> operator()(std::string const& col)const{
		return inner("next_"+col);
	}
};

std::ostream& operator<<(std::ostream&,Next_reader const&)nyi

template<typename Var_reader>
bool get_bool(Var_reader const& vars,string const& name){
	auto x=vars(name);
	if(!x) return 0;
	if(*x=="on") return 1;
	PRINT(*x);
	nyi
}

string rand(Typename const& type){
	#define X(A) if(type==""#A) return as_string(rand((A*)0));
	STANDARD_COL_TYPES(X)
	X(Meeting_length)
	X(bool)
	X(std::optional<unsigned>)
	X(Dimension)
	#undef X
	PRINT(type);
	nyi
}

set<Subsystem_prefix> possible_prefixes(){
	set<Subsystem_prefix> r;
	for(auto a:caps()){
		for(auto b:caps()){
			r|=Subsystem_prefix{a,b};
		}
	}
	return r;
}

Subsystem_prefix unused_prefix(DB db){
	auto available=possible_prefixes()-prefixes(db);
	assert(available.size());
	return *begin(available);
}

template<typename T>
Subsystem_prefix get_prefix(DB db,T t){
	auto s=t("prefix");
	if(s){
		try{
			return parse((Subsystem_prefix*)0,*s);
		}catch(...){}
	}
	return unused_prefix(db);
}

set<Part_number_local> with_prefix(DB db,Subsystem_prefix const& prefix){
	auto p=part_numbers(db);
	vector<Part_number_local> r;
	for(auto elem:p){
		try{
			r|=Part_number_local(elem);
		}catch(...){}
	}
	return to_set(filter([=](auto x){ return prefix==x.subsystem_prefix; },r));
}

Part_number_local make_with_prefix(DB db,Subsystem_prefix const& a){
	auto f=max(with_prefix(db,a));
	if(!f) return Part_number_local{a,Three_digit{0},now().year};
	return next(*f);
}

template<typename Var_reader>
std::optional<Column_value> autofill(
	DB db,
	Var_reader const& vars,
	string const& column_name,
	Typename const& type
){
	(void)db;
	(void)type;
	auto g=vars(column_name);
	if(column_name=="part_number" && type=="Part_number_local"){
		if(g && g->size()) return std::nullopt;
		auto existing=part_numbers(db);
		auto p=get_prefix(db,vars);
		return make_with_prefix(db,p);
	}
	if(column_name=="prefix" && type=="Subsystem_prefix"){
		return get_prefix(db,vars);
	}
	if(column_name=="available" && type=="unsigned"){
		return 0;
	}
	return std::nullopt;
}

template<typename Var_reader>
variant<Get_cgi_var::Success,Get_cgi_var::Errors> table_columns(
	DB db,
	Var_reader const& vars,
	Id_table const& table
){
	auto x=table_types();
	auto f=x.find(table);
	if(f==x.end()){
		cout<<"Error: Unknown table.\n";
		exit(0);
	}
	auto items=f->second;
	std::map<Column_name,Column_value> r;
	Get_cgi_var::Errors invalid;
	for(auto [name,type]:items){
		if(type=="bool"){
			r[name]=get_bool(vars,name);
		}else{
			auto v=vars(name);
			if(!v){
				invalid|=Get_cgi_var::Error(name,type,"Unexpectedly NULL",rand(type));
			}else{
				auto s=*v;
				try{
					r[name]=parse_col(type,s);
				}catch(...){
					auto x=autofill(db,vars,name,type);
					if(x){
						r[name]=*x;
					}else{
						invalid|=Get_cgi_var::Error(name,type,s,rand(type));
					}
				}
			}
		}
	}
	if(invalid.size()) return invalid;
	return r;
}

template<typename T>
Page parse_page(DB,T const&);

template<typename Var_reader>
optional<Page_normal> parse_redirect(DB db,Var_reader const& vars){
	auto p=parse_page(db,Next_reader{vars});
	if(holds_alternative<Page_normal>(p)){
		return get<Page_normal>(p);
	}
	return {};
}

optional<Sort> parse(Sort const*,Var_reader const& vars){
	auto by=vars("sort_by");
	if(!by) return {};
	return Sort{
		*by,
		[&]()->Sort_order{
			auto x=vars("sort_order");
			if(!x) return Sort_order::ASC;
			auto p=parse((Sort_order*)0,*x);
			if(!p) return Sort_order::ASC;
			return *p;
		}()
	};
}

#define X(A,B,C) Column_value parse1(B##_id const*,optional<string> const& a){\
	if(!a) throw "invalid";\
	return B##_id{stoi(*a)};\
}
TABLES(X)
#undef X

optional<Filter> parse(Filter const*,Var_reader const& vars){
	auto col=vars("filter_col");
	if(!col) return {};
	auto v=vars("filter_value");

	#define X(A,B,C) if(*col==""#A){\
		return Filter{\
			*col,\
			Column_value{parse1((B##_id*)0,v)}\
		};\
	}
	TABLES(X)
	#undef X

	if(!v) return {};
	return Filter{*col,*v};
}

template<typename Var_reader>
Page_raw parse(Page_raw const*,Var_reader const& vars){
	return Page_raw{
		parse((Filter*)0,vars),
		parse((Sort*)0,vars)
	};
}

template<typename Var_reader>
Page_insert parse(Page_insert const*,DB db,Var_reader const& vars){
	auto t=vars("table");
	assert(t);
	auto table=Id_table{*t};
	return Page_insert{
		table,
		table_columns(db,vars,table),
		parse_redirect(db,vars)
	};
}

template<typename Var_reader>
Page_edit parse(Page_edit const*,DB db,Var_reader const& vars){
	auto table=Id_table{*vars("table")};
	auto m=vars("main");
	if(!m) throw std::invalid_argument("No value found for \"main\"");
	auto main=parse((int*)0,*m);
	return Page_edit{
		table,
		main,
		table_columns(db,vars,table),
		parse_redirect(db,vars)
	};
}

template<typename Var_reader>
Page_delete parse(Page_delete const*,DB db,Var_reader const& vars){
	return Page_delete{
		Id_table{deref(vars("table"))},
		parse((int*)0,deref(vars("main"))),
		parse_redirect(db,vars)
	};
}

template<typename Var_reader>
Latest parse(Latest const*,Var_reader const& vars){
	return Latest{
		[&]()->optional<Id_table>{
			auto v=vars("table");
			if(v) return Id_table{*v};
			return std::nullopt;
		}(),
		parse((Filter*)0,vars),
		parse((Sort*)0,vars)
	};
}

Id_table parse(Id_table const*,optional<string> a){
	assert(a);
	return Id_table{*a};
}

template<typename Var_reader>
Item parse(Item const*,Var_reader const& vars){
	return Item{
		parse((Id_table*)0,vars("table")),
		parse((int*)0,vars("main")),
		parse((Filter*)0,vars),
		parse((Sort*)0,vars)
	};
}

Upload_id parse(Upload_id const*,std::string const& s){
	(void)s;
	nyi
}

Upload_id parse(Upload_id const*,optional<string> const& a){
	assert(a);
	return Upload_id{stoi(*a)};
}

template<typename Var_reader>
View parse(View const*,Var_reader const& vars){
	return View{
		parse((Upload_id*)0,vars("main"))
	};
}

template<typename Var_reader>
Search parse(Search const*,Var_reader const& vars){
	return Search{*vars("query")};
}

template<typename Var_reader>
Page_upload parse(Page_upload const*,Var_reader const&){
	return Page_upload{};
}

#define X(A,B,C) optional<B##_id> parse(\
	optional<B##_id> const*,\
	optional<string> const& a\
){\
	if(!a || *a=="NULL") return std::nullopt;\
	return parse((B##_id*)0,*a);\
}
TABLES(X)
#undef X

template<typename T>
Page_upload_rx parse(Page_upload_rx const*,DB db,T const& t){
	return Page_upload_rx{
		#define X(A,B) parse((A*)0,t(""#B)),
		PAGE_UPLOAD_RX_LINKS(X)
		#undef X
		parse_redirect(db,t)
	};
}

optional<Table_name> parse(optional<Table_name> const*,optional<string> const& a){
	if(!a) return {};
	return parse((Table_name*)0,*a);
}

bool parse(bool const*,std::optional<string> const& a){
	if(!a) return 0;
	return parse((bool*)0,*a);
}

template<typename T>
Export parse(Export const*,T const& vars){
	return Export{
		parse((optional<Table_name>*)0,vars("table")),
		parse((bool*)0,vars("current"))
	};
}

template<typename T>
Page_install parse(Page_install const*,DB db,T const& vars){
	return Page_install{
		#define X(A,B) parse((A*)0,deref(vars(""#B))),
		X(Assembly_id,into)
		X(Stock_id,stock)
		X(Positive_int,quantity)
		#undef X
		parse_redirect(db,vars)
	};
}

template<typename T>
Page_remove parse(Page_remove const*,DB db,T const& vars){
	return Page_remove{
		#define X(A,B) parse((A*)0,deref(vars(""#B))),
		X(Assembly_id,from)
		X(Stock_id,stock)
		X(Positive_int,quantity)
		#undef X
		parse_redirect(db,vars)
	};
}

template<typename T>
Nested parse(Nested const*,T const&){
	return Nested{};
}

template<typename T>
Extra parse(Extra const*,T const&){
	return Extra{};
}

template<typename T>
Orders parse(Orders const*,T){
	return Orders{};
}

template<typename T>
Page_arrived parse(Page_arrived const*,DB db,T const& vars){
	return Page_arrived{
		stoi(vars("order")),
		parse_redirect(db,vars)
	};
}

template<typename T>
T deref1(string s,optional<T> a){
	if(!a){
		PRINT(s);
		throw "invalid";
	}
	return *a;
}

//#define GET(A,B) parse((A*)0,deref(vars(""#B))),
/*#define GET(A,B) parse(\
	(A*)0,deref1(""#B,vars(""#B))\
),
*/
#define GET(A,B) parse(\
	(A*)0,vars(""#B)\
),

template<typename T>
optional<T> parse(optional<T> const* x,optional<string> const& s){
	if(!s) return std::nullopt;
	return parse(x,*s);
}

template<typename T>
Page_order parse(Page_order const*,T const& vars){
	return Page_order{
		parse((Supplier_id*)0,deref(vars("supplier"))),
		/*parse((Decimal*)0,deref(vars("cost"))),
		parse((std::optional<User>*)0,deref(vars("who_paid"))),
		parse((Location_id*)0,deref(vars("destination"))),
		parse((Date*)0,deref(vars("expected_arrival"))),*/
		//GET(Supplier_id,supplier)
		GET(optional<Positive_decimal>,cost)
		GET(optional<User>,who_paid)
		GET(optional<Location_id>,destination)
		GET(optional<Date>,expected_arrival)
		[=](){
			vector<Order_member_basic> r;
			for(unsigned i=0;vars("stock_"+as_string(i));i++){
				auto type=parse((Stock_id*)0,deref(vars("stock_"+as_string(i))));
				auto n=parse((unsigned*)0,deref(vars("q_"+as_string(i))));
				if(n){
					r|=Order_member_basic(type,n);
				}
			}
			return r;
		}()
	};
}

/*Nonempty_string parse(Nonempty_string const*,std::optional<string>);
unsigned parse(unsigned const*,std::optional<string>);
Supplier_id parse(Supplier_id const*,optional<string>);
*/
//template<typename T>
//T parse(T const*,std::optional<string>);

template<typename T>
T get(T const* t,std::optional<std::string> const& a){
	assert(a);
	return parse(t,*a);
}

template<typename T>
Page_stock_new parse(Page_stock_new*,T const& vars){
	Get_cgi_var::Errors errors;
	auto try_elem=[&](auto x,string name){
		auto v=vars(name);
		if(!v){
			errors|=Get_cgi_var::Error{
				Column_name{name},
				type(x),
				"NULL",
				as_string(rand(x))
			};
			return;
		}
		auto s=*v;
		try{
			get(x,s);
		}catch(...){
			errors|=Get_cgi_var::Error{
				Column_name{name},
				type(x),
				s,
				as_string(rand(x))
			};
		}
	};
	#define X(A,B) try_elem((A*)0,""#B);
	STOCK_NEW_ITEMS(X)
	#undef X
	if(errors.size()) return Page_stock_new{errors};

	return Page_stock_new{
		Stock_new{
			#define X(A,B) get((A*)0,vars(""#B)),
			STOCK_NEW_ITEMS(X)
			#undef X
		}
	};
}

template<typename Var_reader>
Page parse_page(DB db,Var_reader const& vars){
	auto p=vars("page");
	if(!p) return Home{};
	if(*p=="raw") return parse((Page_raw*)0,vars);
	if(*p=="latest") return parse((Latest*)0,vars);
	if(*p=="insert") return parse((Page_insert*)0,db,vars);
	if(*p=="edit") return Page{parse((Page_edit*)0,db,vars)};
	if(*p=="delete") return parse((Page_delete*)0,db,vars);
	if(*p=="recent") return Page{Page_normal{Recent{}}};
	if(*p=="item") return parse((Item*)0,vars);
	if(*p=="status") return parse((Status*)0,vars);
	if(*p=="view") return parse((View*)0,vars);
	if(*p=="upload") return parse((Page_upload*)0,vars);
	if(*p=="upload_rx") return parse((Page_upload_rx*)0,db,vars);
	if(*p=="export") return parse((Export*)0,vars);
	if(*p=="nested") return parse((Nested*)0,vars);
	if(*p=="extra") return parse((Extra*)0,vars);
	if(*p=="search") return parse((Search*)0,vars);
	if(*p=="install") return parse((Page_install*)0,db,vars);
	if(*p=="remove") return parse((Page_remove*)0,db,vars);
	if(*p=="orders") return parse((Orders*)0,vars);
	if(*p=="arrived") return parse((Page_arrived*)0,db,vars);
	if(*p=="order") return parse((Page_order*)0,vars);
	if(*p=="stock_new") return parse((Page_stock_new*)0,vars);
	return E404{*p};
}

Page parse_page(DB db,cgicc::Cgicc &cgi){
	Cgicc_reader a{cgi};
	return parse_page(db,a);
}

std::ostream& operator<<(std::ostream& o,Page const& p){
	visit(
		[&](auto x){
			o<<x;
		},
		p
	);
	return o;
}

std::ostream& operator<<(std::ostream& o,Sort_order a){
	switch(a){
		case Sort_order::ASC: return o<<"ASC";
		case Sort_order::DESC: return o<<"DESC";
		default: assert(0);
	}
}

Sort_order rand(Sort_order const*){
	return (rand()%2)?Sort_order::ASC:Sort_order::DESC;
}

string escape(Sort_order a){
	return as_string(a);
}

optional<Sort_order> parse(Sort_order const*,string const& s){
	if(s=="ASC") return Sort_order::ASC;
	if(s=="DESC") return Sort_order::DESC;
	return {};
}

optional<Table_name> to_table(optional<string> a){
	if(!a) return {};
	return Table_name{move(*a)};
}

template<typename T>
Status parse(Status const*,T const& vars){
	return Status{
		to_table(vars("table")),
		parse((Sort*)0,vars)
	};
}

std::string to_url(std::map<std::string,std::string> const& a){
	return "?"+join(
		"&",
		mapf(
			[](auto p){
				auto [k,v]=p;
				return k+"="+v;
			},
			a
		)
	);
}

template<typename T>
string to_url(T const& t){
	return to_url(vars(t));
}

#define X(A,B) string to_url(A const& a){ return to_url<A>(a); }
PAGES(X)
#undef X

string to_url(Page_normal const& p){
	return visit([](auto x){ return to_url(x); },p);
}

string to_url(Page const& a){
	return visit([](auto x){ return to_url(x); },a);
}

#define RAND_IT(A,B) {Column_name{""#B},Column_value(rand((A*)0))},

#define X(A,B,C) map<Column_name,Column_value> rand_vars(B##_id const*){\
	return {C(RAND_IT)};\
}
TABLES(X)
#undef X

map<Column_name,Column_value> rand_vars(Table_name const& table){
	#define X(A,B,C) if(table==""#A) return rand_vars((B##_id*)0);
	TABLES(X)
	#undef X
	assert(0);
}

Page_insert rand(Page_insert const*){
	auto table=rand((Id_table*)0);
	return Page_insert{
		table,
		rand_vars(table),
		rand((std::optional<Page_normal>*)0)
	};
}

#define RAND_ITEM(A,B) rand((A*)0),

#define RAND_ITEMS(TYPE,ITEMS)\
	TYPE rand(TYPE const*){\
		return TYPE{\
			ITEMS(RAND_ITEM)\
		};\
	}

RAND_ITEMS(Get_cgi_var::Error,ERROR_ITEMS)
RAND_ITEMS(Page_delete,PAGE_DELETE_ITEMS)
RAND_ITEMS(Page_edit,PAGE_EDIT_ITEMS)
RAND_ITEMS(Page_upload_rx,PAGE_UPLOAD_RX_ITEMS)
RAND_ITEMS(Page_install,PAGE_INSTALL_ITEMS)
RAND_ITEMS(Page_remove,PAGE_REMOVE_ITEMS)
RAND_ITEMS(Page_arrived,PAGE_ARRIVED_ITEMS)
RAND_ITEMS(Page_order,PAGE_ORDER_ITEMS)
RAND_ITEMS(Page_stock_new,PAGE_STOCK_NEW_ITEMS)
NORMAL_PAGES(RAND_ITEMS)

template<typename T>
string sort_links(T const& page,Column_name const& col){
	auto p1=page;
	p1.sort=Sort{col,Sort_order::ASC};
	auto p2=page;
	p2.sort=Sort{col,Sort_order::DESC};
	return " "+a(p1,"&#9652;"/*"▾"*/)+" "+a(p2,"&#9662;"/*"▾"*/);
}

#define X(NAME) string sort_links(NAME,Column_name){ assert(0); }
X(Home)
X(Recent)
X(View)
X(Page_upload)
X(Export)
X(E404)
X(Nested)
X(Extra)
X(Search)
X(Orders)
#undef X

string sort_links(Page_normal const& page,Column_name const& col){
	return visit([&](auto x){ return sort_links(x,col); },page);
}

template<typename T>
std::string redirect_to(T const& t){
	std::stringstream ss;
	//ss<<"<meta http-equiv = \"refresh\" content = \"2; url = ";
	ss<<"<meta http-equiv = \"refresh\" content = \"0; url = ";
	ss<<t;
	ss<<"\" />";
	ss<<"You should have been automatically redirected to:"<<t<<"\n";
	return ss.str();
}

void redirect_page(ostream& o,string const& page_name,std::optional<Page_normal> const& a){
	if(a){
		o<<redirect_to(to_url(*a));
		return;
	}
	page(o,page_name,"The change has been made.  This should redirect somewhere... but it doesn't.");
}

vector<pair<Stock_install_id,unsigned>> current_install(DB db,Assembly_id assembly,Stock_id stock){
	return query_to<Stock_install_id,unsigned>(
		db,
		"SELECT main,quantity"+current(Id_table{"stock_install"})+
		"AND assembly="+escape(assembly)+
		" AND stock="+escape(stock)
	);
}

struct Row_not_found{
	Row_id a;
};

template<typename Id,typename Value>
void edit_single(DB db,Id id,Column_name const& column_name,Value const& new_value){
	//Note that there is a possible race condition while running this
	//If there is a different edit between the read and the second write
	//then might overwrite the other user's update
	//Would have to add transactional logic in here to close that.

	//Read out all the data currently held in that row
	auto e=table_types();
	
	auto table1=Table_name{table(id)+"_info"};
	auto interesting=filter(
		[](auto x){ return x.first!="id"; },
		e[table(id)]
	);

	map<Column_name,Column_value> current_values;
	auto q=query(
		db,
		"SELECT "+join(",",firsts(interesting))+current(id)
	);
	if(q.size()==0){
		throw Row_not_found{id};
	}
	assert(q.size()==1);
	auto row=q[0];
	for(auto [col,value]:zip(interesting,row)){
		auto [name,type]=col;
		current_values[name]=to_val(type,value);
	}

	//Which value is put into either of these doesn't matter; it will be ignored
	current_values[Column_name{"edit_user"}]=rand((Column_value*)0);
	current_values[Column_name{"edit_date"}]=rand((Column_value*)0);

	current_values[Column_name{"main"}]=id;
	current_values[Column_name{"valid"}]=true;

	//Write all the data back in
	run_cmd(
		db,
		"INSERT INTO `"+table1+"` ("
			+join(",",keys(current_values))+
		") VALUES ("
			+join(",",
				mapf(
					[&](auto p)->string{
						auto [name,value]=p;
						if(name=="edit_user"){
							return sql_escape(db,current_user());
						}
						if(name=="edit_date"){
							return "now()";
						}
						if(name==column_name){
							return sql_escape(db,new_value);
						}
						return sql_escape(db,value);
					},
					current_values
				)
			)
		+")"
	);
}

void edit_single(DB db,Row_id id,Column_name const& column_name,Column_value const& new_value){
	#define X(A,B,C) if(holds_alternative<B##_id>(id)){\
		return edit_single(db,get<B##_id>(id),column_name,new_value);\
	}
	TABLES(X)
	#undef X
	assert(0);
}

void show(ostream& o,DB db,Page_remove const& page1){
	unsigned removed=0;

	//remove from the install
	for(auto [id,n]:current_install(db,page1.from,page1.stock)){
		auto to_remove=min(n,page1.quantity-removed);
		auto after=n-to_remove;
		if(after){
			edit_single(db,id,Column_name{"quantity"},after);
			removed+=to_remove;
		}else{
			//delete the item
			edit_single(db,id,Column_name{"valid"},0);
		}
	}

	//put back in the available
	auto current=available(db,page1.stock);
	try{
		edit_single(db,page1.stock,Column_name{"available"},current+removed);
	}catch(Row_not_found const& f){
		page(
			o,
			"Remove",
			"Could not find info about stock:",as_string(page1.stock)+".  Removed "+as_string(removed)+".  Not adding back to available."
		);
		return;
	}

	if(removed!=unsigned(page1.quantity)){
		page(
			o,
			"Remove",
			"Attempted to remove from "+a(db,page1.from)+" "+as_string(page1.quantity)+" "+a(db,page1.stock)+" but only removed "+as_string(removed)+
			p(a(page1.redirect,"Go back"))
		);
		return;
	}
	redirect_page(o,"Remove",page1.redirect);
}

void edit_qty(DB db,Stock_id id,unsigned quantity){
	edit_single(db,id,Column_name{"available"},quantity);
}

void make_install(DB db,Assembly_id into,Stock_id stock,Positive_int quantity){
	auto c=current_install(db,into,stock);
	if(c.size()){
		auto to_edit=c[0];
		edit_single(db,to_edit.first,Column_name{"quantity"},to_edit.second+quantity);
		return;
	}
	using P=std::pair<std::string,std::string>;
	vector<P> values;
	int n=new_item(db,Table_name{"stock_install"});
	values|=P("main",escape(n));
	values|=P("edit_user",sql_escape(db,current_user()));
	values|=P("edit_date","now()");
	values|=P("valid","1");
	values|=P("assembly",escape(into));
	values|=P("stock",escape(stock));
	values|=P("quantity",escape(quantity));
	insert(db,"stock_install_info",values);
}

void show(ostream& o,DB db,Page_install const& p){
	auto av=available(db,p.stock);
	if(unsigned(p.quantity)>av){
		page(
			o,
			"Install error",
			"Attempted to install "+as_string(p.quantity)+" but only "+as_string(av)+" are available."
		);
		return;
	}
	edit_qty(db,p.stock,av-p.quantity);
	make_install(db,p.into,p.stock,p.quantity);
	redirect_page(o,"Install",p.redirect);
}

std::ostream& operator<<(std::ostream& o,Stock_new const&){
	return o<<"Stock_new(...)";
}

bool operator<(Stock_new const& a,Stock_new const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	STOCK_NEW_ITEMS(X)
	#undef X
	return 0;
}

Stock_new rand(Stock_new const*){
	return Stock_new{
		#define X(A,B) rand((A*)0),
		STOCK_NEW_ITEMS(X)
		#undef X
	};
}
