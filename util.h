#ifndef UTIL_H
#define UTIL_H

#include<set>
#include<sstream>
#include<cassert>
#include<vector>
#include<iostream>
#include<optional>
#include<tuple>

#define nyi { std::cout<<"NYI "<<__FILE__<<":"<<__LINE__<<"\n"; exit(44); }
#define PRINT(X) { std::cout<<""#X<<":"<<(X)<<"\n"; }
#define INST(A,B) A B;

//Start vector operations

template<typename T>
std::vector<T>& operator|=(std::vector<T>& a,T t){
	a.push_back(std::move(t));
	return a;
}

std::vector<std::string> operator|=(std::vector<std::string>&,const char *);

template<typename T>
std::ostream& operator<<(std::ostream& o,std::vector<T> const& a){
	o<<"[ ";
	for(auto elem:a) o<<elem<<" ";
	return o<<"]";
}

template<typename T>
std::vector<T> operator+(std::vector<T> a,std::vector<T> const& b){
	std::vector<T> r=std::move(a);
	for(auto elem:b) r|=elem;
	return r;
}

bool operator==(std::vector<std::string> const& a,std::vector<const char*> const& b);
bool operator!=(std::vector<std::string> const& a,std::vector<const char*> const& b);

template<typename T>
std::vector<T>& operator|=(std::vector<T>& a,std::vector<T> const& b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
std::vector<T>& operator|=(std::vector<T> &a,std::optional<T> const& b){
	if(b){
		a|=*b;
	}
	return a;
}

template<typename T>
std::vector<T> to_vec(std::vector<T> a){
	return a;
}

//start pair operations

template<typename A,typename B>
std::ostream& operator<<(std::ostream& o,std::pair<A,B> const& a){
	return o<<"("<<a.first<<","<<a.second<<")";
}

template<typename T>
std::vector<T> to_vec(std::pair<T,T> a){
	return {a.first,a.second};
}

//start std::optional operations

template<typename T>
std::ostream& operator<<(std::ostream& o,std::optional<T> const& a){
	if(a) return o<<*a;
	return o<<"NULL";
}

template<typename T>
std::optional<T>& operator+=(std::optional<T>& a,std::optional<T> const& b){
	if(a){
		if(b){
			a=*a+*b;
		}
	}else{
		if(b) a=b;
	}
	return a;
}

//start tuple operations

template<typename ... Ts>
std::ostream& operator<<(std::ostream& o,std::tuple<Ts...> const& a){
	o<<"( ";
	std::apply([&](auto&&... x){ ((o<<x<<" "), ...); },a);
	return o<<")";
}

template<typename T>
std::vector<T> to_vec(std::tuple<T,T,T> const& a){
	return std::vector{std::get<0>(a),std::get<1>(a),std::get<2>(a)};
}

std::vector<size_t> range(size_t lim);
std::vector<int> range(int min,int lim);

template<typename A,typename B>
std::vector<std::pair<A,B>> zip(std::vector<A> const& a,std::vector<B> const& b){
	std::vector<std::pair<A,B>> r;
	auto a_at=begin(a);
	auto b_at=begin(b);
	auto a_end=end(a);
	auto b_end=end(b);
	while(a_at!=a_end && b_at!=b_end){
		r|=std::make_pair(*a_at,*b_at);
		a_at++;
		b_at++;
	}
	return r;
}

template<typename A,typename B,typename C>
std::vector<std::tuple<A,B,C>> zip(std::vector<A> const& a,std::vector<B> const& b,std::vector<C> const& c){
	std::vector<std::tuple<A,B,C>> r;
	auto a_at=begin(a);
	auto b_at=begin(b);
	auto c_at=begin(c);
	auto a_end=end(a);
	auto b_end=end(b);
	auto c_end=end(c);
	while(a_at!=a_end && b_at!=b_end && c_at!=c_end){
		r|=std::make_tuple(*a_at,*b_at,*c_at);
		a_at++;
		b_at++;
		c_at++;
	}
	return r;
}

template<typename A,typename B,typename C,typename D,typename E>
std::tuple<
	std::pair<std::string,A>,
	std::pair<std::string,B>,
	std::pair<std::string,C>,
	std::pair<std::string,D>,
	std::pair<std::string,E>
> zip(std::vector<std::string> const& a,std::tuple<A,B,C,D,E> const& b){
	assert(a.size()>=5);
	return make_tuple(
		make_pair(a[0],get<0>(b)),
		make_pair(a[1],get<1>(b)),
		make_pair(a[2],get<2>(b)),
		make_pair(a[3],get<3>(b)),
		make_pair(a[4],get<4>(b))
	);
}

template<typename A,typename B>
std::vector<std::pair<std::optional<A>,std::optional<B>>> zip_extend(std::vector<A> const& a,std::vector<B> const& b){
	std::vector<std::pair<std::optional<A>,std::optional<B>>> r;
	for(auto i:range(std::max(a.size(),b.size()))){
		auto a1=[=]()->std::optional<A>{ if(i<a.size()) return a[i]; return std::nullopt; }();
		auto b1=[=]()->std::optional<B>{ if(i<b.size()) return b[i]; return std::nullopt; }();
		r|=std::make_pair(a1,b1);
	}
	return r;
}


template<typename T>
std::vector<T> firsts(std::vector<std::vector<T>> const& a){
	return mapf([](auto x){ return x.at(0); },a);
}

template<typename A,typename B>
std::vector<A> firsts(std::vector<std::pair<A,B>> const& a){
	return mapf([](auto x){ return x.first; },a);
}

template<typename A,typename B>
std::vector<B> seconds(std::vector<std::pair<A,B>> const& a){
	return mapf([](auto x){ return x.second; },a);
}

template<typename T>
std::vector<std::pair<bool,T>> mark_last(std::vector<T> a){
	std::vector<std::pair<bool,T>> r;
	for(auto i:range(a.size())){
		r|=std::make_pair(i==a.size()-1,std::move(a[i]));
	}
	return r;
}

template<typename T>
T const& choose(std::vector<T> const& a){
	assert(a.size());
	return a[rand()%a.size()];
}

template<typename Func,typename T>
void mapv(Func f,T const& t){
	for(auto elem:t){
		f(elem);
	}
}

#define MAP(F,A) mapf([&](auto x){ return (F)(x); },(A))

template<typename Func,typename T>
auto mapf(Func f,T const& v){
	std::vector<decltype(f(*std::begin(v)))> r;
	for(auto elem:v){
		r|=f(elem);
	}
	return r;
}

template<typename Func,typename A,typename B>
auto mapf(Func f,std::pair<A,B> const& t){
	return std::make_pair(
		f(t.first),
		f(t.second)
	);
}

template<typename Func,typename A,typename B,typename C>
auto mapf(Func f,std::tuple<A,B,C> const& t){
	return make_tuple(
		f(std::get<0>(t)),
		f(std::get<1>(t)),
		f(std::get<2>(t))
	);
}

template<typename Func,typename A,typename B,typename C,typename D>
auto mapf(Func f,std::tuple<A,B,C,D> const& t){
	return make_tuple(
		f(std::get<0>(t)),
		f(std::get<1>(t)),
		f(std::get<2>(t)),
		f(std::get<3>(t))
	);
}

template<typename Func,typename A,typename B,typename C,typename D,typename E>
auto mapf(Func f,std::tuple<A,B,C,D,E> const& t){
	return make_tuple(
		f(std::get<0>(t)),
		f(std::get<1>(t)),
		f(std::get<2>(t)),
		f(std::get<3>(t)),
		f(std::get<4>(t))
	);
}

float rand(const float*);
double rand(const double*);
bool rand(const bool*);
unsigned rand(const unsigned*);
std::string rand(const std::string*);

template<typename Func,typename T>
std::vector<T> filter(Func f,std::vector<T> const& v){
	std::vector<T> r;
	for(auto const& elem:v){
		if(f(elem)) r|=elem;
	}
	return r;
}

template<typename T>
std::vector<std::pair<size_t,T>> enumerate(std::vector<T> const& a){
	std::vector<std::pair<size_t,T>> r;
	for(size_t i=0;i<a.size();i++){
		r|=std::make_pair(i,a[i]);
	}
	return r;
}

template<typename T>
T sum(std::vector<T> const& v){
	T r{};
	for(auto elem:v){
		r+=elem;
	}
	return r;
}

template<typename T>
std::vector<T> tail(std::vector<T> a){
	if(a.size()){
		a.erase(begin(a));
	}
	return a;
}

template<typename T>
bool all(std::vector<T> const& a){
	for(auto elem:a){
		if(!elem){
			return 0;
		}
	}
	return 1;
}

template<typename T>
std::vector<T> non_null(std::vector<std::optional<T>> v){
	std::vector<T> r;
	for(auto elem:v){
		if(elem){
			r|=*elem;
		}
	}
	return r;
}

template<typename T>
std::vector<T> non_null(std::tuple<std::optional<T>,std::optional<T>,std::optional<T>> const& t){
	std::vector<T> r;
	r|=get<0>(t);
	r|=get<1>(t);
	r|=get<2>(t);
	return r;
}

template<typename T>
std::vector<T> sorted(std::vector<T> a){
	sort(begin(a),end(a));
	return a;
}

template<typename Func,typename T>
std::vector<T> sorted(std::vector<T> a,Func f){
	std::sort(begin(a),end(a),[&](auto a,auto b){ return f(a)<f(b); });
	return a;
}

#define SORTED(A,B) sorted(A,[&](auto x){ return (B)(x); })

template<typename T,typename Func>
std::vector<T> sort_by(std::vector<T> a,Func f){
	std::sort(
		begin(a),
		end(a),
		[&](auto a,auto b){ return f(a)<f(b); }
	);
	return a;
}

template<typename T>
std::vector<T> reversed(std::vector<T> a){
	std::reverse(begin(a),end(a));
	return a;
}

template<typename T>
std::vector<T> flatten(std::vector<std::vector<T>> const& a){
	std::vector<T> r;
	for(auto elem:a){
		r|=elem;
	}
	return r;
}

template<typename T>
std::vector<std::pair<T,T>> adjacent_pairs(std::vector<T> const& a){
	std::vector<std::pair<T,T>> r;
	for(size_t i=1;i<a.size();i++){
		r|=std::make_pair(a[i-1],a[i]);
	}
	return r;
}


template<typename T>
std::vector<T> take(unsigned len,std::vector<T> v){
	std::vector<T> r;
	for(unsigned i=0;i<len && i<v.size();i++){
		r|=v[i];
	}
	return r;
}

template<typename T>
std::vector<T> skip(unsigned i,std::vector<T> v){
	std::vector<T> r;
	for(;i<v.size();i++){
		r|=v[i];
	}
	return r;
}

template<typename T>
T min(std::vector<T> const& a){
	assert(a.size());
	auto r=a[0];
	for(auto elem:a){
		r=std::min(r,elem);
	}
	return r;
}

template<typename T>
std::optional<T> max(std::vector<T> const& a){
	if(a.empty()) return std::nullopt;
	T r=a[0];
	for(auto elem:a){
		r=std::max(r,elem);
	}
	return r;
}

template<typename Func,typename T>
auto argmax(Func f,T const& t){
	auto s=sorted(mapf(
		[=](auto x){
			return std::make_pair(f(x),x);
		},
		t
	));
	assert(s.size());
	return s[s.size()-1].second;
}

#define ARGMAX(A,B) argmax([&](auto x){ return (A)(x); },(B))

//start string operations

void indent(size_t);

template<typename T>
void print_lines(T const& t){
	for(auto elem:t){
		std::cout<<elem<<"\n";
	}
}

template<typename T>
std::string as_string(T const& t){
	std::stringstream ss;
	ss<<t;
	return ss.str();
}

std::string join(std::string const&,std::vector<std::string> const&);

template<typename T>
std::string join(std::string delim,std::vector<T> v){
	std::stringstream ss;
	for(auto [last,elem]:mark_last(v)){
		ss<<elem;
		if(!last) ss<<delim;
	}
	return ss.str();
}

template<typename T>
std::string join(std::vector<T> const& v){
	return join("",v);
}

std::string join(std::pair<std::string,std::string> const&);
std::string join(std::tuple<std::string,std::string,std::string> const&);

template<typename T>
std::string join(std::tuple<T,T,T,T,T> const& a){
	return join(std::vector<T>{
		get<0>(a),
		get<1>(a),
		get<2>(a),
		get<3>(a),
		get<4>(a)
	});
}

bool prefix(std::string const& needle,std::string const& haystack);
std::vector<std::string> split(char delim,std::string const&);
int stoi(std::optional<std::string> const&);


//start HTML-related operations

std::string tag(std::string const& name,std::string const& body);
std::string a(std::string const& dest,std::string const& body);

#define TAGS(X)\
X(h1)\
X(title)\
X(head)\
X(html)\
X(body)\
X(p)\
X(td)\
X(th)\
X(h2)\
X(h3)\
X(h4)\
X(tr)\
X(table)\
X(ol)\
X(li)\
X(ul)\
X(big)\

#define X(A) std::string A(std::string const&);
TAGS(X)
#undef X

std::string hidden(std::string const& name,std::string const& value);

template<typename T>
std::string to_table(std::vector<std::vector<T>> const& a){
	std::stringstream ss;
	ss<<"<table border>";
	for(auto row:a){
		ss<<"<tr>";
		for(auto elem:row){
			ss<<td(as_string(elem));
		}
		ss<<"</tr>";
	}
	ss<<"</table>";
	return ss.str();
}

bool contains(std::string const& haystack,std::string const& needle);

template<typename T>
std::vector<T> operator+(T a,std::vector<T> b){
	std::vector<T> r;
	r|=a;
	r|=b;
	return r;
}

template<typename A,typename B>
std::vector<A> operator+(std::vector<A> a,B b){
	a|=b;
	return a;
}

bool endswith(std::string const& haystack,std::string const& needle);
bool startswith(std::string const& haystack,std::string const& needle);
bool contains(std::string const&,char);
bool any(std::vector<bool> const&);
std::string rm_suffix(std::string const& haystack,std::string const& needle);

template<typename T>
bool contains(std::vector<T> const& v,T const& t){
	for(auto const& a:v){
		if(a==t){
			return 1;
		}
	}
	return 0;
}

template<typename T>
std::vector<T> operator|(std::vector<T> v,T t){
	return v|=t;
}

std::string hidden(std::string const&,std::optional<std::string> const&);
std::string hidden(const char*,const char*);

template<typename T>
std::optional<T> max(std::vector<std::optional<T>> a){
	std::optional<T> r;
	for(auto elem:a){
		r=max(elem,r);
	}
	return r;
}

template<typename A,typename B,typename C>
std::pair<A,B> take2(std::tuple<A,B,C> const& t){
	return std::make_pair(get<0>(t),get<1>(t));
}

template<typename T>
T deref(std::optional<T> const& a){
	if(!a) throw "Unexpectedly null";
	return *a;
}

std::vector<char> caps();

template<typename A,typename B,typename C>
auto product(std::tuple<A,B,C> const& t){
	return get<0>(t)*get<1>(t)*get<2>(t);
}

template<typename T>
std::vector<T> after(T const& t,std::vector<T> const& v){
	//all elements after the given one
	auto at=begin(v);
	while(at!=end(v) && *at!=t){
		at++;
	}
	if(at==end(v)){
		return {};
	}
	return std::vector<T>{at+1,end(v)};
}

std::string join(std::tuple<std::string,std::string,std::string,std::string> const&);

template<typename A,typename B>
std::vector<std::pair<B,A>> swap_pairs(std::vector<std::pair<A,B>> const& v){
	return mapf(
		[](auto p){ return std::make_pair(p.second,p.first); },
		v
	);
}

#endif
