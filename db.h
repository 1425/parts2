#ifndef DB_H
#define DB_H

#include<map>
#include<variant>
#include "data_types.h"
#include "util.h"
#include "cgicc/Cgicc.h"

#define ID_TABLE(X)\
	struct X##_id:Wrap<X##_id,Id>{};\
	std::ostream& operator<<(std::ostream&,X##_id const&);\
	Input show_input(DB,std::string const& name,X##_id);

#define INFO_TABLE(X,ITEMS)\
	struct X##_info{\
		int id;\
		X##_id main;\
		bool valid;\
		User edit_user;\
		Date edit_date;\
		ITEMS(INST)\
	};\
	std::ostream& operator<<(std::ostream&,X##_info const&);\
	X##_info rand(X##_info const*);

#define TABLE_DATA(X,ITEMS)\
	struct X{\
		ITEMS(INST)\
	};\
	std::ostream& operator<<(std::ostream&,X const&);\
	X rand(X const*);\
	Input show_input(DB,std::string const& name,X##_info);\

#define DEFINE_ITEM(TABLE_NAME,NAME,INFO_ITEMS)\
	ID_TABLE(NAME)\

using Dimensions=Int_limited<0,3>;

#define STOCK_CATEGORY_ITEMS(X)\
	X(Nonempty_string,name)\
	X(Dimensions,dimensions)

using Dimension=std::optional<Positive_decimal>;

#define STOCK_ITEMS(X)\
	X(Stock_category_id,category)\
	X(Dimension,length)\
	X(Dimension,width)\
	X(Dimension,thickness)\
	X(unsigned,available)\
	X(std::string,notes)\
	X(std::optional<unsigned>,wishlist)\

using Days=unsigned;

#define AVAILABILITY_ITEMS(X)\
	X(Stock_id,stock)\
	X(Supplier_id,supplier)\
	X(Part_number,part_number)\
	X(Positive_decimal,cost)\
	X(std::optional<Days>,lead_time)\
	X(URL,url)\

#define PART_DESIGN_ITEMS(X)\
	X(Part_number_local,part_number)\
	X(std::string,name)\
	X(Part_design_state,state)\
	X(std::optional<Days>,design_time_left)\
	X(std::optional<Stock_id>,stock)\
	X(std::optional<Weight>,weight)

#define MANUFACTURE_STEP_ITEMS(X)\
	X(Part_design_id,part_design)\
	X(std::optional<Machine_id>,machine)\
	X(Positive_decimal,time)\

using In_design=Defaulted<bool,1>;

#define ASSEMBLY_DESIGN_ITEMS(X)\
	X(Subsystem_prefix,prefix)\
	X(Part_number_local,part_number)\
	X(std::string,name)\
	X(In_design,in_design)\
	X(std::optional<Days>,design_time_left)\
	X(std::optional<Positive_decimal>,assembly_time)\
	X(std::optional<Weight>,weight)

#define ASSEMBLY_DESIGN_ASSEMBLY_ITEMS(X)\
	X(Assembly_design_id,parent)\
	X(Assembly_design_id,child)\
	X(Positive_int,quantity)

#define ASSEMBLY_DESIGN_PART_ITEMS(X)\
	X(Assembly_design_id,parent)\
	X(Part_design_id,child)\
	X(Positive_int,quantity)\

#define ASSEMBLY_DESIGN_STOCK_ITEMS(X)\
	X(Assembly_design_id,parent)\
	X(Stock_id,child)\
	X(Positive_int,quantity)\

#define ASSEMBLY_ITEMS(X)\
	X(std::string,name)\
	X(Assembly_design_id,assembly_design)\
	X(std::optional<Assembly_id>,parent)\
	X(std::optional<Weight>,weight_override)\
	X(Assembly_state,state)\
	X(std::optional<Decimal>,time_left)\
	X(std::optional<std::string>,notes)\

#define PART_ITEMS(X)\
	X(Part_design_id,part_design)\
	X(std::optional<Assembly_id>,parent)\
	X(Part_state,state)\
	X(std::optional<Manufacture_step_id>,manufacture_step)\
	X(std::optional<Weight>,weight_override)\
	X(std::string,notes)\

#define STOCK_INSTALL_ITEMS(X)\
	X(Assembly_id,assembly)\
	X(Stock_id,stock)\
	X(Positive_int,quantity)

#define LOCATION_ITEMS(X)\
	X(Nonempty_string,name)\
	X(std::string,notes)

using Meeting_length=Positive_decimal;

#define MEETING_ITEMS(X)\
	X(Date,date)\
	X(Meeting_length,length)\
	X(Location_id,location)\
	X(Color,color)\
	X(std::string,notes)\

#define MACHINE_ITEMS(X)\
	X(Nonempty_string,name)\
	X(Location_id,location)\
	X(std::string,notes)\

#define SUPPLIER_ITEMS(X)\
	X(Nonempty_string,name)\
	X(std::string,notes)\
	X(std::optional<URL>,url)\

#define ORDER_ITEMS(X)\
	X(Supplier_id,supplier)\
	X(Positive_decimal,cost)\
	X(std::optional<User>,who_paid)\
	X(std::optional<Location_id>,destination)\
	X(Date,submitted)\
	X(Date,expected_arrival)\

#define ORDER_MEMBER_ITEMS(X)\
	X(Order_id,parent)\
	X(Stock_id,stock)\
	X(Positive_int,quantity)\
	X(bool,arrived)\

#define UPLOAD_ITEMS(X)\
	X(Nonempty_string,name)\
	X(Nonempty_string,datatype)\
	X(Upload_data,data)

#define UPLOAD_ASSEMBLY_ITEMS(X)\
	X(Upload_id,upload)\
	X(Assembly_id,parent)

#define UPLOAD_ASSEMBLY_DESIGN_ITEMS(X)\
	X(Upload_id,upload)\
	X(Assembly_design_id,parent)

#define UPLOAD_LOCATION_ITEMS(X)\
	X(Upload_id,upload)\
	X(Location_id,parent)\

#define UPLOAD_MACHINE_ITEMS(X)\
	X(Upload_id,upload)\
	X(Machine_id,parent)

#define UPLOAD_MANUFACTURE_STEP_ITEMS(X)\
	X(Upload_id,upload)\
	X(Manufacture_step_id,parent)\

#define UPLOAD_MEETING_ITEMS(X)\
	X(Upload_id,upload)\
	X(Meeting_id,parent)

#define UPLOAD_ORDER_ITEMS(X)\
	X(Upload_id,upload)\
	X(Order_id,parent)

#define UPLOAD_PART_ITEMS(X)\
	X(Upload_id,upload)\
	X(Part_id,parent)

#define UPLOAD_PART_DESIGN_ITEMS(X)\
	X(Upload_id,upload)\
	X(Part_design_id,parent)

#define UPLOAD_STOCK_ITEMS(X)\
	X(Upload_id,upload)\
	X(Stock_id,parent)

#define UPLOAD_SUPPLIER_ITEMS(X)\
	X(Upload_id,upload)\
	X(Supplier_id,parent)

#define UPLOAD_LINK_TABLES(X)\
	X(upload_assembly,Upload_assembly,UPLOAD_ASSEMBLY_ITEMS)\
	X(upload_assembly_design,Upload_assembly_design,UPLOAD_ASSEMBLY_DESIGN_ITEMS)\
	X(upload_location,Upload_location,UPLOAD_LOCATION_ITEMS)\
	X(upload_machine,Upload_machine,UPLOAD_MACHINE_ITEMS)\
	X(upload_manufacture_step,Upload_manufacture_step,UPLOAD_MANUFACTURE_STEP_ITEMS)\
	X(upload_meeting,Upload_meeting,UPLOAD_MEETING_ITEMS)\
	X(upload_order,Upload_order,UPLOAD_ORDER_ITEMS)\
	X(upload_part,Upload_part,UPLOAD_PART_ITEMS)\
	X(upload_part_design,Upload_part_design,UPLOAD_PART_DESIGN_ITEMS)\
	X(upload_stock,Upload_stock,UPLOAD_STOCK_ITEMS)\
	X(upload_supplier,Upload_supplier,UPLOAD_SUPPLIER_ITEMS)

#define TABLES(X)\
	X(supplier,Supplier,SUPPLIER_ITEMS)\
	X(stock_category,Stock_category,STOCK_CATEGORY_ITEMS)\
	X(stock,Stock,STOCK_ITEMS)\
	X(availability,Availability,AVAILABILITY_ITEMS)\
	X(location,Location,LOCATION_ITEMS)\
	X(machine,Machine,MACHINE_ITEMS)\
	X(part_design,Part_design,PART_DESIGN_ITEMS)\
	X(assembly_design,Assembly_design,ASSEMBLY_DESIGN_ITEMS)\
	X(assembly,Assembly,ASSEMBLY_ITEMS)\
	X(meeting,Meeting,MEETING_ITEMS)\
	X(assembly_design_stock,Assembly_design_stock,ASSEMBLY_DESIGN_STOCK_ITEMS)\
	X(assembly_design_part,Assembly_design_part,ASSEMBLY_DESIGN_PART_ITEMS)\
	X(assembly_design_assembly,Assembly_design_assembly,ASSEMBLY_DESIGN_ASSEMBLY_ITEMS)\
	X(order,Order,ORDER_ITEMS)\
	X(order_member,Order_member,ORDER_MEMBER_ITEMS)\
	X(manufacture_step,Manufacture_step,MANUFACTURE_STEP_ITEMS)\
	X(part,Part,PART_ITEMS)\
	X(upload,Upload,UPLOAD_ITEMS)\
	X(stock_install,Stock_install,STOCK_INSTALL_ITEMS)\
	UPLOAD_LINK_TABLES(X)\

TABLES(DEFINE_ITEM)

#define COLUMN_TYPE_ITEMS(X)\
	X(std::string,type)\
	X(bool,null)\
	X(bool,key)

struct Column_type{
	COLUMN_TYPE_ITEMS(INST)
};

std::ostream& operator<<(std::ostream&,Column_type const&);
bool operator==(Column_type const&,Column_type const&);

class Column_name{
	std::string data;

	public:
	explicit Column_name(std::string);
	explicit Column_name(const char*);
	std::string const& get()const;
	operator std::string const&()const;
};
std::ostream& operator<<(std::ostream&,Column_name const&);
bool operator<(Column_name const&,Column_name const&);
std::string operator+(std::string const&,Column_name const&);
std::string operator+(Column_name const&,std::string const&);
bool operator==(Column_name const&,Column_name const&);
bool operator==(Column_name const&,std::string const&);
bool operator==(std::string const&,Column_name const&);
bool operator==(Column_name const&,const char*);
Column_name rand(Column_name const*);

using Table_type=std::vector<std::pair<Column_name,Column_type>>;

Table_type read(DB db,std::string const& name);

class Table_name{
	std::string data;

	public:
	explicit Table_name(const char*);
	explicit Table_name(std::string const&);
	operator std::string const&()const;
};
std::ostream& operator<<(std::ostream&,Table_name const&);
std::string operator+(const char *,Table_name const&);
std::string operator+(std::string const&,Table_name const&);
std::string operator+(Table_name const&,char const*);
bool operator==(Table_name const&,const char*);
bool operator<(Table_name const&,Table_name const&);
Table_name rand(Table_name const*);
Table_name parse(Table_name const*,std::optional<std::string> const&);

class Id_table{
	std::string data;

	public:
	explicit Id_table(std::string);
	operator std::string const&()const;
	operator Table_name()const;
	std::string const& get()const;
};
std::ostream& operator<<(std::ostream&,Id_table const&);
bool operator<(Id_table const&,Id_table const&);
std::string operator+(const char*,Id_table const&);
Id_table rand(Id_table const*);
bool operator==(Id_table const&,Id_table const&);
bool operator==(Id_table const&,const char*);
std::vector<Id_table> options(Id_table const*);

void check_table(DB,Table_name const&,Table_type const&);
std::set<std::string> show_tables(DB);

void create_table(DB,Table_name const&,Table_type const&);

std::map<Table_name,Table_type> const& expected_tables();

struct Typename:Wrap<Typename,std::string>{};
bool operator==(Typename const&,const char*);
bool operator==(Typename const&,std::string const&);
std::string operator+(const char*,Typename const&);

using Table_types=std::vector<std::pair<Column_name,Typename>>;
std::map<Id_table,Table_types> table_types();

int parse(int const*,std::optional<std::string> const&);
std::string parse(std::string const*,std::optional<std::string> const&);
std::optional<Assembly_state> parse(Assembly_state const*,std::optional<int> const&);
Assembly_state parse(Assembly_state const*,std::optional<std::string> const&);

#define IDS(A,B,C) X(B##_id)

#define STANDARD_COL_TYPES(X)\
	X(int)\
	X(unsigned)\
	X(std::string)\
	X(Assembly_state)\
	X(Part_number)\
	X(Part_number_local)\
	X(Decimal)\
	X(Positive_decimal)\
	X(URL)\
	X(Date)\
	X(Part_design_state)\
	X(Part_state)\
	X(Subsystem_prefix)\
	X(Nonempty_string)\
	X(Upload_data)\
	X(Positive_int)\
	X(Color)\
	X(Dimensions)\
	X(In_design)\
	X(std::optional<std::string>)\
	X(std::optional<URL>)\
	X(std::optional<User>)\
	X(std::optional<Weight>)\
	X(std::optional<Positive_decimal>)\
	X(std::optional<Decimal>)\
	X(std::optional<Days>)\
	X(std::optional<Date>)\
	X(std::optional<Assembly_design_id>)\
	X(std::optional<Location_id>)\
	X(std::optional<Assembly_id>)\
	X(std::optional<Stock_id>)\
	X(std::optional<Machine_id>)\
	X(std::optional<Manufacture_step_id>)\
	X(std::optional<Meeting_id>)\
	X(std::optional<Order_id>)\
	X(std::optional<Part_id>)\
	X(std::optional<Part_design_id>)\
	X(std::optional<Supplier_id>)\
	TABLES(IDS)

using Column_value=std::variant<
	#define X(NAME) NAME,
	STANDARD_COL_TYPES(X)
	#undef X
	bool
>;

struct Get_cgi_var{
	//This looks like it should just be a namespace,
	//but that doesn't quite work.
	#define ERROR_ITEMS(X)\
		X(Column_name,col)\
		X(Typename,type)\
		X(std::string,value)\
		X(std::string,example)
	struct Error{
		ERROR_ITEMS(INST)
	};
	using Errors=std::vector<Error>;
	using Success=std::map<Column_name,Column_value>;
	using Result=std::variant<Success,Errors>;
};

std::ostream& operator<<(std::ostream&,Get_cgi_var::Error const&);
bool operator<(Get_cgi_var::Error const&,Get_cgi_var::Error const&);
std::optional<std::string> maybe_get(cgicc::Cgicc const&,std::string const&);
Column_value parse_col(Typename const&,std::string const&);

std::string table_description(Table_name const&);

std::string sql_escape(DB,std::string const&);
std::string sql_escape(DB,Nonempty_string const&);
std::string sql_escape(DB,std::optional<std::string> const&);

std::string show_elem(
	std::pair<std::string,Column_type> const&,
	std::optional<std::string> const&
);

std::string show_elem(
	std::pair<std::string,Column_type> const&,
	std::optional<std::string> const&,
	std::optional<std::string> const&
);

template<typename T>
std::vector<
	std::pair<
		T,
		std::optional<T>
	>
> with_delayed(std::vector<T> const& a){
	std::vector<std::pair<T,std::optional<T>>> r;
	for(auto i:range(a.size())){
		if(i==0){
			r|=std::make_pair(a[i],std::nullopt);
		}else{
			r|=std::make_pair(a[i],a[i-1]);
		}
	}
	return r;
}

std::string sql_escape(DB,Column_value const&);

int new_item(DB,Table_name const&);
std::string current_user();
std::vector<std::string> const& table_names();

template<typename T>
T get_item(T const* t,std::optional<std::string> a){
	assert(a);
	return parse(t,*a);
}

template<typename T>
std::optional<T> get_item(std::optional<T> const*,std::optional<std::string> const& a){
	if(!a) return std::nullopt;
	return parse((T*)0,*a);
}

template<typename T>
std::vector<T> query_to(DB db,std::string const& query_string){
	auto q=query(db,query_string);
	return mapf(
		[&](auto const& row){
			if(row.size()!=1){
				PRINT(query_string);
				PRINT(row);
			}
			assert(row.size()==1);
			return get_item((T*)0,row[0]);
		},
		q
	);
}

template<typename A,typename B>
std::vector<std::pair<A,B>> query_to(DB db,std::string const& s){
	auto q=query(db,s);
	return mapf(
		[=](auto row){
			assert(row.size()==2);
			return std::make_pair(
				get_item((A*)0,row[0]),
				get_item((B*)0,row[1])
			);
		},
		q
	);
}

template<typename A,typename B,typename C>
std::vector<std::tuple<A,B,C>> query_to(DB db,std::string const& s){
	return mapf(
		[](auto row){
			assert(row.size()==3);
			return std::make_tuple(
				get_item((A*)0,row[0]),
				get_item((B*)0,row[1]),
				get_item((C*)0,row[2])
			);
		},
		query(db,s)
	);
}	

template<typename A,typename B,typename C,typename D>
std::vector<std::tuple<A,B,C,D>> query_to(DB db,std::string const& s){
	return mapf(
		[](auto row){
			assert(row.size()==4);
			return std::make_tuple(
				get_item((A*)0,row[0]),
				get_item((B*)0,row[1]),
				get_item((C*)0,row[2]),
				get_item((D*)0,row[3])
			);
		},
		query(db,s)
	);
}

template<typename A,typename B,typename C,typename D,typename E>
std::vector<std::tuple<A,B,C,D,E>> query_to(DB db,std::string const& s){
	return mapf(
		[&](auto row){
			assert(row.size()==5);
			return std::make_tuple(
				get_item((A*)0,row[0]),
				get_item((B*)0,row[1]),
				get_item((C*)0,row[2]),
				get_item((D*)0,row[3]),
				get_item((E*)0,row[4])
			);
		},
		query(db,s)
	);
}

template<typename A,typename B,typename C,typename D,typename E,typename F>
std::vector<std::tuple<A,B,C,D,E,F>> query_to(DB db,std::string const& s){
	return mapf(
		[&](auto row){
			assert(row.size()==6);
			return std::make_tuple(
				get_item((A*)0,row[0]),
				get_item((B*)0,row[1]),
				get_item((C*)0,row[2]),
				get_item((D*)0,row[3]),
				get_item((E*)0,row[4]),
				get_item((F*)0,row[5])
			);
		},
		query(db,s)
	);
}

template<typename A,typename B,typename C,typename D,typename E,typename F,typename G>
std::vector<std::tuple<A,B,C,D,E,F,G>> query_to(DB db,std::string const& s){
	return mapf(
		[&](auto row){
			assert(row.size()==7);
			return std::make_tuple(
				get_item((A*)0,row[0]),
				get_item((B*)0,row[1]),
				get_item((C*)0,row[2]),
				get_item((D*)0,row[3]),
				get_item((E*)0,row[4]),
				get_item((F*)0,row[5]),
				get_item((G*)0,row[6])
			);
		},
		query(db,s)
	);
}

template<
	typename A,typename B,typename C,typename D,
	typename E,typename F,typename G,typename H
>
std::vector<std::tuple<A,B,C,D,E,F,G,H>> query_to(DB db,std::string const& s){
	return mapf(
		[&](auto row){
			assert(row.size()==8);
			return std::make_tuple(
				get_item((A*)0,row[0]),
				get_item((B*)0,row[1]),
				get_item((C*)0,row[2]),
				get_item((D*)0,row[3]),
				get_item((E*)0,row[4]),
				get_item((F*)0,row[5]),
				get_item((G*)0,row[6]),
				get_item((H*)0,row[7])
			);
		},
		query(db,s)
	);
}

template<
	typename A,typename B,typename C,typename D,
	typename E,typename F,typename G,typename H,
	typename I,typename J,typename K,typename L
>
std::vector<std::tuple<A,B,C,D,E,F,G,H,I,J,K,L>> query_to(DB db,std::string const& s){
	return mapf(
		[&](auto row){
			assert(row.size()==12);
			return std::make_tuple(
				get_item((A*)0,row[0]),
				get_item((B*)0,row[1]),
				get_item((C*)0,row[2]),
				get_item((D*)0,row[3]),
				get_item((E*)0,row[4]),
				get_item((F*)0,row[5]),
				get_item((G*)0,row[6]),
				get_item((H*)0,row[7]),
				get_item((I*)0,row[8]),
				get_item((J*)0,row[9]),
				get_item((K*)0,row[10]),
				get_item((L*)0,row[11])
			);
		},
		query(db,s)
	);
}

std::set<Part_number> part_numbers(DB&);
std::set<Subsystem_prefix> prefixes(DB);

struct DB_connection{
	DB db;

	~DB_connection();
};

DB_connection connect_db();
DB_connection connect_db(std::string const&);
void populate(DB);
void drop_test_data();

Id new_item(DB db,Table_name const& table);
std::string table_description(Table_name const&);

template<typename T>
T query_single(DB db,std::string const& s){
	auto q=query_to<T>(db,s);
	if(q.size()==1) return q[0];
	throw "Unexpected size ("+as_string(q.size())+"):"+s;
}

template<typename A,typename B,typename C>
std::tuple<A,B,C> query_single(DB db,std::string const& s){
	auto q=query_to<A,B,C>(db,s);
	if(q.size()==1) return q[0];
	throw "Expected size 3, found "+as_string(q.size());
}

template<typename T>
std::optional<T> query_optional(DB db,std::string const& s){
	auto q=query_to<T>(db,s);
	switch(q.size()){
		case 0:
			return std::nullopt;
		case 1:
			return q[0];
		default:
			throw "Unexpected size ("+as_string(q.size())+"):"+s;
	}
}

#endif
