#include "stock.h"
#include "db.h"
#include "page.h"
#include "item.h"
#include "insert.h"
#include "main.h"
#include "orders.h"

using namespace std;

Stock_category_id add_stock_category(DB db,Stock_new const& page){
	auto id=Stock_category_id{new_item(db,Table_name{"stock_category"})};

	auto dimensions=[&](){
		int i=0;
		if(page.thickness) i++;
		if(page.width) i++;
		if(page.length) i++;
		return i;
	}();

	insert(
		db,
		"stock_category_info",
		{
			{"main",escape(id)},
			{"valid","1"},
			{"edit_user",escape(current_user())},
			{"edit_date","now()"},
			{"name",escape(page.name)},
			{"dimensions",escape(dimensions)}
		}
	);

	return id;
}

Stock_id add_stock(DB db,Stock_category_id cat,Stock_new const& page){
	auto id=Stock_id{new_item(db,Table_name{"stock"})};
	insert(
		db,
		"stock_info",
		{
			{"main",escape(id)},
			{"valid","1"},
			{"edit_user",escape(current_user())},
			{"edit_date","now()"},
			{"category",escape(cat)},
			{"length",escape(page.length)},
			{"width",escape(page.width)},
			{"thickness",escape(page.thickness)},
			{"available",escape(page.available)},
			{"notes",escape(page.notes)},
			{"wishlist",escape(page.wishlist)}
		}
	);
	return id;
}

void add_availability(DB db,Stock_id stock,Stock_new const& page){
	auto id=Availability_id{new_item(db,Table_name{"availability"})};
	insert(
		db,
		"availability_info",
		{
			{"main",escape(id)},
			{"valid","1"},
			{"edit_user",escape(current_user())},
			{"edit_date","now()"},
			{"stock",escape(stock)},
			{"supplier",escape(page.supplier)},
			{"part_number",escape(page.part_number)},
			{"cost",escape(page.cost)},
			{"lead_time",escape(page.lead_time)},
			{"url",escape(page.url)}
		}
	);
}

void show(std::ostream& o,DB db,Stock_new const& page1){
	if(!exists(db,page1.supplier)){
		page(o,"Stock new: Error","Invalid supplier");
		return;
	}
	auto cat=add_stock_category(db,page1);
	auto stock=add_stock(db,cat,page1);
	add_availability(db,stock,page1);
	redirect_page(o,"Stock new",page_normal(stock));
}

void show(std::ostream& o,DB,Get_cgi_var::Errors const& a){
	invalid_data(o,"Stock new",a);
}

void show(std::ostream& o,DB db,Page_stock_new const& page1){
	visit([&](auto x){ show(o,db,x); },page1.data);
}
