#include "status.h"
#include<functional>
#include "extra.h"
#include "main.h"
#include "get_name.h"
#include "page.h"
#include "orders.h"
#include "latest.h"
#include "type.h"
#include "variant_util.h"
#include "map_util.h"
#include "set_util.h"

using namespace std;

//start utility functions

template<typename K,typename V>
string to_table(DB db,Page_normal const& page,string const& a,string const& b,map<K,V> const& m){
	return to_table(db,page,a,b,to_vec(m));
}

template<typename A,typename B>
multiset<A> get_alt(multiset<B> const& a){
	multiset<A> r;
	for(auto elem:a){
		if(holds_alternative<A>(elem)){
			r|=get<A>(elem);
		}
	}
	return r;
}

template<typename A,typename B>
string to_table(vector<pair<A,B>> const& a){
	stringstream ss;
	ss<<"<table border>";
	for(auto [c1,c2]:a){
		ss<<tr(td(as_string(c1))+td(as_string(c2)));
	}
	ss<<"</table>";
	return ss.str();
}

string show(DB,string s){
	return s;
}

//This is obviously slow.
#define X(A,B,C) bool order(DB db,B##_id a,B##_id b,Sort_order ord){\
	auto as=get_name(db,a);\
	auto bs=get_name(db,b);\
	if(ord==Sort_order::ASC){\
		return as<bs;\
	}\
	return bs<as;\
}
TABLES(X)
#undef X

template<typename T>
bool order(DB db,T const& a,T const& b,Sort_order ord){
	(void)db;
	if(ord==Sort_order::ASC){
		return a<b;
	}
	return b<a;
}

template<typename A,typename B,typename C>
vector<tuple<A,B,C>> sort(
	DB db,
	optional<Sort> const& s,
	vector<tuple<A,B,C>> v,
	std::vector<std::string> const& labels
){
	if(!s) return v;
	int x=[&]()->int{
		for(auto [i,label]:enumerate(labels)){
			if(label==s->first){
				return i;
			}
		}
		return -1;
	}();
	if(x==-1) return v;
	std::sort(
		begin(v),
		end(v),
		[&](auto a,auto b){
			if(x==0) return order(db,get<0>(a),get<0>(b),s->second);
			if(x==1) return order(db,get<1>(a),get<1>(b),s->second);
			if(x==2) return order(db,get<2>(a),get<2>(b),s->second);
			//if(x==3) return order(db,get<3>(a),get<3>(b),s->second);
			assert(0);
		}
	);
	return v;
}

template<typename A,typename B>
vector<pair<A,B>> sort(
	DB db,
	optional<Sort> const& s,
	vector<pair<A,B>> v,
	std::string const& a,
	std::string const& b
){
	if(!s) return v;
	int col=-1;
	if(a==s->first){
		col=0;
	}else if(b==s->first){
		col=1;
	}
	if(col==-1) return v;
	sort(
		begin(v),
		end(v),
		[&](auto a,auto b){
			if(col==0) return order(db,get<0>(a),get<0>(b),s->second);
			return order(db,get<1>(a),get<1>(b),s->second);
		}
	);
	return v;
}

template<typename A,typename B>
string to_table(DB db,Page_normal const& page,string a,string b,std::vector<std::pair<A,B>> const& v){
	stringstream o;
	o<<"<table border>";
	o<<tr(
		th(a+sort_links(page,Column_name{a}))+
		th(b+sort_links(page,Column_name{b}))
	);
	auto sorted=sort(db,sort(page),v,a,b);
	for(auto r:sorted){
		o<<tr(join(mapf(
			[&](auto x){ return td(show(db,x)); },
			r
		)));
	}
	o<<"</table>";
	return o.str();
}

template<typename A,typename B,typename C>
string to_table(DB db,Page_normal const& page,string a,string b,string c,std::vector<tuple<A,B,C>> const& v){
	stringstream o;
	o<<"<table border>";
	//o<<tr(th(a)+th(b)+th(c));
	o<<tr(join(mapf(
		[&](auto x){ return th(x+sort_links(page,Column_name{x})); },
		vector<string>{a,b,c}
	)));
	auto sorted=sort(db,sort(page),v,vector{a,b,c});
	for(auto r:sorted){
		o<<tr(join(mapf(
			[&](auto x){ return td(show(db,x)); },
			r
		)));
	}
	o<<"</table>";
	return o.str();
}

template<typename A,typename B,typename C,typename D>
string to_table(
	DB db,
	string a,string b,string c,string d,
	std::vector<tuple<A,B,C,D>> const& v
){
	stringstream o;
	o<<"<table border>";
	o<<tr(th(a)+th(b)+th(c)+th(d));
	for(auto r:v){
		o<<tr(join(mapf(
			[&](auto x){ return td(show(db,x)); },
			r
		)));
	}
	o<<"</table>";
	return o.str();
}

//end utility functions

//start status functions

vector<Part_design_id> part_designs(DB db){
	return query_to<Part_design_id>(
		db,
		"SELECT main "+current(Id_table{"part_design"})
	);
}

map<Stock_id,unsigned> available_stock(DB db){
	auto q=query_to<Stock_id,unsigned>(
		db,
		"SELECT main,available "+current(Id_table{"stock"})
	);
	return to_map(q);
}

std::map<Part_design_id,std::pair<std::string,unsigned>> available_parts(DB db){
	auto q=query_to<Part_design_id,unsigned>(
		db,
		"SELECT part_design,COUNT(*) "+current(Id_table{"part"})+
		"AND part_design IS NOT NULL "
		"AND parent IS NULL "
		"GROUP BY part_design"
	);
	map<Part_design_id,pair<string,unsigned>> r;
	for(auto [part_design,c]:q){
		r[part_design]=make_pair(get_name(db,part_design),c);
	}
	return r;
}

std::map<Assembly_design_id,std::string> assembly_design_names(DB db){
	//almost the same as part_design_names()
	auto q=query_to<Assembly_design_id,string,string>(
		db,
		"SELECT main,part_number,name "+current(Id_table{"assembly_design"})
	);
	map<Assembly_design_id,std::string> r;
	for(auto [main,part_num,name]:q){
		r[main]=part_num+" "+name;
	}
	return r;
}

map<Assembly_design_id,std::pair<std::string,unsigned>> available_assemblies(DB db){
	auto q=query_to<Assembly_design_id,unsigned>(
		db,
		"SELECT assembly_design,COUNT(*) "+current(Id_table{"assembly"})+
		"AND parent IS NULL "
		"GROUP BY assembly_design"
	);
	auto names=assembly_design_names(db);
	map<Assembly_design_id,pair<string,unsigned>> r;
	for(auto [assembly_design,c]:q){
		r[assembly_design]=make_pair(names[assembly_design],c);
	}
	return r;
}

//end status functions

//start scheduling info

struct Make_order{
	Stock_id stock;
};

ostream& operator<<(ostream& o,Make_order a){
	return o<<a.stock;
}

bool operator<(Make_order const& a,Make_order const& b){
	return a.stock<b.stock;
}

struct Assemble{
	//what you do once you have all of the parts
	Assembly_design_id type;
};

ostream& operator<<(std::ostream& o,Assemble const& a){
	return o<<a.type;
}

bool operator<(Assemble const& a,Assemble const& b){
	return a.type<b.type;
}

using Work_item=std::variant<
	Make_order,
	Manufacture_step_id,
	Assemble
>;

string show_item(DB db,Make_order m){
	return a(m.stock,get_name(db,Id_table{"stock"},m.stock.data,0));
}

string show_item(DB db,Manufacture_step_id id){
	return a(id,get_name(db,Id_table{"manufacture_step"},id.data,0));
}

string show_item(DB db,Assemble m){
	return a(m.type,get_name(db,Id_table{"assembly_design"},m.type.data,0));
}

string show_item(DB db,Work_item const& a){
	return visit([&](auto x){ return show_item(db,x); },a);
}

vector<pair<Machine_id,Decimal>> manufacture_steps(DB db,Part_design_id const& pd){
	return query_to<Machine_id,Decimal>(
		db,
		"SELECT machine,time "+current(Id_table{"manufacture_step"})+
		"AND part_design="+as_string(pd.data)+" "
		"AND machine IS NOT NULL AND time IS NOT NULL"
	);
}

vector<Manufacture_step_id> manufacture_step_ids(DB db,Part_design_id a){
	return query_to<Manufacture_step_id>(
		db,
		"SELECT main "+
		current(Id_table{"manufacture_step"})+
		" AND part_design="+as_string(a.data)+" ORDER BY id"
	);
}

optional<Stock_id> stock(DB db,Part_design_id id){
	auto q=query_to<optional<Stock_id>>(
		db,
		/*"SELECT stock FROM part_design_info WHERE "
		"(id) IN ("
			"SELECT MAX(id) FROM part_design_info "
			"WHERE main="+as_string(id.data)
		+") AND valid AND stock IS NOT NULL"*/
		"SELECT stock FROM part_design_current WHERE main="+as_string(id.data)
	);
	if(q.empty()) return std::nullopt;
	assert(q.size()==1);
	return q[0];
}

#define X(A,B,C) string show(DB db,B##_id id){\
	return a(id,get_name(db,Id_table{""#A},id.data,0));\
}
TABLES(X)
#undef X

string show(DB,Date x){ return as_string(x); }

template<typename T>
string show(DB db,set<T> const& a){
	return join(" ",mapf(
		[&](auto x){ return show(db,x); },
		a
	));
}

string show(DB,Decimal a){ return as_string(a); }

template<typename A,typename B>
string fancy_table(
	DB db,
	string la,string lb,
	std::vector<std::pair<A,B>> const& v
){
	stringstream ss;
	ss<<"<table border>";
	ss<<tr(th(la)+th(lb))<<"\n";
	for(auto [a,b]:v){
		ss<<tr(
			td(show(db,a))+
			td(show(db,b))
		);
	}
	ss<<"</table>";
	return ss.str();
}

vector<Manufacture_step_id> mfg(DB db,Part_design_id id){
	return query_to<Manufacture_step_id>(
		db,
		"SELECT main "+current(Id_table{"manufacture_step"})+
		"AND part_design="+as_string(id.data)
	);
}

vector<Work_item> needed(DB db,Part_design_id id){
	vector<Work_item> r;
	{
		auto s=stock(db,id);
		if(s){
			r|=Work_item{Make_order{*s}};
		}
	}
	r|=mapf([](auto x){ return Work_item(x); },mfg(db,id));
	return r;
}

multiset<Part_design_id> parts(DB db,Assembly_design_id id){
	auto q=query_to<Part_design_id,unsigned>(
		db,
		"SELECT child,quantity "+current(Id_table{"assembly_design_part"})+
		"AND parent="+as_string(id.data)
	);
	multiset<Part_design_id> r;
	for(auto [id,n]:q){
		for(auto _:range(n)){
			(void)_;
			r|=id;
		}
	}
	return r;
}

vector<Assembly_design_id> subassemblies(DB db,Assembly_design_id id){
	(void)db;
	(void)id;
	return query_to<Assembly_design_id>(
		db,
		"SELECT child FROM assembly_design_assembly_info WHERE "
		"(id) IN ("
			"SELECT MAX(id) FROM assembly_design_assembly_info WHERE "
			"parent="+as_string(id.data)
		+") AND valid"
	);
}

vector<Work_item> needed1(DB db,Assembly_design_id id,set<Assembly_design_id> s){
	if(s.count(id)){
		throw "Dependency loop in assembly designs: "+as_string(s);
	}
	/*if(s.count(id)){
		PRINT(s);
	}
	assert(s.count(id)==0);*/

	std::vector<Work_item> r;
	for(auto part:parts(db,id)){
		r|=needed(db,part);
	}

	auto subs=subassemblies(db,id);
	//PRINT(id);
	//PRINT(subs);
	for(auto sub:subs){
		r|=needed1(db,sub,s|id);
	}

	//add the assembly time for this itself.
	r|=Work_item{Assemble{id}};
	return r;
}

vector<Work_item> needed(DB db,Assembly_design_id id){
	return needed1(db,id,{});
}

string part_buildability(DB db,Part_design_id id){
	auto s=stock(db,id);
	auto m=manufacture_steps(db,id);
	return h3(a(id,get_name(db,id)))+
		p("Stock: "+a(s,get_name(db,s)))+
		fancy_table(db,"Machine","Time",m);
}

string part_buildability(DB db){
	auto m=mapf([&](auto x){ return part_buildability(db,x); },part_designs(db));
	return join(m);
}



string assembly_buildability(DB db,Assembly_design_id id){
	//what would we have to do if we had nothing in stock
	//eventually, will want to create one of these where can assume have some things partially built already
	auto x=needed(db,id);
	//return as_string(to_multiset(x));
	stringstream o;
	o<<h3(a(id,get_name(db,id)));
	for(auto elem:to_set(x)){
		//o<<p(as_string(elem));
		o<<p(show_item(db,elem));
	}
	return o.str();
}

vector<Assembly_design_id> assembly_designs(DB db){
	return query_to<Assembly_design_id>(
		db,
		"SELECT main "+current(Id_table{"assembly_design"})
	);
}

string assembly_buildability(DB db){
	return join(mapf(
		[&](auto x){
			return assembly_buildability(db,x);
		},
		assembly_designs(db)
	));
}

template<typename K>
string to_table(map<K,pair<string,unsigned int>> const& data){
	stringstream o;
	o<<"<table border>";
	o<<tr(th("Item")+th("Quantity"));
	for(auto [id,v]:data){
		auto [name,qty]=v;
		o<<tr(td(a(id,name))+td(to_string(qty)));
	}
	o<<"</table>";
	return o.str();
}

/*set<Machine_id> at_location(DB db,Loction_id id){
	auto q=query_to<Machine_id>(
		db,
		"SELECT main FROM machine_info WHERE 
}*/

std::map<Location_id,std::set<Machine_id>> machines(DB db){
	auto q=query_to<Location_id,Machine_id>(
		db,
		"SELECT location,main "+current(Id_table{"machine"})
	);
	map<Location_id,set<Machine_id>> r;
	for(auto [k,v]:q){
		r[k]|=v;
	}
	return r;
}

using Time=Decimal;
using Meeting_t=tuple<Meeting_id,Date,Time,set<Machine_id>>;

vector<Meeting_t> upcoming_meetings(DB db){
	auto m=machines(db);
	auto q=query_to<Meeting_id,Date,Time,Location_id>(
		db,
		"SELECT main,date,length,location "+current(Id_table{"meeting"})+
		" ORDER BY date" //TODO: Add in a where here that actually looks at the dates of the meetings to see that we're only looking at upcoming ones.
	);
	return mapf(
		[&](auto x){
			return make_tuple(
				get<0>(x),
				get<1>(x),
				get<2>(x),
				m[get<3>(x)]
			);
		},
		q
	);
}

string to_table(DB db,std::vector<Meeting_t> const& v){
	/*stringstream o;
	o<<"<table border>";
	o<<tr(th("Meeting")+th("Length")+th("Machines"));
	for(auto r:v){
		o<<tr(join(mapf(
			[&](auto x){ return td(show(db,x)); },
			r
		)));
	}
	o<<"</table>";
	return o.str();*/
	(void)db; (void)v;
	return to_table(db,"Meeting","Date","Length","Machines",v);
}

template<typename K,typename V>
string to_table(DB db,map<K,V> const& v){
	stringstream o;
	o<<"<table border>";
	for(auto r:v){
		o<<tr(join(mapf(
			[&](auto x){ return td(show(db,x)); },
			r
		)));
	}
	o<<"</table>";
	return o.str();
}

map<Stock_id,unsigned> min_leads(DB db){
	auto q=query_to<Stock_id,unsigned>(
		db,
		"SELECT stock,MIN(lead_time) "+current(Id_table{"availability"})+
		"AND lead_time IS NOT NULL GROUP BY stock"
	);
	map<Stock_id,unsigned> r;
	for(auto [a,b]:q){
		r[a]=b;
	}
	return r;
}

vector<tuple<Stock_id,unsigned,Date/*of expected arrival*/>> on_order(DB db){
	return query_to<Stock_id,unsigned,Date>(
		db,
		"SELECT "
			"order_member_info.stock,"
			"order_member_info.quantity, "
			"expected_arrival "
		"FROM order_member_info,order_info "
		"WHERE (order_member_info.id) IN ("
			"SELECT MAX(order_member_info.id) FROM order_member_info"
			" GROUP BY order_member_info.main"
		")"
		" AND arrived=0"
		" AND order_member_info.valid "
		" AND (order_info.id) IN ("
			"SELECT MAX(order_info.id) FROM order_info GROUP BY order_info.main"
		")"
		" AND order_info.valid "
		" AND order_member_info.parent=order_info.main"
	);
}

std::optional<Stock_id> stock_needed(DB db,Part_design_id a){
	auto q=query_to<Stock_id>(
		db,
		"SELECT stock "+current(Id_table{"part_design"})+
		"AND main="+as_string(a.data)
		+" AND stock IS NOT NULL"
	);
	if(q.empty()) return std::nullopt;
	assert(q.size()==1);
	return q[0];
}

bool operator<(Part_n const& a,Part_n const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	PART_N_ITEMS(X)
	#undef X
	return 0;
}

std::ostream& operator<<(std::ostream& o,Part_n const& a){
	o<<"Part_n( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	PART_N_ITEMS(X)
	#undef X
	return o<<")";
}

bool operator<(Assembly_n const&,Assembly_n const&){
	nyi
}

std::ostream& operator<<(std::ostream& o,Assembly_n const& a){
	o<<"Assembly_n(";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	ASSEMBLY_N_ITEMS(X)
	#undef X
	return o<<")";
}

using Leaf=std::variant<
	Stock_id,
	Manufacture_step_id, //might want to put a time in here too.
	pair<Assembly_design_id,Time>
>;

multiset<Leaf> leaves(Part_n const& a){
	if(a.stock) return {Leaf{*a.stock}};

	if(a.ops.size()==0) return {};

	if(a.ops.size()){
		return std::multiset<Leaf>{a.ops[0]};
	}

	PRINT(a);
	nyi
}

multiset<Leaf> leaves(Assembly_n const& a){
	if(a.stock.empty() && a.part.empty() && a.subs.empty()){
		if(a.time>0){
			return {make_pair(a.item,a.time)};
		}
		return {};
	}
	return to_multiset(MAP(Leaf,a.stock)) | 
		flatten(MAP(leaves,a.part)) | 
		flatten(MAP(leaves,a.subs));
}

using T1=tuple<Assembly_design_id,Assembly_state,optional<Time>>;

string recent(string table){
	return "(id) IN (SELECT MAX(id) FROM "+table+" GROUP BY main)";
}

string current(Id_table const& table){
	return " FROM "+table+"_current WHERE 1 ";
	//return " FROM "+table+"_info WHERE "+recent(table+"_info")+" AND valid ";
}

std::vector<Assembly_design_id> asm_order(DB db){
	map<Assembly_design_id,std::set<Assembly_design_id>> m;
	//look up all the dependencies
	auto q=query_to<Assembly_design_id,Assembly_design_id>(db,
		"SELECT parent,child "+current(Id_table{"assembly_design_assembly"})
	);
	for(auto [parent,child]:q){
		m[parent]|=child;
	}
	std::function<bool(Assembly_design_id)> ready;
	ready=[&](auto x){
		for(auto elem:m[x]){
			if(!ready(elem)) return 0;
		}
		return 1;
	};
	std::vector<Assembly_design_id> r;
	std::set<Assembly_design_id> queue=keys(m);

	auto attempt=[&](){
		for(auto elem:queue){
			if(ready(elem)){
				r|=elem;
				queue-=elem;
				return 1;
			}
		}
		return 0;
	};

	while(attempt());

	if(queue.size()){
		throw "Loop found.  Giving up.";
	}

	return r;
}

std::vector<T1> topo_sort(DB db,std::vector<T1> const& v){
	//make items with assemblies that are lower-level go first
	auto m=to_map(swap_pairs(enumerate(asm_order(db))));
	return sort_by(v,[&](auto x){ return m[get<0>(x)]; });
}

multiset<Stock_id> stock(DB db,Assembly_design_id a){
	auto q=query_to<Stock_id,unsigned>(db,
		"SELECT child,quantity "+current(Id_table{"assembly_design_stock"})+
		" AND parent="+as_string(a.data)
	);
	multiset<Stock_id> r;
	for(auto [id,qty]:q){
		for(auto _:range(qty)){
			(void)_;
			r|=id;
		}
	}
	return r;
}

multiset<Assembly_design_id> assemblies(DB db,Assembly_design_id a){
	//it might be better if this was known as "subassemblies".
	return to_multiset(query_to<Assembly_design_id>(db,
		"SELECT child "+current(Id_table{"assembly_design_assembly"})+
		" AND parent="+as_string(a.data)
	));
}

optional<unsigned> min_lead_time(DB db,Stock_id id){
	//returns NULL if no supplier found.
	auto a=min_leads(db);
	auto f=a.find(id);
	if(f==a.end()) return std::nullopt;
	return f->second;
}

template<typename T>
string show(DB db,multiset<T> const& a){
	return join(mapf(
		[&](auto x){ return show(db,x); },
		count(a)
	));
}

template<typename T>
string show(DB db,std::optional<T> const& a){
	if(a) return show(db,*a);
	return "NULL";
}

template<typename T>
string show(DB db,vector<T> const& a){
	return join(mapf(
		[&](auto x){ return show(db,x); },
		a
	));
}

template<typename A,typename B>
string show(DB db,pair<A,B> const& p){
	return "("+show(db,p.first)+" "+show(db,p.second)+")";
}

string show(DB db,Part_n a){
	return table(tr(
		#define X(A,B) td(show(db,a.B))+
		PART_N_ITEMS(X)
		#undef X
		""
	));
}

string show(DB db,Assembly_n a){
	return tag("table border",
		tr(
			#define X(A,B) td(""#B)+
			ASSEMBLY_N_ITEMS(X)
			#undef X
			""
		)+
		tr(
			#define X(A,B) td(show(db,a.B))+
			ASSEMBLY_N_ITEMS(X)
			#undef X
			""
		)
	);
}

#define REMOVE_RESULTS(X)\
	X(UNUSED)\
	X(USED)\
	X(DONE)

enum class Remove_result{
	#define X(A) A,
	REMOVE_RESULTS(X)
	#undef X
};

Remove_result remove_done(Part_n&,Stock_id const&){
	nyi
}

Remove_result remove_done(Part_n& a,Manufacture_step_id const& b){
	if(a.stock) return Remove_result::UNUSED;
	for(auto it=begin(a.ops);it!=a.ops.end();++it){
		if(*it==b){
			a.ops.erase(it);
			if(a.ops.empty()){
				return Remove_result::DONE;
			}
			return Remove_result::USED;
		}
	}
	return Remove_result::UNUSED;
}

Remove_result remove_done(Assembly_n& a,Manufacture_step_id const& b){
	(void)b;
	for(auto it=begin(a.subs);it!=a.subs.end();it++){
		auto x=remove_done(*it,b);
		switch(x){
			case Remove_result::UNUSED:
				break;
			case Remove_result::USED:
				return Remove_result::USED;
			case Remove_result::DONE:
				a.subs.erase(it);
				return Remove_result::USED;
			default: assert(0);
		}
	}
	for(auto it=begin(a.part);it!=a.part.end();it++){
		auto x=remove_done(*it,b);
		switch(x){
			case Remove_result::UNUSED:
				break;
			case Remove_result::USED:
				return Remove_result::USED;
			case Remove_result::DONE:
				a.part.erase(it);
				return Remove_result::USED;
			default:
				assert(0);
		}
	}
	return Remove_result::UNUSED;
}

Remove_result remove_done(Assembly_n& ,Stock_id const&){
	nyi
}

Remove_result remove_done(Assembly_n& a,pair<Assembly_design_id,Time> const& b){
	for(auto it=begin(a.subs);it!=a.subs.end();++it){
		auto x=remove_done(*it,b);
		switch(x){
			case Remove_result::UNUSED:
				break;
			case Remove_result::USED:
				return Remove_result::USED;
			case Remove_result::DONE:
				a.subs.erase(it);
				return Remove_result::USED;
			default:
				assert(0);
		}
	}
	if(!a.stock.empty() || !a.part.empty() || !a.subs.empty()){
		//not used because prereqs not ready.
		return Remove_result::UNUSED;
	}
	if(b.first==a.item){
		//could check time here to choose between USED or DONE
		return Remove_result::DONE;
	}
	//not a match
	return Remove_result::UNUSED;
}

Remove_result remove_done(Assembly_n& a,Leaf const& b){
	return visit([&](auto x){ return remove_done(a,x); },b);
}

void done(std::vector<Assembly_n>& list,Leaf const& leaf){
	for(auto it=begin(list);it!=list.end();++it){
		auto r=remove_done(*it,leaf);
		switch(r){
			case Remove_result::UNUSED:
				break;
			case Remove_result::USED:
				return;
			case Remove_result::DONE:
				list.erase(it);
				return;
			default:
				assert(0);
		}
	}
	//at some point, will want to check that we don't get here.
}

All_work all_work(DB db){
	auto stock_on_hand=[&](){
		multiset<Stock_id> r;
		for(auto [k,v]:available_stock(db)){
			auto qty=v;
			for(auto _:range(qty)){
				(void)_;
				r|=k;
			}
		}
		return r;
	}();

	auto get_stock=[&](Stock_id a)->optional<Stock_id>{
		if(stock_on_hand.count(a)){
			stock_on_hand-=a;
			return std::nullopt;
		}
		return a;
	};

	auto q=query_to<Part_design_id,Part_state,optional<Manufacture_step_id>>(
		db,
		"SELECT part_design,state,manufacture_step "+current(Id_table{"part"})+
		"AND parent IS NULL"
	);
	using Part_build_state=pair<Part_state,optional<Manufacture_step_id>>;
	map<Part_design_id,multiset<Part_build_state>> parts_in_progress;

	multiset<Part_design_id> parts_free;

	for(auto [design,state,step]:q){
		switch(state){
			case Part_state::get_stock:{
				auto n=stock_needed(db,design);
				auto ops=manufacture_steps(db,design);
				if(!n){
					if(ops.empty()){
						parts_free|=design;
					}else{
						parts_in_progress[design]|=make_pair(state,step);
					}
				}else{
					parts_in_progress[design]|=make_pair(state,step);
				}
				break;
			}
			case Part_state::manufacture:
				parts_in_progress[design]|=make_pair(state,step);
				break;
			case Part_state::done:
				parts_free|=design;
				break;
			default:
				assert(0);
		}
	}

	auto get_part=[&](Part_design_id id)->optional<Part_n>{
		//If there is one done, use it
		if(parts_free.count(id)){
			parts_free-=id;
			return std::nullopt;
		}
		//If there is one in progress, take it
		auto f=parts_in_progress.find(id);
		if(f!=parts_in_progress.end()){
			auto chosen=*begin(f->second);
			f->second-=chosen;
			switch(chosen.first){
				case Part_state::get_stock:{
					auto n=stock_needed(db,id);
					auto ops=manufacture_step_ids(db,id);
					if(!n){
						if(ops.size()==0){
							return std::nullopt;
						}
					}
					return Part_n{id,n,ops};
				}
				case Part_state::manufacture:{
					auto ops=manufacture_step_ids(db,id);
					//filter the list to only the ones remaining
					if(chosen.second==std::nullopt){
						return Part_n{id,std::nullopt,ops};
					}
					//PRINT(chosen);
					//nyi
					return Part_n{id,std::nullopt,after(*chosen.second,ops)};
				}
				case Part_state::done:
					nyi
				default:
					assert(0);
			}
			nyi
		}
		//create a new one here.
		return Part_n{id,stock(db,id),manufacture_step_ids(db,id)};
	};

	auto q1=query_to<Assembly_design_id,Assembly_state,optional<Time>>(db,
		"SELECT assembly_design,state,time_left "+current(Id_table("assembly"))+
		"AND state IS NOT NULL"
	);

	multiset<Assembly_design_id> asm_free;

	std::function<std::optional<Assembly_n>(Assembly_design_id)> get_asm;
	get_asm=[&](Assembly_design_id design)->optional<Assembly_n>{
		if(asm_free.count(design)){
			asm_free-=design;
			return {};
		}
		auto missing_stock=non_null(mapf(get_stock,stock(db,design)));
		auto missing_parts=non_null(mapf(get_part,parts(db,design)));
		auto missing_asm=non_null(mapf(get_asm,assemblies(db,design)));
		return Assembly_n{
			design,
			to_multiset(missing_stock),
			missing_parts,
			missing_asm,
			100 //fixme; obviously should look up the default total
		};
	};

	vector<Assembly_n> asm_needed;
	for(auto [design,state,time_left]:topo_sort(db,q1)){
		switch(state){
			case Assembly_state::in_design:
				//if still in design, then treat the same as "parts"
			case Assembly_state::parts:{
				auto missing_stock=non_null(mapf(get_stock,stock(db,design)));
				auto missing_parts=non_null(mapf(get_part,parts(db,design)));
				auto missing_asm=non_null(mapf(get_asm,assemblies(db,design)));
				asm_needed|=Assembly_n{
					design,
					to_multiset(missing_stock),
					missing_parts,
					missing_asm,
					time_left?*time_left:0
				};
				break;
			}
			case Assembly_state::assembly:
				//assume that all of the parts have been claimed.
				asm_needed|=Assembly_n{
					design,
					{},
					{},
					{},
					time_left?*time_left:0
				};
				break;
			case Assembly_state::done:
				asm_free|=design;
				break;
			default:
				assert(0);
		}
	}

	return {parts_free,asm_needed,stock_on_hand,asm_free};
}

string schedule(DB db,Page_normal page){
	auto x=all_work(db);
	auto parts_free=x.parts_free;
	auto asm_needed=x.asm_needed;
	auto stock_on_hand=x.stock_on_hand;
	auto asm_free=x.asm_free;

	map<Date,multiset<Stock_id>> on_order;

	auto todo=flatten(MAP(leaves,asm_needed));
	set<Stock_id> missing_supplier;
	for(Stock_id a:get_alt<Stock_id>(todo)){
		auto t=min_lead_time(db,a);
		if(t){
			on_order[now()+*t]|=a;
		}else{
			missing_supplier|=a;
			//if no way known to buy it, then for now just don't.
		}
	}

	auto meetings=upcoming_meetings(db);

	for(auto meeting:meetings){
		//PRINT(meeting);
		auto todo=flatten(MAP(leaves,asm_needed));
		for(auto x:todo){
			if(holds_alternative<Stock_id>(x)){
				//do nothing
			}else if(holds_alternative<pair<Assembly_design_id,Time>>(x)){
				auto a=get<pair<Assembly_design_id,Time>>(x);
				done(asm_needed,a);
			}else if(holds_alternative<Manufacture_step_id>(x)){
				auto a=get<Manufacture_step_id>(x);
				done(asm_needed,a);
			}else{
				PRINT(x);
				nyi
			}
		}
	}

	stringstream o;
	auto print_lines=[&](auto x){
		for(auto elem:x){
			o<<show(db,elem)<<"<br>\n";
		}
	};

	o<<h3("Missing supplier");
	print_lines(missing_supplier);

	o<<h3("Stock leftover:");
	o<<to_table(db,page,"Stock","Quantity",count(stock_on_hand));
	o<<"\n";

	o<<h3("Parts on hand:");
	o<<to_table(db,page,"Part","Quantity",count(parts_free));
	o<<"\n";

	o<<h3("Asm free:");
	print_lines(asm_free);
	o<<"\n";

	o<<h3("Asm_needed:");
	print_lines(asm_needed);
	o<<"\n";

	return o.str();
}

/*vector<pair<Stock_id,unsigned>> wishlist(DB db){
	return query_to<Stock_id,unsigned>(db,
		"SELECT main,wishlist "+current(Id_table{"stock"})+
		"AND wishlist>0"
	);
}*/

void show(std::ostream& o,DB db,Status const& status){
	(void)status;
	{
		auto c=loop_check(db);
		if(c){
			page(o,"Status","Not calculating status due to data error."+*c);
			return;
		}
	}
	/*
	things that would be interesting to show:
	1) what is our current inventory of stock 
	2) what is our stock of finished parts? That are not installed in something already
	3) what is our stock of finished assemblies?
	4) for any given part, do we have the stock to be able to build it? -> is it waiting for parts or something?
	5) for any given assembly, what do we have to do in order to finish it?
	6) For a part design, what do we need to do to get one?
	7) For a given assembly design, what do we need to get one?
	*/

	auto stock=available_stock(db);
	auto stock_on_order=on_order(db);
	auto parts=available_parts(db);
	auto assemblies=available_assemblies(db);
	auto part_buildability1=part_buildability(db);
	auto meetings=upcoming_meetings(db);

	/*Strategy of how to go about scheduling things:
	-go with existing assuptions about how many projects can happen at once

	-result will be a list of things to do on each machine each night and a list of things that cannot be completed

	how to represent the status of stock:
	-expected arrival date
	status of a part:
	-does not exist yet (not represented if not materials)
	-have raw material
	-number of manufacturing steps completed (or remaining?)
	-amount of time remaining on the current step
	how to represent the status of an assembly:
	-set of parts that has been reserved
	-if full set, then time spent so far on the assembly
	*/
	page(
		o,
		"Status",
		//as_string(status)+
		h2("Stock on hand")+
		to_table(db,status,"Item","Quantity",stock)+
		h2("Stock on order")+
		a(Orders{},"More details")+
		to_table(db,status,"Item","Quantity","Expected Arrival",stock_on_order)+
		h2("Stock lead times")+
		to_table(db,status,"Stock","Minimum lead time (days)",min_leads(db))+
		h2("Wishlist")+
		to_table(db,status,"Stock","Quantity",count(wishlist(db)))+
		h2("Finished free parts")+
		to_table(db,status,"Item","Quantity",parts)+
		h2("Free assemblies")+
		to_table(assemblies)+
		h2("Part Buildability")+
		part_buildability1+
		h2("Assembly Buildability")+
		assembly_buildability(db)+
		h2(a(Id_table{"machine"},"Machines"))+
		to_table(db,status,"Location","Machines",machines(db))+
		h2(a(Id_table{"meeting"},"Meetings"))+
		to_table(db,meetings)+
		h2("Schedule part build")+
		schedule(db,status)
		+h2("Schedule asm build")+
		"TODO"
	);
}
