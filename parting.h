#ifndef PARTING_H
#define PARTING_H

#include<set>
#include<map>
#include<vector>
#include<decimal/decimal>
#include "data_types.h"
#include "map_util.h"

using Part1=Positive_decimal;
using Inventory1=std::multiset<Part1>;
using Cost=std::decimal::decimal32;

Inventory1 to_buy(std::map<Part1,Cost> const&,Inventory1 const&,Inventory1 const&);

template<typename Item,typename Cost>
std::vector<std::multiset<Item>> by_cost(std::map<Item,Cost> available){
	auto cost=[&](std::multiset<Item> const& m){
		return sum(mapf(
			[&](auto x){ return available[x]; },
			m
		));
	};
	return sorted(
		to_vec(flatten(mapf(
			[&](auto x){ return of_size(x,keys(available)); },
			range(10)
		))),
		cost
	);
}

#endif
