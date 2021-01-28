#ifndef VARIANT_UTIL_H
#define VARIANT_UTIL_H

#include<variant>

template<typename T,typename ... Ts>
bool operator==(std::variant<Ts...> const& a,T const& t){
	return a==std::variant<Ts...>(t);
}

template<typename ...Ts>
std::ostream& operator<<(std::ostream& o,std::variant<Ts...> const& a){
	std::visit([&](auto && x){ o<<x; },a);
	return o;
}

template<typename ... Ts>
void diff(std::variant<Ts...> const& a,std::variant<Ts...> const& b){
	bool found=std::visit(
		[&](auto x){
			using T=decltype(x);
			if(std::holds_alternative<T>(b)){
				diff(x,std::get<T>(b));
				return 1;
			}
			return 0;
		},
		a
	);
	if(found) return;
	std::cout<<"variant types differ:\n";
	PRINT(a);
	PRINT(b);
}

template<typename A,typename B>
struct Either{
	std::variant<A,B> data;

	public:
	Either(A a):data(a){}
	Either(B b):data(b){}

	bool is_first()const{
		return holds_alternative<A>(data);
	}

	A const& first()const{
		return get<A>(data);
	}

	B const& second()const{
		return get<B>(data);
	}
};

template<typename A,typename B>
std::pair<std::vector<A>,std::vector<B>> group(std::vector<Either<A,B>> const& a){
	std::pair<std::vector<A>,std::vector<B>> r;
	for(auto elem:a){
		if(elem.is_first()){
			r.first|=elem.first();
		}else{
			r.second|=elem.second();
		}
	}
	return r;
}

#endif
