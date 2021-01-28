#ifndef PARTING2D
#define PARTING2D

#include<set>
#include<map>
#include "parting.h"
#include "parting3d.h"

using Flat_part=std::pair<Dim,Dim>;
using Inventory=std::multiset<Flat_part>;

std::optional<Inventory> cut_out(Inventory stock,Inventory const& finished);

std::optional<Inventory> to_buy(
	std::map<Flat_part,Cost> const&,
	Inventory const&,
	Inventory const&
);

#endif
