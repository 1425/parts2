#include "orders.h"
#include "main.h"
#include "status.h"
#include "item.h"
#include "latest.h"
#include "page.h"
#include "get_name.h"
#include "parting3d.h"
#include "parting2d.h"
#include "variant_util.h"
#include "set_util.h"

using namespace std;

template<typename A,typename B>
string table(vector<string> labels,vector<tuple<A,B>> const& v){
	stringstream o;
	o<<"<table border>";
	o<<tr(join(MAP(th,labels)));
	for(auto [a,b]:v){
		o<<tr(td(a)+td(b));
	}
	o<<"</table>";
	return o.str();
}

bool exists(DB db,Row_id const& id){
	auto q=query_to<int>(db,"SELECT id"+current(id));
	return !q.empty();
}


multiset<Stock_id> stock_needed(Part_n const&);
multiset<Stock_id> stock_needed(Assembly_n const&);

template<typename T>
multiset<Stock_id> stock_needed(vector<T> const& v){
	return flatten(MAP(stock_needed,v));
}

multiset<Stock_id> stock_needed(Part_n const& a){
	return to_multiset(a.stock);
}

multiset<Stock_id> stock_needed(Assembly_n const& a){
	return a.stock|stock_needed(a.part)|stock_needed(a.subs);
}

multiset<Stock_id> stock_needed(All_work const& a){
	return stock_needed(a.asm_needed);
}

multiset<Stock_id> wishlist(DB db){
	auto q=query_to<Stock_id,optional<unsigned>>(
		db,
		"SELECT main,wishlist"+current(Id_table("stock"))
	);
	multiset<Stock_id> r;
	for(auto [type,n]:q){
		if(n){
			for(auto _:range(*n)){
				(void)_;
				r|=type;
			}
		}
	}
	return r;
}

multiset<Stock_id> wanted(DB db){
	auto x=all_work(db);
	auto needed=stock_needed(x);
	auto w=wishlist(db)-x.stock_on_hand;
	return needed|w;
}

vector<Supplier_id> active_suppliers(DB db){
	//suppliers that actually have things entered as being available
	return query_to<Supplier_id>(
		db,
		"SELECT DISTINCT(supplier)"+current(Id_table{"availability"})
	);
}

string suppliers(DB db,Stock_id id){
	auto q=query_to<Supplier_id>(
		db,
		"SELECT supplier"+current(Id_table{"availability"})+
		"AND stock="+escape(id)
	);
	return join(" ",mapf([&](auto x){ return a(db,x); },q));
}

set<Stock_id> stock(DB db,Stock_category_id a){
	return to_set(query_to<Stock_id>(
		db,
		"SELECT main"+current(Id_table{"stock"})+" AND category="+escape(a)
	));
}

set<Availability_id> availability(DB db,Stock_id a){
	auto q=query_to<Availability_id>(db,
		"SELECT main"+current(Id_table{"availability"})+"AND stock="+escape(a)
	);
	return to_set(q);
}

set<Availability_id> availability(DB db,Stock_category_id a){
	return flatten(mapf(
		[&](auto x){ return availability(db,x); },
		stock(db,a)
	));
}

auto category(DB db,Stock_id a){
	return query_optional<Stock_category_id>(db,"SELECT category"+current(a));
}

Dimensions dimensions(DB db,Stock_category_id a){
	return query_single<Dimensions>(
		db,
		"SELECT dimensions"+current(a)
	);
}

vector<Positive_decimal> dimensions(DB db,Stock_id a){
	auto q=query_single<Dimension,Dimension,Dimension>(
		db,
		"SELECT length,width,thickness"+current(a)
	);
	return non_null(q);
}

map<Part1,Cost> availability_with_cost1(DB db,Stock_category_id a){
	auto stock1=stock(db,a);
	map<Part1,set<Cost>> r;
	for(auto s1:stock1){
		//auto av=availability(db,s1);
		auto q=query_to<Positive_decimal>(
			db,
			"SELECT cost"+current(Id_table{"availability"})+
			"AND stock="+escape(s1)
		);
		auto d=dimensions(db,s1);
		assert(d.size()==1);
		Part1 p=d[0];
		for(auto cost1:q){
			r[p]|=cost1.get();
		}
	}
	return MAP_VALUES(min,r);
}

string order_cat1(DB db,Stock_category_id category,multiset<Stock_id> needed){
	auto d=mapf(
		[&](auto x)->Either<string,Positive_decimal>{
			auto d=dimensions(db,x);
			if(d.size()!=1){
				return "Expected 1 dimension for "+get_name(db,x)+" but found "+as_string(d.size());
			}
			return d[0];
		},
		needed
	);
	auto g=group(d);
	if(g.first.size()){
		return join("<br>",g.first);
	}
	auto av=availability_with_cost1(db,category);
	Inventory1 b;
	try{
		b=to_buy(av,{},to_multiset(g.second));
	}catch(...){
		return "Unable to calculate how to buy";
	}
	stringstream o;
	o<<"<table border>";
	o<<tr(th("Length")+th("Quantity"));
	for(auto [x,n]:count(b)){
		o<<"<tr>";
		o<<td(as_string(x));
		o<<td(as_string(n));
		o<<"</tr>";
	}
	o<<"</table>";
	return o.str();
}

map<Flat_part,Cost> availability_with_cost2(DB db,Stock_category_id a){
	auto stock1=stock(db,a);
	map<Flat_part,set<Cost>> r;
	for(auto s1:stock1){
		//auto av=availability(db,s1);
		auto q=query_to<Positive_decimal>(
			db,
			"SELECT cost"+current(Id_table{"availability"})+
			"AND stock="+escape(s1)
		);
		auto d=dimensions(db,s1);
		assert(d.size()==2);
		Flat_part p{d[0],d[1]};
		for(auto cost1:q){
			r[p]|=cost1.get();
		}
	}
	return MAP_VALUES(min,r);
}

string order_cat2(DB db,Stock_category_id category,multiset<Stock_id> needed){
	auto d=mapf(
		[&](auto x)->Either<string,Flat_part>{
			auto d=dimensions(db,x);
			if(d.size()!=2){
				return "Expected 2 dimensions for "+get_name(db,x)+" but found "+as_string(d.size());
			}
			return Flat_part(d[0],d[1]);
		},
		needed
	);
	auto g=group(d);
	if(g.first.size()){
		return join("<br>",g.first);
	}

	auto av=availability_with_cost2(db,category);
	auto b=to_buy(av,{},to_multiset(g.second));
	if(!b){
		return "Unable to calculate how to buy";
	}
	stringstream o;
	o<<"<table border>";
	o<<tr(th("Length")+th("Width")+th("Quantity"));
	for(auto [x,n]:count(*b)){
		o<<"<tr>";
		o<<td(as_string(get<0>(x)));
		o<<td(as_string(get<1>(x)));
		o<<td(as_string(n));
		o<<"</tr>";
	}
	o<<"</table>";
	return o.str();
}

map<Part3,Cost> availability_with_cost(DB db,Stock_category_id a){
	auto stock1=stock(db,a);
	map<Part3,set<Cost>> r;
	for(auto s1:stock1){
		//auto av=availability(db,s1);
		auto q=query_to<Positive_decimal>(
			db,
			"SELECT cost"+current(Id_table{"availability"})+
			"AND stock="+escape(s1)
		);
		auto d=dimensions(db,s1);
		assert(d.size()==3);
		Part3 p{d[0],d[1],d[2]};
		for(auto cost1:q){
			r[p]|=cost1.get();
		}
	}
	return MAP_VALUES(min,r);
}

string order_cat3(DB db,Stock_category_id category,multiset<Stock_id> needed){
	auto d=mapf(
		[&](auto x)->Either<string,Part3>{
			auto d=dimensions(db,x);
			if(d.size()!=3){
				return "Expected 3 dimensions for "+get_name(db,x)+" but found "+as_string(d.size());
			}
			return Part3(d[0],d[1],d[2]);
		},
		needed
	);
	auto g=group(d);
	if(g.first.size()){
		return join("<br>",g.first);
	}

	auto av=availability_with_cost(db,category);
	auto b=to_buy(av,{},to_multiset(g.second));
	if(!b){
		return "Unable to calculate how to buy";
	}
	//return join("<br>",MAP(as_string,*b));
	stringstream o;
	o<<"<table border>";
	o<<tr(th("Length")+th("Width")+th("Thickness")+th("Quantity"));
	for(auto [x,n]:count(*b)){
		o<<"<tr>";
		o<<td(as_string(get<0>(x)));
		o<<td(as_string(get<1>(x)));
		o<<td(as_string(get<2>(x)));
		o<<td(as_string(n));
		o<<"</tr>";
	}
	o<<"</table>";
	return o.str();
}

string order_cat(DB db,Stock_category_id category,multiset<Stock_id> needed){
	auto x=availability(db,category);
	auto d=dimensions(db,category);
	switch(d){
		case 0:
			//Obviously no need if the thing is not divisible.
			return "Not an aggregate.";
		case 1:
			return order_cat1(db,category,needed);
		case 2:
			return order_cat2(db,category,needed);
		case 3:
			return order_cat3(db,category,needed);
		default:
			assert(0);
	}
}

string to_order2(DB db){
	auto g=group(
		[&](auto x){ return category(db,x); },
		wanted(db)
	);
	auto m=mapf(
		[&](auto x){
			return make_pair(x.first,order_cat(db,x.first,x.second));
		},
		non_null(g)
	);
	if(m.empty()) return "";
	
	stringstream o;
	o<<h3("Aggregated");
	o<<"<table border>";
	o<<tr(th(a(Id_table{"stock_category"},"Category"))+th("Recommended"));
	for(auto [cat,result]:m){
		o<<"<tr>";
		o<<td(a(db,cat));
		o<<td(result);
		o<<"</tr>";
	}
	o<<"</table>";
	return o.str();
}

string to_order(DB db){
	(void)db;
	stringstream o;
	o<<h2("To order");
	auto x=wanted(db);
	
	o<<"<table border>";
	o<<tr(
		th(a(Id_table{"stock"},"Stock"))+
		th("Quantity")+
		th(a(Id_table{"supplier"},"Supplier"))
	);
	for(auto [type,n]:count(x)){
		o<<"<tr>";
		o<<td(a(db,type));
		o<<td(as_string(n));
		o<<td(suppliers(db,type));
		o<<"</tr>";
	}
	o<<"</table>";

	o<<to_order2(db);
	return o.str();
}

string all_arrived(Page_normal const& page,Order_id order){
	stringstream o;
	o<<"<form>";
	o<<hidden("page","arrived");
	o<<hidden("order",escape(order));
	o<<redirect_items(page);
	o<<"<input type=\"submit\" value=\"Mark arrived\">";
	o<<"</form>";
	return o.str();
}

string on_order(DB db,Page_normal const& page){
	(void)db;
	stringstream o;
	o<<h2("On order");
	//subsystem
	//part
	//supplier
	//supplier part #
	//qty
	//expected arrival
	//arrived button

	//supplier,destination,submitted,expected_arrival, - (parent) stock,qty,arrived
	auto q=query_to<Order_id,Supplier_id,optional<Location_id>,Date,Date>(
		db,
		"SELECT main,supplier,destination,submitted,expected_arrival"+current(Id_table{"order"})
	);
	o<<"<table border>";
	o<<"<tr>";
	o<<th(big(big("&#x1f5ce;")));
	o<<th(a(Id_table{"supplier"},"Supplier"));
	o<<th(a(Id_table{"location"},"Destination"));
	o<<th("Ordered");
	o<<th("Expected arrival");
	o<<th("Items");
	o<<th("All arrived");
	o<<"</tr>";
	for(auto [order,supplier,destination,submitted,expected_arrival]:q){
		auto q1=query_to<Order_member_id,Stock_id,Positive_int,bool>(
			db,
			"SELECT main,stock,quantity,arrived"+current(Id_table{"order_member"})+
			"AND parent="+escape(order)
		);
		auto arrived=mapf([](auto x){ return get<3>(x); },q1);
		//o<<order<<arrived<<"<br>\n";
		if(!all(arrived)){
			o<<"<tr>";
			o<<td(a(order,big("&#x1f5ce;")));
			o<<td(a(db,supplier));
			o<<td(a(db,destination));
			o<<td(as_string(submitted));
			o<<td(as_string(expected_arrival));
			o<<"<td>";
			o<<"<table border>";
			o<<tr(
				th(big(big("&#x1f5ce;")))+
				th(a(Id_table{"stock"},"Stock"))+
				th("Quantity")+
				th("Arrived")
			);
			for(auto [order_member,stock,qty,arrived]:q1){
				o<<tr(
					td(a(order_member,big("&#x1f5ce;")))+
					td(a(db,stock))+
					td(as_string(qty))+
					td(as_string(arrived))
				);
			}
			o<<"</table>";
			o<<"</td>";
			o<<td(all_arrived(page,order));
			o<<"</tr>";
		}
	}
	o<<"</table>";
	return o.str();
}

optional<Date> actual_arrival(DB db,Order_member_id id){
	//oldest date where arrived is set.
	//note that ignoring "valid"...
	auto q=query_to<Date>(db,
		"SELECT edit_date FROM order_member_info WHERE "
		"main="+escape(id)+
		" AND arrived"+
		" ORDER BY id DESC LIMIT 1"
	);
	if(q.size()==0){
		return std::nullopt;
	}
	return q[0];
}

vector<Order_member_id> order_members(DB db,Order_id order){
	return query_to<Order_member_id>(
		db,
		"SELECT parent"+current(Id_table{"order_member"})+
		" AND parent="+escape(order)
	);
}

optional<pair<Date,optional<Date>>> actual_arrival(DB db,Order_id id){
	auto f=mapf([&](auto x){ return actual_arrival(db,x); },order_members(db,id));
	auto nn=non_null(f);
	if(nn.empty()){
		return std::nullopt;
	}
	if(nn.size()!=f.size()){
		return make_pair(min(nn),std::nullopt);
	}
	return make_pair(min(nn),max(nn));
}

string arrived(DB db){
	(void)db;
	//subsystem
	//part
	//supplier
	//supplier part #
	//qty
	//arrival date
	auto q=query_to<
		Order_id,Supplier_id,Positive_decimal,
		optional<User>,optional<Location_id>,Date,Date
	>(
		db,
		"SELECT main,supplier,cost,who_paid,destination,submitted,expected_arrival"+
		current(Id_table{"order"})
	);
	stringstream o;
	o<<h2("Arrived");
	o<<"<table border>";
	o<<tr(
		th(big(big("&#x1f5ce;")))+
		th(a(Id_table{"supplier"},"Supplier"))+
		th("Cost")+
		th("Who paid")+
		th(a(Id_table{"location"},"Destination"))+
		th("Submitted")+
		th("Expected arrival")+
		th("Actual arrival")+
		th("Items")
	);
	for(auto [order,supplier,cost,who_paid,destination,submitted,expected_arrival]:q){
		auto q=query_to<Order_member_id,Stock_id,Positive_int,bool>(
			db,
			"SELECT main,stock,quantity,arrived"+current(Id_table{"order_member"})+"AND parent="+escape(order)
		);
		auto av=mapf([](auto x){ return get<3>(x); },q);
		if(!all(av)){
			continue;
		}
		if(av.empty()) continue;
		o<<"<tr>";
		o<<td(a(order,big("&#x1f5ce;")));
		o<<td(a(db,supplier));
		o<<td(as_string(cost));
		o<<td(as_string(who_paid));
		o<<td(a(db,destination));
		o<<td(as_string(submitted));
		o<<td(as_string(expected_arrival));
		{
			auto n=actual_arrival(db,order);
			if(n){
				if(n->first==n->second){
					o<<td(as_string(n->first));
				}else{
					o<<td(
						as_string(n->first)+"-"+
						as_string(n->second)
					);
				}
			}else{
				o<<td("");
			}
		}
		o<<"<td>";
		o<<"<table border>";
		o<<tr(
			th(big(big("&#x1f5ce;")))+
			th("Stock")+th("Quantity")
		);
		for(auto [main,stock,quantity,arrived]:q){
			o<<tr(
				td(a(main,big(big("&#x1f5ce;"))))+
				td(a(db,stock))+
				td(as_string(quantity))
			);
		}
		o<<"</table>";
		o<<"</td>";
		o<<"</tr>";
	}
	o<<"</table>";
	return o.str();
}

void show(ostream& o,DB db,Orders const& page1){
	page(
		o,
		"Orders",
		to_order(db)+
		on_order(db,page1)+
		arrived(db)
	);
}

void set_arrived(DB db,Order_member_id id){
	//set arrived
	edit_single(db,id,Column_name{"arrived"},1);
	
	//set into available list
	auto q=query_to<Stock_id,unsigned>(
		db,
		"SELECT stock,quantity"+current(id)
	);
	assert(q.size()==1);
	auto [stock,new_pcs]=q[0];
	auto old=available(db,stock);
	edit_single(db,stock,Column_name{"available"},old+new_pcs);
}

void set_arrived(DB db,Order_id id){
	auto q=query_to<Order_member_id>(
		db,
		"SELECT main "+current(Id_table{"order_member"})+
		"AND parent="+escape(id)
		+" AND arrived=0"
	);
	mapv([&](auto x){ return set_arrived(db,x); },q);
}

void show(ostream& o,DB db,Page_arrived const& p){
	set_arrived(db,p.order);
	redirect_page(o,"Arrived",p.redirect);
}

Order_id make(DB db,Page_order const& p){

	Order_id id=Order_id{new_item(db,Table_name{"order"})};
	
	run_cmd(
		db,
		"INSERT INTO order_info ("+
		join(
			",",
			mapf(
				[](auto x){ return "`"+x+"`"; },
				vector<Column_name>{
					Column_name{"main"},
					Column_name{"valid"},
					Column_name{"edit_user"},
					Column_name{"edit_date"},

					Column_name{"supplier"},
					Column_name{"cost"},
					Column_name{"who_paid"},
					Column_name{"destination"},
					Column_name{"expected_arrival"},

					Column_name{"submitted"}
				}
			)
		)+
		") VALUES ("+
		join(
			",",
			vector<string>{escape(id),"1",sql_escape(db,current_user()),"now()"}+
			mapf(
				[&](auto x){ return sql_escape(db,x); },
				vector<Column_value>{
					Column_value{p.supplier},
					Column_value{p.cost},
					Column_value{p.who_paid},
					Column_value{*p.destination},
					Column_value{*p.expected_arrival}
				}
			)+"now()"
		)+
		")"
	);
	return id;
}

void insert(DB db,Id_table const& table,map<Column_name,Column_value> m){
	run_cmd(
		db,
		"INSERT INTO `"+table+"_info` ("+
		join(
			",",
			mapf(
				[](auto x){ return "`"+x+"`"; },
				vector<Column_name>{
					Column_name{"valid"},
					Column_name{"edit_user"},
					Column_name{"edit_date"},
				}+keys(m)
			)
		)+
		") VALUES ("+
		join(
			",",
			vector<string>{"1",sql_escape(db,current_user()),"now()"}+
			mapf(
				[&](auto x){ return sql_escape(db,x); },
				values(m)
			)
		)+
		")"
	);
}

void make(DB db,Order_id order,Order_member_basic const& a){
	insert(
		db,
		Id_table{"order_member"},
		map<Column_name,Column_value>{
			{Column_name{"main"},order},
			{Column_name{"parent"},Column_value{order}},
			{Column_name{"stock"},Column_value{a.first}},
			{Column_name{"quantity"},Column_value{a.second}},
			{Column_name{"arrived"},Column_value{false}}
		}
	);
}

void show(ostream& o,DB db,Page_order const& p){
	{
		vector<tuple<string,string>> errors;
		#define X(NAME) if(!p.NAME) errors|=tuple<string,string>{""#NAME,"Unexpectedly null"};
		X(cost)
		X(destination)
		X(expected_arrival)
		#undef X
		if(!exists(db,p.supplier)){
			errors|=tuple<string,string>{"supplier","Does not exist"};
		}
		for(auto [stock,n]:p.members){
			if(!exists(db,stock)){
				errors|=tuple<string,string>{"stock","Does not exist"};
			}
		}
		if(errors.size()){
			page(
				o,
				"Make order",
				"Not creating order.  Errors:"+
				table({"Column","Error"},errors)
			);
			return;
		}
	}

	//make the order
	auto new_order=make(db,p);

	//put in each of the order members
	mapv([&](auto x){ make(db,new_order,x); },p.members);

	//redirect to the page for the new order that was just created
	redirect_page(o,"Order",page_normal(new_order));
}
