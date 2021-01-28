#include "util.h"
#include<cstring>

using namespace std;

std::vector<size_t> range(size_t lim){
	std::vector<size_t> r;
	for(size_t i=0;i<lim;i++) r|=i;
	return r;
}

std::string join(std::string const& a,std::vector<std::string> const& b){
	std::stringstream ss;
	for(auto [last,elem]:mark_last(b)){
		ss<<elem;
		if(!last) ss<<a;
	}
	return ss.str();
}

bool prefix(std::string const& needle,std::string const& haystack){
	if(needle.size()>haystack.size()) return 0;
	for(auto i:range(needle.size())){
		if(needle[i]!=haystack[i]){
			return 0;
		}
	}
	return 1;
}

std::vector<std::string> split(char delim,std::string const& s){
	std::vector<std::string> r;
	std::stringstream ss;
	for(auto c:s){
		if(c==delim){
			r|=ss.str();
			ss.str("");
		}else{
			ss<<c;
		}
	}
	if(ss.str().size()){
		r|=ss.str();
	}
	return r;
}

std::vector<std::string> split(std::string s){
	return split(' ',s);
}

float rand(const float*){
	return float(rand())/99;
}

double rand(const double*){
	return double(rand())/99;
}

bool rand(const bool*){ return rand()%2; }

std::string rand(const std::string*){ return (rand()%2)?"cow":"chicken"; }

bool operator==(std::vector<std::string> const& a,std::vector<const char*> const& b){
	if(a.size()!=b.size()) return 0;
	for(auto [a1,b1]:zip(a,b)){
		if(a1!=b1) return 0;
	}
	return 1;
}

bool operator!=(std::vector<std::string> const& a,std::vector<const char*> const& b){
	return !(a==b);
}

std::string tag(std::string const& name,std::string const& body){
	assert(name.size());
	std::stringstream ss;
	ss<<"<"<<name<<">"<<body<<"</"<<split(name)[0]<<">";
	return ss.str();
}

#define TAG(X) std::string X(std::string const& s){ return tag(""#X,s); }
TAGS(TAG)
#undef TAG

std::vector<std::string> operator|=(std::vector<std::string> &a,const char *s){
    assert(s);
	a.push_back(s);
	return a;
}

void indent(size_t i){
	for(auto _:range(i)){
		(void)_;
		std::cout<<"\t";
	}
}

unsigned rand(const unsigned*){ return rand()%100; }

std::string a(std::string const& dest,std::string const& body){
	return "<a href=\""+dest+"\">"+body+"</a>";
}

std::vector<std::string> operator+(std::vector<const char *> const& a,std::set<std::string> const& b){
	std::vector<std::string> r;
	for(auto elem:a) r|=elem;
	for(auto elem:b) r|=elem;
	return r;
}

int stoi(optional<string> const& a){
	if(!a) throw "invalid value";
	return stoi(*a);
}

string hidden(string const& name,string const& value){
	//Note that this is not doing any escaping.
	return "<input type=\"hidden\" name=\""+name+"\" value=\""+value+"\">\n";
}

std::string join(std::tuple<std::string,std::string,std::string> const& a){
	return join(to_vec(a));
}

std::string join(std::pair<std::string,std::string> const& a){
	return a.first+a.second;
}

std::vector<int> range(int min,int lim){
	std::vector<int> r;
	for(int i=min;i<lim;i++){
		r|=i;
	}
	return r;
}

bool contains(std::string const& haystack,std::string const& needle){
	return !!strstr(haystack.c_str(),needle.c_str());
}

bool endswith(string const& haystack,string const& needle){
	if(haystack.size()<needle.size()) return 0;
	auto suffix=haystack.substr(haystack.size()-needle.size(),needle.size());
	return suffix==needle;
}

bool startswith(string const& haystack,string const& needle){
	auto prefix=haystack.substr(0,needle.size());
	return prefix==needle;
}

bool contains(string const& s,char c){
	for(auto c1:s){
		if(c1==c){
			return 1;
		}
	}
	return 0;
}

bool any(vector<bool> const& a){
	for(auto x:a){
		if(x) return 1;
	}
	return 0;
}

string rm_suffix(string const& haystack,string const& needle){
	return haystack.substr(0,haystack.size()-needle.size());
}

std::string hidden(std::string const& name,std::optional<std::string> const& value){
	if(value) return hidden(name,*value);
	return hidden(name,std::string{});
}

std::string hidden(const char *a,const char *b){
	assert(a);
	if(b) return hidden(std::string{a},std::string{b});
	return hidden(a,std::optional<std::string>{});
}

std::vector<char> caps(){
	std::vector<char> r;
	for(auto i:range(26)){
		r|=char('A'+i);
	}
	return r;
}

string join(tuple<string,string,string,string> const& t){
	return get<0>(t)+get<1>(t)+get<2>(t)+get<3>(t);
}
