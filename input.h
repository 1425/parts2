#ifndef INPUT_H
#define INPUT_H

#include "db.h"

using Row_id=std::variant<
	#define X(A,B,C) B##_id,
	TABLES(X)
	#undef X
	bool //this option is not intended to actully be used and is here for syntax reasons
>;

Id_table table(Row_id);

std::string input_form(DB,Column_name const&,Column_value const&,Row_id);
std::string input_form(DB,std::string const&,Typename const&);
std::string input_form(DB,std::string const&,Column_value const&);

Part_design_id design(DB,Part_id);
std::optional<Assembly_design_id> design(DB,Assembly_id);

#define X(A,B,C) std::string current(B##_id);
TABLES(X)
#undef X
std::string current(Row_id);

std::string hidden(Column_name const&,std::string const&);

#endif
