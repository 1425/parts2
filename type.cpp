#include "type.h"

using namespace std;

#ifdef __GNUG__

#include<cstdlib>
#include<memory>
#include<cxxabi.h>

std::string demangle(const char *name){
	int status=0;
	std::unique_ptr<char,void(*)(void*)> r{
		abi::__cxa_demangle(name,NULL,NULL,&status),
		std::free
	};
	return status?name:r.get();
}

#else

string demangle(const char *s){
	assert(s);
	return s;
}

#endif
