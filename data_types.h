#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include<iostream>
#include<vector>
#include<decimal/decimal>
#include "util.h"
#include "db_util.h"

std::string parse(const std::string*,std::string_view const&);
bool parse(const bool*,std::string_view const&);
unsigned parse(const unsigned*,std::string_view const&);
float parse(const float*,std::string_view const&);
double parse(const double*,std::string_view const&);

std::string to_db_type(const bool*);

template<typename T,T VALUE>
struct Defaulted{
	T t;
};

template<typename T,T VALUE>
bool operator<(Defaulted<T,VALUE> const& a,Defaulted<T,VALUE> const& b){
	return a.t<b.t;
}

template<typename T,T VALUE>
std::ostream& operator<<(std::ostream& o,Defaulted<T,VALUE> const& a){
	return o<<a.t;
}

template<typename T,T VALUE>
Defaulted<T,VALUE> rand(Defaulted<T,VALUE> const*){
	return Defaulted<T,VALUE>(rand((T*)0));
}

template<typename T,T VALUE>
std::string to_db_type(Defaulted<T,VALUE> const*){
	return to_db_type((T*)0);
}

template<typename T,T VALUE>
Defaulted<T,VALUE> parse(Defaulted<T,VALUE> const*,std::string_view const& s){
	return Defaulted<T,VALUE>{parse((T*)0,s)};
}

class Nonempty_string{
	std::string data;

	public:
	explicit Nonempty_string(std::string const&);

	operator std::string const&()const;
	std::string const& get()const;
};

std::ostream& operator<<(std::ostream&,Nonempty_string const&);
bool operator<(Nonempty_string const&,Nonempty_string const&);
Nonempty_string rand(Nonempty_string const*);
std::string to_db_type(Nonempty_string const*);
Nonempty_string parse(Nonempty_string const*,std::string_view const&);
std::string escape(Nonempty_string const&);
Nonempty_string operator+(std::string const&,Nonempty_string const&);
Nonempty_string operator+(Nonempty_string const&,std::string const&);

struct Input{
	std::string name,form,notes;
};

Input with_suggestions(std::string const& name,std::string const& value,std::vector<std::string> const& suggestions);
Input show_input(DB,std::string const& name,std::string const& value);
Input show_input(DB,std::string const& name,int value);
Input show_input(DB,std::string const& name,double value);
Input show_input(DB,std::string const& name,unsigned value);
Input show_input(DB,std::string const& name,bool value);
std::string to_db_type(const std::string*);
std::string to_db_type(bool const*);

std::string escape(std::string const&);

std::string escape(int);
int parse(const int*,std::string_view const&);
std::string to_db_type(const int*);
std::string to_db_type(const int unsigned*);
int rand(const int*);

using Decimal=std::decimal::decimal32;

std::ostream& operator<<(std::ostream&,Decimal const&);
Decimal rand(const Decimal*);
Decimal parse(const Decimal*,std::string_view const&);
std::string to_db_type(Decimal const *);
Input show_input(DB db,std::string const& name,Decimal value);
std::string escape(Decimal);

class Positive_decimal{
	Decimal data;

	public:
	explicit Positive_decimal(Decimal);
	Decimal get()const;
};
std::ostream& operator<<(std::ostream&,Positive_decimal const&);
Positive_decimal rand(Positive_decimal const*);
std::string to_db_type(Positive_decimal const*);
Positive_decimal parse(Positive_decimal const*,std::string_view const&);
bool operator<(Positive_decimal,Positive_decimal);
bool operator>=(Positive_decimal,Positive_decimal);
std::string escape(Positive_decimal);

Decimal operator-(Positive_decimal,Positive_decimal);
Decimal operator-(Decimal,Positive_decimal);
Decimal operator*(Positive_decimal,Positive_decimal);
Decimal operator*(Decimal,Positive_decimal);
Decimal sum(std::vector<Positive_decimal> const&);

template<typename Sub,typename Data>
struct Wrap{
	Data data;
};

template<typename Sub,typename Data>
std::string to_db_type(const Wrap<Sub,Data>*){
	return to_db_type((Data*)0);
}

template<typename Sub,typename Data>
bool operator==(Wrap<Sub,Data> const& a,Wrap<Sub,Data> const& b){
	return a.data==b.data;
}

template<typename Sub,typename Data>
bool operator!=(Wrap<Sub,Data> const& a,Wrap<Sub,Data> const& b){
	return a.data!=b.data;
}

template<typename Sub,typename Data>
bool operator<(Wrap<Sub,Data> const& a,Wrap<Sub,Data> const& b){
	return a.data<b.data;
}

template<typename Sub,typename Data>
bool operator>(Wrap<Sub,Data> const& a,Wrap<Sub,Data> const& b){
	return a.data>b.data;
}

template<typename Sub,typename Data>
std::ostream& operator<<(std::ostream& o,Wrap<Sub,Data> const& a){
	return o<<a.data;
}

template<typename Sub,typename Data>
Wrap<Sub,Data>& operator+=(Wrap<Sub,Data>& a,Wrap<Sub,Data> const& b){
	a.data+=b.data;
	return a;
}

/*template<typename Sub,typename Data,typename T>
auto operator>(Wrap<Sub,Data> const& a,T const& t){
	return a.data>t;
}*/

template<typename A,typename Sub,typename Data>
auto operator*(A const& a,Wrap<Sub,Data> const& b){
	return a*b.data;
}

template<typename Sub,typename Data>
Sub rand(const Wrap<Sub,Data>*){
	return Sub{rand((Data*)0)};
}

template<typename Sub,typename Data>
Sub parse(const Wrap<Sub,Data>*,std::string_view const& s){
	return Sub{parse((Data*)0,s)};
}

template<typename Sub,typename Data>
auto show_input(DB db,std::string const& name,Wrap<Sub,Data> const& current){
	return show_input(db,name,current.data);
}

template<typename Sub,typename Data>
std::string escape(Wrap<Sub,Data> const& a){
	return escape(a.data);
}

using Id=int;

struct Part_number:Wrap<Part_number,std::string>{};
std::string to_db_type(const Part_number*);
Input show_input(DB,std::string const&,Part_number const&);
bool operator==(Part_number const&,const char *);

struct Subsystem_id:Wrap<Subsystem_id,Id>{};
std::ostream& operator<<(std::ostream&,Subsystem_id const&);
Input show_input(DB,std::string const& name,Subsystem_id const& current);
Input show_input(DB,std::string const& name,std::optional<Subsystem_id> const& current);

struct User:Wrap<User,std::string>{};
std::string to_db_type(const User*);

struct Datetime:Wrap<Datetime,std::string>{};
std::string to_db_type(const Datetime*);

struct Out_of_range{
	int min,max,value;
};

std::ostream& operator<<(std::ostream&,Out_of_range const&);

template<int MIN,int MAX>
class Int_limited{
	//ends are inclusive.
	int value;

	public:
	Int_limited():value(MIN){}

	explicit Int_limited(int x):value(x){
		if(x<MIN || x>MAX) throw Out_of_range(MIN,MAX,value);
	}

	Int_limited& operator=(int i){
		value=i;
		return *this;
	}
	
	operator int()const{ return value; }

	int get()const{ return value; }
};

template<int MIN,int MAX>
Int_limited<MIN,MAX> rand(Int_limited<MIN,MAX> const*){
	return Int_limited<MIN,MAX>{MIN+rand()%(MAX+1-MIN)};
}

template<int A,int B>
int min(int a,Int_limited<A,B> b){
	return std::min(a,b.get());
}

template<int MIN,int MAX>
std::string to_db_type(Int_limited<MIN,MAX> const*){
	return to_db_type((int*)0);
}

template<int MIN,int MAX>
Int_limited<MIN,MAX> parse(Int_limited<MIN,MAX> const*,std::string_view const& s){
	return Int_limited<MIN,MAX>(atoi(s.data()));
}

template<int MIN,int MAX>
std::string escape(Int_limited<MIN,MAX> const& a){
	return as_string(a);
}

using Year=Int_limited<1000,2050>;
using Month=Int_limited<1,12>;
using Day=Int_limited<1,31>;

#define DATE_ITEMS(X)\
	X(Year,year)\
	X(Month,month)\
	X(Day,day)
struct Date{
	DATE_ITEMS(INST)
};
Date parse(Date const*,std::string_view const&);
Input show_input(DB db,std::string const& name,Date const& current);
Input show_input(DB db,std::string const& name,Date const*);
std::string to_db_type(const Date*);
bool operator==(Date const&,Date const&);
bool operator!=(Date const&,Date const&);
bool operator<(Date const&,Date const&);
std::ostream& operator<<(std::ostream&,Date const&);
std::string escape(Date const&);
Date rand(Date const*);
bool in_past(Date const&);
Date now();
Date operator+(Date,unsigned);

struct URL:Wrap<URL,std::string>{};
Input show_input(DB db,std::string const& name,URL const& value);
std::string to_db_type(URL const*);
URL rand(URL const*);

/*#define NO_ADD(X) std::optional<X>& operator+=(std::optional<X>& a,std::optional<X> const&);
NO_ADD(Subsystem_id)
NO_ADD(Part_id)*/

#define PART_STATES(X)\
	X(get_stock)\
	X(manufacture)\
	X(done)

#define PART_DESIGN_STATES(X)\
	X(in_design)\
	X(need_prints)\
	X(need_build_plan)\
	X(need_to_cam)\
	X(ready)

#define COMMA(A) A,

#define ENUM_DECL(NAME,OPTIONS)\
	enum class NAME{ OPTIONS(COMMA) };\
	std::ostream& operator<<(std::ostream&,NAME const&);\
	std::vector<NAME> options(const NAME*);\
	NAME rand(const NAME* a);\
	NAME parse(const NAME*,std::string_view const&);\
	Input show_input(DB db,std::string const& name,NAME const& value);\
	std::string escape(NAME const&);\
	std::string to_db_type(const NAME*);\
	std::optional<NAME>& operator+=(std::optional<NAME>&,std::optional<NAME>);\

ENUM_DECL(Part_state,PART_STATES)
ENUM_DECL(Part_design_state,PART_DESIGN_STATES)

#define BEND_TYPES(X) X(none) X(easy_90) X(complex_not_90_or_mult)
ENUM_DECL(Bend_type,BEND_TYPES)

#define ASSEMBLY_STATES(X)\
	X(in_design)\
	X(parts)\
	X(assembly)\
	X(done)
ENUM_DECL(Assembly_state,ASSEMBLY_STATES)

#define EXPORT_ITEMS(X)\
	X(SUBSYSTEM)\
	X(SUBSYSTEM_INFO)\
	X(PART)\
	X(PART_INFO)\
	X(MEETING)\
	X(MEETING_INFO)
ENUM_DECL(Export_item,EXPORT_ITEMS)

#define BOM_EXEMPTION_OPTIONS(X)\
	X(none)\
	X(KOP)\
	X(FIRST_Choice)
ENUM_DECL(Bom_exemption,BOM_EXEMPTION_OPTIONS)

#define BOM_CATEGORY_OPTIONS(X)\
	X(STANDARD)\
	X(DNI)\
	X(SUB_5D)\
	X(KOP)\
	X(FIRST_Choice)
ENUM_DECL(Bom_category,BOM_CATEGORY_OPTIONS)

template<typename T>
struct Suggest{
	std::string s;

	Suggest(){}
	Suggest(std::string s1):s(std::move(s1)){}
	Suggest(T);
	virtual std::vector<std::string> suggestions()const=0;
};

template<typename T>
bool operator==(Suggest<T> const& a,Suggest<T> const& b){
	return a.s==b.s;
}

template<typename T>
bool operator!=(Suggest<T> const& a,Suggest<T> const& b){
	return !(a==b);
}

template<typename T>
bool operator<(Suggest<T> const& a,Suggest<T> const& b){
	return a.s<b.s;
}

template<typename T>
std::ostream& operator<<(std::ostream& o,Suggest<T> const& a){
	return o<<a.s;
}

template<typename T>
T rand(const Suggest<T>*){
	T r;
	r.s=rand((std::string*)0);
	return r;
}

template<typename T>
std::string to_db_type(const Suggest<T>*){
	return "varchar(20)";
}

template<typename T>
T parse(const Suggest<T>*,std::string_view s){
	T r;
	r.s=s;
	return r;
}

template<typename T>
auto show_input(DB db,std::string const& name,Suggest<T> const& value){
	return with_suggestions(name,value.s,value.suggestions());
}

template<typename T>
std::string escape(Suggest<T> const& s){
	return escape(s.s);
}

/*struct Supplier:public Suggest<Supplier>{
	std::vector<std::string> suggestions()const;
};*/

struct Material:public Suggest<Material>{
	std::vector<std::string> suggestions()const;
};

template<typename T>
std::string to_db_type(const std::optional<T>*){
	return to_db_type((T*)nullptr);
}

template<typename A,typename B>
std::pair<A,B> rand(std::pair<A,B> const*){
	return std::make_pair(rand((A*)0),rand((B*)0));
}

template<typename T>
std::optional<T> rand(const std::optional<T>*){
	if(rand()%2) return rand((T*)0);
	return std::nullopt;
}

template<typename T>
std::optional<T> parse(const std::optional<T>*,std::string const& s){
	try{
		return parse((T*)0,s);
	}catch(...){
		return std::nullopt;
	}
}

template<typename T>
auto show_input(DB db,std::string const& name,std::optional<T> const& a){
	if(a) return show_input(db,name,*a);
	return show_input(db,name,T{});
}

template<typename T>
std::string escape(std::optional<T> const& a){
	if(a) return escape(*a);
	return "NULL";
}

struct Dummy{};
Dummy parse(const Dummy*,std::string_view const&);
std::ostream& operator<<(std::ostream&,Dummy);

class Subsystem_prefix{
	char a,b;

	public:
	Subsystem_prefix();
	Subsystem_prefix(char,char);

	std::string get()const;	
};
std::string to_db_type(const Subsystem_prefix*);
Subsystem_prefix parse(Subsystem_prefix const*,std::string_view const&);
Input show_input(DB,std::string const&,Subsystem_prefix const&);
std::string escape(Subsystem_prefix const&);
bool operator==(Subsystem_prefix const&,Subsystem_prefix const&);
bool operator!=(Subsystem_prefix const&,Subsystem_prefix const&);
bool operator<(Subsystem_prefix const&,Subsystem_prefix const&);
bool operator>(Subsystem_prefix const&,Subsystem_prefix const&);
std::ostream& operator<<(std::ostream&,Subsystem_prefix const&);
Subsystem_prefix rand(Subsystem_prefix const*);

class Three_digit{
	int value;//000-999 only.

	public:
	Three_digit();
	explicit Three_digit(int);
	Three_digit& operator=(int);
	operator int()const;
};
std::ostream& operator<<(std::ostream& o,Three_digit);
Three_digit rand(Three_digit const*);

struct Part_number_local{
	//Should look something like:
	//XX000-1425-2019
	Subsystem_prefix subsystem_prefix;
	Three_digit num;
	Year year;

	explicit Part_number_local(std::string const&);
	explicit Part_number_local(Part_number const&);
	Part_number_local(Subsystem_prefix,Three_digit,Year);

	std::string get()const;
};

std::ostream& operator<<(std::ostream&,Part_number_local const&);
bool operator<(Part_number_local const&,Part_number_local const&);
std::string escape(Part_number_local const&);
Part_number_local next(Part_number_local);
Part_number_local rand(Part_number_local const*);
std::string to_db_type(Part_number_local const*);
Part_number_local parse(Part_number_local const*,std::string_view const&);

/*struct Part_checkbox:Wrap<Part_checkbox,Part_id>{};
Input show_input(DB,std::string const&,Part_checkbox const&);
*/
struct Weight:Wrap<Weight,Decimal>{};

struct Valid:Wrap<Valid,bool>{
	Valid& operator=(bool);
};
Input show_input(DB,std::string const&,Valid const&);

struct DNI:Wrap<DNI,bool>{};
Input show_input(DB,std::string const&,DNI const&);

struct Hours:Wrap<Hours,Decimal>{};
Input show_input(DB,std::string const&,Hours const&);

struct Priority:Wrap<Priority,int>{};
Input show_input(DB,std::string const&,Priority const&);

struct Color:Wrap<Color,std::string>{};
Color rand(Color const*);

std::string viz_func();

#define UPDATE_TYPES(X)\
	X(INSERT)\
	X(UPDATE)\
	X(DELETE)
ENUM_DECL(Update_type,UPDATE_TYPES)

struct Upload_data{
	std::string inner;
};

std::ostream& operator<<(std::ostream&,Upload_data const&);
bool operator<(Upload_data const&,Upload_data const&);
Upload_data rand(Upload_data const*);
std::string to_db_type(Upload_data const*);
Upload_data parse(Upload_data const*,std::string_view const&);
std::string escape(Upload_data const&);
std::string sql_escape(DB,Upload_data const&);

using Positive_int=Int_limited<1,1000>;
std::string escape(Positive_int);
std::string sql_escape(DB,Positive_int);
Positive_int parse(Positive_int const*,std::string_view const&);
std::string to_db_type(Positive_int const*);

#endif
