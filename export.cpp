#include "export.h"
#include<cgicc/HTTPHTMLHeader.h>
#include "db.h"
#include "base64.h"
#include "status.h"
#include "main.h"
#include "page.h"
#include "get_name.h"

using namespace std;

string export_table(){
	return tag(
		"table border",
		tr(join(mapf(
			th,
			vector<string>{"Table","Raw","Current"}
		)))+
		join(mapf(
			[](auto x){
				return tr(
					td(x)+
					td(a(Export{Table_name{x},0},"CSV"))+
					td([x]()->string{
						if(endswith(x,"_info")){
							return a(Export{Table_name{x},1},"CSV");
						}
						return "";
					}())
				);
			},
			table_names()
		))
	);
}

bool needs_escape(char c){
	if(isalpha(c) || isdigit(c)) return 0;
	if(contains(":;.-=\\/ '`~!@#$%^&*()_+?",c)) return 0;
	if(c=='"') return 1;
	if(contains("\r\n,",c)) return 1;
	PRINT(c);
	nyi;
}

bool needs_escape(string s){
	return any(MAP(needs_escape,s));
}

string csv_escape(string s){
	(void)s;
	if(!needs_escape(s)){
		return s;
	}
	stringstream o;
	o<<"\"";
	for(auto c:s){
		if(c=='"'){
			o<<"\"\"";
		}else{
			o<<c;
		}
	}
	o<<"\"";
	return o.str();
}

string csv_show( pair<pair<string,Column_type>,optional<string>> const& p){
	auto [col,data]=p;
	auto [name,col_type]=col;
	auto data_type=col_type.type;
	if(
		data_type=="tinyint(1)" ||
		data_type=="int(11)" || 
		data_type=="int(11) unsigned" ||
		data_type=="datetime" ||
		data_type=="decimal(8,3)" ||
		data_type=="date" ||
		startswith(data_type,"enum(")
	){
		if(data) return as_string(data);
		return "";
	}
	if(
		startswith(data_type,"varchar(")
	){
		if(!data) return "";
		return csv_escape(*data);
	}
	if(data_type=="longblob"){
		if(!data) return "";
		return base64_encode(*data);
	}
	
	PRINT(data_type);
	nyi
}

std::vector<Column_name> column_names(DB db,Table_name const& table){
	auto a=read(db,table);
	return firsts(a);
}

string export_table(DB db,Table_name const& a){
	//Could do a "SELECT ... INTO" except that would require extra permissions
	stringstream o;
	auto cols=read(db,a);
	o<<join(",",firsts(cols))<<"\r\n";
	for(auto row:query(db,"SELECT * FROM `"+a+"`")){
		vector<pair<pair<Column_name,Column_type>,optional<string>>> z=zip(cols,row);
		//o<<z;
		o<<join(",",mapf(csv_show,z));
		o<<"\r\n";
	}
	return o.str();
}

string export_current(DB db,Id_table const& table){
	/*if(!endswith(table,"_info")){
		return "Error: wrong table.";
	}*/
	auto q=query(db,"SELECT * "+current(table));
	auto cols=read(db,table+"_info");
	stringstream o;
	o<<join(",",firsts(cols))<<"\r\n";
	for(auto row:q){
		vector<pair<pair<Column_name,Column_type>,optional<string>>> z=zip(cols,row);
		//o<<z;
		o<<join(",",mapf(csv_show,z));
		o<<"\r\n";
	}
	return o.str();
}

void show(ostream& o,DB db,Export const& a){
	(void)a;
	if(!a.table){
		o<<cgicc::HTTPHTMLHeader();
		page(
			o,
			"Export",
			"This is for if you'd like to put the data into a spreadsheet or something."+
			export_table() //+as_string(mapf([=](auto x){ return export_table(db,x); },table_names()))
		);
		return;
	}
	//return page("Export","of "+as_string(a));
	if(a.current){
		if(!endswith(*a.table,"_info")){
			page(o,"Export","Error: wrong table.");
			return;
		}
		o<<"Content-type: text/csv\r\n";
		o<<"Content-Disposition: filename=\""<<*a.table<<"-current.csv\"\r\n\r\n";
		o<<export_current(db,Id_table{rm_suffix(*a.table,"_info")});
		return;
	}
	o<<"Content-type: text/csv\r\n";
	o<<"Content-Disposition: filename=\""<<*a.table<<".csv\"\r\n\r\n";
	o<<export_table(db,*a.table);
	return;
}
