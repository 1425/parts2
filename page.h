#ifndef PAGE_H
#define PAGE_H

#include<variant>
#include<string>
#include<cgicc/Cgicc.h>
#include "db.h"
#include "input.h"

#define HOME_ITEMS(X)

using Filter=std::pair<Column_name,Column_value>;

enum class Sort_order{ASC,DESC};
std::ostream& operator<<(std::ostream& o,Sort_order);
Sort_order rand(Sort_order const*);
std::optional<Sort_order> parse(Sort_order const*,std::string const&);
std::string escape(Sort_order);

using Sort=std::pair<Column_name,Sort_order>;

#define PAGE_RAW_ITEMS(X)\
	X(std::optional<Filter>,filter)\
	X(std::optional<Sort>,sort)

#define LATEST_ITEMS(X)\
	X(std::optional<Id_table>,table)\
	X(std::optional<Filter>,filter)\
	X(std::optional<Sort>,sort)

#define RECENT_ITEMS(X)

#define ITEM_ITEMS(X)\
	X(Id_table,table)\
	X(int,main)\
	X(std::optional<Filter>,filter)\
	X(std::optional<Sort>,sort)

#define STATUS_ITEMS(X)\
	X(std::optional<Table_name>,table)\
	X(std::optional<Sort>,sort)

#define VIEW_ITEMS(X)\
	X(Upload_id,main)

#define PAGE_UPLOAD_ITEMS(X)\

#define PAGE_EXPORT_ITEMS(X)\
	X(std::optional<Table_name>,table)\
	X(bool,current)

#define NESTED_ITEMS(X)

#define EXTRA_ITEMS(X)

#define SEARCH_ITEMS(X)\
	X(std::string,query)

#define ORDERS_ITEMS(X)

#define E404_ITEMS(X) X(std::string,requested)

#define NORMAL_PAGES(X)\
	X(Home,HOME_ITEMS)\
	X(Page_raw,PAGE_RAW_ITEMS)\
	X(Latest,LATEST_ITEMS)\
	X(Recent,RECENT_ITEMS)\
	X(Item,ITEM_ITEMS)\
	X(Status,STATUS_ITEMS)\
	X(Page_upload,PAGE_UPLOAD_ITEMS)\
	X(View,VIEW_ITEMS)\
	X(Export,PAGE_EXPORT_ITEMS)\
	X(Nested,NESTED_ITEMS)\
	X(Extra,EXTRA_ITEMS)\
	X(Search,SEARCH_ITEMS)\
	X(Orders,ORDERS_ITEMS)\
	X(E404,E404_ITEMS)\

#define PAGE(A,B) \
	struct A{ B(INST) };\
	std::ostream& operator<<(std::ostream&,A const&);\
	std::string to_url(A const&);\
	A rand(A const*);\
	bool operator<(A const&,A const&);

NORMAL_PAGES(PAGE)

using Page_normal=std::variant<
	Home,
	Page_raw,
	Latest,
	Recent,
	Item,
	Status,
	Page_upload,
	View,
	Export,
	Nested,
	Extra,
	Search,
	Orders,
	E404
>;

std::string to_url(Page_normal const&);

using Value=std::string;
using Values=std::map<Column_name,Value>;

#define PAGE_INSERT_ITEMS(X)\
	X(Id_table,table)\
	X(Get_cgi_var::Result,values)\
	X(std::optional<Page_normal>,redirect)

#define PAGE_DELETE_ITEMS(X)\
	X(Id_table,table)\
	X(int,main)\
	X(std::optional<Page_normal>,redirect)

#define PAGE_EDIT_ITEMS(X)\
	X(Id_table,table)\
	X(int,main)\
	X(Get_cgi_var::Result,values)\
	X(std::optional<Page_normal>,redirect)

#define PAGE_UPLOAD_RX_LINKS(X)\
	X(std::optional<Assembly_id>,assembly)\
	X(std::optional<Assembly_design_id>,assembly_design)\
	X(std::optional<Location_id>,location)\
	X(std::optional<Machine_id>,machine)\
	X(std::optional<Manufacture_step_id>,manufacture_step)\
	X(std::optional<Meeting_id>,meeting)\
	X(std::optional<Order_id>,order)\
	X(std::optional<Part_id>,part)\
	X(std::optional<Part_design_id>,part_design)\
	X(std::optional<Stock_id>,stock)\
	X(std::optional<Supplier_id>,supplier)

#define PAGE_UPLOAD_RX_ITEMS(X)\
	PAGE_UPLOAD_RX_LINKS(X)\
	X(std::optional<Page_normal>,redirect)

#define PAGE_INSTALL_ITEMS(X)\
	X(Assembly_id,into)\
	X(Stock_id,stock)\
	X(Positive_int,quantity)\
	X(std::optional<Page_normal>,redirect)

#define PAGE_REMOVE_ITEMS(X)\
	X(Assembly_id,from)\
	X(Stock_id,stock)\
	X(Positive_int,quantity)\
	X(std::optional<Page_normal>,redirect)

#define PAGE_ARRIVED_ITEMS(X)\
	X(Order_id,order)\
	X(std::optional<Page_normal>,redirect)

#define STOCK_NEW_ITEMS(X)\
	X(Nonempty_string,name)\
	X(Dimension,length)\
	X(Dimension,width)\
	X(Dimension,thickness)\
	X(unsigned,available)\
	X(std::string,notes)\
	X(std::optional<unsigned>,wishlist)\
	X(Supplier_id,supplier)\
	X(Part_number,part_number)\
	X(Positive_decimal,cost)\
	X(std::optional<Days>,lead_time)\
	X(URL,url)

struct Stock_new{
	STOCK_NEW_ITEMS(INST)
};
std::ostream& operator<<(std::ostream&,Stock_new const&);
bool operator<(Stock_new const&,Stock_new const&);
Stock_new rand(Stock_new const*);

using Stock_new_result=std::variant<Stock_new,Get_cgi_var::Errors>;

#define PAGE_STOCK_NEW_ITEMS(X)\
	X(Stock_new_result,data)

using Order_member_basic=std::pair<Stock_id,Positive_int>;

#define PAGE_ORDER_ITEMS(X)\
	X(Supplier_id,supplier)\
	X(std::optional<Positive_decimal>,cost)\
	X(std::optional<User>,who_paid)\
	X(std::optional<Location_id>,destination)\
	X(std::optional<Date>,expected_arrival)\
	X(std::vector<Order_member_basic>,members)\

#define MODIFY_PAGES(X)\
	X(Page_insert,PAGE_INSERT_ITEMS)\
	X(Page_delete,PAGE_DELETE_ITEMS)\
	X(Page_edit,PAGE_EDIT_ITEMS)\
	X(Page_upload_rx,PAGE_UPLOAD_RX_ITEMS)\
	X(Page_install,PAGE_INSTALL_ITEMS)\
	X(Page_remove,PAGE_REMOVE_ITEMS)\
	X(Page_arrived,PAGE_ARRIVED_ITEMS)\
	X(Page_order,PAGE_ORDER_ITEMS)\
	X(Page_stock_new,PAGE_STOCK_NEW_ITEMS)\

MODIFY_PAGES(PAGE)

#define PAGES(X)\
	NORMAL_PAGES(X)\
	MODIFY_PAGES(X)

using Page=std::variant<
	Page_normal,
	Page_insert,
	Page_delete,
	Page_edit,
	Page_upload_rx,
	Page_install,
	Page_remove,
	Page_arrived,
	Page_order,
	Page_stock_new
>;

Page parse_page(DB,cgicc::Cgicc&);
std::ostream& operator<<(std::ostream&,Page const&);
std::string to_url(Page const&);

std::map<std::string,std::string> vars(Page_normal const&);
std::string sort_links(Page_normal const&,Column_name const&);

template<typename T>
T operator|(T a,T const& b){
	a|=b;
	return a;
}

void redirect_page(std::ostream&,std::string const&,std::optional<Page_normal> const&);

std::string rand(Typename const&);
void edit_single(DB,Row_id,Column_name const&,Column_value const&);

#endif
