#include "vars.h"
#include "latest.h"
#include "map_util.h"

using namespace std;

std::map<string,string> vars(Orders const&){
	return {{"page","orders"}};
}

std::map<string,string> vars(Home const&){ return {}; }

std::map<string,string> vars(Search const& a){
	map<string,string> r;
	r["page"]="search";
	r["query"]=a.query;
	return r;
}

std::map<string,string> vars(Export const& a){
	std::map<string,string> r;
	r["page"]="export";
	if(a.table){
		r["table"]=*a.table;
	}
	r["current"]=as_string(a.current);
	return r;
}

template<typename T>
std::map<string,string> vars_sort(T const& a){
	map<string,string> r;
	if(a.sort){
		r["sort_by"]=a.sort->first;
		r["sort_order"]=escape(a.sort->second);
	}
	return r;
}

template<typename T>
std::map<string,string> sort_filter(T a){
	std::map<string,string> m;
	if(a.filter){
		m["filter_col"]=a.filter->first;
		m["filter_value"]=escape(a.filter->second);
	}
	m|=vars_sort(a);
	return m;
}

std::map<string,string> vars(View const& a){
	map<string,string> r;
	r["page"]="view";
	r["main"]=as_string(a.main.data);
	return r;
}

std::map<string,string> vars(Page_raw const& a){
	map<string,string> m;
	m["page"]="raw";
	return m|sort_filter(a);
}

std::map<string,string> vars(Latest const& a){
	std::map<string,string> r;
	r["page"]="latest";
	if(a.table){
		r["table"]=*a.table;
	}
	return r|sort_filter(a);
}

std::map<string,string> vars(Recent const&){
	std::map<string,string> r;
	r["page"]="recent";
	return r;
}

std::map<string,string> vars(E404 const& a){
	std::map<string,string> r;
	r["page"]="E404";
	r["requested"]=a.requested;
	return r;
}

std::map<string,string> vars(Item const& a){
	std::map<string,string> r;
	r["page"]="item";
	r["table"]=a.table;
	r["main"]=as_string(a.main);
	return r|sort_filter(a);
}

std::map<string,string> vars(Status const& a){
	map<string,string> r;
	r["page"]="status";
	if(a.table){
		r["table"]=*a.table;
	}
	r|=vars_sort(a);
	return r;
}

std::map<string,string> vars(Extra const&){
	return {{"page","extra"}};
}

std::map<string,string> vars(Page_normal const& a){
	return visit([](auto x){ return vars(x); },a);
}

std::map<string,string> redirect_vars(std::optional<Page_normal> const& a){
	if(!a) return {};
	return vars(*a);
	nyi
}

std::map<std::string,std::string> vars(Page_edit const& a){
	std::map<string,string> m;
	m["page"]="edit";
	m["table"]=a.table;
	m["main"]=a.main;
	m|=redirect_vars(a.redirect);
	return m;
}

std::map<string,string> vars(Page_insert const& a){
	std::map<string,string> m;
	m["page"]="insert";
	m["table"]=a.table;
	m|=redirect_vars(a.redirect);
	return m;
}

std::map<string,string> vars(Page_delete const& a){
	std::map<string,string> m;
	m["page"]="delete";
	m["table"]=a.table;
	m["main"]=a.main;
	m|=redirect_vars(a.redirect);
	return m;
}

std::map<string,string> vars(Page_upload const&){
	return {{"page","upload"}};
}

std::map<string,string> vars(Page_upload_rx const&){
	return {{"page","upload_rx"}};
}

map<string,string> vars(Page_install const& a){
	map<string,string> r;
	r["page"]="install";
	r["into"]=escape(a.into);
	r["stock"]=escape(a.stock);
	r["quantity"]=escape(a.quantity);
	r|=redirect_vars(a.redirect);
	return r;
}

map<string,string> vars(Page_remove const& a){
	map<string,string> r;
	r["page"]="install";
	r["from"]=escape(a.from);
	r["stock"]=escape(a.stock);
	r["quantity"]=escape(a.quantity);
	r|=redirect_vars(a.redirect);
	return r;
}

map<string,string> vars(Page_arrived const& a){
	map<string,string> r;
	r["page"]="arrived";
	r["order"]=escape(a.order);
	r|=redirect_vars(a.redirect);
	return r;
}

std::map<string,string> vars(Nested const&){
	return {{"page","nested"}};
}

map<string,string> vars(Page_order const& a){
	map<string,string> r;
	r["page"]="order";
	r["supplier"]=escape(a.supplier);
	r["cost"]=escape(a.cost);
	r["who_paid"]=escape(a.who_paid);
	r["destination"]=escape(a.destination);
	r["expected_arrival"]=escape(a.expected_arrival);
	for(auto [i,p]:enumerate(a.members)){
		auto [type,n]=p;
		r["stock_"+as_string(i)]=escape(type);
		r["q_"+as_string(i)]=escape(n);
	}
	return r;
}

map<string,string> vars(Page_stock_new const& a){
	map<string,string> r;
	r["page"]="stock_new";
	if(holds_alternative<Stock_new>(a.data)){
		auto x=get<Stock_new>(a.data);
		#define X(A,B) r[""#B]=escape(x.B);
		STOCK_NEW_ITEMS(X)
		#undef X
	}
	return r;
}
