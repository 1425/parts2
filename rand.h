#ifndef RAND_H
#define RAND_H

template<typename A,typename B,typename C,typename D,typename E>
std::variant<A,B,C,D,E> rand(std::variant<A,B,C,D,E> const*){
	switch(rand()%5){
		case 0: return rand((A*)0);
		case 1: return rand((B*)1);
		case 2: return rand((C*)2);
		case 3: return rand((D*)3);
		case 4: return rand((E*)4);
		default: assert(0);
	}
}

template<typename A,typename B,typename C,typename D,typename E,typename F>
std::variant<A,B,C,D,E,F> rand(std::variant<A,B,C,D,E,F> const*){
	switch(rand()%6){
		case 0: return rand((A*)0);
		case 1: return rand((B*)1);
		case 2: return rand((C*)2);
		case 3: return rand((D*)3);
		case 4: return rand((E*)4);
		case 5: return rand((F*)5);
		default: assert(0);
	}
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G
>
std::variant<A,B,C,D,E,F,G> rand(std::variant<A,B,C,D,E,F,G> const*){
	switch(rand()%7){
		case 0: return rand((A*)0);
		case 1: return rand((B*)1);
		case 2: return rand((C*)2);
		case 3: return rand((D*)3);
		case 4: return rand((E*)4);
		case 5: return rand((F*)5);
		case 6: return rand((G*)6);
		default: assert(0);
	}
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H
>
std::variant<A,B,C,D,E,F,G,H> rand(std::variant<A,B,C,D,E,F,G,H> const*){
	switch(rand()%8){
		case 0: return rand((A*)0);
		case 1: return rand((B*)1);
		case 2: return rand((C*)2);
		case 3: return rand((D*)3);
		case 4: return rand((E*)4);
		case 5: return rand((F*)5);
		case 6: return rand((G*)6);
		case 7: return rand((H*)7);
		default: assert(0);
	}
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I
>
std::variant<A,B,C,D,E,F,G,H,I> rand(std::variant<A,B,C,D,E,F,G,H,I> const*){
	switch(rand()%9){
		case 0: return rand((A*)0);
		case 1: return rand((B*)1);
		case 2: return rand((C*)2);
		case 3: return rand((D*)3);
		case 4: return rand((E*)4);
		case 5: return rand((F*)5);
		case 6: return rand((G*)6);
		case 7: return rand((H*)7);
		case 8: return rand((I*)8);
		default: assert(0);
	}
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J
>
std::variant<A,B,C,D,E,F,G,H,I,J> rand(std::variant<A,B,C,D,E,F,G,H,I,J> const*){
	switch(rand()%10){
		case 0: return rand((A*)0);
		case 1: return rand((B*)1);
		case 2: return rand((C*)2);
		case 3: return rand((D*)3);
		case 4: return rand((E*)4);
		case 5: return rand((F*)5);
		case 6: return rand((G*)6);
		case 7: return rand((H*)7);
		case 8: return rand((I*)8);
		case 9: return rand((J*)8);
		default: assert(0);
	}
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K
>
std::variant<A,B,C,D,E,F,G,H,I,J,K> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K> const*){
	switch(rand()%11){
		case 0: return rand((A*)0);
		case 1: return rand((B*)1);
		case 2: return rand((C*)2);
		case 3: return rand((D*)3);
		case 4: return rand((E*)4);
		case 5: return rand((F*)5);
		case 6: return rand((G*)6);
		case 7: return rand((H*)7);
		case 8: return rand((I*)8);
		case 9: return rand((J*)8);
		case 10: return rand((K*)8);
		default: assert(0);
	}
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L
>
std::variant<A,B,C,D,E,F,G,H,I,J,K,L> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L> const*){
	switch(rand()%12){
		case 0: return rand((A*)0);
		case 1: return rand((B*)0);
		case 2: return rand((C*)0);
		case 3: return rand((D*)0);
		case 4: return rand((E*)0);
		case 5: return rand((F*)0);
		case 6: return rand((G*)0);
		case 7: return rand((H*)0);
		case 8: return rand((I*)0);
		case 9: return rand((J*)0);
		case 10: return rand((K*)0);
		case 11: return rand((L*)0);
		default: assert(0);
	}
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M
>
std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M> const*){
	switch(rand()%13){
		case 0: return rand((A*)0);
		case 1: return rand((B*)0);
		case 2: return rand((C*)0);
		case 3: return rand((D*)0);
		case 4: return rand((E*)0);
		case 5: return rand((F*)0);
		case 6: return rand((G*)0);
		case 7: return rand((H*)0);
		case 8: return rand((I*)0);
		case 9: return rand((J*)0);
		case 10: return rand((K*)0);
		case 11: return rand((L*)0);
		case 12: return rand((M*)0);
		default: assert(0);
	}
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N
>
std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N> const*){
	switch(rand()%14){
		case 0: return rand((A*)0);
		case 1: return rand((B*)0);
		case 2: return rand((C*)0);
		case 3: return rand((D*)0);
		case 4: return rand((E*)0);
		case 5: return rand((F*)0);
		case 6: return rand((G*)0);
		case 7: return rand((H*)0);
		case 8: return rand((I*)0);
		case 9: return rand((J*)0);
		case 10: return rand((K*)0);
		case 11: return rand((L*)0);
		case 12: return rand((M*)0);
		case 13: return rand((N*)0);
		default: assert(0);
	}
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O> const*);

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P> const*){
	int i=rand()%16;
	#define X(INDEX,TYPE) if(i==INDEX) return rand((TYPE*)0);
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q> const*){
	int i=rand()%17;
	#define X(INDEX,TYPE) if(i==INDEX) return rand((TYPE*)0);
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R> const*){
	int i=rand()%18;
	#define X(INDEX,TYPE) if(i==INDEX) return rand((TYPE*)0);
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S>;
	int i=rand()%19;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T>;
	int i=rand()%20;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U>;
	int i=rand()%21;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V>;
	int i=rand()%22;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W>;
	int i=rand()%23;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X>;
	int i=rand()%24;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y>;
	int i=rand()%25;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z>;
	int i=rand()%26;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA>;
	int i=rand()%27;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB>;
	int i=rand()%28;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC>;
	int i=rand()%29;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD>;
	int i=rand()%30;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	#undef X
	assert(0);
}


template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD,
	typename AE
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE>;
	int i=rand()%31;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	X(30,AE)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD,
	typename AE,typename AF
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE,AF
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE,AF> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE,AF>;
	int i=rand()%32;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	X(30,AE)
	X(31,AF)
	#undef X
	assert(0);
}


template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE,AF,AG
> rand(std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE,AF,AG> const*){
	using RET=std::variant<A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE,AF,AG>;
	int i=rand()%33;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	X(30,AE)
	X(31,AF)
	X(32,AG)
	#undef X
	assert(0);
}



template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,
	typename AH
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH
	>;
	int i=rand()%34;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	X(30,AE)
	X(31,AF)
	X(32,AG)
	X(33,AH)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,
	typename AH,typename AI
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI
	>;
	int i=rand()%35;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	X(30,AE)
	X(31,AF)
	X(32,AG)
	X(33,AH)
	X(34,AI)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,
	typename AH,typename AI,typename AJ
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ
	>;
	int i=rand()%36;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	X(30,AE)
	X(31,AF)
	X(32,AG)
	X(33,AH)
	X(34,AI)
	X(35,AJ)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,
	typename AH,typename AI,typename AJ,
	typename AK
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK
	>;
	int i=rand()%37;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	X(30,AE)
	X(31,AF)
	X(32,AG)
	X(33,AH)
	X(34,AI)
	X(35,AJ)
	X(36,AK)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,
	typename AH,typename AI,typename AJ,
	typename AK,typename AL
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL
	>;
	int i=rand()%38;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	X(30,AE)
	X(31,AF)
	X(32,AG)
	X(33,AH)
	X(34,AI)
	X(35,AJ)
	X(36,AK)
	X(37,AL)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,
	typename D,typename E,typename F,
	typename G,typename H,typename I,
	typename J,typename K,typename L,
	typename M,typename N,typename O,
	typename P,typename Q,typename R,
	typename S,typename T,typename U,
	typename V,typename W,typename X,
	typename Y,typename Z,typename AA,
	typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,
	typename AH,typename AI,typename AJ,
	typename AK,typename AL,typename AM
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM
	>;
	int i=rand()%39;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A)
	X(1,B)
	X(2,C)
	X(3,D)
	X(4,E)
	X(5,F)
	X(6,G)
	X(7,H)
	X(8,I)
	X(9,J)
	X(10,K)
	X(11,L)
	X(12,M)
	X(13,N)
	X(14,O)
	X(15,P)
	X(16,Q)
	X(17,R)
	X(18,S)
	X(19,T)
	X(20,U)
	X(21,V)
	X(22,W)
	X(23,X)
	X(24,Y)
	X(25,Z)
	X(26,AA)
	X(27,AB)
	X(28,AC)
	X(29,AD)
	X(30,AE)
	X(31,AF)
	X(32,AG)
	X(33,AH)
	X(34,AI)
	X(35,AJ)
	X(36,AK)
	X(37,AL)
	X(38,AM)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,
	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,
	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT
	>;
	int i=rand()%46;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,
	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,
	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT,typename AU
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU
	>;
	int i=rand()%47;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT) X(46,AU)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,
	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,
	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT,typename AU,typename AV
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV
	>;
	int i=rand()%48;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT) X(46,AU) X(47,AV)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,

	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,

	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT,typename AU,typename AV,typename AW,typename AX,
	typename AY,typename AZ,typename BA,typename BB,typename BC,
	typename BD,typename BE,typename BF,typename BG,typename BH,

	typename BI,typename BJ,typename BK
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
		BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK
	>;
	int i=rand()%63;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT) X(46,AU) X(47,AV) X(48,AW) X(49,AX)
	X(50,AY) X(51,AZ) X(52,BA) X(53,BB) X(54,BC)
	X(55,BD) X(56,BE) X(57,BF) X(58,BG) X(59,BH)
	X(60,BI) X(61,BJ) X(62,BK)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,

	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,

	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT,typename AU,typename AV,typename AW,typename AX,
	typename AY,typename AZ,typename BA,typename BB,typename BC,
	typename BD,typename BE,typename BF,typename BG,typename BH,

	typename BI,typename BJ,typename BK,typename BL
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
		BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL
	>;
	int i=rand()%64;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT) X(46,AU) X(47,AV) X(48,AW) X(49,AX)
	X(50,AY) X(51,AZ) X(52,BA) X(53,BB) X(54,BC)
	X(55,BD) X(56,BE) X(57,BF) X(58,BG) X(59,BH)
	X(60,BI) X(61,BJ) X(62,BK) X(63,BL)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,

	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,

	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT,typename AU,typename AV,typename AW,typename AX,
	typename AY,typename AZ,typename BA,typename BB,typename BC,
	typename BD,typename BE,typename BF,typename BG,typename BH,

	typename BI,typename BJ,typename BK,typename BL,typename BM
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
		BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM
	>;
	int i=rand()%65;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT) X(46,AU) X(47,AV) X(48,AW) X(49,AX)
	X(50,AY) X(51,AZ) X(52,BA) X(53,BB) X(54,BC)
	X(55,BD) X(56,BE) X(57,BF) X(58,BG) X(59,BH)
	X(60,BI) X(61,BJ) X(62,BK) X(63,BL) X(64,BM)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,

	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,

	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT,typename AU,typename AV,typename AW,typename AX,
	typename AY,typename AZ,typename BA,typename BB,typename BC,
	typename BD,typename BE,typename BF,typename BG,typename BH,

	typename BI,typename BJ,typename BK,typename BL,typename BM,
	typename BN
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
		BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN
	>;
	int i=rand()%66;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT) X(46,AU) X(47,AV) X(48,AW) X(49,AX)
	X(50,AY) X(51,AZ) X(52,BA) X(53,BB) X(54,BC)
	X(55,BD) X(56,BE) X(57,BF) X(58,BG) X(59,BH)
	X(60,BI) X(61,BJ) X(62,BK) X(63,BL) X(64,BM)
	X(65,BN)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,

	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,

	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT,typename AU,typename AV,typename AW,typename AX,
	typename AY,typename AZ,typename BA,typename BB,typename BC,
	typename BD,typename BE,typename BF,typename BG,typename BH,

	typename BI,typename BJ,typename BK,typename BL,typename BM,
	typename BN,typename BO
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
		BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO
	>;
	int i=rand()%67;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT) X(46,AU) X(47,AV) X(48,AW) X(49,AX)
	X(50,AY) X(51,AZ) X(52,BA) X(53,BB) X(54,BC)
	X(55,BD) X(56,BE) X(57,BF) X(58,BG) X(59,BH)
	X(60,BI) X(61,BJ) X(62,BK) X(63,BL) X(64,BM)
	X(65,BN) X(66,BO)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,

	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,

	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT,typename AU,typename AV,typename AW,typename AX,
	typename AY,typename AZ,typename BA,typename BB,typename BC,
	typename BD,typename BE,typename BF,typename BG,typename BH,

	typename BI,typename BJ,typename BK,typename BL,typename BM,
	typename BN,typename BO,typename BP
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO,BP
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO,BP
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
		BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO,BP
	>;
	int i=rand()%68;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT) X(46,AU) X(47,AV) X(48,AW) X(49,AX)
	X(50,AY) X(51,AZ) X(52,BA) X(53,BB) X(54,BC)
	X(55,BD) X(56,BE) X(57,BF) X(58,BG) X(59,BH)
	X(60,BI) X(61,BJ) X(62,BK) X(63,BL) X(64,BM)
	X(65,BN) X(66,BO) X(67,BP)
	#undef X
	assert(0);
}

template<
	typename A,typename B,typename C,typename D,typename E,
	typename F,typename G,typename H,typename I,typename J,
	typename K,typename L,typename M,typename N,typename O,
	typename P,typename Q,typename R,typename S,typename T,

	typename U,typename V,typename W,typename X,typename Y,
	typename Z,typename AA,typename AB,typename AC,typename AD,
	typename AE,typename AF,typename AG,typename AH,typename AI,
	typename AJ,typename AK,typename AL,typename AM,typename AN,

	typename AO,typename AP,typename AQ,typename AR,typename AS,
	typename AT,typename AU,typename AV,typename AW,typename AX,
	typename AY,typename AZ,typename BA,typename BB,typename BC,
	typename BD,typename BE,typename BF,typename BG,typename BH,

	typename BI,typename BJ,typename BK,typename BL,typename BM,
	typename BN,typename BO,typename BP,typename BQ
>
std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO,BP,BQ
> rand(std::variant<
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
	BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO,BP,BQ
> const*){
	using RET=std::variant<
		A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO,AP,AQ,AR,AS,AT,AU,AV,AW,AX,AY,AZ,
		BA,BB,BC,BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO,BP,BQ
	>;
	int i=rand()%69;
	#define X(INDEX,TYPE) if(i==INDEX) return RET{rand((TYPE*)0)};
	X(0,A) X(1,B) X(2,C) X(3,D) X(4,E)
	X(5,F) X(6,G) X(7,H) X(8,I) X(9,J)
	X(10,K) X(11,L) X(12,M) X(13,N) X(14,O)
	X(15,P) X(16,Q) X(17,R) X(18,S) X(19,T)
	X(20,U) X(21,V) X(22,W) X(23,X) X(24,Y)
	X(25,Z) X(26,AA) X(27,AB) X(28,AC) X(29,AD)
	X(30,AE) X(31,AF) X(32,AG) X(33,AH) X(34,AI)
	X(35,AJ) X(36,AK) X(37,AL) X(38,AM) X(39,AN)
	X(40,AO) X(41,AP) X(42,AQ) X(43,AR) X(44,AS)
	X(45,AT) X(46,AU) X(47,AV) X(48,AW) X(49,AX)
	X(50,AY) X(51,AZ) X(52,BA) X(53,BB) X(54,BC)
	X(55,BD) X(56,BE) X(57,BF) X(58,BG) X(59,BH)
	X(60,BI) X(61,BJ) X(62,BK) X(63,BL) X(64,BM)
	X(65,BN) X(66,BO) X(67,BP) X(68,BQ)
	#undef X
	assert(0);
}

template<typename A,typename B>
std::variant<A,B> rand(std::variant<A,B> const*){
	if(rand()%2) return rand((A*)0);
	return rand((B*)0);
}

template<typename... T>
std::variant<T...> rand(std::variant<T...> const *){
	std::variant<T...> r;
	
	//return visit([&](auto x){ return rand(&x); },
	auto v=std::variant_size_v<std::variant<T...>>;
	PRINT(v);
	nyi
	return r;
}

template<typename K,typename V>
std::map<K,V> rand(std::map<K,V> const*){
	std::map<K,V> r;
	for(auto _:range(rand()%20)){
		(void)_;
		r[rand((K*)0)]=rand((V*)0);
	}
	return r;
}

template<typename T>
std::vector<T> rand(std::vector<T> const*){
	return mapf(
		[](auto _){
			(void)_;
			return rand((T*)0);
		},
		range(rand()%5)
	);
}

#endif
