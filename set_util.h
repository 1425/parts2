#ifndef SET_UTIL_H
#define SET_UTIL_H

template<typename T>
std::set<T> to_set(std::vector<T> const& a){
	return std::set<T>{a.begin(),a.end()};
}

template<typename T>
std::set<T>& operator-=(std::set<T>& s,T t){
	s.erase(t);
	return s;
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::set<T> const& a){
	o<<"{ ";
	for(auto elem:a){
		o<<elem<<" ";
	}
	return o<<"}";
}

template<typename T>
std::set<T>& operator|=(std::set<T>& a,T t){
	a.insert(std::move(t));
	return a;
}

template<typename T>
std::vector<T> operator+(std::vector<T> a,std::set<T> const& b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
std::set<T> operator-(std::set<T> a,std::set<T> const& b){
	for(auto elem:b){
		a.erase(elem);
	}
	return a;
}

template<typename T>
std::set<T> operator|(std::set<T> a,std::set<T> const& b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
bool operator&(T const& t,std::set<T> const& s){
	return s.count(t);
}

template<typename T>
std::set<T> operator|(std::set<T> a,T t){
	a|=t;
	return a;
}

template<typename T>
std::vector<T> to_vec(std::set<T> a){
	return std::vector<T>(begin(a),end(a));
}

template<typename T>
std::vector<T> operator|=(std::vector<T> &a,std::set<T> const& b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
std::set<T>& operator|=(std::set<T>& a,std::set<T> const& b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
std::set<T> flatten(std::vector<std::set<T>> const& a){
	std::set<T> r;
	for(auto elem:a){
		r|=elem;
	}
	return r;
}

std::vector<std::string> operator+(std::vector<const char *> const&,std::set<std::string> const&);

template<typename T>
std::string join(std::string const& delim,std::set<T> const& a){
	return join(delim,to_vec(a));
}

template<typename T>
bool contains(std::set<T> const& a,T const& t){
	return a.count(t);
}

template<typename T>
std::set<std::multiset<T>> of_size(unsigned size,std::set<T> values){
	if(size==0){
		std::set<std::multiset<T>> r;
		r|=std::multiset<T>{};
		return r;
	}
	auto prev=of_size(size-1,values);
	std::set<std::multiset<T>> r;
	for(auto p1:prev){
		for(auto value:values){
			r|=(p1|value);
		}
	}
	return r;
}

template<typename T>
std::set<std::optional<T>> operator|=(std::set<std::optional<T>> &a,T t){
	a|=std::optional<T>(t);
	return a;
}

template<typename T>
std::set<std::optional<T>>& operator|=(std::set<std::optional<T>> &a,std::set<T> b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
std::set<std::optional<T>> operator|(std::optional<T> const& a,std::set<T> const& b){
	std::set<std::optional<T>> r;
	r|=a;
	r|=b;
	return r;
}

template<typename T>
std::set<T> to_set(std::multiset<T> const& a){
	return std::set<T>(a.begin(),a.end());
}

template<typename T>
T min(std::set<T> const& a){
	return min(to_vec(a));
}

template<typename T>
std::multiset<T> to_multiset(std::optional<T> const& a){
	if(a) return {*a};
	return {};
}

template<typename T>
auto max(std::set<T> const& a){
	return max(to_vec(a));
}

//start multiset operations

template<typename T>
std::multiset<T> to_multiset(std::vector<T> const& a){
	return std::multiset<T>{begin(a),end(a)};
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::multiset<T> const& a){
	o<<"{ ";
	for(auto const& elem:a){
		o<<elem<<" ";
	}
	return o<<"}";
}

template<typename T>
std::multiset<T>& operator-=(std::multiset<T>& a,T t){
	//not just calling a.erase(t) because that remove all occurrances of t.
	auto f=a.find(t);
	if(f!=a.end()){
		a.erase(f);
	}
	return a;
}

template<typename T>
std::multiset<T>& operator|=(std::multiset<T>& a,T t){
	a.insert(std::move(t));
	return a;
}

template<typename T>
std::multiset<T> operator-(std::multiset<T> a,T t){
	a-=t;
	return a;
}

template<typename T>
std::multiset<T> operator|(std::multiset<T> a,T t){
	a|=t;
	return a;
}

template<typename T>
std::multiset<T> operator|(std::multiset<T> a,std::multiset<T> const& b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
std::vector<T> to_vec(std::multiset<T> const& a){
	return std::vector<T>{begin(a),end(a)};
}

template<typename T>
std::multiset<T>& operator|=(std::multiset<T>& a,std::multiset<T> const& b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
std::multiset<T>& operator-=(std::multiset<T>& a,std::optional<T> b){
	if(b) a|=*b;
	return a;
}

template<typename T>
std::multiset<T> operator|=(std::multiset<T>&,std::optional<T>){
	nyi
}

template<typename T>
std::multiset<T> flatten(std::vector<std::multiset<T>> a){
	std::multiset<T> r;
	for(auto elem:a){
		r|=elem;
	}
	return r;
}

//end multiset operations

template<typename Func,typename T>
std::multiset<T> filter(Func f,std::multiset<T> a){
	std::multiset<T> r;
	for(auto elem:a){
		if(f(elem)){
			r|=elem;
		}
	}
	return r;
}


template<typename T,typename Func>
std::vector<T> sort_by(std::multiset<T> a,Func f){
	return sort_by(to_vec(a),f);
}

template<typename T>
T min(std::multiset<T> const& a){
	return min(to_vec(a));
}

template<typename T>
std::multiset<T> operator-(std::multiset<T> a,std::multiset<T> b){
	for(auto elem:b){
		a-=elem;
	}
	return a;
}

template<typename T>
unsigned count(std::multiset<T> const& v,T const& t){
	return count(begin(v),end(v),t);
}

template<typename T>
auto enumerate(std::multiset<T> a){
	return enumerate(to_vec(a));
}

template<typename T>
std::multiset<T> to_multiset(std::vector<std::pair<T,unsigned>> const& a){
	std::multiset<T> r;
	for(auto [k,v]:a){
		for(auto _:range(v)){
			(void)_;
			r|=k;
		}
	}
	return r;
}

template<typename Func,typename T>
std::vector<T> sorted(std::multiset<T> m,Func f){
	return sorted(to_vec(m),f);
}

#endif
