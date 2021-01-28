#include "nested.h"
#include "status.h"
#include "extra.h"
#include "main.h"
#include "get_name.h"
#include "page.h"
#include "map_util.h"
#include "set_util.h"

using namespace std;

namespace{

string td_t(string const& a){
	return tag("td valign=top",a);
}

string show(DB,Stock_id);

set<Assembly_design_id> assembly_designs(DB& db){
	return to_set(query_to<Assembly_design_id>(
		db,
		"SELECT main "+current(Id_table{"assembly_design"})
	));
}

set<Part_design_id> part_designs(DB& db){
	return to_set(query_to<Part_design_id>(
		db,
		"SELECT main "+current(Id_table{"part_design"})
	));
}

set<Stock_id> stock(DB& db){
	return to_set(query_to<Stock_id>(
		db,
		"SELECT main"+current(Id_table{"stock"})
	));
}

set<Assembly_id> free_assemblies(DB db,Assembly_design_id a){
	return to_set(query_to<Assembly_id>(db,
		"SELECT main "+current(Id_table{"assembly"})+
		"AND parent IS NULL "
		"AND assembly_design="+as_string(a.data)
	));
}

set<Part_design_id> assembly_design_parts(DB db,Assembly_design_id a){
	return to_set(query_to<Part_design_id>(db,
		"SELECT child "+current(Id_table{"assembly_design_part"})+
		"AND parent="+as_string(a.data)
	));
}

set<Assembly_design_id> assembly_design_assemblies(DB db,Assembly_design_id a){
	return to_set(query_to<Assembly_design_id>(db,
		"SELECT child "+current(Id_table{"assembly_design_assembly"})+
		"AND parent="+as_string(a.data)
	));
}

set<Stock_id> assembly_design_stock(DB db,Assembly_design_id a){
	return to_set(query_to<Stock_id>(db,
		"SELECT child "+current(Id_table{"assembly_design_stock"})+
		"AND parent="+as_string(a.data)
	));
}

set<Part_id> free_parts(DB db,Part_design_id a){
	return to_set(query_to<Part_id>(db,
		"SELECT main"+current(Id_table{"part"})+
		" AND part_design="+as_string(a.data)
	));
}

string table2(string a,string b){
	return tag("table border",tr(td_t(a)+td_t(b)));
}

string color(Part_state const& a){
	switch(a){
		case Part_state::get_stock:
			//blue
			return "#ccccff";
		case Part_state::manufacture:
			//white
			return "#ffffff";
		case Part_state::done:
			//green
			return "#ccffcc";
		default:
			assert(0);
	}
}

string color(Part_design_state const& a){
	switch(a){
		case Part_design_state::in_design:
		case Part_design_state::need_prints:
			//red
			return "#ffcccc";
		case Part_design_state::need_build_plan:
		case Part_design_state::need_to_cam:
			//yellow
			return "#ffffcc";
		case Part_design_state::ready:
			//green
			return "#ccffcc";
		default:
			assert(0);
	}
}

string color(std::optional<Part_design_state> const& a){
	if(a) return color(*a);
	return "#cccccc";
}

Part_state state(DB db,Part_id id){
	return query_single<Part_state>(
		db,
		"SELECT state "+current(Id_table{"part"})+" AND main="+as_string(id.data)
	);
}

optional<Part_design_state> state(DB db,Part_design_id id){
	return query_optional<Part_design_state>(
		db,
		"SELECT state "+current(Id_table{"part_design"})+" AND main="+as_string(id.data)

	);
}

string show(DB db,Part_id id){
	//should probably show 
	//return td(a(id,get_name(db,id)));
	return tag(
		"td bgcolor=\""+color(state(db,id))+"\"",
		a(id,get_name(db,id))
	);
}

vector<Manufacture_step_id> manufacture_steps(DB db,Part_design_id id){
	return query_to<Manufacture_step_id>(
		db,
		"SELECT main"+current(Id_table{"manufacture_step"})+
		" AND part_design="+escape(id)
	);
}

template<typename T>
string show(DB db,optional<T> const& a){
	if(!a) return "";
	return show(db,*a);
}

string show(DB db,Manufacture_step_id id){
	/*auto q=query_single<optional<Machine_id>,Positive_decimal>(
		db,
		"SELECT machine,time"+current(id)
	);*/
	return td(a(db,id));
}

/*string show(DB db,Stock_id id){
	return td(get_name(db,id));
}*/

string show(DB db,Part_design_id id){
	/*return td(table2(
		a(id,get_name(db,id)),
		//could make this show stock later
		table(join(mapf(
			[&](auto x){ return tr(show(db,x)); },
			free_parts(db,id)
		)))
	));*/
	return tag(
		"td bgcolor=\""+color(state(db,id))+"\"",
		table2(
			a(id,get_name(db,id)),
			//could make this show stock later
			table2(
				a(
					Latest{
						Id_table{"part"},
						Filter{
							Column_name{"part_design"},
							Column_value{id}
						},
						{}
					},
					"Examples"
				),
				table(join(mapf(
					[&](auto x){ return tr(show(db,x)); },
					free_parts(db,id)
				)))
			)+
			table2(
				"Stock",
				table(tr(show(db,stock(db,id))))
			)+
			table2(
				a(
					Latest{
						Id_table{"manufacture_step"},
						Filter{
							Column_name{"part_design"},
							Column_value{id}
						},
						{}
					},
					"Steps"
				),
				table(join(mapf(
					[&](auto x){ return tr(show(db,x)); },
					manufacture_steps(db,id)
				)))
			)
		)
	);
}

/*string show(DB db,Assembly_design_part_id a){
	//could do something to show quantity here.
	//return show(db,part_design(db,a));
	auto p=part_design(db,a);
	if(!p) return as_string(a);
	return show(db,*p);
}*/

string show(DB,unsigned a){ return as_string(a); }

template<typename A,typename B>
string show(DB db,std::pair<A,B> const& a){
	return show(db,a.first)+show(db,a.second);
}

template<typename T>
string show(DB db,std::set<T> const& a){
	return td(join(mapf(
		[&](auto x){ return show(db,x); },
		a
	)));
}

template<typename T>
string show(DB db,vector<T> const& a){
	return join(mapf([&](auto x){ return show(db,x); },a));
}

template<typename K,typename V>
string show(DB db,std::map<K,V> const& m){
	return show(db,to_vec(m));
}

set<Part_id> parts(DB db,Assembly_id id){
	return to_set(query_to<Part_id>(
		db,
		"SELECT main "+current(Id_table{"part"})+" AND parent="+as_string(id.data)
	));
}

map<Stock_id,unsigned> stock(DB db,Assembly_id a){
	auto q=query_to<Stock_id,unsigned>(db,
		"SELECT stock,quantity "+current(Id_table{"stock_install"})+
		" AND assembly="+as_string(a.data)
	);
	map<Stock_id,unsigned> r;
	for(auto [stock,qty]:q){
		r[stock]+=qty;
	}
	return r;
}

string show(DB db,Assembly_id id){
	return td(table2(
		a(id,get_name(db,id)),
		[&](){
			stringstream ss;
			ss<<"<table border>";
			ss<<"<tr>";
			ss<<td(a(
				Latest{Id_table{"part"},Filter{"parent",id},{}},
				"Parts"
			));
			ss<<show(db,parts(db,id));
			ss<<"</tr>";
			ss<<"<tr>";
			ss<<td(a(
				Latest{Id_table{"assembly_design_stock"},Filter{"parent",id},{}},
				"Stock"
			));
			ss<<show(db,stock(db,id));
			ss<<"</tr>";
			ss<<"</table>";
			return ss.str();
		}()
	));
}

string show(DB db,Stock_id id){
	//could later make this show more the qty on hand.
	return td(
		a(id,get_name(db,id)) //+" "+as_string(qty(db,id))
	);
}

string unhide(string const& id){
	//return " <span onclick=\"document.getElementById('"+id+"').style.display='block';\">&#9658;</span>";
	return " <span onclick=\"toggle('"+id+"');\">&#9658;</span>";
}

string hidden_table(string id){
	return "<table id=\""+id+"\" style=\"display:none;\">";
}

string hidden(string id,string body){
	return hidden_table(id)+tr(td(body))+"</table>";
}

string hidden_table(string id,string body){
	return hidden_table(id)+body+"</table>";
}

string show(DB &db,Assembly_design_id id){
	auto name="ad_"+as_string(id);
	auto ex_name=name+"_ex";

	auto ex=table2(
			a(
				Page{Latest{Id_table{"assembly"},Filter{"assembly_design",id},{}}},
				"Examples"
			)+unhide(ex_name),
			hidden_table(
				ex_name,
				join(mapf(
					[&](auto x){ return tr(show(db,x)); },
					free_assemblies(db,id)
				))
			)
		);

	auto subs=table2(
			a(
				Latest{Id_table{"assembly_design_assembly"},Filter{"parent",id},{}},
				"Subassemblies"
			),
			table(join(mapf(
				[&](auto x){ return tr(show(db,x)); },
				assembly_design_assemblies(db,id)
			)))
		);

	auto parts=table2(
			a(
				Page{Latest{
					Id_table{"assembly_design_part"},Filter{"parent",id},{}
				}},
				"Parts"
			),
			table(join(mapf(
				[&](auto x){ return tr(show(db,x)); },
				assembly_design_parts(db,id)
			)))
		);

	auto stock=table2(
			a(
				Latest{
					Id_table{"assembly_design_stock"},
					Filter{"parent",id},{}
				},
				"Stock"
			),
			table(join(mapf(
				[&](auto x){ return tr(show(db,x)); },
				assembly_design_stock(db,id)
			)))
		);
	return td(table2(
		a(id,get_name(db,id))+unhide(name),
		hidden(name,ex+subs+parts+stock)
	));
}


}

void show(ostream& o,DB db,Nested const&){
	o<<"<script>\n";
	o<<"function toggle(name){\n";
	o<<"	var x=document.getElementById(name);\n";
	o<<"	if(x.style.display=='none')\n";
	o<<"		x.style.display='block';\n";
	o<<"	else\n";
	o<<"		x.style.display='none';\n";
	o<<"}\n";
	o<<"</script>\n";
	{
		auto c=loop_check(db);
		if(c){
			page(o,"Nested","Aborted."+*c);
			return;
		}
	}
	stringstream ss;
	ss<<"Click on arrows to expand.\n";
	ss<<"<table border>";

	ss<<"<tr>";
	ss<<td_t(
		a(Id_table{"assembly_design"},"Assembly designs")
		+unhide("assembly_designs")
		//+" <span onclick=\"document.getElementById('assembly_designs').style.display='block';\">&#9658;</span>"
		//+"<div id=\"testdiv\" style=\"display:none\">Hello</div>"
	);
	ss<<"<td>";
	//ss<<"<table id=\"assembly_designs\" style=\"display:none;\">";
	ss<<hidden_table("assembly_designs");
	for(auto a:assembly_designs(db)){
		ss<<tr(show(db,a));
	}
	ss<<"</table>";
	ss<<"</td>";
	ss<<"</tr>";

	ss<<"<tr>";
	ss<<td_t(
		a(Id_table{"part_design"},"Part designs")
		+unhide("part_designs")
	);
	ss<<"<td>";
	//ss<<"<table>";
	ss<<hidden_table("part_designs");
	for(auto a:part_designs(db)){
		ss<<tr(show(db,a));
	}
	ss<<"</table>";
	ss<<"</td>";
	ss<<"</tr>";

	ss<<"<tr>";
	ss<<td_t(
		a(Id_table{"stock"},"Stock")
		+unhide("stock")
	);
	ss<<"<td>";
	//ss<<"<table border>";
	ss<<hidden_table("stock");
	for(auto a:stock(db)){
		ss<<tr(show(db,a));
	}
	ss<<"</table>";
	ss<<"</td>";
	ss<<"</tr>";

	ss<<"</table>";

	page(o,"Nested view",ss.str());
}

