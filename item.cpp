#include "item.h"
#include "latest.h"
#include "recent.h"
#include "status.h"
#include "extra.h"
#include "input.h"
#include "main.h"
#include "get_name.h"
#include "orders.h"
#include "map_util.h"
#include "set_util.h"

using namespace std;

std::string hidden(const char *a,Id_table const& b){
	return hidden(a,b.get());
}

template<typename T>
optional<Filter> filter(T const&){
	return std::nullopt;
}

#define X(NAME) optional<Filter> filter(NAME const& a){ return a.filter; }
X(Page_raw)
X(Latest)
X(Item)
#undef X

optional<Filter> filter(Page_normal const& a){
	return visit([&](auto x){ return filter(x); },a);
}

void show_table(std::ostream& o,DB db,Page_normal const& page,Table_name const& name){
	auto filter=::filter(page);
	auto sort=::sort(page);
	o<<h2(name)<<"\n";
	o<<table_description(name);
	auto r=read(db,name);
	auto q=query(
		db,
		"SELECT * FROM `"+name+"`"
		+[=]()->std::string{
			if(!filter) return "";
			if(!(filter->first&to_set(firsts(r)))) return "";
			return " WHERE `"+filter->first+"`="+sql_escape(db,filter->second);
		}()+
		[=]()->std::string{
			if(!sort) return "";
			if(!(sort->first&to_set(firsts(r)))) return "";
			return " ORDER BY `"+sort->first+"` "+as_string(sort->second);
		}()
	);
	o<<"<table border>";
	o<<"<tr>";
	for(auto elem:r){
		o<<th(
			elem.first+" "+
			elem.second.type+sort_links(page,elem.first)
		);
	}
	o<<"</tr>";
	for(auto [i,row]:enumerate(q)){
		o<<"<tr>";
		for(auto [j,p]:enumerate(zip(r,row))){
			auto [type,x]=p;
			if(i){
				o<<show_elem(type,x,q[i-1][j]);
			}else{
				o<<show_elem(type,x);
			}
		}
		o<<"</tr>";
	}
	o<<"</table>";
}

#define X(A,B,C) Page page(B##_id a){\
	return Item{table(a),a.data,{},{}};\
}
TABLES(X)
#undef X

#define X(A,B,C) Page_normal page_normal(B##_id a){\
	return Item{table(a),a.data,{},{}};\
}
TABLES(X)
#undef X

std::string link_col_label(std::string const& s){
	#define X(A,B,C) if(s==""#A) return a(Latest{Id_table{""#A},{},{}},s);
	TABLES(X)
	#undef X
	return s;
}

/*template<typename A,typename B>
pair<A,B> query_single(DB db,std::string const& query_string){
	auto q=query_to<A,B>(db,query_string);
	if(q.size()!=1){
		PRINT(q);
	}
	assert(q.size()==1);
	return q[0];
}*/

//end utility functions

Typename id_type(Id_table const& t){
	#define X(A,B,C) if(""#A==t) return Typename{""#B+string("_id")};
	TABLES(X)
	#undef X
	assert(0);
}

Typename corresponding_type(Id_table const& a){
	#define X(A,B,C) if(a==""#A) return Typename{""#B "_id"};
	TABLES(X)
	#undef X
	assert(0);
}

set<Typename> corresponding_types(Id_table const& a){
	auto x=corresponding_type(a);
	return set<Typename>{x,Typename{"std::optional<"+x+">"}};
}

Column_value parse_col(Id_table const& table,int i){
	#define X(A,B,C) if(table==""#A) return B##_id{i};
	TABLES(X)
	#undef X
	PRINT(table);
	nyi
}

string show_table(
	Item const& item,
	Id_table const& table,
	std::vector<std::vector<std::optional<std::string>>> const& q,
	DB db
){
	//auto e=table_types();
	//auto cols=e[table];
	stringstream ss;
	ss<<"<table border>";
	auto cols=form_columns(table);
	ss<<tr(join(
		th("&#x1f5ce;")+
		mapf(
			[&](auto x)->string{
				if(x.category==Category::SHOW){
					return th(
						link_type(db,x.type,x.name)+
						" "+sort_links(item,x.name)
					);
				}
				return "";
			},
			cols
		)+
		th("Edit")+th("Delete")
	));
	for(auto row:q){
		show_row(
			ss,
			table,
			item,
			cols,
			row,
			db,
			1
		);
	}

	ss<<"<tr bgcolor=\"#ccccff\">";
	ss<<td("");
	ss<<"<form method=\"post\">\n";
	ss<<hidden("page","insert");
	ss<<hidden("table",table);
	ss<<redirect_items(item);
	auto e=table_types();
	auto this_table=e[table];
	for(auto [name,type]:this_table){
		if(contains(corresponding_types(item.table),type)){
			//Make it default to choosing the things whose page is being displayed.
			/*ss<<td(
				hidden(name,as_string(item.main))+
				get_name(db,item.table,item.main)
			);*/
			ss<<td(input_form(
				db,name,
				parse_col(item.table,item.main)
			));
		}else{
			ss<<td(input_form(db,name,type));
		}
	}
	ss<<"<td colspan=2>";
	ss<<"<input type=\"submit\" value=\"New\">";
	ss<<"</td>";
	ss<<"</form>";
	ss<<"</tr>";

	ss<<"</table>";
	return ss.str();
}

string sort_filter_query(Table_name const& table,Item const& item){
	auto n=to_set(column_names(table));
	stringstream ss;
	if(item.filter && n.count(item.filter->first)){
		auto f=*item.filter;
		ss<<" AND `"<<f.first<<"`="<<escape(f.second);
	}
	if(item.sort && n.count(item.sort->first) ){
		ss<<" ORDER BY `"<<item.sort->first<<"` "<<as_string(item.sort->second);
	}
	return ss.str();
}

string users(DB db,Item const& item){
	auto target=id_type(item.table);

	//p(" 3) things that use this: links and their current state")
	map<Id_table,std::vector<Column_name>> interesting_tables;
	for(auto [name,types]:table_types()){
		for(auto [col_name,col_type]:types){
			if(col_type==target || col_type=="std::optional<"+target+">"){
				interesting_tables[name]|=col_name;
			}
		}
	}

	stringstream ss;
	ss<<h2("Users");
	for(auto [table,cols]:interesting_tables){
		ss<<h3(a(Latest{table,{},{}},table));
		if(prefix("upload_",table)){
			ss<<"<table border><tr><td>";
			ss<<"Upload new item:<br>";
			ss<<"<form method=\"post\" enctype=\"multipart/form-data\">";
			ss<<hidden("page","upload_rx");
			ss<<hidden(item.table,escape(item.main));
			ss<<redirect_items(item);
			ss<<"<input type=\"file\" name=\"userfile\"><br>";
			ss<<"<input type=\"submit\" value=\"Upload\">";
			ss<<"</form>";
			ss<<"</td></tr></table>";
			ss<<"<br>";
		}
		auto q=query(
			db,
			"SELECT * "
			"FROM "+table+"_info "
			"WHERE (id) IN ("
				"SELECT max(id) FROM "+table+"_info GROUP BY main"
			") AND valid"
			" AND ("
			+join(
				" OR ",
				mapf(
					[&](auto x){ return "`"+x+"`="+as_string(item.main); },
					cols
				)
			)+
			") "+sort_filter_query(table,item)
		);
		ss<<show_table(item,table,q,db);
	}
	return ss.str();
}

optional<Id_table> table_type(Typename const& type){
	#define X(A,B,C) if(type==""#B "_id") return Id_table{""#A};
	TABLES(X)
	#undef X
	#define X(A,B,C) if(type=="std::optional<"#B "_id>") return Id_table{""#A};
	TABLES(X)
	#undef X
	return {};
}

string current(Typename const& type,optional<string> const& value){
	{
		auto table=table_type(type);
		if(table && value){
			return " "+a(Item{*table,stoi(value),{},{}},"Detail");
		}
	}
	return "";
}

optional<tuple<Availability_id,string,Decimal,URL>> available_from(
	DB db,
	Supplier_id supplier,
	Stock_id stock
){
	(void)db;
	(void)supplier;
	(void)stock;
	auto q=query_to<Availability_id,string,Decimal,URL>(db,
		//db,
		"SELECT main,part_number,cost,url"+current(Id_table("availability"))+
		"AND stock="+escape(stock)+
		" AND supplier="+escape(supplier)
	);
	if(q.empty()){
		return {};
	}
	assert(q.size()==1);
	return q[0];
}

string hot_buttons(DB db,Page_normal const&,Stock_id stock){
	using V=pair<string,multiset<Stock_id>>;
	vector<pair<string,multiset<Stock_id>>> v;
	auto x=all_work(db);
	v|=V("Free stock on hand",x.stock_on_hand);
	auto needed=stock_needed(x);
	v|=V("Extra Needed for build",needed);
	auto w=wishlist(db);
	v|=V("Wishlist (target to have extra)",w);
	v|=V("Number wanted to order",w-x.stock_on_hand);

	stringstream o;
	o<<h2("Inventory");
	o<<"<table border>";
	o<<tr(th("Category")+th("Quantity"));
	for(auto [s,m]:v){
		o<<tr(
			td(s)+
			td(as_string(count(m,stock)))
		);
	}
	o<<"</table>";
	o<<h3("Suppliers");
	o<<suppliers(db,stock);
	return o.str();
}

optional<Days> lead_time(DB db,Availability_id id){
	return query_single<optional<Days>>(db,"SELECT lead_time"+current(id));
}

string hot_buttons(DB db,Page_normal const&,Supplier_id supplier){
	stringstream o;
	o<<h2("New order");
	o<<"<form>";
	o<<hidden("page","order");
	o<<hidden("supplier",escape(supplier));
	o<<"<table border>";
	o<<tr(th("Stock")+th("Supplier part number")+th("Cost each")+th("URL")+th("Quantity"));
	vector<tuple<Availability_id,Decimal,unsigned>> listed;
	for(auto [i,p]:enumerate(count(wanted(db)))){
		auto [type,n]=p;
		auto af=available_from(db,supplier,type);
		if(!af) continue;
		auto [availability_id,part_number,cost,url]=*af;
		listed|=make_tuple(availability_id,cost,n);
		o<<"<tr>";
		o<<td(a(db,type));
		o<<td(a(availability_id,part_number));
		o<<td(as_string(cost));
		o<<td(
			//empty(url)?string():a(url,"Part page")
			a(url,"Part page")
		);
		o<<hidden("stock_"+as_string(i),escape(type));
		o<<td(
			"<input name=\"q_"+as_string(i)+"\" type=\"number\" min=0 max="+escape(n)+" value="+escape(n)+">"
		);
		o<<"</tr>";
	}
	auto cost=sum(mapf([](auto x){ return get<1>(x)*get<2>(x); },listed));
	auto lead_times=mapf(
		[&](auto x){
			return lead_time(db,get<0>(x));
		},
		listed
	);
	auto expected=[&]()->optional<Date>{
		auto m=max(lead_times);
		if(m) return now()+*m;
		return std::nullopt;
	}();
	o<<"</table>";
	o<<"<br>";
	o<<"<table border>";
	o<<"<tr>";
	o<<th("Cost");
	o<<td("<input name=\"cost\" type=\"number\" min=\"0\" step=\".01\" value=\""+as_string(cost)+"\""">");
	o<<"</tr>";
	o<<"<tr>";
	o<<th("Who paid");
	o<<td(input_form(db,Column_name{"who_paid"},current_user()));
	o<<"</tr>";
	o<<"<tr>";
	o<<th("Destination");
	o<<td(input_form(db,Column_name{"destination"},Typename{"Location_id"}));
	o<<"</tr>";
	o<<"<tr>";
	o<<th("Expected arrival");
	o<<td(input_form(db,Column_name{"expected_arrival"},expected));
	o<<"</tr>";
	o<<"</table>";
	o<<"<br>";
	o<<"<input type=\"submit\" value=\"Record order\">";
	//once submitted, should go to the page of the newly created order
	//items to see in the form:
	//pairs of (stock_id,qty)
	//hide(line1,stock_id)
	o<<"</form>";
	return o.str();
}

string current(DB db,Item const& item){
	auto cols=filter(
		[&](auto x){ return !(item.table=="upload" && x.name=="data"); },
		form_columns(item.table)
	);
	auto q=query(
		db,
		"SELECT "
		+join(
			",",
			mapf([](auto x){ return x.name; },cols)
		)
		+" FROM "+item.table+"_info WHERE "
		"main="+as_string(item.main)+
		" ORDER BY id DESC LIMIT 1"
	);
	if(q.empty()){
		return p("Could not find item:"+as_string(item));
	}
	assert(q.size()==1);
	auto row=q[0];

	return h2("Current")+
		[=]()->string{
			if(item.table==Id_table{"upload"}){
				return a(View{item.main},"Download");
			}
			return "";
		}()+
		tag("table border",
			[&](){
				stringstream ss;
				ss<<tr(join(mapf(
					[&](auto p)->string{
						auto [x,data]=p;
						if(x.category!=Category::SHOW){
							return "";
						}
						return th(
							//link_col_label(x.name)+
							link_type(db,x.type,x.name)+
							current(x.type,data)
						);
					},
					zip(cols,row)
				))+th("Edit")+th("Delete")
				);
				show_row(
					ss,
					item.table,
					item,
					form_columns(item.table),
					row,
					db,
					0
				);
				return ss.str();
			}()
		);
}

string history(DB db,Item const& item){
	stringstream ss;
	ss<<h2("History");
	show_table(
		ss,
		db,
		[=](){
			auto i2=item;
			i2.filter={Column_name{"main"},as_string(item.main)};
			return i2;
		}(),
		Table_name{item.table.get()+"_info"}
	);
	return ss.str();
}

multiset<Assembly_design_id> asm_needed(DB db,optional<Assembly_design_id> a){
	return to_multiset(query_to<Assembly_design_id,unsigned>(
		db,
		"SELECT child,quantity"+current(Id_table{"assembly_design_assembly"})+
		"AND parent="+escape(a)
	));
}

multiset<Part_design_id> part_needed(DB db,optional<Assembly_design_id> a){
	return to_multiset(query_to<Part_design_id,unsigned>(
		db,
		"SELECT child,quantity"+current(Id_table{"assembly_design_part"})+
		"AND parent="+escape(a)
	));
}

multiset<Stock_id> stock_needed(DB db,optional<Assembly_design_id> a){
	return to_multiset(query_to<Stock_id,unsigned>(
		db,
		"SELECT child,quantity"+current(Id_table{"assembly_design_stock"})+
		"AND parent="+escape(a)
	));
}

multiset<Assembly_id> asm_installed(DB db,Assembly_id a){
	return to_multiset(query_to<Assembly_id>(
		db,
		"SELECT main"+current(Id_table{"assembly"})+"AND parent="+escape(a)
	));
}

multiset<Part_id> part_installed(DB db,Assembly_id a){
	return to_multiset(query_to<Part_id>(
		db,
		"SELECT main"+current(Id_table{"part"})+"AND parent="+escape(a)
	));
}

multiset<Stock_id> stock_installed(DB db,Assembly_id a){
	return to_multiset(query_to<Stock_id,unsigned>(
		db,
		"SELECT stock,quantity"+current(Id_table{"stock_install"})+
		"AND assembly="+escape(a)
	));
}

multiset<Assembly_design_id> designs(DB db,multiset<Assembly_id> a){
	return to_multiset(non_null(mapf([&](auto x){ return design(db,x); },a)));
}

multiset<Part_design_id> designs(DB db,multiset<Part_id> a){
	return to_multiset(mapf([&](auto x){ return design(db,x); },a));
}

set<Assembly_id> available(DB db,Assembly_design_id a){
	return to_set(query_to<Assembly_id>(
		db,
		"SELECT main"+current(Id_table{"assembly"})+
		"AND assembly_design="+escape(a)
	));
}

set<Part_id> available(DB db,Part_design_id a){
	return to_set(query_to<Part_id>(
		db,
		"SELECT main"+current(Id_table{"part"})+
		"AND part_design="+escape(a)
	));
}

unsigned available(DB db,Stock_id a){
	auto q=query_to<unsigned>(
		db,
		"SELECT available"+current(a)
	);
	switch(q.size()){
		case 0:
			//If the type of stock doesn't exist
			return 0;
		case 1:
			return q[0];
		default:
			assert(0);
	}
}

string make_link(DB db,Assembly_id parent,Part_id child,Page_normal const& page){
	stringstream ss;
	ss<<"<form>";
	//might need to go read all of the values
	ss<<hidden("page","edit");
	ss<<hidden("table","part");
	ss<<hidden("main",escape(child));
	ss<<hidden("parent",escape(parent));
	vector<string> retained_cols{"part_design","state","manufacture_step","weight_override","notes"};
	auto q=query(
		db,
		"SELECT "+join(",",retained_cols)+current(child)
	);
	assert(q.size()==1);
	for(auto [name,value]:zip(retained_cols,q[0])){
		ss<<hidden(name,value);
	}
	ss<<redirect_items(page);
	ss<<"<input type=\"submit\" value=\"Add\">";
	ss<<"</form>";
	return ss.str();
}

string make_link(DB db,Assembly_id parent,Assembly_id child,Page_normal const& page){
	stringstream ss;
	ss<<"<form>";
	//might need to go read all of the values
	ss<<hidden("page","edit");
	ss<<hidden("table","assembly");
	ss<<hidden("main",escape(child));
	ss<<hidden("parent",escape(parent));
	vector<string> retained_cols{"name","assembly_design","weight_override","state","time_left"};
	auto q=query(
		db,
		"SELECT "+join(",",retained_cols)+current(child)
	);
	assert(q.size()==1);
	for(auto [name,value]:zip(retained_cols,q[0])){
		ss<<hidden(name,value);
	}
	ss<<redirect_items(page);
	ss<<"<input type=\"submit\" value=\"Add\">";
	ss<<"</form>";
	return ss.str();
}

vector<Assembly_id> installed_of_design(DB db,Assembly_id base,Assembly_design_id design){
	return query_to<Assembly_id>(
		db,
		"SELECT main"+current(Id_table{"assembly"})+
		"AND parent="+escape(base)+
		" AND assembly_design="+escape(design)
	);
}

vector<Part_id> installed_of_design(DB db,Assembly_id base,Part_design_id design){
	return query_to<Part_id>(
		db,
		"SELECT main"+current(Id_table{"part"})+
		"AND parent="+escape(base)+
		" AND part_design="+escape(design)
	);
}

string remove_button(DB db,Page_normal const& page1,Assembly_id child){
	//if wanted to do this better, would have to lookup of the existing things on the backend
	//and done transactionally.
	stringstream ss;
	ss<<"<form>";
	ss<<hidden("page","edit");
	ss<<hidden("table","assembly");
	ss<<hidden("main",escape(child));
	ss<<hidden("parent",{});
	vector<string> retained_cols{"name","assembly_design","weight_override","state","time_left"};
	auto q=query(
		db,
		"SELECT "+join(",",retained_cols)+current(child)
	);
	assert(q.size()==1);
	for(auto [name,value]:zip(retained_cols,q[0])){
		ss<<hidden(name,value);
	}
	ss<<redirect_items(page1);
	
	ss<<"<input type=\"submit\" value=\"Remove\">";
	ss<<"</form>";
	return ss.str();
}

string remove_button(DB db,Page_normal const& page1,Part_id child){
	//similar to removing a part, this would be better if the lookup was done transactionally on the backend
	stringstream ss;
	ss<<"<form>";
	ss<<hidden("page","edit");
	ss<<hidden("table","part");
	ss<<hidden("main",escape(child));
	ss<<hidden("parent",{});
	vector<string> retained_cols{"part_design","state","manufacture_step","weight_override","notes"};
	auto q=query(
		db,
		"SELECT "+join(",",retained_cols)+current(child)
	);
	assert(q.size()==1);
	for(auto [name,value]:zip(retained_cols,q[0])){
		ss<<hidden(name,value);
	}
	ss<<redirect_items(page1);
	
	ss<<"<input type=\"submit\" value=\"Remove\">";
	ss<<"</form>";
	return ss.str();
}

template<typename Item,typename Design>
string removals(
	DB db,
	Page_normal const& page1,
	Item base,
	multiset<Design> extra
){
	if(extra.empty()){
		return "";
	}
	stringstream ss;
	ss<<h3("Extra");
	ss<<"<table border>";
	ss<<tr(th("Name")+th("Overage")+th("Current"));
	for(auto [type,n]:count(extra)){
		ss<<"<tr>";
		ss<<td(get_name(db,type));
		ss<<td(as_string(n));
		ss<<td(
			join("<br>",
				mapf(
					[&](auto x){
						return a(db,x)+remove_button(db,page1,x);
					},
					installed_of_design(db,base,type)
				)
			)
		);
		ss<<"</tr>";
	}
	ss<<"</table>";
	return ss.str();
}

string remove_form(Assembly_id from,Stock_id stock,unsigned n,Page_normal const& page1){
	stringstream o;
	o<<"<form>";
	o<<hidden("page","remove");
	o<<hidden("from",escape(from));
	o<<hidden("stock",escape(stock));
	o<<"<input name=\"quantity\" type=\"number\" min=0 max="+escape(n)+" value="+escape(n)+">";
	o<<redirect_items(page1);
	o<<"<input type=\"submit\" value=\"Remove\">";
	o<<"</form>";
	return o.str();
}

string removals(
	DB db,
	Page_normal const& page1,
	Assembly_id base,
	multiset<Stock_id> extra
){
	if(extra.empty()){
		return "";
	}
	stringstream ss;
	ss<<h3("Extra");
	ss<<"<table border>";
	ss<<tr(th("Name")+th("Overage")+th("Remove"));
	for(auto [type,n]:count(extra)){
		ss<<"<tr>";
		ss<<td(a(db,type));
		ss<<td(as_string(n));
		ss<<td(remove_form(base,type,n,page1));
		ss<<"</tr>";
	}
	ss<<"</table>";
	return ss.str();
}

template<typename Item,typename Design>
string needed(DB db,Page_normal const& page1,Item id,multiset<Design> needed){
	if(needed.empty()){
		return "";
	}
	stringstream ss;
	ss<<h3("Needed");
	for(auto [type,n]:count(needed)){
		//ss<<h4(get_name(db,type));
		ss<<h4(a(page(type),get_name(db,type)));
		ss<<"Number still required:"<<n<<"<br>\n";
		auto av=available(db,type);
		ss<<"Available:\n";
		for(auto elem:av){
			//todo: put a button here to include
			//todo: link
			//todo: show what the current state of it is
			ss<<a(page(elem),get_name(db,elem))<<"<br>";
			ss<<make_link(db,id,elem,page1);
		}
	}
	return ss.str();
}

string needed(DB db,Page_normal const& page1,Assembly_id id,multiset<Stock_id> const& needed){
	if(needed.empty()){
		return "";
	}
	stringstream ss;
	ss<<h3("Needed");
	for(auto [type,n]:count(needed)){
		ss<<h4(a(db,type));
		ss<<"Number still required:"<<n<<"<br>\n";
		ss<<"Available:\n";
		auto av=min(available(db,type),n);
		ss<<"<form>";
		ss<<hidden("page","install");
		ss<<hidden("into",escape(id));
		ss<<hidden("stock",escape(type));
		ss<<"<input name=\"quantity\" type=\"number\" min=1 max="<<av<<" value="<<av<<">";
		ss<<"<input type=\"submit\" value=\"Install\">";
		ss<<redirect_items(page1);
		ss<<"</form>";
	}
	return ss.str();
}

string hot_buttons(DB db,Page_normal const& page1,Assembly_id id){
	stringstream ss;
	auto d=design(db,id);
	{
		auto asm1=asm_needed(db,d);
		auto in=asm_installed(db,id);
		auto asm2=designs(db,in);
		if(asm1!=asm2){
			ss<<h2("Subassemblies");
			ss<<needed(db,page1,id,asm1-asm2);
			ss<<removals(db,page1,id,asm2-asm1);
		}
	}

	{
		auto parts1=part_needed(db,d);
		auto in=part_installed(db,id);
		auto parts2=designs(db,in);
		if(parts1!=parts2){
			ss<<h2("Parts");
			ss<<needed(db,page1,id,parts1-parts2);
			ss<<removals(db,page1,id,parts2-parts1);
		}
	}

	{
		auto stock1=stock_needed(db,d);
		auto stock2=stock_installed(db,id);
		if(stock1!=stock2){
			ss<<h2("Stock");
			ss<<needed(db,page1,id,stock1-stock2);
			ss<<removals(db,page1,id,stock2-stock1);
		}
	}

	return ss.str();
}

bool arrived(DB db,Order_member_id id){
	return query_single<bool>(db,"SELECT arrived"+current(id));
}

bool all_arrived(DB db,Order_id id){
	return all(mapf(
		[&](auto x){ return arrived(db,x); },
		order_members(db,id)
	));
}

string hot_buttons(DB db,Page_normal const& page,Order_id id){
	if(!all_arrived(db,id)){
		return p(all_arrived(page,id));
	}
	return "";
}

template<typename T>
string hot_buttons(DB,Page_normal const&,T const&){
	return "";
}

string hot_buttons(DB db,Page_normal const& page,Item const& item){
	#define X(A,B,C) if(item.table==Id_table{""#A}) return hot_buttons(db,page,B##_id{item.main});
	TABLES(X)
	#undef X
	assert(0);
}

void show(ostream& o,DB db,Item const& item){
	page(
		o,
		get_name(db,item.table,item.main,0),
		get_name(db,item.table,item.main,1),
		table_description(Table_name{item.table+"_info"})+
		current(db,item)+
		hot_buttons(db,item,item)+
		users(db,item)+
		history(db,item)
	);
}
