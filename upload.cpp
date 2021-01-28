#include "upload.h"
#include "util.h"
#include "base64.h"
#include "latest.h"
#include "extra.h"
#include "input.h"
#include "main.h"
#include "get_name.h"

using namespace std;

string sql_escape(DB db,Upload_data const& a){
	return sql_escape(db,a.inner);
}

#define X(A,B,C) \
string sql_escape(DB,B##_id a){\
	return escape(a.data);\
}\
string sql_escape(DB db,optional<B##_id> const& a){\
	if(!a) return "NULL";\
	return sql_escape(db,*a);\
}
TABLES(X)
#undef X

void show(std::ostream& out,DB db,Page_upload const&){
	stringstream o;
	o<<p("This is for uploading either drawings or G code.\n");
	o<<p(a(Latest{Id_table{"upload"},{},{}},"Already uploaded"));
	o<<h2("Upload new");
	o<<"<form method=\"post\" enctype=\"multipart/form-data\">";
	o<<hidden("page","upload_rx");
	o<<redirect_items(Latest{Id_table{"upload"},{},{}});
	//o<<td("<input type=\"file\" name=\"userfile\" accept=\"image/*\" />");
	//o<<"<p><input type=\"file\" name=\"userfile\" accept=\"image/*,.nc,.ncc,.tap,.gcode\" /></p>";
	o<<"<p><input type=\"file\" name=\"userfile\"/></p>";

	o<<"<table border>";
	o<<tr(th("Table")+th("Item"));
	#define X(A,B) o<<tr(\
		td(a(Latest{Id_table{""#B},{},{}},""#B))+\
		td(input_form(db,""#B,Typename{""#A}))\
	);
	PAGE_UPLOAD_RX_LINKS(X)
	#undef X
	o<<"</table>";

	o<<"<p><input type=\"submit\">";
	o<<"</form>";
	page(out,"Upload",o.str());
}

Upload_id add_data(DB db,std::string const& name,std::string const& datatype,Upload_data data){
	auto id=new_item(db,Table_name{"upload"});
	run_cmd(db,
		"INSERT INTO `upload_info` ("
			"main,valid,edit_user,edit_date,name,datatype,data"
		") VALUES ("
			+as_string(id)+","+
			"1,"
			+sql_escape(db,current_user())+","
			"now(),"
			+sql_escape(db,name)+","
			+sql_escape(db,datatype)+","
			+sql_escape(db,data)
		+")"
	);
	return Upload_id{id};
}

template<typename T>
void add_link(DB db,Upload_id upload,std::optional<T> const& t){
	if(!t) return;
	auto table_name=Id_table{"upload_"+table(&*t)};
	auto link_id=new_item(db,table_name);
	auto q="INSERT INTO `"+table_name+"_info` ("
		"main,valid,edit_user,edit_date,upload,parent"
		") VALUES ("
		+as_string(link_id)+","
		"1,"
		+sql_escape(db,current_user())+","+
		"now(),"
		+sql_escape(db,upload)+","+
		sql_escape(db,t)
		+")";
	run_cmd(db,q);
}

void add_link(DB db,Upload_id upload,Page_upload_rx const& page1){
	#define X(A,B) add_link(db,upload,page1.B);
	PAGE_UPLOAD_RX_LINKS(X)
	#undef X
}

void upload(ostream& o,cgicc::Cgicc &cgi,DB db,Page_upload_rx const& page1){
	cgicc::const_file_iterator file=cgi.getFile("userfile");
	if(file==cgi.getFiles().end()){
		page(o,"Upload file","Upload failed.  File was not received.\n");
		return;
	}

	auto id=add_data(
		db,
		file->getFilename(),
		file->getDataType(),
		Upload_data{file->getData()}
	);

	add_link(db,id,page1);

	redirect_page(o,"Upload rx",page1.redirect);
}
