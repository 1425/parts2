#ifndef STATUS_H
#define STATUS_H

#include "db.h"

struct Status;
struct View;

void show(std::ostream&,DB,Status const&);
std::string current(Id_table const&);

std::multiset<Assembly_design_id> assemblies(DB,Assembly_design_id);

#define PART_N_ITEMS(X)\
	X(Part_design_id,item)\
	X(std::optional<Stock_id>,stock)\
	X(std::vector<Manufacture_step_id>,ops)
//item for identifying reasons only, should not participate in scheduling

struct Part_n{
	PART_N_ITEMS(INST)
};

using Time=Decimal;

#define ASSEMBLY_N_ITEMS(X)\
	X(Assembly_design_id,item)\
	X(std::multiset<Stock_id>,stock)\
	X(std::vector<Part_n>,part)\
	X(std::vector<Assembly_n>,subs)\
	X(Time,time)
//item for identifying reasons; should not participate in scheduling

struct Assembly_n{
	ASSEMBLY_N_ITEMS(INST)
};

struct All_work{
	std::multiset<Part_design_id> parts_free;
	std::vector<Assembly_n> asm_needed;
	std::multiset<Stock_id> stock_on_hand;
	std::multiset<Assembly_design_id> asm_free;
};

All_work all_work(DB);

std::optional<Stock_id> stock(DB,Part_design_id);

#endif
