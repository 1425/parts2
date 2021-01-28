#ifndef MAP_UTIL_H
#define MAP_UTIL_H

#include<map>

template<typename K,typename V>
std::ostream& operator<<(std::ostream& o,std::map<K,V> const& a){
	o<<"{ ";
	for(auto p:a) o<<p<<" ";
	return o<<"}";
}

template<typename K,typename V>
std::map<K,V> without_key(std::map<K,V> a,K const& k){
	auto f=a.find(k);
	if(f==a.end()) return a;
	a.erase(f);
	return a;
}

template<typename K,typename V>
std::map<K,V>& operator|=(std::map<K,V>& a,std::map<K,V> const& b){
	for(auto [k,v]:b){
		a[k]=v;
	}
	return a;
}

template<typename K,typename V>
std::map<std::optional<K>,V> operator|=(std::map<std::optional<K>,V> &a,std::map<K,V> const& b){
	for(auto [k,v]:b){
		a[k]=v;
	}
	return a;
}

template<typename K,typename V>
std::set<K> keys(std::map<K,V> const& a){
	std::set<K> r;
	for(auto& [k,v]:a){
		(void)v;
		r|=k;
	}
	return r;
}

template<typename K,typename V>
std::vector<V> values(std::map<K,V> const& a){
	std::vector<V> r;
	for(auto [_,v]:a){
		(void)_;
		r|=v;
	}
	return r;
}

template<typename A,typename B>
std::map<A,B> to_map(std::vector<std::tuple<A,B>> const& v){
	std::map<A,B> r;
	for(auto [a,b]:v){
		r[a]=b;
	}
	return r;
}

template<typename A,typename B>
std::map<A,B> to_map(std::vector<std::pair<A,B>> const& v){
	std::map<A,B> r;
	for(auto [a,b]:v){
		r[a]=b;
	}
	return r;
}

template<typename A,typename ... Ts>
std::map<A,std::tuple<A,Ts...>> to_map(std::vector<std::tuple<A,Ts...>> const& v){
	std::map<A,std::tuple<A,Ts...>> r;
	for(auto elem:v){
		r[std::get<0>(elem)]=elem;
	}
	return r;
}

#define MAP_VALUES(A,B) map_values([&](auto x){ return (A)(x); },(B))

template<typename K,typename V,typename Func>
auto map_values(Func f,std::map<K,V> const& a){
	using V2=decltype(f(begin(a)->second));
	std::map<K,V2> r;
	for(auto [k,v]:a){
		r[k]=f(v);
	}
	return r;
}
template<typename K,typename V>
std::vector< std::pair<bool,std::pair<const K,V>> > mark_last(std::map<K,V> const& a){
	std::vector< std::pair<bool,std::pair<const K,V>> > r;
	for(auto at=begin(a);at!=end(a);++at){
		auto n=at;
		n++;
		r|=std::make_pair(n==end(a),*at);
	}
	return r;
}

template<typename K,typename V>
void diff(std::map<K,V> const& a,std::map<K,V> const& b){
	auto ka=keys(a);
	auto kb=keys(b);
	if(ka!=kb){
		PRINT(ka-kb);
		PRINT(kb-ka);
		nyi
	}
	for(auto k:ka){
		auto va=a[k];
		auto vb=b[k];
		if(va!=vb){
			PRINT(k);
			PRINT(va);
			PRINT(vb);
		}
	}
}

template<typename Func,typename T>
auto group(Func f,std::vector<T> a){
	using K=decltype(f(a[0]));
	std::map<K,std::vector<T>> r;
	for(auto elem:a){
		r[f(elem)]|=elem;
	}
	return r;
}

#define GROUP(A,B) group([&](auto x){ return (A)(x); },(B))

template<typename T>
std::map<T,unsigned> count(std::multiset<T> const& a){
	std::map<T,unsigned> r;
	for(auto elem:a){
		r[elem]++;
	}
	return r;
}

template<typename K,typename V>
std::vector<std::pair<K,V>> to_vec(std::map<K,V> const& a){
	return mapf([](auto x){ return std::make_pair(x.first,x.second); },a);
}

template<typename K,typename V>
auto enumerate(std::map<K,V> const& a){
	return enumerate(to_vec(a));
}

template<typename K,typename V>
std::map<K,V> non_null(std::map<std::optional<K>,V> const& a){
	std::map<K,V> r;
	for(auto [k,v]:a){
		if(k){
			r[*k]=v;
		}
	}
	return r;
}

template<typename Func,typename T>
auto group(Func f,std::multiset<T> const& a){
	using K=decltype(f(*begin(a)));
	std::map<K,std::multiset<T>> r;
	for(auto elem:a){
		r[f(elem)]|=elem;
	}
	return r;
}

#endif
