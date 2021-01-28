#include "parting3d.h"
#include "util.h"
#include "parting.h"
#include "data_types.h"
#include "set_util.h"

using namespace std;

static const Dim kerf=Dim{Decimal{1.0/8}};

tuple<Part3,Part3,Part3,Part3,Part3,Part3> orientations(Part3 p){
	auto x=get<0>(p);
	auto y=get<1>(p);
	auto z=get<2>(p);
	return make_tuple(
		Part3{x,y,z},
		Part3{x,z,y},
		Part3{y,x,z},
		Part3{y,z,x},
		Part3{z,x,y},
		Part3{z,y,x}
	);
}

optional<multiset<Part3>> cut_out(Part3 large,Part3 small){
	if(
		get<0>(large) < get<0>(small) ||
		get<1>(large) < get<1>(small) ||
		get<2>(large) < get<2>(small)
	){
		return std::nullopt;
	}
	auto [x,y,z]=small;
	auto nx=get<0>(large)-get<0>(small)-kerf;
	auto ny=get<1>(large)-get<1>(small)-kerf;
	auto nz=get<2>(large)-get<2>(small)-kerf;

	vector<Dim> xs;
	xs|=x;
	if(nx>0) xs|=Dim{nx};

	vector<Dim> ys;
	ys|=y;
	if(ny>0) ys|=Dim{ny};

	vector<Dim> zs;
	zs|=z;
	if(nz>0) zs|=Dim{nz};

	multiset<Part3> extra;
	for(auto x:xs){
		for(auto y:ys){
			for(auto z:zs){
				extra|=make_tuple(x,y,z);
			}
		}
	}
	extra-=make_tuple(x,y,z);

	return extra;
}

auto value(Part3 part){
	return product(part);
}

auto value(Inventory3 inv){
	return sum(MAP(value,inv));
}

optional<multiset<Part3>> cut_out(Inventory3 available,Part3 part){
	auto m=non_null(mapf(
		[&](auto x)->optional<Inventory3>{
			auto r=cut_out(x,part);
			if(!r) return r;
			return (available-x)|*r;
		},
		available
	));
	if(m.empty()){
		return std::nullopt;
	}
	return ARGMAX(value,m);
}

optional<Inventory3> cut_out(Inventory3 available,Inventory3 parts){
	for(auto part:SORTED(parts,value)){
		auto c=cut_out(available,part);
		if(!c) return std::nullopt;
		available=*c;
	}
	return available;
}

std::optional<Inventory3> to_buy(
	std::map<Part3,Cost> const& from_suppliers,
	Inventory3 const& on_hand,
	Inventory3 const& wanted
){
	Inventory3 available=on_hand;
	Inventory3 still_needed;
	for(auto w:wanted){
		auto x=cut_out(available,w);
		if(!x){
			still_needed|=w;
		}else{
			available=*x;
		}
	}

	for(auto x:by_cost(from_suppliers)){
		auto v=cut_out(x,still_needed);
		if(v){
			return x;
		}
	}

	return std::nullopt;
}

void parting3d_test(){
	map<Part3,Cost> from_suppliers{
		{Part3{5,5,5},4}
	};
	auto x=to_buy(
		from_suppliers,
		{},
		multiset<Part3>{
			Part3{1,2,3},
			Part3{1,2,3},
			Part3{1,2,3},
			Part3{1,2,3},

			Part3{1,2,3},
			Part3{1,2,3},
			Part3{1,2,3},
			Part3{1,2,3},

			Part3{1,2,3},
		}
	);
	PRINT(x);
}

/*int main(){
	test();
}*/
