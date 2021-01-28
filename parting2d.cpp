#include "parting2d.h"
#include "util.h"
#include "parting.h"
#include "set_util.h"

using namespace std;

static const Dim kerf=Positive_decimal{Decimal{1.0/8}};

Flat_part flip(Flat_part p){
	return make_pair(p.second,p.first);
}

bool fits_normal(Flat_part stock,Flat_part finished){
	return stock.first<finished.first && stock.second<finished.second;
}

bool fits(Flat_part stock,Flat_part finished){
	return fits_normal(stock,finished) || fits_normal(flip(stock),finished);
}

optional<multiset<Flat_part>> cut_out_simple(Flat_part stock,Flat_part finished){
	//going to assume that everything is going to stay in rectangles, so for every cut there are two cuts made, turning everything into four parts.
	//next level would be to make it turn into max of 3 parts instead
	
	//cut 1 is vertical
	if(stock.first<finished.first){
		return std::nullopt;
	}
	if(stock.second<finished.second){
		return std::nullopt;
	}

	auto v1=finished.first;
	auto v2=stock.first-finished.first-kerf;
	auto h1=finished.second;
	auto h2=stock.second-finished.second-kerf;
	multiset<Flat_part> r;
	r|=finished;//upper left
	if(v2>0){
		if(h2>0){
			r|=Flat_part{v2,h2};//lower right
		}
		//lower left
		r|=Flat_part{v2,h1};
	}
	if(h2>0){
		//upper right
		r|=Flat_part{v1,h2};
	}
	return r;
}

auto value(Flat_part part){
	return part.first*part.second;
}

auto value(Inventory inventory){
	return sum(MAP(value,inventory));
}

#define ARGMAX(A,B) argmax([&](auto x){ return (A)(x); },(B))

optional<multiset<Flat_part>> cut_out(Flat_part stock,Flat_part finished){
	vector<multiset<Flat_part>> v;
	v|=cut_out_simple(stock,finished);
	v|=cut_out_simple(flip(stock),finished);
	if(v.empty()){
		return std::nullopt;
	}
	return ARGMAX(value,v);
}

optional<multiset<Flat_part>> cut_out(Inventory stock,Flat_part finished){
	//returns set of leftover pieces
	auto m=non_null(mapf(
		[=](auto x)->optional<multiset<Flat_part>>{
			auto c=cut_out(x,finished);
			if(!c) return std::nullopt;
			return *c|(stock-x);
		},
		stock
	));
	if(m.empty()){
		return std::nullopt;
	}
	return argmax(
		[](auto x){ return value(x); },
		m
	);
}

optional<multiset<Flat_part>> cut_out(Inventory stock,Inventory const& finished){
	//going to go with largest to smallest items as they're being fit
	//for now, we're not worrying about how the cuts are made just the shapes that result.  To be helpful to users, might want to make it so that they get a cut list.
	auto s=sort_by(finished,[](auto x){ return -value(x); });
	for(auto part:s){
		//PRINT(part);
		//PRINT(stock);
		auto result=cut_out(stock,part);
		if(!result){
			return result;
		}
		*result-=part;
		stock=*result;
	}
	return stock;
}

int parting2_test1(){
	Inventory available{
		Flat_part{10,20},
		Flat_part{4,4},
		Flat_part{4,5},
		Flat_part{Positive_decimal{Decimal{8.5}},11}
	};
	Inventory needed{
		Flat_part{1,10},
		Flat_part{3,40},
		Flat_part{5,5},
		Flat_part{5,5},
		Flat_part{5,5},
		//{5,10},
		//{5,5},
		//{5,5},
		//{5,5},
		//{5,5}
	};
	auto p=cut_out(available,needed);
	PRINT(p);
	return 0;
}

//#define SORTED(A,B) sorted(A,[&](auto x){ return (B)(x); }

std::optional<Inventory> to_buy(
	std::map<Flat_part,Cost> const& for_purchase,
	Inventory const& available1,
	Inventory const& wanted
){
	(void)for_purchase;
	(void)wanted;

	Inventory available=available1;
	Inventory still_needed;

	//for(auto part:reversed(SORTED(wanted,value))){
	for(auto part:wanted){
		auto x=cut_out(available,part);
		if(x){
			available=*x;
		}else{
			still_needed|=part;
		}
	}
	for(auto opt:by_cost(for_purchase)){
		auto x=cut_out(opt,still_needed);
		if(x) return opt;
	}
	return std::nullopt;
}

void parting2_test2(){
	map<Flat_part,Cost> av{
		{Flat_part{1,2},1},
		{Flat_part{4,5},10},
		{Flat_part{40,40},1000}
	};
	auto x=to_buy(
		av,
		Inventory{},
		Inventory{
			Flat_part{2,3}
		}
	);
	PRINT(x);
}

/*int main(){
	//return test1();
	test2();
}*/
