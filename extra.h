#ifndef EXTRA_H
#define EXTRA_H

#include "db.h"

struct Extra;

void show(std::ostream&,DB,Extra const&);

#define X(A,B,C) std::set<B##_id> current_options(DB,B##_id const*);
TABLES(X)
#undef X

std::optional<std::string> loop_check(DB);

#define X(A,B,C) Id_table table(B##_id const*);
TABLES(X)
#undef X

#define X(A,B,C) Id_table table(B##_id);
TABLES(X)
#undef X

#endif
