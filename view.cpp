#include "view.h"
#include "main.h"
#include "page.h"

using namespace std;

void show(ostream& o,DB db,View const& a){
	(void)db;
	(void)a;
	auto q=query(db,
		"SELECT name,datatype,data FROM upload_info WHERE "
		"(id) IN ("
			"SELECT MAX(id) FROM upload_info WHERE main="+as_string(a.main.data)
		+")"
	);
	if(q.empty()){
		o<<"Content-type: text/html\n\n";
		page(o,"Image view","Image not found.");
		return;
	}
	assert(q.size()==1);
	auto row=q[0];
	assert(row[0]);
	auto filename=*row[0];
	assert(row[1]);
	auto datatype=*row[1];
	o<<"Content-type: "<<datatype<<"\n";
	o<<"Content-Disposition: filename=\""<<filename<<"\"\n";
	o<<"\n";
	assert(row[2]);
	o<<*row[2];
}
