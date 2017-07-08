#pragma once

#include <fstream>
#include <exception>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
using namespace std;

struct lcdata_type {
	lcdata_type(int w): width(w) {}
	virtual ~lcdata_type() {}
	unsigned int width;
};

class lctype final {
private:
    stack<shared_ptr<lcdata_type>> Q;
public:
    string name;
public:
    lctype(string s = ""):
        name(s)
    {
    }
    template <class T>
    lctype(const shared_ptr<T>& new_type, string s = ""):
        name(s), Q({ new_type }) 
    {
    }
    template <class T>
    void add_type(const shared_ptr<T>& new_type)
    {
    	Q.push(new_type);
    }
    void pop_type()
    {
    	if (Q.empty()) {
    		throw std::logic_error("type??");
   		}
    	Q.pop();
    }
    lcdata_type& type()
    {
    	if (Q.empty()) {
    		throw std::logic_error("type??");
   		}
    	return *Q.top();
    }
    unsigned width() const
    {
    	if (Q.empty()) {
    		throw std::logic_error("type??");
   		}
   		return Q.top()->width;
    }
};

//vector<lctype> types;

struct lcint8: lcdata_type {
	lcint8(): lcdata_type(8) {}
};
struct lcint16: lcdata_type {
	lcint16(): lcdata_type(16) {}
};
struct lcuint8: lcdata_type {
	lcuint8(): lcdata_type(8) {}
};
struct lcuint16: lcdata_type {
	lcuint16(): lcdata_type(16) {}
};
struct lcvoid: lcdata_type {
	lcvoid(): lcdata_type(0) {}
};
struct lcpointer: lcdata_type {
	lcpointer(): lcdata_type(16) {}
};
struct lcarray: lcdata_type {
	lcarray(const lcdata_type& type, unsigned sz):
        lcdata_type(type.width * sz), size(sz) {}
    int size;
};
struct lcfunction: lcdata_type {
	lcfunction(const vector<lctype>& vec):
	   lcdata_type(16), params(vec) {}
    vector<lctype> params;
};
