#include "db.h"
#include "status.h"
#include "set_util.h"
#include "map_util.h"

using namespace std;

//start generic code

bool prefix(string const& a,optional<string> const& b){
	if(!b) return 0;
	return prefix(a,*b);
}

template<typename A,typename B>
vector<A> firsts(set<pair<A,B>> a){
	return firsts(to_vec(a));
}

template<typename A,typename B>
vector<B> seconds(set<pair<A,B>> a){
	return seconds(to_vec(a));
}

template<typename T>
set<T> non_null(set<optional<T>> const& a){
	return to_set(non_null(to_vec(a)));
}

template<typename T>
set<T> operator-(set<T> a,set<optional<T>> b){
	return a-non_null(b);
}

template<typename Func,typename T>
set<T> filter(Func f,set<T> s){
	return to_set(filter(f,to_vec(s)));
}

template<typename K,typename V>
optional<V> get(map<K,V> const& a,optional<K> const& b){
	if(!b) return std::nullopt;
	auto f=a.find(*b);
	if(f==a.end()){
		return std::nullopt;
	}
	return f->second;
}

template<typename K,typename V>
optional<V> get(map<K,V> const& a,K const& b){
	auto f=a.find(b);
	if(f==a.end()){
		return std::nullopt;
	}
	return f->second;
}

using Node=int;
using Edge=pair<Node,std::optional<Node>>;

vector<Node> topological_sort(set<Edge> v){
	//first is from, second is to
	//going to put in order where things that don't have anything pointing to them go first
	vector<Node> r;
	while(v.size()){
		auto to_remove=to_set(firsts(v))-to_set(seconds(v));
		if(to_remove.empty()){
			throw "Loop found";
		}
		v=filter([=](auto x){ return !to_remove.contains(x.first); },v);
		r|=to_remove;
	}
	return r;
}

//end generic code

//start program-specific code

Location_id unknown_location(DB db){
	auto q=query_to<Location_id>(
		db,
		"SELECT main"+current(Id_table{"location"})+" AND name=\"unknown\""
	);
	if(q.size()){
		return q[0];
	}
	auto n=Location_id{new_item(db,Table_name{"location"})};
	insert(
		db,
		"location_info",
		vector<pair<string,string>>{
			{"main",escape(n)},
			{"valid","1"},
			{"edit_user",escape("Migration_tool")},
			{"edit_date","now()"},
			{"name",escape("unknown")},
			{"notes",escape("Added as a placeholder as this did not exist in the previous system.")}
		}
	);
	return n;
}

Supplier_id unknown_supplier(DB db){
	auto q=query_to<Supplier_id>(
		db,
		"SELECT main"+current(Id_table{"supplier"})+"AND name=\"unknown\""
	);
	if(q.size()){
		return q[0];
	}
	auto n=Supplier_id{new_item(db,Table_name{"supplier"})};
	insert(
		db,
		"supplier_info",
		{
			{"main",escape(n)},
			{"valid","1"},
			{"edit_user",escape("Migration_tool")},
			{"edit_date","now()"},
			{"name",escape("unknown")}
		}
	);
	return n;
}

void migrate_meeting(DB db_old,DB db_new){
	//print_lines(read(db_old,"meeting_info"));

	auto q=query_to<int,string,string,string,Meeting_length,string,string>(
		db_old,
		"SELECT meeting_id,edit_user,edit_date,date,length,color,notes"
		" FROM meeting_info WHERE (id) IN ("
			"SELECT MAX(id) FROM meeting_info GROUP BY meeting_id"
		") AND valid"
		//"valid"
	);
	auto location=unknown_location(db_new);
	using P=std::pair<string,string>;
	for(auto [meeting_id,edit_user,edit_date,date,length,color,notes]:q){
		auto n=new_item(db_new,Table_name{"meeting"});
		//PRINT(edit_date);
		//PRINT(date);
		if(date=="0000-00-00"){
			continue;
		}
		Date edit_date1=parse((Date*)0,edit_date);
		Date date1=parse((Date*)0,date);
		insert(
			db_new,
			"meeting_info",
			vector<pair<string,string>>{
				P{"main",escape(n)},
				P{"valid","1"},
				P{"edit_user",escape(edit_user)},
				P{"edit_date",escape(edit_date1)},

				P{"date",escape(date1)},
				P{"length",escape(length)},
				P{"location",escape(location)},
				P{"color",escape(color)},
				P{"notes",escape(notes)}
			}
		);
	}
}

void add_assembly_design_assembly(
	DB db,
	string edit_user,
	Date edit_date,
	Assembly_design_id parent,
	Assembly_design_id child
){
	auto id=new_item(db,Table_name{"assembly_design_assembly"});
	insert(
		db,
		"assembly_design_assembly_info",
		vector<pair<string,string>>{
			{"main",escape(id)},
			{"valid","1"},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},

			{"parent",escape(parent)},
			{"child",escape(child)},
			{"quantity","1"}
		}
	);
}

Part_number_local adjust_part_number(std::optional<string> a){
	if(!a){
		return rand((Part_number_local*)0);
	}
	try{
		return parse((Part_number_local*)0,*a);
	}catch(...){
		return rand((Part_number_local*)0);
	}
}

Assembly_state adjust_state(std::optional<std::string> const& a){
	if(!a){
		return Assembly_state::done;
	}
	try{
		return parse((Assembly_state*)0,*a);
	}catch(...){
		nyi
	}
}

using Old_id=int;
using Migrate_result=std::pair<
	map<Old_id,Assembly_design_id>,
	map<Old_id,Assembly_id>
>;

Migrate_result migrate_subsystem(DB db_old,DB db_new){
	auto q=query_to<
		int,string,Date,optional<string>,
		optional<string>,optional<int>,optional<string>,optional<Time>,
		optional<string>,optional<bool>,optional<int>,optional<Decimal>
	>(
		db_old,
		"SELECT "
			"subsystem_id,edit_user,edit_date,name,"
			"prefix,parent,part_number,time,"
			"state,dni,priority,weight_override"
		" FROM subsystem_info"
		" WHERE valid AND (id) IN ("
			"SELECT MAX(id) FROM subsystem_info GROUP BY subsystem_id"
		")"
	);
	
	//need to do a topological sort here to put in all the highest-level things first so that when putting in the smaller things they can link to the higher level things.

	set<Edge> edges;
	for(auto x:q){
		auto here=get<0>(x);
		auto parent=get<5>(x);
		edges|=make_pair(here,parent);
	}
	PRINT(edges);
	auto t=topological_sort(edges);
	PRINT(t);
	auto index=to_map(swap_pairs(enumerate(t)));

	auto q2=reversed(sort_by(
		q,
		[=](auto x){
			auto id=get<0>(x);
			auto f=index.find(id);
			assert(f!=index.end());
			return f->second;
		}
	));

	map<Old_id,Assembly_design_id> new_assembly_design;
	map<Old_id,Assembly_id> new_assembly;
	for(auto [id,edit_user,edit_date,name,prefix,parent,part_number,time,state,dni,priority,weight_override]:q2){
		auto asm_design=Assembly_design_id{new_item(db_new,Table_name{"assembly_design"})};
		new_assembly_design[id]=asm_design;
		using P=std::pair<string,string>;
		insert(
			db_new,
			"assembly_design_info",
			{
				P{"main",escape(asm_design)},
				P{"valid","1"},
				P{"edit_user",escape(edit_user)},
				P{"edit_date",escape(edit_date)},

				P{"prefix",escape(prefix)},
				P{"part_number",escape(adjust_part_number(part_number))},
				P{"name",escape(name)},
				P{"in_design","0"},
				P{"design_time_left","0"},
				P{"assembly_time",escape(time)},
				P{"weight",escape(weight_override)}
			}
		);
		if(parent){
			auto f=new_assembly_design.find(*parent);
			if(f==new_assembly_design.end()){
				PRINT(parent);
				PRINT(new_assembly_design);
				nyi
			}
			add_assembly_design_assembly(db_new,edit_user,edit_date,new_assembly_design[*parent],asm_design);
		}

		auto assembly=Assembly_id{new_item(db_new,Table_name{"assembly"})};
		new_assembly[id]=assembly;
		insert(
			db_new,
			"assembly_info",
			{
				{"main",escape(assembly)},
				{"valid","1"},
				{"edit_user",escape(edit_user)},
				{"edit_date",escape(edit_date)},
				
				{"name",escape(name)},
				{"assembly_design",escape(asm_design)},
				{"parent",escape(get(new_assembly,parent))},
				//{"weight_override",escape(weight_override)},
				{"state",escape(adjust_state(state))},
				//{"time_left","0"},
				//{"notes",}
			}
		);
	}
	return make_pair(new_assembly_design,new_assembly);
}

void availability(
	DB db,
	User edit_user,Date edit_date,
	Stock_id stock,Supplier_id supplier,
	optional<string> part_number,
	Positive_decimal cost,
	optional<URL> url
){
	insert(
		db,
		"availability_info",
		{
			{"main",escape(new_item(db,Table_name{"availability"}))},
			{"valid","1"},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},

			{"stock",escape(stock)},
			{"supplier",escape(supplier)},
			{"part_number",escape(part_number)},
			{"cost",escape(cost)},
			//{"lead_time",},
			{"url",escape(url)}
		}
	);
}

Supplier_id supplier(DB db,User edit_user,Date edit_date,Nonempty_string name,optional<URL> url){
	auto q=query_to<Supplier_id>(
		db,
		"SELECT main "+current(Id_table{"supplier"})+"AND name="+escape(name)
	);
	if(q.size()){
		//if multiple, arbitrarily take the first one.
		return q[0];
	}
	auto n=Supplier_id{new_item(db,Table_name{"supplier"})};
	insert(
		db,
		"supplier_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"name",escape(name)},
			{"notes",escape("Created by conversion process")},
			{"url",escape(url)}
		}
	);
	return n;
}

Stock_category_id stock_category(
	DB db,
	User edit_user,Date edit_date,
	Nonempty_string name,Dimensions dimensions
){
	auto q=query_to<Stock_category_id>(
		db,
		"SELECT main"+current(Id_table{"stock_category"})+"AND name="+escape(name)
	);
	if(q.size()){
		return q[0];
	}

	auto n=Stock_category_id{new_item(db,Table_name{"stock_category"})};
	insert(
		db,
		"stock_category_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"name",escape(name)},
			{"dimensions",escape(dimensions)}
		}
	);
	return n;
}

Stock_id stock(
	DB db,
	User edit_user,
	Date edit_date,

	Nonempty_string name,
	Dimension length,
	Dimension width,
	Dimension thickness,
	optional<URL> part_link,
	optional<string> drawing_link,
	optional<string> cam_link
){
	auto dimensions=[&](){
		int i=0;
		if(length)i++;
		if(width)i++;
		if(thickness)i++;
		return Dimensions{i};
	}();
	auto cat=stock_category(db,edit_user,edit_date,name,dimensions);
	auto n=Stock_id{new_item(db,Table_name{"stock"})};
	insert(
		db,
		"stock_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"category",escape(cat)},
			{"length",escape(length)},
			{"width",escape(width)},
			{"thickness",escape(thickness)},
			{"available","1"},
			{
				"notes",
				[=](){
					stringstream ss;
					if(part_link){
						ss<<part_link<<" ";
					}
					if(drawing_link){
						ss<<drawing_link<<" ";
					}
					if(cam_link){
						ss<<cam_link<<" ";
					}
					return escape(ss.str());
				}()
			},
			//{"wishlist",},
		}
	);
	return n;
}

void availability(
	DB db,
	User edit_user,
	Date edit_date,

	Stock_id stock,
	Supplier_id supplier,
	optional<string> part_number,
	int cost,
	optional<URL> url
){
	auto q=query_single<unsigned>(
		db,
		"SELECT COUNT(*)"+current(Id_table{"availability"})+"AND "
	);
	if(q){
		//already in, so don't add it.
		return;
	}
	nyi
	auto n=new_item(db,Table_name{"availability"});
	using P=pair<string,string>;
	insert(
		db,
		"availability_info",
		vector<P>{
			P{"main",escape(n)},
			P{"edit_user",escape(edit_user)},
			P{"edit_date",escape(edit_date)},
			P{"valid","1"},

			P{"stock",escape(stock)},
			P{"supplier",escape(supplier)},
			P{"part_number",escape(part_number)},
			P{"cost",escape(cost)},
			//{"lead_time",},
			P{"url",escape(url)}
		}
	);
}

void stock_install(
	DB db,
	User edit_user,
	Date edit_date,
	Assembly_id assembly,
	Stock_id stock,
	Positive_int quantity
){
	auto n=new_item(db,Table_name{"stock_install"});
	insert(
		db,
		"stock_install_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"assembly",escape(assembly)},
			{"stock",escape(stock)},
			{"quantity",escape(quantity)}
		}
	);
}

void assembly_design_stock(
	DB db,
	User edit_user,
	Date edit_date,

	Assembly_design_id parent,
	Stock_id child,
	Positive_int quantity
){
	auto n=new_item(db,Table_name{"assembly_design_stock"});
	insert(
		db,
		"assembly_design_stock_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"parent",escape(parent)},
			{"child",escape(child)},
			{"quantity",escape(quantity)}
		}
	);
}

Part_design_id part_design(
	DB db,
	User edit_user,
	Date edit_date,
	string part_number_local,
	string name,
	Part_design_state state,
	optional<string> weight
){
	auto n=Part_design_id{new_item(db,Table_name{"part_design"})};
	insert(
		db,
		"part_design_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"part_number",escape(part_number_local)},
			{"name",escape(name)},
			{"state",escape(state)},
			//{"design_time_left"},
			//{"stock",},
			{"weight",escape(weight)}
		}
	);
	return n;
}

Assembly_design_part_id assembly_design_part(
	DB db,
	User edit_user,
	Date edit_date,
	Assembly_design_id parent,
	Part_design_id child,
	Positive_int quantity
){
	auto n=Assembly_design_part_id{new_item(db,Table_name{"assembly_design_part"})};
	insert(
		db,
		"assembly_design_part_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"parent",escape(parent)},
			{"child",escape(child)},
			{"quantity",escape(quantity)}
		}
	);
	return n;
}

void part(
	DB db,
	User edit_user,
	Date edit_date,

	Part_design_id part_design,
	optional<Assembly_id> parent,
	Part_state state,
	std::optional<Manufacture_step_id> manufacture_step,
	std::optional<string> weight_override,
	std::string notes
){
	auto n=Part_id{new_item(db,Table_name{"part"})};
	insert(
		db,
		"part_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"part_design",escape(part_design)},
			{"parent",escape(parent)},
			{"state",escape(state)},
			{"manufacture_step",escape(manufacture_step)},
			{"weight_override",escape(weight_override)},
			{"notes",escape(notes)}
		}
	);
}

Machine_id get_machine(DB db,User edit_user,Date edit_date,Nonempty_string name){
	auto q=query_to<Machine_id>(
		db,
		"SELECT main"+current(Id_table{"machine"})+"AND name="+escape(name)
	);
	if(q.size()){
		return q[0];
	}
	auto n=Machine_id{new_item(db,Table_name{"machine"})};
	insert(
		db,
		"machine_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"name",escape(name)},
			{"location",escape(unknown_location(db))},
			{"notes",escape("Automatically added by migration process")}
		}
	);
	return n;
}

Manufacture_step_id manufacture_step(
	DB db,
	User edit_user,
	Date edit_date,
	
	Part_design_id part_design,
	std::optional<Machine_id> machine,
	Positive_decimal time
){
	auto n=Manufacture_step_id{new_item(db,Table_name{"manufacture_step"})};
	insert(
		db,
		"manufacture_step_info",
		{
			{"main",escape(n)},
			{"edit_user",escape(edit_user)},
			{"edit_date",escape(edit_date)},
			{"valid","1"},

			{"part_design",escape(part_design)},
			{"machine",escape(machine)},
			{"time",escape(time)}
		}
	);
	return n;
}

void migrate_parts(
	DB db_old,
	DB db_new,
	map<int,Assembly_design_id> const& assembly_design_map,
	map<int,Assembly_id> const& assembly_map
){
	//going to need to take in the listing of old->new assemblies to make this all link up.
	auto q=query(
		db_old,
		"SELECT "
			"edit_user,edit_date,subsystem,"
			"name,part_number,part_state,length,width,thickness,"
			"material,qty,time,manufacture_date,"
			"who_manufacture,machine,place,bent,"
			"bend_type,drawing_link,cam_link,part_supplier,"
			"part_link,arrival_date,price,dni,"
			"weight,bom_exemption,bom_cost_override"
		" FROM part_info WHERE valid AND (id) IN ("
			"SELECT MAX(id) FROM part_info GROUP BY part_id"
		")"
	);
	for(auto row:q){
		auto edit_user=get_item((User*)0,row[0]);
		auto edit_date=get_item((Date*)0,row[1]);
		auto subsystem=get_item((int*)0,row[2]);
		auto name=[&]()->Nonempty_string{
			//get_item((Nonempty_string*)0,row[3]);
			auto os=row[3];
			if(!os || os->size()==0){
				return Nonempty_string{"No name "+as_string(rand())};
			}
			return Nonempty_string{*os};
		}();
		auto part_number=row[4];
		auto part_state=row[5];//get_item((Part_design_state*)0,row[5]);

		auto get_dim=[&](int i)->Dimension{
			auto s=row[i];
			if(!s) return std::nullopt;
			if(*s=="") return std::nullopt;
			return parse((Dimension*)0,*s);
		};
		auto length=get_dim(6);//get_item((Dimension*)0,row[6]);
		auto width=get_dim(7);//get_item((Dimension*)0,row[7]);
		auto thickness=get_dim(8);//get_item((Dimension*)0,row[8]);

		auto material=row[9];

		if(!row[10]){
			continue;
		}
		auto qty=get_item((int*)0,row[10]);

		auto time=row[11];
		auto manufacture_date=row[12];
		auto who_manufacture=row[13];
		auto machine=row[14];
		auto place=row[15];
		auto bent=row[16];
		auto bend_type=row[17];
		auto drawing_link=row[18];
		auto cam_link=row[19];

		auto part_supplier=[&]()->optional<Nonempty_string>{
			auto x=row[20];
			if(!x) return std::nullopt;
			if(x->empty()) return std::nullopt;
			return Nonempty_string{*x};
		}();

		auto part_link=get_item((optional<URL>*)0,row[21]);
		auto arrival_date=row[22];
		auto price=[&]()->optional<Positive_decimal>{
			auto x=row[23];
			if(!x) return std::nullopt;
			auto v=parse((Decimal*)0,*x);
			if(v==0){
				return Positive_decimal{Decimal{.001}};
			}
			return Positive_decimal{v};
		}();
		auto dni=row[24];
		auto weight=row[25];
		auto bom_exemption=row[26];
		auto bom_cost_override=row[27];

		bool local=[&](){
			if(!part_number) return 0;
			try{
				parse((Part_number_local*)0,*part_number);
				return 1;
			}catch(...){
				return 0;
			}
		}();

		if(
			part_supplier || 
			prefix("217-",part_number) || 
			prefix("am-",part_number) ||
			prefix("rev-",part_number) ||
			prefix("REV-",part_number) ||
			!local
		){
			auto supplier1=[&]()->Nonempty_string{
				if(part_supplier) return *part_supplier;
				if(prefix("217-",part_number)) return Nonempty_string{"Vex"};
				if(prefix("am-",part_number)) return Nonempty_string{"AndyMark"};
				if(
					prefix("rev-",part_number) ||
					prefix("REV-",part_number)
				){
					return Nonempty_string{"REV Robotics"};
				}
				return Nonempty_string{"unknown"};
			}();
			auto supplier_id=supplier(
				db_new,edit_user,edit_date,supplier1,
				part_link
			);
			auto stock_id=stock(db_new,edit_user,edit_date,name,length,width,thickness,part_link,drawing_link,cam_link);
			PRINT(stock_id);
			//TODO: Stock install
			//might want to check that it doesn't say to use some sort of machine here...
			if( (machine && machine!="none") || (who_manufacture && who_manufacture->size()) ){
				PRINT(machine);
				PRINT(who_manufacture);
				nyi
			}
			assert(price);
			availability(db_new,edit_user,edit_date,stock_id,supplier_id,part_number,*price,part_link);
			auto assembly_design_id=get(assembly_design_map,subsystem);
			if(assembly_design_id && qty){
				assembly_design_stock(db_new,edit_user,edit_date,*assembly_design_id,stock_id,Positive_int{qty});
			}
			auto assembly_id=get(assembly_map,subsystem);
			if(assembly_id){
				if(qty){
					stock_install(db_new,edit_user,edit_date,*assembly_id,stock_id,Positive_int(qty));
				}
			}
		}else{
			assert(part_number);

			try{
				auto p=parse((Part_number_local*)0,*part_number);
				(void)p;
			}catch(string const& s){
				cout<<"Error: "<<s<<"\n";
				PRINT(part_number);
				nyi
			}

			auto state2=[&](){
				#define X(A) if(part_state==""#A) return Part_design_state::A;
				PART_DESIGN_STATES(X)
				#undef X
				if(
					part_state=="fab" ||
					part_state=="build_list" ||
					part_state=="arrived" ||
					part_state=="fabbed" ||
					part_state=="found" ||
					part_state=="_3d_print" ||
					part_state=="buy_list" ||
					part_state=="find" ||
					part_state=="ordered"
				){
					return Part_design_state::ready;
				}
				PRINT(part_state);
				nyi
			}();
			auto part_design_id=part_design(db_new,edit_user,edit_date,*part_number,name,state2,weight);
			optional<Manufacture_step_id> manufacture_step_id;
			if(machine){
				assert(!machine->empty());
				auto machine_id=get_machine(db_new,edit_user,edit_date,Nonempty_string{*machine});
				manufacture_step_id=manufacture_step(
					db_new,edit_user,edit_date,part_design_id,machine_id,
					[=](){
						auto d=get_item((Decimal*)0,time);
						if(d>0) return Positive_decimal{d};
						return Positive_decimal{Decimal{1}};
					}()
				);
			}

			{
				auto parent=get(assembly_design_map,subsystem);
				if(parent && qty){
					assembly_design_part(db_new,edit_user,edit_date,*parent,part_design_id,Positive_int{qty});
				}
			}
			

			auto parent=get(assembly_map,subsystem);
			if(parent && part_state!="in_design"){
				auto state=[&]()->Part_state{
					if(
						part_state=="fab" ||
						part_state=="build_list"
					){
						return Part_state::manufacture;
					}
					if(
						part_state=="find" ||
						part_state=="ordered" ||
						part_state=="buy_list"
					){
						return Part_state::get_stock;
					}
					if(
						part_state=="arrived" ||
						part_state=="fabbed" ||
						part_state=="found"
					){
						return Part_state::done;
					}
					//get_stock
					//manufacture
					//done
					PRINT(part_state);
					nyi
				}();
				auto notes="";
				part(db_new,edit_user,edit_date,part_design_id,*parent,state,manufacture_step_id,weight,notes);
			}
		}
	}
}

int migrate(){
	//going to just try to migrate the latest, or all?
	//just latest
	auto db_old=connect_db("auth_old.json");
	auto db_new=connect_db();
	try{
		migrate_meeting(db_old.db,db_new.db);
		auto m=migrate_subsystem(db_old.db,db_new.db);
		migrate_parts(db_old.db,db_new.db,m.first,m.second);
	}catch(Out_of_range const& a){
		cout<<"Error:"<<a<<"\n";
		return 1;
	}catch(std::string const& s){
		cout<<"Error:"<<s<<"\n";
		return 1;
	}
	return 0;
}
