#include "data_types.h"
#include<cassert>
#include<strings.h>
#include "base64.h"
#include "status.h"

using namespace std;

std::ostream& operator<<(std::ostream& o,Out_of_range const& a){
	return o<<"Out_of_range("<<a.min<<" "<<a.max<<" "<<a.value<<")";
}

std::string parse(std::string const*,std::string const& a){
	return a;
}

string to_db_type(const bool*){ return "tinyint(1)"; }

string to_db_type(URL const*){
	//This is so long because AndyMark has sometimes started giving out addresses over 300 bytes long.
	return "varchar(1000)";
}

unsigned short rand(unsigned short const*){
	return rand();
}

string to_db_type(const int*){
	return "int(11)";
}

std::string to_db_type(const Part_number*){
	return "varchar(100)";
}

bool operator==(Part_number const& a,const char* b){
	return a.data==b;
}

Input show_input(DB db,std::string const& name,Part_number const& current){
	auto r=show_input(db,name,current.data);
	r.notes+="If left blank, will be automatically generated.";
	return r;
}

int rand(const int*){ return rand()%100; }

int parse(const int*,string const& s){ return stoi(s); }
unsigned int parse(const unsigned int*,string const& s){ return stoi(s); }

Input with_suggestions(string const& name,string const& value,vector<string> const& suggestions){
	stringstream ss;
	ss<<"<input name=\""<<name<<"\" list=\"list_"<<name<<"\" value=\""<<value<<"\">";
	ss<<"<datalist id=\"list_"<<name<<"\">";
	for(auto a:suggestions){
		ss<<"<option value=\""<<a<<"\">";
	}
	ss<<"</datalist>";
	return Input{name,ss.str(),""};
}

Input show_input(DB,string const& name,string const& value){
	//return "<br>"+name+":<input type=\"text\" name=\""+name+"\" value=\""+value+"\">";
	return Input{name,"<input type=\"text\" name=\""+name+"\" value=\""+value+"\">",""};
}

Input show_input(DB db,string const& name,int value){
	auto x=show_input(db,name,as_string(value));
	x.notes="(integer)";
	return x;
}

Input show_input(DB db,string const& name,double value){
	auto x=show_input(db,name,as_string(value));
	x.notes="(fraction)";
	return x;
}

Input show_input(DB db,string const& name,unsigned value){
	auto x=show_input(db,name,as_string(value));
	x.notes="(non-negative integer)";
	return x;
}

/*Input show_input(DB db,string const& name,bool value){
	//return "<br>"+name+":<input type=\"checkbox\" name=\""+name+"\" "+
	//	[=](){ if(value) return "checked=on"; return ""; }()+"\">";
	return Input{
		name,
		"<input type=\"checkbox\" name=\""+name+"\" "+
		[=](){ if(value) return "checked=on"; return ""; }()+"\">"
		""
	};
}*/

string escape(string const& s){
	stringstream ss;
	ss<<"\"";
	for(auto c:s){
		if(c=='"'){
			nyi
		}else{
			ss<<c;
		}
	}
	ss<<"\"";
	return ss.str();
}

string escape(int i){ return as_string(i); }

template<typename Value,typename Display>
Input drop_down(string const& name,Value const& current,vector<pair<Value,Display>> const& v){
	stringstream ss;
	ss<<"<select name=\""<<name<<"\">";
	for(auto elem:v){
		ss<<"<option value="<<escape(elem.first)<<"";
		if(elem.first==current) ss<<" selected=selected";
		ss<<">"<<elem.second<<"</option><br>\n";
	}
	ss<<"</select>";
	return Input{name,ss.str(),""};
}

template<typename T>
auto drop_down(string const& name,T const& current,vector<T> const& options){
	return drop_down(name,current,mapf([](auto x){ return make_pair(x,x); },options));
}

string to_db_type(const User*){ return "varchar(11)"; }

string to_db_type(const Datetime*){ return "datetime"; }

bool in_past(Date const& date){
	return date<now();
}

Date now(){
	time_t tt;
	time(&tt);
	struct tm *ti=localtime(&tt);
	assert(ti);
	return Date{Year{ti->tm_year+1900},Month{ti->tm_mon+1},Day{ti->tm_mday}};
}

Date operator+(Date orig,unsigned offset){
	//It seems like there should be a cleaner way to do this.
	struct tm tmp;
	tmp.tm_year=orig.year;
	tmp.tm_mon=orig.month;
	//this is defined to be understood by mktime even if off the end
	tmp.tm_mday=orig.day+offset;
	tmp.tm_hour=5;
	tmp.tm_isdst=0;
	tmp.tm_min=0;
	tmp.tm_sec=0;
	time_t t=mktime(&tmp);

	struct tm *after;
	after=localtime(&t);
	assert(after);
	return Date{
		Year{after->tm_year},
		Month{after->tm_mon},
		Day{after->tm_mday}
	};
}

Date parse(Date const*,std::string const& s){
	auto sp=split('-',s);
	if(sp.size()!=3) throw "Invalid date:"+s;
	return Date{
		parse((Year*)0,sp[0]),
		parse((Month*)0,sp[1]),
		parse((Day*)0,sp[2])
	};
	/*Date r;
	if(s=="0000-00-00"){

	}
	bzero(&r,sizeof(r));
	char *res=strptime(s.c_str(),"%Y-%m-%d",&r);
	if(!res){
		PRINT(s);
	}
	assert(res);
	assert(res[0]=='\0');
	return r;*/
}

bool operator<(Date const& a,Date const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	DATE_ITEMS(X)
	#undef X
	return 0;
}

bool operator==(Date const& a,Date const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	DATE_ITEMS(X)
	#undef X
	return 1;
}

bool operator!=(Date const& a,Date const& b){
	return !(a==b);
}

Year current_year(){
	return now().year;
}

std::ostream& operator<<(std::ostream& o,Date const& a){
	//Try to display as something like "Thursday, March 5"
	stringstream ss;
	ss<<a.year<<"-"<<a.month<<"-"<<a.day;
	if(a.year!=current_year()){
		return o<<ss.str();
	}
	struct tm tm1;
	bzero(&tm1,sizeof tm1);
	char *res=strptime(ss.str().c_str(),"%Y-%m-%d",&tm1);
	if(!res || res[0]) return o<<ss.str();
	static const size_t LEN=26;
	char s[LEN];
	strftime(s,LEN,"%a, %b %d",&tm1);
	return o<<s;
}

Date rand(Date const*){
	Date r;
	#define X(A,B) r.B=rand((A*)0);
	DATE_ITEMS(X)
	#undef X
	r.day=min(28,r.day); //just to make invalid less common.
	return r;
}

string escape(Date const& a){
	static const size_t LEN=26;
	char s[LEN];
	sprintf(s,"'%4d-%02d-%02d'",a.year.get(),a.month.get(),a.day.get());
	return s;
}

string escape_date(Date const& a){
	static const size_t LEN=26;
	char s[LEN];
	sprintf(s,"'%04d-%02d-%02d'",a.year.get(),a.month.get(),a.day.get());
	return s;
}

Input show_input(DB,string const& name,Date const& current){
	return Input{
		name,
		"<input type=\"date\" name=\""+name+"\" value="+escape_date(current)+">",
		""
	};
}

Input show_input(DB,string const& name,Date const*){
	return Input{
		name,
		"<input type=\"date\" name=\""+name+"\">",
		""
	};
}

string to_db_type(const Date*){ return "date"; }

Input show_input(DB db,string const& name,URL const& value){
	auto notes="Must start with something like \"http://\"";
	if(prefix("http",value.data)){
		return Input{
			"<a href=\""+value.data+"\">"+name+"</a>",
			"<input type=\"text\" name=\""+name+"\" value=\""+value.data+"\">",
			notes
		};
	}
	auto r=show_input(db,name,value.data);
	r.notes+=notes;
	return r;
}

#define E_PRINT(A) if(a==T::A) return o<<""#A;
#define E_LIST(A) v|=T::A;
#define E_PARSE(A) if(s==""#A) return T::A;
#define E_ESCAPED(A) values|=string{"'"#A "'"};

#define ENUM_INPUT(NAME,OPTIONS)\
	Input show_input(DB,string const& name,T const& value){\
		return drop_down(name,value,options(&value));\
	}\

#define ENUM_DEFS_MAIN(NAME,OPTIONS)\
	std::ostream& operator<<(std::ostream& o,NAME const& a){\
		OPTIONS(E_PRINT)\
		assert(0);\
	}\
	vector<NAME> options(const NAME*){\
		vector<NAME> v;\
		OPTIONS(E_LIST)\
		return v;\
	}\
	NAME rand(const NAME* a){\
		return choose(options(a));\
	}\
	NAME parse(const NAME*,string const& s){\
		OPTIONS(E_PARSE)\
		throw std::invalid_argument{""#NAME+string()+": "+s};\
	}\
	string escape(T const& a){ return "'"+as_string(a)+"'"; }\
	\
	string to_db_type(const T*){\
		vector<string> values;\
		OPTIONS(E_ESCAPED)\
		stringstream ss;\
		ss<<"enum(";\
		ss<<join(",",values);\
		ss<<")";\
		return ss.str();\
	}\
	std::optional<T>& operator+=(std::optional<T>& a,std::optional<T> ){\
		a={};\
		return a;\
	}

#define ENUM_DEFS(NAME,OPTIONS)\
	ENUM_DEFS_MAIN(NAME,OPTIONS)\
	ENUM_INPUT(NAME,OPTIONS)

#define T Part_state
ENUM_DEFS_MAIN(Part_state,PART_STATES)
#undef T

#define T Part_design_state
ENUM_DEFS_MAIN(Part_design_state,PART_DESIGN_STATES)
#undef T

#define T Update_type
ENUM_DEFS_MAIN(Update_type,UPDATE_TYPES)
#undef T

template<typename Value,typename Display>
Input drop_down_submit(string const& name,Value const& current,vector<pair<Value,Display>> const& v){
	stringstream ss;
	ss<<"<select name=\""<<name<<"\" id=\"part_state_select\" onchange=\"change_viz(1)\">";
	for(auto elem:v){
		ss<<"<option value=\""<<elem.first<<"\"";
		if(elem.first==current) ss<<" selected=selected";
		ss<<">"<<elem.second<<"</option><br>";
	}
	ss<<"</select>";
	return Input{name,ss.str(),""};
}

Input show_input(DB,std::string const& name,Part_state const& current){
	Input input=drop_down_submit(
		name,current,
		mapf(
			[](auto x){ return make_pair(x,x); },
			options(&current)
		)
	);
	stringstream ss;
	//input.form=ss.str()+input.form;
	input.form+=ss.str();
	return input;
}

#define T Bend_type
ENUM_DEFS(Bend_type,BEND_TYPES)
#undef T

#define T Export_item
ENUM_DEFS(Export_item,EXPORT_ITEMS)
#undef T

#define T Assembly_state
ENUM_DEFS(Assembly_state,ASSEMBLY_STATES)
#undef T

#define T Bom_exemption
ENUM_DEFS(Bom_exemption,BOM_EXEMPTION_OPTIONS)
#undef T

#define T Bom_category
ENUM_DEFS(Bom_category,BOM_CATEGORY_OPTIONS)
#undef T

using Decimal=decimal::decimal32;

string to_db_type(const Decimal*){
	return "decimal(8,3)";
}

std::ostream& operator<<(std::ostream& o,Decimal const& a){
	return o<<decimal32_to_double(a);
}

Decimal rand(const Decimal*){ return (rand()%1000)/100; }

Decimal parse(const Decimal*,string const& s1){
	bool negative=0;
	string s;
	if(s1.size() && s1[0]=='-'){
		negative=1;
		s=s1.c_str()+1;
	}else{
		s=s1;
	}
	auto sp=split('.',s);
	if(sp.size()==1){
		auto mag=std::decimal::make_decimal32((long long)stoi(sp[0]),0);
		if(negative) return -mag;
		return mag;
	}
	if(sp.size()==2){
		unsigned long long whole=[=](){
			if(sp[0].size()) return stoi(sp[0]);
			return 0;
		}();
		int frac;
		if(sp[1].size()==1){
			frac=stoi(sp[1])*100;
		}else if(sp[1].size()==2){
			frac=stoi(sp[1])*10;
		}else if(sp[1].size()==3){
			frac=stoi(sp[1]);
		}else{
			/*PRINT(s);
			PRINT(sp[1]);
			nyi*/
			throw "Max of 3 decimal places";
		}
		auto mag=std::decimal::make_decimal32(whole*1000+frac,-3);
		if(negative) return -mag;
		return mag;
	}
	throw "Not a decimal: \""+s1+"\"";
}

Input show_input(DB db,string const& name,Decimal value){
	auto x=show_input(db,name,as_string(value));
	x.notes="Max 3 decimal places";
	return x;
}

string escape(Decimal a){ return as_string(a); }

vector<string> Material::suggestions()const{
	return {
		".090 Aluminum Plate",
		"1/8 Aluminum 6061",
		"1/8 Aluminum 5052",
		"Steel",
		"1/4 Aluminun Plate",
		"1x1 1/16 Tube",
		"1x2 1/16 Tube",
		"1x1 1/8 Tube",
		"1x2 1/8 Tube",
		"1/2 Hex",
		"3/8 Hex",
		"80/20",
		"Delrin",
	};
}

string escape(Material const& s){
	return escape(s.s);
}

std::string to_db_type(Subsystem_prefix const*){
	return "varchar(2)";
}

Subsystem_prefix::Subsystem_prefix():a('A'),b('A'){}

Subsystem_prefix::Subsystem_prefix(char a1,char b1):a(a1),b(b1){
}

std::string Subsystem_prefix::get()const{
	return string()+a+b;
}

std::ostream& operator<<(std::ostream& o,Subsystem_prefix const& a){
	return o<<a.get();
}

bool operator<(Subsystem_prefix const& a,Subsystem_prefix const& b){
	return a.get()<b.get();
}

bool operator>(Subsystem_prefix const& a,Subsystem_prefix const& b){
	return a.get()<b.get();
}

bool operator==(Subsystem_prefix const& a,Subsystem_prefix const& b){
	return a.get()==b.get();
}

bool operator!=(Subsystem_prefix const& a,Subsystem_prefix const& b){
	return a.get()!=b.get();
}

Subsystem_prefix rand(Subsystem_prefix const*){
	return Subsystem_prefix(
		'A'+(rand()%26),
		'A'+(rand()%26)
	);
}

std::string escape(Subsystem_prefix const& a){
	return escape(a.get());
}

Subsystem_prefix parse(Subsystem_prefix const*,std::string const& s){
	if(s.size()!=2) throw "Bad prefix length";
	if(s[0]<'A' || s[1]>'Z') throw "Invalid char1";
	if(s[1]<'A' || s[1]>'Z') throw "Invalid char2";
	return Subsystem_prefix{s[0],s[1]};
}

Input show_input(DB db,string const& name,Subsystem_prefix const& a){
	auto x=show_input(db,name,as_string(a));
	x.notes=" Two upper case characters";
	return x;
}

Part_number_local::Part_number_local(std::string const& a){
	//XX000-1425-2020
	//15 chars
	if(a.size()!=15) throw "Part_number_local: wrong length ("+as_string(a.size())+" , expected 15)";
	subsystem_prefix=Subsystem_prefix(a[0],a[1]);

	{
		auto n=a.substr(2,3);
		for(auto c:n){
			if(!isdigit(c)){
				throw "Part_number_local: expected digit";
			}
		}
		num=stoi(n);
	}

	auto team_sect=a.substr(5,6);
	if(team_sect!="-1425-"){
		throw "Part_number_local: Missing team section";
	}

	auto left=a.substr(11,200);
	Year y(stoi(left));
	if(y>now().year){
		throw "Invalid year suffix";
	}
	year=y;
}

Part_number_local::Part_number_local(Part_number const& a):
	Part_number_local(a.data)
{}

Part_number_local::Part_number_local(Subsystem_prefix a,Three_digit b,Year c):
	subsystem_prefix(a),num(b),year(c)
{}

std::string Part_number_local::get()const{
	stringstream ss;
	ss<<subsystem_prefix<<num<<"-1425-"<<year;
	return ss.str();
}

std::ostream& operator<<(std::ostream& o,Part_number_local const& a){
	return o<<a.get();
}

bool operator<(Part_number_local const& a,Part_number_local const& b){
	#define X(A) if(a.A<b.A) return 1; if(a.A>b.A) return 0;
	X(subsystem_prefix)
	X(num)
	#undef X
	return 0;
}

std::string escape(Part_number_local const& a){
	return "'"+a.get()+"'";
}

Part_number_local next(Part_number_local a){
	a.num=a.num+1;
	return a;
}

Part_number_local rand(Part_number_local const*){
	return Part_number_local{
		rand((Subsystem_prefix*)0),
		rand((Three_digit*)0),
		rand((Year*)0)
	};
}

string to_db_type(Part_number_local const*){
	//could make this a lot shorter.
	return to_db_type((string*)0);
}

Part_number_local parse(Part_number_local const*,string const& s){
	return Part_number_local{s};
}

Three_digit::Three_digit():value(0){}

Three_digit::Three_digit(int x):value(x){
	assert(x>=0 && x<=999);
}

Three_digit& Three_digit::operator=(int i){
	if(i<0 || i>999) throw "Invalid Three_digit";
	value=i;
	return *this;
}

Three_digit::operator int()const{
	return value;
}

ostream& operator<<(std::ostream& o,Three_digit a){
	char s[10];
	sprintf(s,"%03d",int(a));
	return o<<s;
}

Three_digit rand(Three_digit const*){
	return Three_digit{rand()%1000};
}

bool parse(const bool*,string const& s){
        //This is how the database keeps track of it
        if(s=="1") return 1;
        if(s=="0") return 0;

        //This is how HTML forms make it appear
        if(s=="on") return 1;
        if(s=="off") return 0;

        //struct X{};
        //throw X{};
	throw invalid_argument("for bool:"+as_string(s));
	PRINT(s);
	nyi
}

Upload_data rand(Upload_data const*){
	return Upload_data{rand((string*)0)};
}

std::ostream& operator<<(std::ostream& o,Upload_data const&){
	return o<<"Upload_data(...)";
}

bool operator<(Upload_data const& a,Upload_data const& b){
	return a.inner<b.inner;
}

Upload_data parse(Upload_data const*,std::string const& s){
	return Upload_data{s};
}

std::string to_db_type(Upload_data const*){
	return "longblob";//up to 4GB
}

std::string escape(Upload_data const& a){
	return base64_encode(a.inner);
}

Nonempty_string::Nonempty_string(string const& s):data(s){
	if(data.empty()) throw invalid_argument("string was empty");
}

Nonempty_string::operator std::string const&()const{
	return data;
}

std::string const& Nonempty_string::get()const{
	return data;
}

bool operator<(Nonempty_string const& a,Nonempty_string const& b){
	return a.get()<b.get();
}

Nonempty_string operator+(std::string const& a,Nonempty_string const& b){
	return Nonempty_string{a+b.get()};
}

Nonempty_string operator+(Nonempty_string const& a,std::string const& b){
	return Nonempty_string{a.get()+b};
}

std::ostream& operator<<(std::ostream& o,Nonempty_string const& a){
	return o<<a.get();
}

Nonempty_string rand(Nonempty_string const*){
	char c='A'+rand()%26;
	return Nonempty_string{string{}+c};
}

string to_db_type(Nonempty_string const*){
	return to_db_type((string*)0);
}

Nonempty_string parse(Nonempty_string const*,string const& s){
	return Nonempty_string(s);
}

string escape(Nonempty_string const& a){
	return escape(a.get());
}

Positive_decimal::Positive_decimal(Decimal a):data(a){
	if(!(data>0)) throw invalid_argument("was not positive");
}

Decimal Positive_decimal::get()const{
	return data;
}

std::ostream& operator<<(std::ostream& o,Positive_decimal const& a){
	return o<<a.get();
}

bool operator<(Positive_decimal a,Positive_decimal b){
	return a.get()<b.get();
}

bool operator>=(Positive_decimal a,Positive_decimal b){
	return a.get()>=b.get();
}

Decimal operator-(Positive_decimal a,Positive_decimal b){
	return a.get()-b.get();
}

Decimal operator-(Decimal a,Positive_decimal b){
	return a-b.get();
}

Decimal operator*(Positive_decimal a,Positive_decimal b){
	//result might be 0 because could run out of precision.
	return a.get()*b.get();
}

Decimal operator*(Decimal a,Positive_decimal b){
	return a*b.get();
}

Decimal sum(vector<Positive_decimal>)nyi

Positive_decimal rand(Positive_decimal const*){
	auto r=rand((Decimal*)0);
	if(r>0) return Positive_decimal{r};
	if(r==0) return Positive_decimal{1};
	return Positive_decimal{-r};
}

std::string escape(Positive_decimal a){
	return escape(a.get());
}

std::string to_db_type(Positive_decimal const*){
	return to_db_type((Decimal*)0);
}

Positive_decimal parse(Positive_decimal const*,std::string const& s){
	return Positive_decimal{parse((Decimal*)0,s)};
}

std::string sql_escape(DB,Positive_int a){
	return escape(a);
}

std::string escape(Positive_int a){
	return as_string(a);
}

Positive_int parse(Positive_int const*,std::string const& s){
	return Positive_int(stoi(s));
}

string to_db_type(Positive_int const*){
	return to_db_type((unsigned*)0);
}

Color rand(Color const*){
	return choose(vector<Color>{
		Color{"red"},
		Color{"green"},
		Color{"blue"},
		Color{"#000000"}
	});
}

URL rand(URL const*){
	return choose(vector<URL>{
		URL{"http://google.com"},
		URL{"http://yahoo.com"}
	});
}
