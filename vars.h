#ifndef VARS_H
#define VARS_H

#include "page.h"

#define X(A,B) std::map<std::string,std::string> vars(A const&);
PAGES(X)
#undef X

#endif
