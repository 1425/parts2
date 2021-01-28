#include "location.h"
#include<sstream>
#include "db.h"

using namespace std;

/*
 *The location operations:
 1) add
 2) remove
 3) view current
 4) view history
 5) Edit current
 
 things to try out:
1) when things are edited, they automatically turn red, then when they get the ok from the database, they turn green
2) empty slot at the bottom for new entries, and if there is stuff added into it, then another item is added onto the bottom

 * */

string show_page(){
	stringstream o;
	o<<"<html>\n";
	o<<"<body>\n";
	o<<"<h1>Demo page</h1>\n";
	o<<"<table border>\n";
	o<<"<tr>\n";
	#define X(A,B) o<<"<th>"<<""#B<<"</th>";
	MACHINE_ITEMS(X)
	#undef X
	//o<<th("Update");
	o<<th("Delete");
	o<<th("Last editor");
	o<<th("Last edit time");
	o<<"</tr>\n";
	for(auto _:range(5)){
		(void)_;
		o<<"<tr>\n";
		#define X(A,B) o<<td(as_string(rand((A*)0)));
		MACHINE_ITEMS(X)
		#undef X
		//o<<td("<button type=\"button\">Update</button>");
		o<<td("<button type=\"button\">Delete</button>\n");//should pull open a confirmation box
		o<<td(as_string(rand((User*)0)))<<"\n";
		o<<td(as_string(rand((Date*)0)))<<"\n";
		o<<"</tr>\n";
	}
	o<<"</table>\n";
	o<<"<button type=\"button\">New entry</button>\n";//this should pull up 
	o<<"</body>\n";
	o<<"</html>\n";
	return o.str();
}
