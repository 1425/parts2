#include "db.h"
#include<boost/property_tree/ptree.hpp>
#include<boost/property_tree/json_parser.hpp>
#include "type.h"
#include "map_util.h"
#include "set_util.h"

using namespace std;

string sql_escape(DB db,std::optional<std::string> const& a){
	if(a) return sql_escape(db,*a);
	return "NULL";
}

std::ostream& operator<<(std::ostream& o,Get_cgi_var::Error const& a){
	o<<"Get_cgi_var::Error( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	ERROR_ITEMS(X)
	#undef X
	return o<<")";
}

bool operator<(Get_cgi_var::Error const& a,Get_cgi_var::Error const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	ERROR_ITEMS(X)
	#undef X
	return 0;
}

string to_db_type(const unsigned*){ return "int(11) unsigned"; }
string to_db_type(const string*){ return "varchar(100)"; }
string to_db_type(const double*){ return "double"; }

#define PRINT_ITEM(B,A)\
	o<<""#A<<":"<<a.A<<" ";

#define RAND_ITEM(A,B)\
	rand((A*)0),

#define PRINT_INFO(TABLE_NAME,A,B)\
	std::ostream& operator<<(std::ostream& o,A##_info const& a){\
		o<<""#A<<"_info( ";\
		PRINT_ITEM(int,id)\
		PRINT_ITEM(A##_id,main)\
		PRINT_ITEM(bool,valid)\
		PRINT_ITEM(User,edit_user)\
		PRINT_ITEM(Date,edit_date)\
		B(PRINT_ITEM)\
		return o<<")";\
	}\
	A##_info rand(A##_info const*){\
		return A##_info{\
			RAND_ITEM(int,id)\
			RAND_ITEM(A##_id,main)\
			RAND_ITEM(bool,valid)\
			RAND_ITEM(User,edit_user)\
			RAND_ITEM(Date,edit_date)\
			B(RAND_ITEM)\
		};\
	}\
	A rand(A const*){\
		return A{\
			B(RAND_ITEM)\
		};\
	}\
	std::ostream& operator<<(std::ostream& o,A const& a){\
		o<<""#A<<"(";\
		B(PRINT_ITEM)\
		return o<<")";\
	}

//TABLES(PRINT_INFO)

#define X(A,B,C)\
	std::ostream& operator<<(std::ostream& o,B##_id const& a){\
		return o<<""#A<<"_id("<<a.data<<")";\
	}
TABLES(X)
#undef X

string to_db_type(std::string const&){
	nyi
}

template<typename T>
Column_type column_type(T const* t){
	return Column_type{to_db_type(t),0,0};
}

template<typename T>
Column_type column_type(std::optional<T> const*){
	auto r=column_type((T*)0);
	r.null=1;
	return r;
}

std::map<Table_name,Table_type> const& expected_tables(){
	static std::map<Table_name,Table_type> r;
	if(r.empty()){
		#define EXPAND_ITEM(A,B)\
			{Column_name{""#B},column_type((A*)0)},

		#define X(TABLE_NAME,A,B) {\
			r[Table_name{""#TABLE_NAME}]={\
				{Column_name{"id"},Column_type{"int(11)",0,1}}\
			};\
			r[Table_name{""#TABLE_NAME "_info"}]={\
				{Column_name{"id"},Column_type{"int(11)",0,1}},\
				{Column_name{"main"},Column_type{"int(11)",0,0}},\
				{Column_name{"valid"},Column_type{"tinyint(1)",0,0}},\
				{Column_name{"edit_user"},Column_type{"varchar(100)",0,0}},\
				{Column_name{"edit_date"},Column_type{"datetime",0,0}},\
				B(EXPAND_ITEM)\
			};\
		}
		TABLES(X)
		#undef X
		#undef EXPAND_ITEM
	}
	return r;
}

std::map<Id_table,Table_types> table_types(){
	std::map<Id_table,Table_types> r;
	#define EXPAND_ITEM(A,B) {Column_name{""#B},Typename{""#A}},

	#define X(TABLE_NAME,A,B)\
		r[Id_table{""#TABLE_NAME}]={\
			B(EXPAND_ITEM)\
		};

	TABLES(X)

	#undef X
	#undef EXPAND_ITEM

	return r;
}

optional<vector<string>> parse_enum(string s){
	if(s.substr(0,5)!="enum(") return std::nullopt;
	if(s[s.size()-1]!=')') return std::nullopt;

	auto middle=s.substr(5,s.size()-5-1);
	auto sp=split(',',middle);

	auto parse_item=[=](string s){
		assert(s.size());
		assert(s[0]=='\'');
		assert(s[s.size()-1]=='\'');
		return s.substr(1,s.size()-2);
	};

	return mapf(parse_item,sp);
}

optional<pair<int,int>> parse_decimal(string s){
	if(s=="decimal(8,2)") return make_pair(8,2);
	if(s=="decimal(8,3)") return make_pair(8,3);
	return {};
}

void alter_column(DB db,Table_name table,pair<string,Column_type> c1,pair<string,Column_type> c2){
	auto name1=c1.first;
	auto name2=c2.first;
	assert(name1==name2);

	auto t1=c1.second;
	auto t2=c2.second;
	PRINT(table);
	PRINT(c1);
	PRINT(c2);
	assert(!t1.key);
	assert(!t2.key);

	PRINT(table);
	PRINT(t1);
	PRINT(t2);

	auto e1=parse_enum(t1.type);
	auto e2=parse_enum(t2.type);

	cout<<"what\n";

	if(t1.type==t2.type && t1.key==t2.key){
		//for now, just ignore nullability and see that stuff still runs.
		run_cmd(
			db,
			"ALTER TABLE "+table+" MODIFY "+name1+" "+t2.type+
			[&](){
				if(!t2.null){
					return " NOT NULL";
				}
				return "";
			}()
		);
		cout<<"mod\n";
		return;
	}

	if(e1 && e2){
		auto missing=to_set(*e1)-to_set(*e2);
		if(missing.size()){
			cout<<"Does not cover all previous cases";
			nyi
		}

		run_cmd(db,"ALTER TABLE "+table+" MODIFY `"+name1+"` "+t2.type);
		cout<<"r1\n";
		return;
	}

	auto d1=parse_decimal(t1.type);
	auto d2=parse_decimal(t2.type);
	if(d1 && d2 && d2->first>=d1->first && d2->second>=d1->second){
		run_cmd(db,"ALTER TABLE "+table+" MODIFY `"+name1+"` "+t2.type);
		cout<<"r2\n";
		return;
	}

	if(name1==name2){
		auto count=query_single<unsigned>(db,"SELECT COUNT(`"+name1+"`) FROM `"+table+"`");
		if(count==0){
			run_cmd(db,"ALTER TABLE "+table+" MODIFY `"+name1+"` "+t2.type);
			cout<<"r3\n";
			return;
		}
	}

	nyi
}

set<string> show_tables(DB db){
	return to_set(query_to<string>(db,"SHOW TABLES"));
}

bool parse_key(string s){
	if(s=="PRI") return 1;
	PRINT(s);
	nyi
}

bool parse_null(string s){
	if(s=="NO") return 0;
	if(s=="YES") return 1;
	PRINT(s);
	nyi
}

Table_type read(DB db,string const& name){
	auto q=query(db,"DESCRIBE `"+name+"`");
	Table_type r;
	for(auto elem:q){
		//PRINT(elem);
		assert(elem.size()==6);
		Column_name name=Column_name{*elem[0]};
		auto type=*elem[1];
		auto null=*elem[2];
		auto primary=*elem[3];
		//PRINT(primary);
		r|=make_pair(name,Column_type(type,parse_null(null),primary=="PRI"));
	}
	return r;
}

void delete_column(DB db,Table_name table,Column_name column){
	auto i=query_single<unsigned>(db,"SELECT COUNT(`"+column+"`) FROM `"+table+"`");
	if(i){
		cout<<"Not deleting non-empty column \""+column+"\" from \""+table+"\"!\n";
		PRINT(i);
		cout<<query(db,"SELECT "+column+" FROM `"+table+"` GROUP BY "+column)<<"\n";
		nyi
	}
	nyi
}

vector<string> alter_table(DB db,Table_name table_name,vector<pair<Column_name,Column_type>> after){
	vector<string> queries;
	auto current=read(db,table_name);

	{
		auto n1=to_set(firsts(current));
		auto n2=to_set(firsts(after));
		auto added=n2-n1;
		auto removed=n1-n2;
		//PRINT(added);
		//PRINT(removed);
		if( added.empty() && removed.size()==1){
			nyi
		}
	}

	for(auto [e1,e2]:zip_extend(current,after)){
		if(e1!=e2){
			if(!e1){
				//add columns at the end
				stringstream ss;
				ss<<"ALTER TABLE "+table_name+" ADD "+e2->first+" "+e2->second.type;
				if(e2->second.key){
					ss<<" UNIQUE PRIMARY KEY";
				}
				queries|=ss.str();
			}else{
				if(e2){
					//change already-existing columns
					if(e1->first==e2->first){
						alter_column(db,table_name,*e1,*e2);
					}else{
						cout<<"Change w/ possibly totally unrelated column.  Not automatically updating.\n";
						PRINT(table_name);
						PRINT(e1);
						PRINT(e2);
						exit(1);
					}
				}else{
					//delete column
					auto x=*e1;
					delete_column(db,table_name,x.first);
				}
			}
		}
	}
	return queries;
}

void populate(DB db){
	auto existing_tables=show_tables(db);
	//not going to worry about missing tables here to start with.
	vector<string> queries;
	for(auto [table_name,after]:expected_tables()){
		if(existing_tables.count(table_name)){
			queries|=alter_table(db,table_name,after);
		}else{
			create_table(db,table_name,after);
		}
	}

	//print_lines(queries);

	//Now apply all the changes...
	for(auto q:queries) run_cmd(db,q);
}

void create_table(DB db,Table_name const& name,Table_type const& type){
	run_cmd(
		db,
		[=](){
			stringstream ss;
			ss<<"CREATE TABLE `"<<name<<"` (";
			for(auto [last,p]:mark_last(type)){
				auto [k,v]=p;
				ss<<"`"<<k<<"` "<<v.type;
				if(v.key){
					ss<<" unique NOT NULL primary key AUTO_INCREMENT";
				}
				if(!last) ss<<",";
			}
			ss<<")";
			return ss.str();
		}()
	);
}

std::ostream& operator<<(std::ostream& o,boost::property_tree::ptree const& a){
	o<<"ptree(";
	for(auto elem:a){
		o<<elem;
	}
	return o<<")";
}

DB_connection::~DB_connection(){
	mysql_close(db);
}

DB_connection connect_db(string const& auth_filename){
	namespace pt=boost::property_tree;
	pt::ptree root;
	pt::read_json(auth_filename,root);
	auto get=[&](string name){
		try{
			return root.get<string>(name);
		}catch(...){
			throw "Failed to read "+name;
		}
	};

	DB_connection db;
	db.db=mysql_init(NULL);

	auto r=mysql_real_connect(
		db.db,
		get("host").c_str(),
		get("user").c_str(),
		get("password").c_str(),
		get("database").c_str(),
		0,NULL,0
	);
	if(!r){
		throw "Could not connect to database";
	}

	//eventually, this should go somewhere else so doesn't do this every time
	//and also need to have it at adjustable timeframe
	//and even if needed to retain it here would be able to drop some of the columns like 'valid'
	//#define X(A,B,C) run_cmd(db.db,"CREATE TEMPORARY TABLE "#A "_current SELECT * FROM "#A"_info WHERE valid AND id IN (SELECT max(id) FROM "#A"_info GROUP BY main)");

	/*auto f=[&](string name){
		run_cmd(
			db.db,
			"CREATE TEMPORARY TABLE "+name+"_current SELECT * FROM "+name+"_info WHERE valid AND id IN (SELECT max(id) FROM "+name+"_info GROUP BY main)"
			" PRIMARY KEY (main)"
		);
	};*/

	/*#define X(A,B,C) run_cmd(\
		db.db,\
		"CREATE TEMPORARY TABLE "#A "_current SELECT * FROM "#A"_info WHERE valid AND id IN (SELECT max(id) FROM "#A"_info GROUP BY main)"\
		" PRIMARY KEY main"\
	);*/
	#define X(A,B,C) run_cmd(\
		db.db,\
		"CREATE TEMPORARY TABLE "#A "_current SELECT * FROM "#A"_info WHERE valid AND id IN (SELECT max(id) FROM "#A"_info GROUP BY main)"\
	); run_cmd(db.db,"ALTER TABLE "#A"_current ADD CONSTRAINT f PRIMARY KEY (main)");
	TABLES(X)
	#undef X

	return db;
}

DB_connection connect_db(){
	return connect_db("auth.json");
}

string table_description(Table_name const& table){
	if(table=="assembly"){
		return "Identifies a particular collection of parts on the robot.";
	}
	if(table=="assembly_info"){
		return "Tells information about a particular assembly of parts.";
	}
	if(table=="availability"){
		return "Identifies a particular type of raw material that we might order.";
	}
	if(table=="availability_info"){
		return "Information about ordering a type of material (or part)";
	}
	if(table=="design"){
		return "Identifies a particular design of a part";
	}
	if(table=="design_info"){
		return "Information about how to build a part";
	}
	if(table=="location"){
		return "Identifies a location that the team might meet.";
	}
	if(table=="location_info"){
		return "Information about a location that the team might meet";
	}
	if(table=="machine"){
		return "Identifies a particular machine tool that we might use to build a part";
	}
	if(table=="machine_info"){
		return "Information about a machine that we might use to make parts";
	}
	if(table=="meeting"){
		return "Identifies team meetings that we might use to make parts";
	}
	if(table=="meeting_info"){
		return "Information about team meetings";
	}
	if(table=="part"){
		return "Identifies a built part that goes on the robot.";
	}
	if(table=="part_info"){
		return "Information about an individual component that goes into the robot.  These should be built from items from the \"stock\" table, and get put into something in the \"assembly\" table.";
	}
	if(table=="stock"){
		return "Identifies off the shelf items that can be turned into parts that go in the parts table.";
	}
	if(table=="stock_info"){
		return "Information about purchasable items";
	}
	if(table=="supplier"){
		return "Identifies sources from which materials can be bought";
	}
	if(table=="supplier_info"){
		return "Information about suppliers that we buy things from";
	}
	if(table=="assembly_design"){
		return "Identifies the design of an assembly of parts";
	}
	if(table=="assembly_design_info"){
		return "Info about the design of an assembly";
	}
	if(table=="assembly_design_part" || table=="assembly_design_part_info"){
		return "Represents a component that needs to go into an assembly";
	}
	if(table=="order" || table=="order_info"){
		return "A purchase that has been made";
	}
	if(table=="order_member" || table=="order_member_info"){
		return "Item from the list of things that were bought";
	}
	if(table=="part_design" || table=="part_design_info"){
		return "A type of part that we might make.  ";
	}
	if(table=="manufacture_step" || table=="manufacture_step_info"){
		return "A process that needs to be done in order to create a part";
	}
	if(table=="upload" || table=="upload_info"){
		return "Stores data uploaded about parts, designs, etc.  Could be pictures, drawings, G-code or anything else.";
	}
	if(table=="assembly_design_assembly" || table=="assembly_design_assembly_info"){
		return "Describes assemblies which go into other assemblies";
	}
	if(table=="assembly_design_stock" || table=="assembly_design_stock_info"){
		return "Describes stock components that directly go into an assembly rather than being built into a manufactured part first.";
	}
	if(table=="image_link" || table=="image_link_info"){
		return "Keeps track of what images are related to";
	}
	if(table=="stock_install" || table=="stock_install_info"){
		return "Purchased pieces that are directly installed into assemblies";
	}
	if(prefix("upload_",table)){
		return "Keeps track of what an upload is related to";
	}
	if(table=="stock_category" || table=="stock_category_info"){
		return "Category of stock.  This exists so that raw materials that get cut down can be recognized as the same thing even if they have different dimensions.  ";
	}
	return "Unrecognized table";
}

#define X(NAME) std::string sql_escape(DB,NAME const& a){ return escape(a); }
X(std::decimal::decimal32)
X(Part_number)
X(URL)
X(Date)
X(User)
X(Part_design_state)
X(Subsystem_prefix)
X(Part_number_local)
X(Part_state)
X(bool)
#undef X

template<int MIN,int MAX>
std::string sql_escape(DB,Int_limited<MIN,MAX> a){
	return escape(a);
}

#define X(A,B,C) std::string sql_escape(DB,B##_id const& a){ return escape(a); }
TABLES(X)
#undef X

std::string sql_escape(DB,Positive_decimal a){
	return escape(a.get());
}

std::string sql_escape(DB,int x){
	return as_string(x);
}

std::string sql_escape(DB,Weight a){
	return as_string(a);
}

std::string sql_escape(DB,Color a){
	return escape(a);
}

template<typename A,A B>
string sql_escape(DB db,Defaulted<A,B> const& a){
	return sql_escape(db,a.t);
}

template<typename T>
std::string sql_escape(DB,T t){
	PRINT(type(t));
	nyi
}

std::string sql_escape(DB,unsigned a){
	return as_string(a);
}

template<typename T>
std::string sql_escape(DB db,std::optional<T> const& a){
	if(a) return sql_escape(db,*a);
	return "NULL";
}

string sql_escape(DB db,string const& s){
	char to[s.size()*2+1];
	mysql_real_escape_string(db,to,s.c_str(),s.size());
	return string("'")+to+"'";
}

string sql_escape(DB db,Nonempty_string const& s){
	return sql_escape(db,s.get());
}

std::string sql_escape(DB db,Assembly_state a){
	return sql_escape(db,as_string(a));
}

std::string sql_escape1(DB db,Column_value const& a){
	return std::visit(
		[&](auto x){
			//PRINT(type(x));
			return sql_escape(db,x);
		},
		a
	);
}

std::string sql_escape(DB db,Column_value const& a){
	return sql_escape1(db,a);
}

Id new_item(DB db,Table_name const& table){
	run_cmd(db,"INSERT INTO `"+table+"` VALUES ()");
	return query_single<unsigned>(db,"SELECT LAST_INSERT_ID()");
}

std::optional<std::string> parse(
	std::optional<std::string> const*,
	std::string_view
);

Column_value parse_col(Typename const& type,std::string const& value){
	#define X(NAME) if(type==""#NAME){\
		NAME p=parse((NAME*)0,value);\
		return Column_value(p);\
	}
	STANDARD_COL_TYPES(X)
	X(Meeting_length)
	X(Color)
	X(std::optional<unsigned>)
	X(Dimension)
	#undef X
	PRINT(type);
	PRINT(value);
	nyi
}

using Adjust_result=std::variant<Get_cgi_var::Success,std::string>;

void drop_test_data(DB db,Id_table const& a){
	run_cmd(db,"DELETE FROM "+a+"_info WHERE edit_user='no_user'");
}

std::set<std::string> options(Id_table const*,bool){
	std::set<std::string> r;
	#define X(A,B,C) r|=string{""#A};
	TABLES(X)
	#undef X
	return r;
}

std::vector<Id_table> options(Id_table const*){
	std::vector<Id_table> r;
	#define X(A,B,C) r|=Id_table{""#A};
	TABLES(X)
	#undef X
	return r;
}


void drop_test_data(){
	auto db=connect_db();
	mapv([&](auto x){ drop_test_data(db.db,x); },options((Id_table*)0));
}

std::vector<std::string> const& table_names(){
	static std::vector<std::string> r;
	if(r.empty()){
		#define X(A,B,C) r|=""#A; r|=""#A "_info";
		TABLES(X)
		#undef X
	}
	return r;
}

Table_name::Table_name(char const *s):data(s){
	auto c=to_set(table_names()).count(s);
	if(!c){
		PRINT(s);
	}
	assert(c);
}

Table_name::Table_name(std::string const& s):data(s){
	auto c=to_set(table_names()).count(s);
	if(!c){
		throw "Invalid table name: \""+data+"\"";
	}
}

Table_name::operator std::string const&()const{
	return data;
}

std::ostream& operator<<(std::ostream& o,Table_name const& a){
	string s=a;
	return o<<s;
}

bool operator<(Table_name const& a,Table_name const& b){
	string a1=a,b1=b;
	return a1<b1;
}

bool operator==(Table_name const& a,char const *b){
	string a1=a;
	return a1==b;
}

string operator+(char const* a,Table_name const& b){
	string b1=b;
	return a+b1;
}

string operator+(std::string const& a,Table_name const& b){
	string b1=b;
	return a+b1;
}

std::string operator+(Table_name const& a,const char* b){
	string s=a;
	return s+b;
}

Table_name rand(Table_name const*){
	return Table_name{choose(table_names())};
}

Table_name parse(Table_name const*,std::optional<std::string_view> const& a){
	assert(a);
	return Table_name{string{*a}};
}

Id_table::Id_table(std::string a):data(std::move(a)){
	if(options((Id_table*)0,0).count(data)==0){
		throw "Invalid Id_table:"+data;
	}
}

Id_table::operator std::string const&()const{
	return data;
}

Id_table::operator Table_name()const{
	return Table_name{this->get()};
}

std::string const& Id_table::get()const{
	return data;
}

std::ostream& operator<<(std::ostream& o,Id_table const& a){
	return o<<a.get();
}

bool operator<(Id_table const& a,Id_table const& b){
	return a.get()<b.get();
}

std::string operator+(const char *a,Id_table const& b){
	return a+string{b};
}

bool operator==(Id_table const& a,Id_table const& b){
	return a.get()==b.get();
}

bool operator==(Id_table const& a,const char *b){
	return a.get()==b;
}

Id_table rand(Id_table const* x){
	return Id_table{choose(options(x))};
}

std::ostream& operator<<(std::ostream& o,Column_type const& a){
	o<<"Column_type(";
	#define X(A,B) o<<" "#B<<a.B;
	COLUMN_TYPE_ITEMS(X)
	#undef X
	return o<<" )";
}

bool operator==(Column_type const& a,Column_type const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	COLUMN_TYPE_ITEMS(X)
	#undef X
	return 1;
}

Column_name::Column_name(const char *s){
	assert(s);
	data=s;
}

Column_name::Column_name(string s):data(std::move(s)){}

Column_name::operator string const& ()const{
	return data;
}

std::string const& Column_name::get()const{
	return data;
}

std::ostream& operator<<(std::ostream& o,Column_name const& a){
	return o<<a.get();
}

bool operator<(Column_name const& a,Column_name const& b){
	return a.get()<b.get();
}

bool operator==(Column_name const& a,Column_name const& b){
	return a.get()==b.get();
}

std::string operator+(std::string const& a,Column_name const& b){
	return a+b.get();
}

std::string operator+(Column_name const& a,std::string const& b){
	return a.get()+b;
}

bool operator==(Column_name const& a,std::string const& b){
	return a.get()==b;
}

bool operator==(std::string const& a,Column_name const& b){
	return a==b.get();
}

bool operator==(Column_name const& a,const char* b){
	return a.get()==b;
}

std::vector<Column_name> options(Column_name const*){
	return firsts(flatten(values(expected_tables())));
}

Column_name rand(Column_name const* x){
	return choose(options(x));
}

bool operator==(Typename const& a,const char *b){
	return a.data==b;
}

bool operator==(Typename const& a,std::string const& b){
	return a.data==b;
}

std::string operator+(const char *a,Typename const& b){
	return a+b.data;
}
