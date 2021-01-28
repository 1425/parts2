#include "parting.h"
#include<iostream>
#include "util.h"
#include "set_util.h"

using namespace std;

using Part=Part1;
using Inventory=Inventory1;

static const Positive_decimal kerf=Positive_decimal{Decimal{1.0/8}};

using Cut=pair<Part,Positive_decimal>;//starting item, length at which to make cut

//This is just going to do the simplest thing to fit stuff in and not even
//attempt to figure out the most efficient way.

struct Too_small{};

pair<Cut,Inventory> plan(Inventory available,Part part){
	auto options=filter(
		[=](auto x){ return x>=part; },
		available
	);
	if(options.empty()) throw Too_small{};
	auto stock_size=min(options);
	available-=stock_size;
	auto leftover=stock_size-part-kerf;
	if(leftover>0){
		available|=Part{leftover};
	}
	return make_pair(Cut{stock_size,part},available);
}

pair<vector<Cut>,Inventory> plan(Inventory available,Inventory wanted){
	vector<Cut> cuts;
	for(auto part:wanted){
		auto [cut,new_inventory]=plan(available,part);
		cuts|=cut;
		available=new_inventory;
	}
	return make_pair(cuts,available);
}

Inventory to_buy(
	map<Part,Cost> const& could_be_bought,
	Inventory const& available1,
	Inventory const& wanted
){
	Inventory available=available1;
	Inventory still_needed;
	for(auto part:wanted){
		try{
			auto [cut,new_inventory]=plan(available,part);
			available=new_inventory;
		}catch(Too_small const&){
			still_needed|=part;
		}
	}
	for(auto opt:by_cost(could_be_bought)){
		try{
			plan(opt,still_needed);
			return opt;
		}catch(Too_small){
			//go on to the next option
		}
	}
	throw "Could not find with a reasonable number of parts.";
}

Inventory to_buy(set<Part> could_be_bought,Inventory available,Inventory wanted){
	Inventory still_needed;
	for(auto part:wanted){
		try{
			auto [cut,new_inventory]=plan(available,part);
			available=new_inventory;
		}catch(Too_small const&){
			still_needed|=part;
		}
	}
	for(auto size:range(10)){
		for(auto opt:of_size(size,could_be_bought)){
			try{
				plan(opt,still_needed);
				return opt;
			}catch(Too_small){
				//go on to the next option
			}
		}
	}
	throw "Could not find with a reasonable number of parts.";
}

int parting1_test2(){
	map<Part,Cost> w{
		{Part{1},Cost{1}},
		{Part{20},Cost{100}},
		{Part{100},Cost{1000}}
	};
	//PRINT(by_cost(w));
	try{
		auto x=to_buy(
			w,//{1,10,64},
			{},
			Inventory{
				Part{1},Part{2},Part{4},Part{8},Part{16},
				Part{32},Part{64}
			}
		);
		PRINT(x);
		return 0;
	}catch(const char *s){
		assert(s);
		cout<<s<<"\n";
		return 1;
	}
}

int parting1_test1(){
	Inventory available{
		Part{4},Part{8},Part{8},Part{6},Part{36}
	};
	Inventory needed{Part{1},Part{2},Part{4},Part{8},Part{16}};
	try{
		auto p=plan(available,needed);
		PRINT(p);
	}catch(char const* s){
		cout<<s<<"\n";
		return 1;
	}
	/*
	possible solution:
	4->4
	8->8
	36->16,19.875
	*/
	return 0;
}

/*int main(){
	parting1_test2();
	return 0;
}*/
