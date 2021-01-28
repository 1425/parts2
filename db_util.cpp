#include "db_util.h"
#include<map>
#include "util.h"

using namespace std;

void run_cmd(DB db,std::string const& cmd){
	auto q=mysql_query(db,cmd.c_str());
	if(q){
		std::cout<<"Mysql fail:"<<mysql_error(db)<<"\n";
		std::cout<<"Query was:"<<cmd<<"\n";
		mysql_close(db);
		exit(1);
	}
}

std::vector<std::vector<std::optional<std::string>>> query(DB db,std::string const& query){
	//this is obviously not the fastest way to do this.
	run_cmd(db,query);
	MYSQL_RES *result=mysql_store_result(db);
	if(result==NULL)nyi
	int fields=mysql_num_fields(result);
	//PRINT(fields);
	MYSQL_ROW row;
	std::vector<std::vector<std::optional<std::string>>> r;
	while((row=mysql_fetch_row(result))){
		unsigned long *lengths = mysql_fetch_lengths(result);
		assert(lengths);
		std::vector<std::optional<std::string>> this_row;
		for(auto i:range(fields)){
			if(row[i]){
				//The reason to ask for all the lengths is so that when there is binary data
				//This you get all the data instread of just having it get truncated on the first null character.
				this_row|=std::optional{std::string(row[i],row[i]+lengths[i])};
			}else{
				this_row|=std::optional<std::string>{std::nullopt};
			}
		}
		r|=this_row;
	}
	mysql_free_result(result);
	return r;
}

std::vector<std::vector<std::optional<std::string>>> const& query_cached(DB db,std::string const& query_string){
	//return query_inner(db,query_string);
	using R=std::vector<std::vector<std::optional<std::string>>>;
	static map<string,R> m;
	auto f=m.find(query_string);
	if(f==m.end()){
		auto r=query(db,query_string);
		m[query_string]=r;
		auto f=m.find(query_string);
		return f->second;//return r;
	}
	return f->second;
}

void insert(DB db,std::string const& table,std::vector<std::pair<std::string,std::string>> const& data){
	//The data fields must already be escaped.
	auto q="INSERT INTO "+table+" ("
		+join(",",firsts(data))
		+") VALUES ("
		+join(",",seconds(data))
		+")";
	run_cmd(db,q);
}

