#ifndef PARTING3D_H
#define PARTING3D_H

#include<set>
#include<map>
#include<decimal/decimal>
#include "data_types.h"

using Dim=Positive_decimal;
using Cost=std::decimal::decimal32;
using Part3=std::tuple<Dim,Dim,Dim>;
using Inventory3=std::multiset<Part3>;

std::optional<Inventory3> to_buy(
	std::map<Part3,Cost> const&,
	Inventory3 const&,
	Inventory3 const&
);

#endif
