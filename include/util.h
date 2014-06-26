/*
 * util.h
 *
 *  Created on: Jan 13, 2013
 *      Author: jzhou
 */
#pragma once
#ifndef UTIL_H_
#define UTIL_H_

#include "Buffer.h"
#include <string>
#include <openssl/sha.h>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <queue>
#include <sys/time.h>
#include <readline/readline.h>
#include <readline/history.h>

template<typename T>
struct ascent_ordering {
    bool operator ()(std::pair<size_t, T> const& a, std::pair<size_t, T> const& b) {
        return a.second < b.second;
    }
};

template<typename T>
struct descent_ordering {
    bool operator ()(std::pair<size_t, T> const& a, std::pair<size_t, T> const& b) {
        return a.second > b.second;
    }
};

// a priority stores up to N element, discard one if insertion exceeds N
// each element is a pair <int, T>, comparison is on T
// default is to keep Top-N pairs. If want minimum N pairs, use ascent_ordering
template<typename T, class KeyCompareFunctor = descent_ordering<T> >
class PriorityQueueNPair {
private:
	int N;
	std::priority_queue<int,std::vector<std::pair<int,T> >,KeyCompareFunctor > q;

public:
	PriorityQueueNPair(int n) : N(n) {}

	void push(std::pair<int, T> pair) { push(pair.first, pair.second); }
	void push(int j, T val) {
		if (((int) q.size()) < N) {
			q.push(std::make_pair(j,val));
		} else if (q.top().second < val) {
			// remove the minimum element, essentially kept top-K
			q.pop();
			q.push(std::make_pair(j, val));
		}
	}

	int size() { return q.size(); }
	void pop() { q.pop(); }
	std::pair<int,T> top() { return q.top(); }
};
typedef PriorityQueueNPair<double, ascent_ordering<double> > MaxDoubleNHeap;
typedef PriorityQueueNPair<float, ascent_ordering<float> >   MaxFloatNHeap;

template< class MapType >
void print_map(const MapType & map, const std::string & separator, std::ostream & os )
{
    typedef typename MapType::const_iterator const_iterator;
    for( const_iterator i = map.begin(), iend = map.end(); i != iend; ++i ) {
        os << i->first << separator << i->second << std::endl;
    }
}

template< class SetType >
void print_set(const SetType & set, const std::string & separator, std::ostream & os )
{
    typedef typename SetType::const_iterator const_iterator;
    for( const_iterator i = set.begin(), iend = set.end(); i != iend; ++i ) {
        os << *i << separator;
    }
    os << std::endl;
}

template< class VectorType >
void print_vector(const VectorType & v, const std::string & separator, std::ostream & os )
{
    typedef typename VectorType::const_iterator const_iterator;
    for( const_iterator i = v.begin(), iend = v.end(); i != iend; ++i ) {
        os << *i << separator;
    }
    os << std::endl;
}

static inline void print_sha1(const unsigned char *str)
{
	if (!str) return;
	for(int j = 0; j < SHA_DIGEST_LENGTH; ++j)
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)str[j];
	std::cout << std::endl;
}

// compute log of input using base 2
static inline unsigned char log2(int n) {
	if (n <= 1) return 0;
	int i;
	for (i = -1; n > 0; i++)
		n = n >> 1;
	return i;
}

inline double get_time(void)
{
	struct timeval val;

	gettimeofday(&val, NULL);
	return val.tv_sec * 1.0 + val.tv_usec * 1e-6;
}

// write red on white
inline void alert(const std::string & str)
{
	std::cerr << "\33[0;31m" << str << "\33[0m" << std::endl ;
}

inline std::string my_read_line(std::string prompt)
{
	char *inp = readline(prompt.c_str()); //Instead of getline()
	if (inp == NULL)
		throw std::runtime_error("input stream ended");

	if (*inp)
		add_history(inp);
	std::string str = (const char *) (inp);
	free(inp);
	return str;
}

#endif /* UTIL_H_ */
