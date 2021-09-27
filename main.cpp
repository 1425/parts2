#include "main.h"
#include<cstring>
#include<cgicc/HTTPHTMLHeader.h>
#include "db.h"
#include "page.h"
#include "status.h"
#include "latest.h"
#include "recent.h"
#include "export.h"
#include "rand.h"
#include "upload.h"
#include "extra.h"
#include "get_name.h"
#include "item.h"
#include "insert.h"
#include "variant_util.h"
#include "map_util.h"
#include "set_util.h"
#include "migrate.h"

using namespace std;

template<typename T>
T head(vector<T> const& v){
	assert(v.size());
	return v[0];
}

int system(string const& s){
	return system(s.c_str());
}

std::vector<string> args(int argc,char **argv){
	return mapf([&](auto i){ return string{argv[i]}; },range(1,argc));
}

string current_user(){
	{
		//In normal operation, this should always be set
		char *s=getenv("REMOTE_USER");
		if(s) return s;
	}
	{
		//When running on dev server w/o user auth enabled
		char *s=getenv("SERVER_NAME");
		if(s) return s;
	}
	//When running directly from command line
	return "no_user";
}

string navbar(){
	auto elements=vector{
		a(Home{},"Home"),
		a(Recent{},"Recent"),
		a(Orders{},"Orders"),
		a(Latest{},"Latest"),
		a(Status{},"Status"),
		//a(Upload{},"Upload"),
		//a(Page_raw{},"Raw"),
		//a(Export{},"Export"),
		a(Nested{},"Nested"),
		a(Extra{},"Extra")
	};
	//return p(join(elements))+"<hr>";
	return tag(
		"table id=navbar",
		tr(
			join(mapf(
				td,
				elements
			))
		)
	);
}

void page(
	std::ostream& o,
	string const& title1,
	string const& heading,
	string const& body1,
	string const& search
){
	return page(
		o,
		title1,
		heading,
		[&](std::ostream& o){ o<<body1; },
		search
	);
}

void page(
	std::ostream& o,
	string const& title1,
	string const& heading,
	std::function<void(std::ostream&)> const& body1,
	string const& search
){
	o<<"<html>";
	o<<head(
		title(title1+" - 1425 parts system")+
		//+"\n<link rel=\"stylesheet\" href=\"style.css\">"

		//This is starting to get large enough that it might make sense to link this in rather than include it inline in every page.
		"<style>"
		"body{"
			//"background-color: #ccc;"
			"border-left: 0px;"
		"}"
		//"table{ background-color: #fff; }"
		"h1{"
			"background-color: #fff;"
			//"color:red; "
			"border-bottom: thin solid red;"
			//"width: 110px;"
			//"border-left: 20 px; "
		"}"
		"h2,h3{"
			//"background-color: #fff;"
			"text-decoration: red underline;"
		"}"
		"table{"
			//background-color: #ddd;
			//"border: ridge; "
			"border-spacing: 0px;"
			"border-collapse: separate;"
		"}"
		/*"td{"
			"padding 10px;"
		"}"*/
		"#navbar{ background-color: #fff; }\n"
		".tooltip{"
			"position: relative;"
			"display: inline-block;"
			"border-bottom: 1px dotted black;"
		"}\n"
		".tooltip .tooltiptext{"
			"visibility: hidden;"
			//"width: 120px;"
			"background-color: #ffffcc;"
			"color: #fff;"
			"text-align: center;"
			"border-radius: 6px;"
			"padding: 5px 0;"
			"position: absolute;"
			"z-index: 1;"
			"top: -5px;"
			"left: 105%;"
		"}\n"
		".tooltip:hover .tooltiptext{"
			"visibility: visible;"
		"}\n"
		"html,body{"
			"height: 100%;"
			"margin:0;"
		"}"
		".content{"
			"min-height: 100%;"
		"}"
		".content-inside{"
			"padding: 1px;"
			"position: relative;"
			//"top: -15px;"
			"padding-left:10px;"
			"padding-bottom: 70px;"
		"}"
		".footer{"
			"height: 50px;"
			//"margin-top: -50px;"
			"background: #aaaaaa;"
		"}"
		"</style>"
	);
	o<<"<body>";
	o<<"<div class=\"content\">"
		"<div class=\"content-inside\">"
			+tag("table width=100%",tr(
				td(h1(heading+navbar()))+
				tag("td align=right","<form>"+
					hidden("page","search")+
					"<label for=\"search\">Search:</label>"
					"<input id=\"search\" type=\"search\" name=\"query\" value=\""+search+"\">"+
				"</form>")
			));
	body1(o);
	o<<"</div>"
	"</div>"
	"<div class=\"footer\">"
		"<table>"
		+tr(td("")+"<td height=10></td>")+
		"<tr>"
			"<td width=10></td>"
			"<td>"
				"<a href=\"https://wilsonvillerobotics.com\">1425</a> Parts System v2.0.  For questions, ask <a href=\"mailto:rippey.e@gmail.com\">Eric Rippey</a>"
				//"<div class=\"tooltip\">tip<span class=tooltiptext>thing&nbsp;here</span></div>" 
			"</td>"
		"</tr></table>"
	"</div>";
	o<<"</body>";
	o<<"</html>\n";
}

void page(
	std::ostream& o,
	string const& title1,
	string const& heading,
	string const& body1
){
	page(o,title1,heading,body1,"");
}

void page(std::ostream& o,string const& a,string const& b){
	page(o,a,a,b);
}

optional<string> maybe_get(cgicc::Cgicc const& cgi,string const& col){
	auto found=cgi.getElement(col);
	if(found==cgi.getElements().end()){
		return optional<string>{};
	}
	return **found;
}

template<typename T>
optional<T> po(T const* t,optional<string> const& s){ //po stands for parse optional
	if(s){ return optional<T>{}; }
	return parse(t,*s);
}

map<string,string> availabilities(DB db){
	auto q=query(db,"SELECT id,name "+current(Id_table("availability")));
	nyi
}

std::optional<Stock_id> parse(std::optional<Stock_id> const&,std::optional<string> const&);
std::optional<Stock_id> parse(std::optional<Stock_id> const&,string const&);

string parse(string const*,std::optional<string> const& a){
	assert(a);
	return *a;
}

int parse(int const*,std::optional<string_view> const& a){
	assert(a);
	return atoi(a->data());
}

std::map<Stock_id,std::string> stock_types(DB db){
	auto q=query_to<Stock_id,string>(db,"SELECT id,name "+current(Id_table{"stock"}));
	return to_map(q);
}

std::optional<Assembly_state> parse(Assembly_state const*,std::optional<int> const& x){
	if(!x) return {};
	PRINT(x);
	nyi
}

Assembly_state parse(Assembly_state const *a,std::optional<std::string> const& s){
	assert(s);
	return parse(a,std::string_view{*s});
}

#define X(A,B) void show(std::ostream&,DB,A const&);
PAGES(X)
#undef X

optional<Id_table> table(Typename type1){
	#define X(A,B,C) if(type1==""#B "_id"){ return Id_table{""#A}; }
	TABLES(X)
	#undef X

	#define X(A,B,C) if(type1=="std::optional<"#B "_id>") return Id_table{""#A};
	TABLES(X)
	#undef X

	return std::nullopt;
}

string escape(Id_table const& a){
	return a.get();
}

int graph(){
	auto e=table_types();
	vector<tuple<Id_table,Id_table,Column_name>> links;
	for(auto [this_table,table_type]:e){
		for(auto [col_name,col_type]:table_type){
			auto t=table(col_type);
			if(t){
				links|=make_tuple(this_table,*t,col_name);
			}
		}
	}

	//Filter out all the upload stuff because it makes the graph hard to look at.
	links=filter(
		[](auto x){
			return !contains(get<0>(x),"upload");
		},
		links
	);

	//could make this go use something in /tmp
	string filename="graph.dot";

	{
		ofstream o(filename);
		o<<"digraph G{\n";
		o<<"\trankdir=LR;\n";
		for(auto [a,b,c]:links){
			//The labels are not very helpful in practice, so omit them
			//o<<"\t"<<escape(a)<<"->"<<escape(b)<<" [label=\""+c+"\"];\n";
			o<<"\t"<<escape(a)<<"->"<<escape(b)<<";\n";
		}
		o<<"}\n";
	}

	string png_name="graph.png";
	{
		auto r=system("dot -Tpng "+filename+" > "+png_name);
		if(r) return r;
	}

	system("rm "+filename);
	return system("eog "+png_name);
}

string tutorial(){
	return h2("Tutorial")+
	h3("Inputting a design")+
	ol(
		li("Make a list of the off the shelf parts that will be used.  ")+
		li("In the navigation bar click on "+a(Latest{},"\"Latest\"")+".  ")+
		li("Under \"Table of Contents\" click on \"stock\".  This should scroll the window down to the \"stock\" section of the page.")+
		li("Click on the heading that says "+a(Id_table{"stock"},"\"stock\"")+".  A page should now appear that has just one table in it.  ")+
		li("For each of the types of off the shelf part, look through the table to see if that already exists.  If it does not:")+
		ol(
			li("The last row of the table should have a blue background.  Put a name for this type of stock in the name column and then hit the \"New\" button.  ")+
			li("The page should reload, now with an extra row in it with the name that was given in the last step.")
		)+
		li("Make a list of all of the parts that will have to be machined, or othewise manufactured in a way beyond assembly.")+
		li("In the navigation bar click on "+a(Latest{},"\"Latest\"")+".  ")+
		li("Under \"Table of Contents\" click on \"part_design\".  This should scroll the window down to the \"part_design\" section of the page.")+
		li("Click on the heading that says "+a(Id_table{"part_design"},"\"part_design\"")+".  A page should now appear that has just one table in it.  ")+
		li("For each of the types of part to be manufactured, look through the table to see if that already exists.  If it does not:")+
		ol(
			li("Find the last row of the table, which should have a blue background.  ")+
			li("Under \"name\" fill in a meaningful name for this type of part")+
			li("If this part is still being designed, make the \"state\" column say \"in_design\".  If not, list it as \"need_prints\".")+
			li("In the \"stock\" column, list what this is made out of.  This should exist because of the earlier steps in which stock types were added.")+
			li("Hit the \"New\" button.  ")+
			li("The page should reload, now with an extra row in it with the data that was given in the last step.")
		)+
		li("For each of the parts that was created in the last step, but which already have a drawing created:")+
		ol(
			li("Find the row of the part which should have a drawing attached.  ")+
			li("On the left hand column there should be a document icon.  Click on it.  ")+
			li("On the new page, there should be a section entitled \"upload_part_design\".  Below this there should be a button labeled \"Browse\".  Click on it to choose a file with the drawing, and then click the \"Upload\" button.  ")+
			li("Under the \"Current\" section, change \"state\" to \"need_build_plan\" and then hit \"Edit\".  ")+
			li("In the page heading, click on \"part_design\".  ")
		)+
		li("In the navigation bar click on "+a(Latest{},"\"Latest\"")+".  ")+
		li("Under \"Table of Contents\" click on \"assembly_design\".  This should scroll the window down to the \"assembly_design\" section of the page.")+
		li("Click on the heading that says "+a(Id_table{"assembly_design"},"\"assembly_design\"")+".  A page should now appear that has just one table in it.  ")+
		li("In the blue row at the bottom of the table, add a name and then click \"New\". ")+
		li("In the new row of the table with the chosen name, click on the document icon in the leftmost column.")+
		li("Scroll down to \"assembly_design_part\" table.")+
		li("For each of the manufactured parts that should be part of this design:")+
		ol(
			li("In the child column, a part design that should be part of this assembly.  ")+
			li("In the quantity column, choose the appropriate number.")+
			li("Hit \"New\"")
		)+
		li("Scroll down to the \"assembly_design_stock\" table.  ")+
		li("For each of the of the shelf parts that are to be assembled directly, follow the same procedure as for manufactured parts.")
	)+
	h3("Building a design")+
	ol(
		li("Create the top-level assembly")+
		ol(
			li("Go to "+a(Latest{},"Latest")+" in the navigration bar.")+
			li("Click on \"assembly\"")+
			li("Click on \"assembly\" header")+
			li("In the blue row and the \"assembly_design\" column, choose the type of assembly you'd like to create.")+
			li("Hit the \"New\" button.")+
			li("A new white-background row should appear with the given assembly design.  ")
		)+
		li("Make the system understand where needed stock can be ordered.")+
		ol(
			li("Click on "+a(Orders{},"Orders")+" in the navbar.")+
			li("Find the \"To order\" table.  ")+
			li("Find the \"Supplier\" column.  ")+
			li("For any row where the \"supplier\" column is empty:")+
			ol(
				li("Figure out where the item can be bought.")+
				li("Click the name column of that row.")+
				li("Find the \"availability\" table.  ")+
				li("Fill in the supplier, part number, and cost, then hit \"New\".")+
				li("Click on "+a(Orders{},"Orders")+" in the navbar.")+
				li("Find the \"To order\" table.  ")+
				li("Find the \"Supplier\" column.  There should now be a supplier listed.")
			)
		)+
		li("Record part orders")+
		ol(
			li("Click on "+a(Orders{},"Orders")+" in the navbar.")+
			li("Find the \"To order\" table.  ")+
			li("Find the \"Supplier\" column.  ")+
			li("Click on one of the supplier names.  ")+
			li("Under the \"New order\" section, adjust the details of how many are actually being bought, then hit \"Record order\".  ")
		)+
		li("Record part arrivals")+
		ol(
			li("Click on "+a(Orders{},"Orders")+" in the navbar.")+
			li("Find the \"On order\" table.  ")+
			li("If all of the items from the order have arrived, click \"Mark arrived\".  ")
		)+
		li("Adding stock into an assembly")+
		ol(
			li("Go to the page for the assembly")+
			ol(
				li("Go to \"Latest\" in the navbar.")+
				li("Find the \"assembly\" table.")+
				li("Find the row in the table which is that assembly.  ")+
				li("Click on the document icon in the leftmost column.  ")
			)+
			li("In the section labeled \"Stock\", there should be a subsection \"Needed\".  Click the \"Install\" button.  ")
		)+
		li("...")
	);
}

void show(std::ostream& o,DB,Home const&){
	auto t1="1425 Parts System v2.0";
	page(
		o,
		t1,
		"Welcome to the team 1425 parts system.  This is serves several purposes:"
		"<ol>"
		"<li>Keeping track of where we are in design and building of parts"
		"<li>Keeping track of parts that are on order and when they're expected back"
		"<li>Scheduling of work into meetings"
		"</ol>"
		"In order to do these things, the system keeps track of:"
		"<ul>"
		"<li>Materials that the team has on hand: "+a(Id_table{"stock"})+
		"<li>Where you might buy more materials: "+a(Id_table{"supplier"})+
		"<li>What materials each supplier has and how long it takes to get them: "+a(Id_table{"availability"})+
		"<li>Orders that have been make: "+a(Id_table{"order"})+
		"<li>What was in each of those orders: "+a(Id_table("order_member"))+
		"<li>Types of parts that the team manufactures: "+a(Id_table{"part_design"})+
		"<li>How to build parts: "+a(Id_table{"manufacture_step"})+
		"<li>Machines that are available to build parts with: "+a(Id_table{"machine"})+
		"<li>When the team is meeting: "+a(Id_table{"meeting"})+
		"<li>Where the team might meet: "+a(Id_table{"location"})+
		"<li>Parts that have actually been built or are in the process of being built: "+a(Id_table{"part"})+
		"<li>Types of things that the team assembles from parts or stock: "+a(Id_table{"assembly_design"})+
		"<li>What kinds of subassemblies are in an assembly: "+a(Id_table{"assembly_design_assembly"})+
		"<li>What kind of parts are directly in an assembly: "+a(Id_table{"assembly_design_part"})+
		"<li>What kind of stock are directly in an assembly: "+a(Id_table{"assembly_design_stock"})+
		"<li>Assemblies that actually exist: "+a(Id_table{"assembly"})+
		"<li>Drawings, G-code, pictures, or other data: "+a(Id_table{"upload"})+
		"<li>What a given upload is related to: "+ul(
			#define X(A,B,C) li(a(Id_table{""#A}))+
			UPLOAD_LINK_TABLES(X)
			#undef X
		"")+"</ul>"
		"To get started, consider looking at:"//take a minute to visit these links:"
		"<ul>"
		"<li>What people have been up to lately: "+a(Recent{},"Recent")+
		"<li>Plans for upcoming meetings: "+a(Status{},"Status")+
		"<li>An overview of the current state: "+a(Nested{},"Nested")+
		"</ul>"+tutorial()
	);
}

void show(ostream& o,DB db,Page_raw const& p){
	auto tables=keys(expected_tables());
	page(
		o,
		"Raw data"+[=](){
			stringstream ss;
			if(p.sort){
				ss<<" sorted by "<<p.sort->first;
			}
			if(p.filter){
				ss<<" filter:"<<p.filter;
			}
			return ss.str();
		}(),
		[&](){
			stringstream ss;
			ss<<"This is the raw history of all of the data that has been input into the program.";
			mapv(
				[&](auto x){ return show_table(ss,db,p,x); },
				tables
			);
			return ss.str();
		}()
	);
}

void show(ostream& o,DB,E404 const& a){
	page(o,"Error 404",as_string(a));
}

void show(ostream& o,DB db,Page_normal const& a){
	visit([&](auto x){ return show(o,db,x); },a);
}


bool print_available(DB &db,Part_design_id a){
	auto q=query_to<unsigned>(db,
		"SELECT COUNT(*) "+current(Id_table{"upload_part_design"})+
		"AND parent="+as_string(a.data)
	);
	assert(q.size()==1);
	return !!q[0];
}

Part_design_id get(Part_design_id const*,Column_value const& a){
	if(holds_alternative<Part_design_id>(a)){
		return get<Part_design_id>(a);
	}
	if(holds_alternative<int>(a)){
		return Part_design_id{get<int>(a)};
	}
	nyi
}

using Adjust_result=std::variant<Get_cgi_var::Success,std::string>;

Adjust_result adjust_data(
	DB db,
	Part_design_id main,
	Get_cgi_var::Success data
){
	auto d=data[Column_name{"state"}];
	if(!holds_alternative<Part_design_state>(d)){
		return "state is of wrong type";
	}
	auto state=get<Part_design_state>(d);
	switch(state){
		case Part_design_state::in_design:
		case Part_design_state::need_prints:
			return data;
		case Part_design_state::need_build_plan:
		case Part_design_state::need_to_cam:
		case Part_design_state::ready:{
			//check that a material has been selected
			auto stock=get<optional<Stock_id>>(data[Column_name{"stock"}]);
			auto link=a(Item{Id_table{"part_design"},main.data,{},{}},"Design page");
			if(!stock){
				return "Not valid because stock must be selected."+link;
			}
			//check that there is something uploaded that points here
			//PRINT(data["main"]);
			//PRINT(data["main"].index());
			//auto m=data["main"];
			//cout<<"as int:"<<get<int>(m)<<"\n";
			//cout<<"ok\n";
			if(!print_available(db,Part_design_id{main})){
				return "Not valid because a print must be uploaded before part design is set to NEED_TO_CAM or READY.  "+link;
			}
			cout<<"h4\n";
			return data;
		}
		default:
			assert(0);
	}
	nyi
}

set<Subsystem_prefix> prefixes(DB db){
	return to_set(query_to<Subsystem_prefix>(
		db,
		"SELECT prefix "+current(Id_table{"assembly_design"})
		+"AND prefix IS NOT NULL"
	));
}

optional<Subsystem_prefix> prefix(DB db,Assembly_design_id id){
	auto q=query_to<Subsystem_prefix>(db,
		"SELECT prefix FROM assembly_design_info WHERE "
		"(id) IN ("
			"SELECT MAX(id) FROM assembly_design_info WHERE main="+as_string(id.data)+
		") AND valid"
	);
	assert(q.size()==1);
	return q[0];
}

auto get_part_numbers(DB db,Id_table const& table){
	return to_set(query_to<Part_number>(
		db,
		"SELECT part_number "+current(table)
	));
}

set<Part_number> asm_part_numbers(DB &db){
	return get_part_numbers(db,Id_table{"assembly_design"});
}

set<Part_number> part_part_numbers(DB &db){
	return get_part_numbers(db,Id_table{"part_design"});
}

set<Part_number> stock_part_numbers(DB &db){
	return to_set(query_to<Part_number>(db,"SELECT part_number "+current(Id_table{"availability"})));
}

set<Part_number> part_numbers(DB &db){
	return asm_part_numbers(db)|part_part_numbers(db)|stock_part_numbers(db);
}

set<Part_number_local> part_numbers_with_prefix(DB db,Subsystem_prefix prefix){
	auto q=[&](string table){
		return to_set(query_to<Part_number_local>(
			db,
			"SELECT part_number "+current(Id_table{table})+
			"AND part_number LIKE '"+prefix.get()+"%-1425-%'"
		));
	};
	return q("part_design")|q("assembly_design");
}

Part_number get_pn(DB db,Assembly_design_id id){
	auto pf=prefix(db,id);
	if(pf){
		auto x=part_numbers_with_prefix(db,*pf);
		PRINT(x);
		
		nyi
	}
	nyi//parent(db,"assembly",main);
}

Adjust_result adjust_data(
	DB db,
	Assembly_design_id id,
	Get_cgi_var::Success data
){
	(void)db;
	(void)id;
	/*auto pn=get<Part_number_local>(data["part_number"]);
	if(pn==""){
		data["part_number"]=get_pn(db,id);
	}*/
	return data;
}

Adjust_result adjust_data(
	DB db,
	Id_table const& table,
	int main,
	Get_cgi_var::Success data
){
	//This is supposed to do things like consistency checks between columns and the 
	//automatic insertion of part numbers
	if(table=="part_design"){
		return adjust_data(db,Part_design_id{main},data);
	}
	if(table=="assembly_design"){
		return adjust_data(db,Assembly_design_id{main},data);
	}
	return data;
}

optional<string> check_links(DB db,map<Column_name,Column_value> const& values);
optional<string> check_redundant(DB db,Id_table const& table,map<Column_name,Column_value> const& values,std::optional<std::string> const&);

optional<string> existing_name(DB db,Page_edit const& p){
	if(!normal_name_tables().count(p.table)){
		return std::nullopt;
	}
	auto data=get<Get_cgi_var::Success>(p.values);
	return query_optional<string>(
		db,
		"SELECT name"+current(p.table)+" AND main="+sql_escape(db,p.main)
	);
}

void show(ostream& o,DB db,Page_edit const& p){
	if(holds_alternative<Get_cgi_var::Errors>(p.values)){
		auto errors=std::get<Get_cgi_var::Errors>(p.values);
		invalid_data(o,"edit",errors);
		return;
	}
	auto data=std::get<Get_cgi_var::Success>(p.values);
	auto d1=adjust_data(db,p.table,p.main,data);
	//return page("Edit","demo"+as_string(d1));
	if(!holds_alternative<Get_cgi_var::Success>(d1)){
		page(
			o,
			"Edit "+get_name(db,p.table,p.main,0),
			"Edit "+get_name(db,p.table,p.main,1),
			get<string>(d1)
		);
		return;
	}
	data=get<Get_cgi_var::Success>(d1);

	{
		auto c=check_column_names(p.table,keys(data));
		if(c){
			page(o,"Edit","Column mismatch");
			return;
		}
	}

	{
		auto c=check_redundant(db,p.table,data,existing_name(db,p));
		if(c){
			page(o,"edit",*c);
			return;
		}
	}

	{
		auto c=check_links(db,data);
		if(c){
			page(o,"edit",*c);
			return;
		}
	}

	auto q="INSERT INTO "+p.table+"_info ("+
		join(
			",",
			mapf(
				[](auto x){ return "`"+x+"`"; },
				vector<Column_name>{
					Column_name{"main"},
					Column_name{"valid"},
					Column_name{"edit_user"},
					Column_name{"edit_date"}
				}+to_vec(keys(data))
			)
		)+
		") VALUES ("+
		join(
			",",
			vector<string>{as_string(p.main),"1",sql_escape(db,current_user()),"now()"}+
			mapf(
				[&](auto x){ return sql_escape(db,x); },
				values(data)
			)
		)+
		")";
	//PRINT(q);
	run_cmd(db,q);
	redirect_page(o,"Edit",p.redirect);
}

void show(ostream& o,DB db,Page_delete const& p){
	auto x=query(db,"SELECT * "+current(p.table)+" AND main="+as_string(p.main));
	if(x.empty()){
		//If it doesn't seem to exist to begin with, then nothing needs to be done
		redirect_page(o,"Delete",p.redirect);
		return;
	}
	assert(x.size()==1);
	
	run_cmd(
		db,
		"INSERT INTO "+p.table+"_info("+
		join(
			",",
			mapf(
				[](auto x){ return "`"+x+"`"; },
				vector<Column_name>{
					Column_name{"main"},
					Column_name{"valid"},
					Column_name{"edit_user"},
					Column_name{"edit_date"}
				}+
				skip(5,column_names(Table_name{p.table.get()+"_info"}))
			)
		)
		+") VALUES ("+
		join(
			",",
			vector<string>{as_string(p.main),"0",sql_escape(db,current_user()),"now()"}+
			mapf([&](auto x){ return sql_escape(db,x); },skip(5,x[0]))
		)
		+")"
	);
	//return page("Delete","Item deleted.  This should redirect.  "+redirect_to(p.redirect));
	redirect_page(o,"Delete",p.redirect);
}

void show(ostream& o,DB db,Page const& page){
	return visit([&](auto x){ return show(o,db,x); },page);
}

int test(){
	auto db=connect_db();

	vector<Page> a;
	for(auto _:range(1000)){
		(void)_;
		a|=rand((Page*)0);
	}	
	PRINT(a.size());
	for(auto before:a){
		auto u=to_url(before);
		PRINT(before);

		//Don't test the deleting because this is annoying.
		if(!holds_alternative<Page_delete>(before)){
			stringstream o;
			show(o,db.db,before);
			//output is intentionally ignored.
		}

		//If get bad data, uncomment this for help figuring out where.
		//stringstream o;
		//show(o,db.db,Latest{});
	}
	return 0;
}

void show(ostream& o,DB,Page_upload_rx const&){
	o<<"It is not intended that this is actually accessible.  Request should have been intercepted upstream.";
}

void dump(DB db,Id_table const& table){
	PRINT(table);
	run_cmd(db,"DROP TABLE IF EXISTS `"+table.get()+"`");
	run_cmd(db,"DROP TABLE IF EXISTS `"+table.get()+"_info`");
}

int dump(){
	auto db=connect_db();

	auto f=[&](){
		int ok=0;
		int bad=0;
		try{
			#define X(A,B,C) dump(db.db,Id_table{""#A});
			TABLES(X)
			#undef X
			ok++;
		}catch(std::string const& s){
			cout<<"Error:"<<s<<"\n";
			bad++;
		}catch(...){
			nyi
		}
		return make_pair(ok,bad);
	};

	while(1){
		auto [good,bad]=f();
		if(good==0){
			if(bad){
				cout<<"Did not get all\n";
				return 1;
			}
			return 0;
		}
		if(bad==0){
			return 0;
		}
		PRINT(good);
		PRINT(bad);
		nyi
	}
}

auto run(auto x){
	try{
		return x();
	}catch(std::invalid_argument const& e){
		cout<<"inv:"<<e.what()<<"\n";
	}catch(std::string const& s){
		cout<<"caught:"<<s<<"\n";
	}catch(const char *s){
		assert(s);
		cout<<"caught:"<<s<<"\n";
	//}catch(Out_of_range const& a){
	//	cout<<a<<"\n";
	}
	exit(1);
}

bool operator==(string const& a,Id_table const& b){
	return a==b.get();
}

bool substring(string haystack,string needle){
	return !!strstr(haystack.c_str(),needle.c_str());
}

bool operator==(Column_name a,Id_table b){
	return string{a}==b;
}

string type_name(Id_table const& a){
	return char(toupper(a.get()[0]))+a.get().substr(1,a.get().size())+"_id";
}

vector<pair<Id_table,vector<Column_name>>> referred_by(Id_table const& target){
	vector<pair<Id_table,vector<Column_name>>> found;

	for(auto [table,cols]:table_types()){
		vector<Column_name> cols_found;
		for(auto [col_name,col_type]:cols){
			if(substring(as_string(col_type),type_name(target))){
				cols_found|=col_name;
			}
		}
		if(cols_found.size()){
			found|=make_pair(table,cols_found);
		}
	}
	return found;
}

Row_id cast(Id_table const& table,int id){
	(void)table;
	(void)id;
	#define X(A,B,C) if(table==""#A) return Row_id{B##_id{id}};
	TABLES(X)
	#undef X
	assert(0);
}

vector<Row_id> cast(Id_table const& table,vector<int> ids){
	return mapf([&](auto x){ return cast(table,x); },ids);
}

int as_int(Row_id a){
	#define X(A,B,C) if(holds_alternative<B##_id>(a)) return std::get<B##_id>(a).data;
	TABLES(X)
	#undef X
	assert(0);
}

string sql_escape(DB,Row_id a){
	/*#define X(A,B,C) if(holds_alternative<B##_id>(a)) return sql_escape(db,std::get<B##_id>(a));
	TABLES(X)
	#undef X
	assert(0);*/
	return as_string(as_int(a));
}

void delete_row(DB db,Row_id row){
	auto base_table=table(row);
	auto x=query(db,"SELECT * "+current(base_table)+" AND main="+as_string(as_int(row)));
	if(x.empty()){
		//If it doesn't seem to exist to begin with, then nothing needs to be done
		return;
	}
	assert(x.size()==1);
	
	run_cmd(
		db,
		"INSERT INTO "+base_table+"_info("+
		join(
			",",
			mapf(
				[](auto x){ return "`"+x+"`"; },
				vector<Column_name>{
					Column_name{"main"},
					Column_name{"valid"},
					Column_name{"edit_user"},
					Column_name{"edit_date"}
				}+
				skip(5,column_names(Table_name{base_table.get()+"_info"}))
			)
		)
		+") VALUES ("+
		join(
			",",
			vector<string>{as_string(as_int(row)),"0",sql_escape(db,current_user()),"now()"}+
			mapf([&](auto x){ return sql_escape(db,x); },skip(5,x[0]))
		)
		+")"
	);
}

void delete_rows(DB db,vector<Row_id> v){
	mapv([db](auto x){ delete_row(db,x); },v);
}

void check_dup(DB db,Id_table const& dedup_table,std::vector<std::string> const& items){
	//todo: at some point, put in a check to see that all of the "main" items are pointing to things that actually exist...

	auto s=query(
		db,
		"SELECT main,"+join(",",items)+current(dedup_table)
	);
	auto g=GROUP(tail,s);

	bool header=0;
	auto show_header=[&](){
		if(header) return;
		header=1;
		PRINT(dedup_table);
		PRINT(items);
	};

	for(auto [k,v]:g){
		auto v1=[&](){
			vector<Row_id> r;
			for(auto row:v){
				if(!row[0]) continue;
				r|=cast(dedup_table,stoi(*row[0]));
			}
			return r;
		}();

		if(v1.size()<2) continue;
		show_header();
		PRINT(k);
		cout<<v1<<"\n";

		//find all of the things that reference that, and redirect them

		auto r=referred_by(dedup_table);
		PRINT(r);

		auto key_keeping=v1[0];
		auto keys_to_rm=tail(v1);

		for(auto [edit_table,cols]:r){
			for(auto column_name:cols){
				//PRINT(to_edit);
				for(auto key_to_remove:keys_to_rm){
					auto q=query_to<int>(
						db,
						"SELECT main"+current(edit_table)+"AND "+column_name+"="+sql_escape(db,key_to_remove)
					);
					for(auto edit_row:cast(edit_table,q)){
						cout<<"Edit:"<<edit_row<<": "<<column_name<<"="<<key_keeping<<"\n";
						edit_single(db,edit_row,column_name,as_int(key_keeping));
					}
				}
			}
		}
		//then delete the extra entries

		delete_rows(db,keys_to_rm);
	}
}

int duplicates(){
	//try to find duplicate items... and them maybe try to dedup them
	auto db=connect_db();

	#define Y(A,B) v|=""#B;
	#define X(A,B,C) {\
		std::vector<std::string> v;\
		C(Y)\
		check_dup(db.db,Id_table{""#A},v);\
	}
	TABLES(X)
	#undef X
	#undef Y
	return 0;
}

int main(int argc,char **argv){
	auto a=args(argc,argv);

	using Flag=std::tuple<std::string,std::function<int(void)>,std::string>;
	std::vector<Flag> flags;

	flags|={"--test",[](){ return run(test); },"Self test"};
	flags|={"--graph",graph,"Show chart of how database tables relate"};
	flags|={"--dup",duplicates,"Examine duplicate items"},
	flags|={
		"--drop",
		[](){
			drop_test_data();
			return 0;
		},
		"Remove data added by the test user."
	};
	flags|=Flag{"--dump",[](){ return dump(); },"Remove all data"};
	flags|={"--migrate",migrate,"Copy data in from v1 database"};

	auto help=[&](){
		cout<<"Run with no arguments to be treated as a cgi script.\n\n";
		cout<<"Available options:\n";
		for(auto [name,f,description]:flags){
			cout<<"\t"<<name<<"\n\t\t"<<description<<"\n";
		}
		return 0;
	};
	flags|={"--help",help,"Show this message"};

	if(a.size()>1){
		cout<<"Unrecognized options given.\n";
		help();
		return 1;
	}

	if(a.size()==1){
		for(auto [flag,f,help]:flags){
			if(flag==a[0]){
				return f();
			}
		}
		cout<<"Error: Unrecognized command.\n";
		help();
		return 1;
	}

	run([](){
		cgicc::Cgicc cgi;
		auto db=connect_db();
		auto p=parse_page(db.db,cgi);
		populate(db.db);
		if(maybe_get(cgi,"page")=="upload_rx"){
			cout<<cgicc::HTTPHTMLHeader();
			Page_upload_rx x=get<Page_upload_rx>(p);
			upload(cout,cgi,db.db,x);
		}else if(
			maybe_get(cgi,"page")=="view" ||
			maybe_get(cgi,"page")=="export"
		){
			//these handled differently so that it doesn't send the HTML header.
			show(cout,db.db,p);
		}else{
			cout<<cgicc::HTTPHTMLHeader();
			show(cout,db.db,p);
		}
	});
	return 0;
}

std::string show_elem(
	std::pair<std::string,Column_type> const& a,
	std::optional<std::string> const& b
){
	auto db_type=a.second.type;
	if(contains(db_type,"blob")){
		return tag("td bgcolor=grey","Ignoring blob");
	}
	if(!b) return tag("td bgcolor=grey","");
	return td(as_string(b));
}

std::string show_elem(
	std::pair<std::string,Column_type> const& a,
	std::optional<std::string> const& b,
	std::optional<std::string> const& c
){
	auto db_type=a.second.type;
	if(contains(db_type,"blob")){
		return tag("td bgcolor=grey","Ignoring blob");
	}
	if(!b) return tag("td bgcolor=grey","");
	if(b==c){
		return td(as_string(b));
	}
	return tag("td bgcolor=\"#ccffcc\"",as_string(b));
}

