#include "latest.h"
#include<typeinfo>
#include "base64.h"
#include "db.h"
#include "page.h"
#include "data_types.h"
#include "type.h"
#include "status.h"
#include "input.h"
#include "main.h"
#include "get_name.h"
#include "map_util.h"
#include "set_util.h"

using namespace std;

string hidden(string const& a,Id_table const& b){
	return hidden(a,b.get());
}

std::map<Order_id,string> order_labels_inner(DB db);

std::map<Order_id,string> order_labels(DB db){
	static std::map<Order_id,string> r;
	if(r.empty()){
		r=order_labels_inner(db);
	}
	return r;
}

std::map<Order_id,string> order_labels_inner(DB db){
	auto q=query_to<Supplier_id,Date,Order_id>(
		db,
		"SELECT supplier,submitted,main "+current(Id_table{"order"})+
		"ORDER BY submitted,supplier,main"
	);
	auto m=to_multiset(mapf(
		[](auto x){ return take2(x); },
		q
	));
	
	map<pair<Supplier_id,Date>,int> used;

	auto next_name=[&](auto row){
		auto key=take2(row);
		auto c=m.count(key);
		assert(c);
		auto base=get_name(db,get<0>(row))+" "+as_string(get<1>(row));
		if(c==1){
			return base;
		}
		used[key]++;
		return base+" "+as_string(used[key]);
	};

	return to_map(mapf(
		[&](auto row){
			return make_pair(
				get<2>(row),
				next_name(row)
			);
		},
		q
	));
}

optional<Order_id> parse(Order_id const*,optional<int> const& a){
	if(a) return Order_id{*a};
	return std::nullopt;
}

Upload_id parse(Upload_id const*,int i){
	return Upload_id{i};
}

Part_state parse(Part_state const*,optional<int> const& a){
	if(!a) return Part_state{};
	PRINT(a);
	assert(0);
}

Part_design_state parse(Part_design_state const* t,std::optional<int> const& a){
	if(a) return parse(t,*a);
	return {};
}

std::string redirect_items(Page_normal const& page){
	return join(mapf(
		[](auto p){
			return hidden("next_"+p.first,p.second);
		},
		vars(page)
	));
}

#define X(A,B) std::optional<Sort> sort(A const&);
NORMAL_PAGES(X)
#undef X

#define X(A) optional<Sort> sort(A const&){ return {}; }
X(Home)
X(Recent)
X(E404)
X(View)
X(Page_upload)
X(Export)
X(Nested)
X(Extra)
X(Search)
X(Orders)
#undef X

#define X(A) optional<Sort> sort(A const& a){ return a.sort; }
X(Page_raw)
X(Latest)
X(Item)
X(Status)
#undef X

optional<Sort> sort(Page_normal const& a){
	return visit([](auto x){ return sort(x); },a);
}

template<typename T>
std::optional<T> empty(std::optional<T> const*){
	return std::nullopt;
}

template<typename T>
T empty(T const*){
	assert(0);
}

Column_value to_val(Typename type,std::optional<std::string> a){
	if(type=="std::string"){
		assert(a);
		return *a;
	}

	if(type=="Assembly_design_id"){
		assert(a);
		return parse((Assembly_design_id*)0,*a);
	}
	#define X(A) if(type==""#A){\
		if(!a) return empty((A*)0);\
		try{\
			return parse((A*)0,*a);\
		}catch(...){\
			throw "Invalid for "+type+": \""+as_string(a)+"\"";\
		}\
	}
	X(std::optional<unsigned>)
	X(std::optional<Assembly_id>)
	X(std::optional<Weight>)
	X(Assembly_state)
	X(std::optional<Decimal>)
	X(Part_number_local)
	X(std::optional<Positive_decimal>)
	X(Subsystem_prefix)
	STANDARD_COL_TYPES(X)
	X(Meeting_length)
	X(Color)
	X(bool)
	X(Part_number_local)
	X(Assembly_id)
	X(Dimension)
	#undef X
	PRINT(type);
	nyi
}

/*using Row_id=std::variant<
	#define X(A,B,C) B##_id,
	TABLES(X)
	#undef X
	bool //this option is not intended to actully be used and is here for syntax reasons
>;*/

int stoi(Row_id a){
	return visit([](auto x){ return stoi(x); },a);
}

string escape(Row_id a){
	return visit([](auto x){ return escape(x); },a);
}

Row_id to_id(Id_table const& table,std::optional<std::string> const& a){
	assert(a);
	auto i=stoi(*a);
	#define X(A,B,C) if(table==""#A) return B##_id{i};
	TABLES(X)
	#undef X
	assert(0);
}

void show_row(
	std::ostream &ss,
	Id_table const& table,
	Page_normal const& page,
	vector<Form_item> const& cols,
	std::vector<std::optional<std::string>> const& row,
	DB db,
	bool link_more
){
	ss<<"<tr>";

	auto main=[&](){
		for(auto [meta,value]:zip(cols,row)){
			auto [show,name,type]=meta;
			if(name=="main") return to_id(table,value);
		}
		assert(0);
	}();

	if(link_more){
		ss<<td(a(
			Item{table,stoi(main),{},sort(page)},
			tag("big",tag("big","&#x1f5ce;")) //used to say "More"
		));
	}
	//ss<<"<form method=\"post\">";
	ss<<"<form method=\"get\">";//for debugging
	ss<<hidden("page","edit");
	ss<<hidden("table",table);
	ss<<redirect_items(page);
	for(auto [meta,value]:zip(cols,row)){
		auto [show,name,type]=meta;
		switch(show){
			case Category::IGNORE:
				break;
			case Category::HIDE:
				assert(value);
				ss<<hidden(name,*value);
				break;
			case Category::SHOW:
				ss<<td(input_form(db,name,to_val(type,value),main));
				break;
			default:
				assert(0);
		}
	}
	ss<<tag(
		"td alight=center valign=center",
		[&](){
			if(table=="upload"){
				return "Not editable.";
			}
			return "<input type=\"submit\" value=\"Edit\">";
		}()
	);
	ss<<"</form>";
	ss<<tag(
		"td align=center valign=center",
		"<form method=\"post\">"
		+hidden("page","delete")
		+hidden("table",table)
		+hidden("main",escape(main))+
		redirect_items(page)
		+"<input type=\"submit\" value=\"Delete\">"
		"</form>"
	);
	ss<<"</tr>";
}

std::vector<Form_item> form_columns(Id_table const& table){
	vector<Form_item> cols{
		Form_item{Category::IGNORE,Column_name{"id"},""},
		Form_item{Category::HIDE,Column_name{"main"},""},
		Form_item{Category::HIDE,Column_name{"valid"},""},
		Form_item{Category::IGNORE,Column_name{"edit_user"},""},
		Form_item{Category::IGNORE,Column_name{"edit_date"},""}
	};
	auto e=table_types();
	auto this_table=e[table];
	for(auto [a,b]:this_table){
		cols|=Form_item(Category::SHOW,a,b);
	}
	return cols;
}

string link_type_inner(Typename const& type,string const& name){
	#define X(A,B,C) if(type==""#B "_id" || type=="std::optional<"#B "_id>"){\
		return a(Latest{Id_table{""#A},{},{}},name);\
	}
	TABLES(X)
	#undef X
	return name;
}

optional<string> get_info(Typename const& type,string const& name){
	if(name=="parent"){
		return "What this is a part of";
	}
	if(type=="Stock_id" && name=="stock"){
		return "A type of off the shelf puchasable item";
	}
	if(type=="Positive_int"){
		return "How many of this item.  ";
	}
	if(type=="bool" && name=="arrived"){
		return "Has this item arrived so we can build robots with it?";
	}
	if(name=="cost"){
		return "In dollars";
	}
	if(name=="weight" || name=="weight_override"){
		return "In lbs; up to three decimal places";
	}
	if(name=="time_left"){
		return "Hours left to complete";
	}
	if(type=="Decimal"){
		return "Up to three decimal points";
	}
	if(type=="Subsystem_prefix"){
		return "What parts of this should have their numbers start with.  Must be two uppercase letters.";
	}
	if(type=="Part_number_local"){
		return "What this part should be called in the form AAXXX&#8209;1425&8209;YYYY where:"
		+ul(
			li("\"AA\" is the subsystem prefix")+
			li("\"XXX\" is a number 000-999")+
			li("\"YYYY\" is the current year")
		);
	}
	if(name=="name"){
		return "A human-understandable name";
	}
	if(name=="design_time_left"){
		return "Number of days expected in the design process";
	}
	if(name=="assembly_time"){
		return "Hours to assemble after all the parts are done";
	}
	if(name=="child"){
		return "The thing that is a component of \"parent\".";
	}
	if(name=="supplier"){
		return "Organization that things can be bought from";
	}
	if(type=="Part_number"){
		return "What the supplier calls the part";
	}
	if(name=="lead_time"){
		return "In days.  Be sure to include shipping.";
	}
	if(name=="url"){
		return "The page to order the part from";
	}
	if(name=="notes"){
		return "Freeform text";
	}
	if(name=="part_design"){
		return "What part is being built";
	}
	if(name=="machine"){
		return "Which machine tool should be used (if any)";
	}
	if(type=="Positive_decimal" && name=="time"){
		return "Hours this manufacturing step is expected to take";
	}
	if(type=="Meeting_length"){
		return "In hours";
	}
	if(name=="manufacture_step"){
		return "Which step needs to happen next";
	}
	if(name=="stock"){
		return "What this part is made out of";
	}
	if(type=="Dimension"){
		return "In inches";
	}
	if(name=="available"){
		return "The number we have sitting on the shelf";
	}
	if(name=="wishlist"){
		return "How many we would like to keep on hand";
	}
	if(name=="assembly"){
		return "What it is bolted (etc.) onto";
	}
	if(name=="datatype"){
		return "The MIME type of the uplaoded item.  Kind of like a file extension.";
	}
	if(name=="upload"){
		return "Which uploaded file.  Shows the MIME type.";
	}
	if(type=="Upload_data"){
		return "If you'd like to change the data, upload a new version.";
	}
	if(type=="Dimensions"){
		return "In how many dimensions this type of stock is expected to be parted."+
		tag(
			"table border",
			tr(th("0")+td("Not expected to be parted off at all"))+
			tr(th("1")+td("Linear, like shaft stock"))+
			tr(th("2")+td("Sheet"))+
			tr(th("3")+td("Blocks"))
		);
	}
	return {};
}

string link_type(DB db,Typename const& type,string const& name){
	(void)db;
	return "<div class=\"tooltip\">"+link_type_inner(type,name)+"<span class=\"tooltiptext\">"+
		tag(
			"table border",
			[&]()->string{
				auto info=get_info(type,name);
				if(!info) return "";
				return tr(tag("td colspan=2",*info));
			}()+
			tr(td("Type")+td(html_escape(type.data)))+
			tr(td("Example")+td(rand(type)))
		)+
		"</span></div>";
}

#define X(A,B,C) std::string escape(B##_id a){ return as_string(a.data); }
TABLES(X)
#undef X

std::string escape(Column_value const& value){
	return visit(
		[](auto x){
			//PRINT(type(x));
			return escape(x);
		},
		value
	);
}

void extra(ostream& o,DB db,Id_table const& table){
	(void)db;
	if(table!="stock"){
		return;
	}

	o<<h3("Combined part design creation");
	o<<"Used to create a new type of stock, along with a new category for it to belong to, and where to buy it at the same time.";
	o<<"<form>";
	o<<hidden("page","stock_new");
	o<<"<table border>";
	#define X(A,B) o<<tr(\
		th(link_type(db,Typename{""#A},""#B))+\
		td(input_form(db,""#B,Typename{""#A}))\
	);
	STOCK_NEW_ITEMS(X)
	#undef X
	o<<tr(td("")+td("<input type=\"submit\" value=\"New stock\">"));
	o<<"</table>";
	o<<"</form>";
	//This is the thing that links via Page_stock_new.	
	/*stock_category
		name
		dimensions (implicit by things in stock)
	stock
		cat (new)
		length
		width
		thickness
		available
		notes
		wishlist
	availability
		stock (new)
		supplier
		part_number
		cost
		lead_time
		url*/
}

void latest(ostream& o,DB db,Id_table const& table,Latest const& page){
	o<<h2(tag(
		"a name=\""+table+"\"",
		a(table,table)
	));
	o<<table_description(table);
	auto e=table_types();
	auto this_table=e[table];
	auto cols=filter(
		[&](auto x){
			//avoid showing potentially GB of info
			return !(table=="upload" && x.name=="data");
		},
		form_columns(table)
	);

	auto q=query(
		db,
		string()+"SELECT "+
		//"*"
		join(",",mapf([](auto x){ return x.name; },cols))
		+" "+
		"FROM "+table+"_info "+
		"WHERE (id) IN "
			"(SELECT MAX(id) FROM "+table+"_info GROUP BY main) AND valid"+
		[&]()->string{
			if(!page.filter) return "";
			if(!(page.filter->first&to_set(firsts(this_table)))){
				return "";
			}
			return " AND "+page.filter->first+"="+escape(page.filter->second);
		}()+
		[&]()->string{
			if(!page.sort) return "";
			if(!(page.sort->first&to_set(firsts(this_table)))){
				return "";
			}
			return " ORDER BY "+page.sort->first+" "+as_string(page.sort->second);
		}()
	);
	o<<"<table border>";
	o<<"<tr>";
	o<<th("&#x1f5ce;");//Used to say "More", now the document icon
	for(auto [action,name,type]:cols){
		if(action==Category::SHOW){
			o<<th(link_type(db,type,name)+sort_links(page,name));
		}
	}
	o<<th("Update");
	o<<th("Delete");
	o<<"</tr>";
	for(auto const& row:q){
		show_row(o,table,page,cols,row,db,1);
	}
	o<<"<tr bgcolor=\"#ccccff\">";
	o<<td("");
	//ss<<"<form method=\"post\">\n";
	o<<"<form method=\"get\">\n";
	o<<hidden("page","insert");
	o<<hidden("table",table);
	o<<redirect_items(page);
	for(auto [name,type]:this_table){
		o<<td(input_form(db,name,type));
	}
	o<<"<td colspan=2>";
	o<<"<input type=\"submit\" value=\"New\">";
	o<<"</td>";
	o<<"</form>";
	o<<"</tr>";
	o<<"</table>";

	extra(o,db,table);
}

string show(DB,string const& s){
	return s;
}

template<typename A,A B>
string show(DB db,Defaulted<A,B> const& a){
	return show(db,a.t);
}

string show(DB,Color a){
	//Could make this a patch of that color
	return escape(a);
}

string show(DB,int x){ return as_string(x); }
string show(DB,Assembly_state a){ return as_string(a); }
string show(DB,Part_number const& a){ return as_string(a); }
string show(DB,Part_number_local const& a){ return as_string(a); }
//string show(DB,optional<string>);
string show(DB,Decimal);
string show(DB,Positive_decimal a){ return as_string(a); }
string show(DB db,URL const& a){ return sql_escape(db,a); }
string show(DB,Date);
string show(DB db,Part_design_state a){ return sql_escape(db,a); }
string show(DB db,Part_state a){ return sql_escape(db,a); }
string show(DB db,Subsystem_prefix const& a){ return sql_escape(db,a); }
string show(DB db,Nonempty_string const& a){ return sql_escape(db,a); }
string show(DB,User const& a){ return as_string(a); }
string show(DB,Weight a){ return as_string(a); }

template<typename T>
string show(DB db,optional<T> const& a){
	if(!a) return "NULL";
	return show(db,*a);
}

#define X(A,B,C) string show(DB,B##_id);
TABLES(X)
#undef X

string show(DB,Upload_data const&){
	return "Upload data goes here";
}

string show(DB db,Filter const& f){
	return visit([&](auto x){ return f.first+"="+show(db,x); },f.second);
}

template<typename T>
string get_name(DB,T t){
	return as_string(t);
}

string get_name(DB db,Column_value const& value){
	return visit([&](auto x){ return get_name(db,x); },value);
}

void show(ostream& o,DB db,Latest const& p){
	auto tables=[&](){
		if(p.table){
			return vector{*p.table};
		}
		std::vector<Id_table> tables;
		#define X(A,B,C) tables|=Id_table{""#A};
		TABLES(X)
		#undef X
		return tables;
	}();
	tables=sorted(tables);
	page(
		o,
		[=]()->string{
			if(tables.size()==1){
				return tables[0];
			}
			return "Latest info";
		}()+[=]()->string{
			if(p.filter){
				return ": "+p.filter->first+"="+get_name(db,p.filter->second);
			}
			return "";
		}(),
		[=]()->string{
			if(tables.size()==1){
				return tables[0];
			}
			return "Latest info";
		}()+[=]()->string{
			if(p.filter){
				return " "+show(db,*p.filter);
			}
			return "";
		}(),
		[&](std::ostream& ss){
			if(tables.size()!=1){
				ss<<h2("Table of Contents");
				for(auto table:tables){
					ss<<a(table)<<"<br>";
				}
			}else{
				mapv(
					[&](auto x){ return latest(ss,db,x,p); },
					tables
				);
			}
		},
		""
	);
}

#define X(A,B,C) int stoi(B##_id a){ return a.data; }
TABLES(X)
#undef X
