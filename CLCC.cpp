#include <iostream>
#include "new_parser/parser.h"
#include <fstream>
#include <exception>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <memory>
//#include "lctype.h"
#include <sstream>
#include "new_parser/variant/variant.h"
#include "clcc_lib.h"

using namespace std;

enum value_type { rvalue = 0, lvalue = 1, fn = 2 };

using rtype = variant<string, short, vector<string>, value_type>;

using ast_type = ast<rtype>;

constexpr long long overflow_mask = 0xFFFFFFFFFFFF0000;

#define parser_do(...) \
    make_reflect<ast_type>([](ast_type& ast)->rtype{\
    	__VA_ARGS__\
    	return rvalue;\
    })

#define parser_passon() \
    make_reflect<ast_type>([](ast_type& ast)->rtype{\
        return ast[0].gen();\
    })

#define parser_expand() \
    make_reflect<ast_type>([](ast_type& ast)->rtype{\
        for (auto& item: ast) item.gen();\
    	return short(0);\
    })


#define _STR(S) #S
#define STR(S)  _STR(S)

#define read_previous(R) \
    "           LDR         " STR(R) ", R4, #" << eoffset << "\n"
#define write_current(R) \
    "           STR         " STR(R) ", R4, #" << eoffset << "\n"
    
using namespace std;

int offset, min_offset;

vector<map<string, int>> local;

int fncount;

struct fndata: std::pair<string, int> {
	int offset, param;
	fndata()
	{}
	fndata(const string& src, int n, int off, int par):
	   std::pair<string, int>(src, n), offset(off), param(par)
    {}
	bool operator < (const fndata& other) const
	   { return second < other.second; }
};

map<string, fndata> fnlist;

map<string, int> goffset;

string global("");

string Qfnlbl("");

stringstream out("");

int eoffset, lblcount, glbcount = 1;

stack<int> lblbreak, lblcontinue;

bool set_offset;

inline void warning(const string& str)
{
	std::cerr << str << std::endl;
}

template <typename T>
    inline rtype& confirm(rtype& data, const T& value)
    {
    	if (data.get<T>() != value)
    	   throw std::logic_error("consflict!");
    	return data;
    }
    
inline void to_lvalue(const rtype& data)
{
	if (const_cast<rtype&>(data).get<value_type>() != lvalue)
	   throw std::logic_error("cannot assign to a rvalue.");
}

inline void to_rvalue(const rtype& data)
{
	if (const_cast<rtype&>(data).get<value_type>() != rvalue)
	{
    	out << read_previous(R7) <<
    	       "           LDR         R7, R7, #0\n"
    	       << write_current(R7) << "; To Rvalue\n";
    }
}

inline void to_fn(const rtype& data)
{
	if (const_cast<rtype&>(data).get<value_type>() != fn)
	   throw std::logic_error("cannot call a none-function identifier.");
}

#include "clcc_lexer.h"
#include "clcc_parser.h"

int stack_size = 0x1;
int static_size = 0x20;
int buffer_size = 0x301;

string preprocessor(const string& str)
{	
	for (auto itr = &str[0]; *itr != 0; ++itr)
	{
    	if (*itr != ' ' && *itr != '\t' && *itr != '\r')
    	{
    		if (*itr == '#')
    		{
    			char tag[256];
    			int size;
            	if (sscanf(itr + 1, "%s%x", tag, &size) != 2) 
                	throw std::logic_error("unknown preprocessor.");
            	string s = tag;
            	if (s == "stack") {
                	stack_size = size;
                } else if (s == "static") {
                	static_size = size;
                } else if (s == "buffer") {
                	buffer_size = size;
                } else {
                	throw std::logic_error("unknown preprocessor.");
                }
            	return "";
            }
            break;
        }
    }
    return str;
}

int main(int argc, char *argv[])
{
	string buffer, ln;
	if (argc > 1)
    {
		for (int i = 1; i != argc; ++i)
		{
			ifstream in;
			in.open(argv[1]);
            while (getline(in, ln)) {
            	buffer += preprocessor(ln);
            }
			in.close();
		}
	} else 
    {
		while (getline(cin, ln)) {
			if (ln == "") break;
			buffer += preprocessor(ln);
		}
	}
	try {
    	cparser.parse(buffer.c_str());
	} catch(parser_exception<reflected_lexer<ast_type>> e) {
    	cerr << e.what();
    	return 1;
    } catch(std::logic_error e) {
    	cerr << e.what();
    	return 1;
    } catch(std::bad_cast e) {
    	cerr << "invalid token.";
    	return 1;
    }
    
    ostream& output = argc == 1 ? cout :
        *new ofstream("out.asm");
    output << "           .ORIG       x3000\n";
    if (fnlist["main"].first == "") {
    	std::cerr << "lack of main function.\n";
    	return 1;
   	} else 
   	    output << fnlist["main"].first << endl;
    vector<pair<string, fndata>> fnvec(fnlist.begin(), fnlist.end());
    sort(fnvec.begin(), fnvec.end(), [](const pair<string, fndata>& a, 
                const pair<string, fndata>& b){
                	return a.second < b.second;
                });
    for (auto& s: fnvec)
        if (s.first != "main" && s.second.first != "")
            output << "\n\nFn" << s.first << endl << s.second.first << endl;
    output << clcc_io;
    output <<       "FUNCTION               \n";
    for (auto& s: fnvec)
        if (s.second.first != "")
            output <<   "           .FILL       Fn" << s.first << endl;
    //output << global << endl;
    output <<       "           .FILL       CSCANS\n"
                    "           .FILL       CPRINTI\n"
                    "           .FILL       CSCANI\n"
                    "           .FILL       CSCANC\n";
    output <<       "STATIC     .BLKW       #" << static_size << "\n"
                    "           .BLKW       x30\n"
                    "STACK      .BLKW       #1\n"
                    "           .END\n";
    if (argc != 1) {
    	static_cast<ofstream&>(output).close();
        delete &output;
    }
//	cout << buffer;
} 
