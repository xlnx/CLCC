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

reflected_lexer<ast_type> lex(
	"+"_t, "-"_t, "~"_t,
	"*"_t, "/"_t, "%%"_t,
    "<"_t, ">"_t,
	"<<"_t, ">>"_t,
	"<="_t, ">="_t, 
	"=="_t, "!="_t,
	"&"_t, "|"_t, "^"_t, "?"_t, ":"_t, 
	"&&"_t, "||"_t, 
	","_t, ";"_t,
	"="_t, "+="_t, "-="_t, "*="_t, "/="_t, "&="_t, "|="_t,
	"^="_t, "<<="_t, ">>="_t,
	"("_t, ")"_t, "["_t, "]"_t, "{"_t, "}"_t,
	"!"_t, "~"_t, "++"_t, "--"_t, "sizeof"_t,
	"integer"_t = "(0[xX][0-9a-fA-F]+)|([1-9]\\d*)|(0[0-7]*)"_r
		>> lexer_reflect<ast_type>([](const string& src)->rtype{
			size_t idx; long long result = stoll(src, &idx, 0);
			if (result & overflow_mask) {
				warning("integer literal overflow.");
			}
			return short(result);
		}),
	"char"_t = "\\'("
                "(\\\\("
                    "([0-7]+)"
                    "|([xX][0-9A-Fa-f]+)"
                    "|([\\\\abfnrtv\\\'\\\"\\?])"
                "))"
            "|([^\\n\\\\]))*\\'"_r
	    >> lexer_reflect<ast_type>([](const string& src)->rtype{
	    	short result = 0, mby = 0;
	    	for (auto i = 1; i < src.length() - 1; ++i) {
	    		if (mby >= 2) {
                	warning("multiple char literal overflow.");
                	break;
                }
            	switch (src[i]) {
            	case '\\': {
            		switch (src[++i]) {
       	            case '\\': case '\'': case '\"': case '?': 
       	                result |= src[i] << mby++; break;
   	                case 'a': result |= '\a' << mby++; break;
   	                case 'b': result |= '\b' << mby++; break;
   	                case 'f': result |= '\f' << mby++; break;
   	                case 'n': result |= '\n' << mby++; break;
   	                case 'r': result |= '\r' << mby++; break;
   	                case 't': result |= '\t' << mby++; break;
   	                case 'v': result |= '\v' << mby++; break;
   	                case '0': {
                        int diff = 0;
                        ++i;
                        while (src[i] < '8' && src[i] >= '0') {
                        	diff <<= 3;
                        	diff |= src[i] - '0';
                        	++i;
                        }
                        --i;
                        result |= diff << mby++; break;
                    } break;
                    default: {
                    	int diff = 0;
                        ++i;
                        while (src[i] <= '9' && src[i] >= '0' ||
                            src[i] >= 'A' && src[i] <= 'Z' ||
                                src[i] >= 'a' && src[i] <= 'z') {	
                        	diff <<= 4;
                        	if (src[i] <= '9' && src[i] >= '0') {
                            	diff |= src[i] - '0';
                            } else if (src[i] >= 'A' && src[i] <= 'Z') {
                            	diff |= src[i] - 'A';
                            } else {
                            	diff |= src[i] - 'a';
                            }
                        	++i;
                        }
                        --i;
                        result |= diff << mby++; break;
                    } break;
                    }
                } break;
                default: {
                	result |= src[i] << mby++;
                }
                }
            }
            return result;
        }),
	"id"_t = "[A-Za-z_]\\w*"_r
        >> lexer_reflect<ast_type>([](const string& src)->rtype{
       	    return string(src);
        }),
	"var"_t, "fn"_t,
	"break"_t, "else"_t, "switch"_t, "case"_t, "enum"_t, "register"_t,
	"typedef"_t, "char"_t, "return"_t, "union"_t, "const"_t, "continue"_t,
	"for"_t, "void"_t, "default"_t, "goto"_t, "sizeof"_t, "do"_t,
	"if"_t, "while"_t
);

parser<ast_type> cparser( lex, 
	"S"_p =
		"S"_p + "GlobalDecl"_p
			>> parser_expand()
		|""_t
            >> parser_do(),
    "GlobalDecl"_p = 
        "Function"_p
            >> parser_passon()
        |"GlobalVar"_p
            >> parser_passon(),
    "GlobalVar"_p = 
        "var"_t + "id"_t + "GVarR"_p + ";"_t
            >> parser_do(
                auto name = ast.term(1).get<string>();
                if (goffset[name] != 0)
                    throw std::logic_error("global variable redefined.");
                if (fnlist[name].first != "")
                    throw std::logic_error("global variable conflicted with function.");
                if ((goffset[name] = glbcount++) > 0x1F)
                    throw std::logic_error("too many global variable.");
                ast[0].gen();
            ),
    "GVarR"_p = 
        ","_t + "id"_t + "GVarR"_p
            >> parser_do(
                auto name = ast.term(1).get<string>();
                if (goffset[name] != 0)
                    throw std::logic_error("global variable redefined.");
                if (fnlist[name].first != "")
                    throw std::logic_error("global variable conflicted with function.");
                if ((goffset[name] = glbcount++) > 0x1F)
                    throw std::logic_error("too many global variable.");
                ast[0].gen();
            )
        |""_t
            >> parser_do(),
    "IdListL"_p =
        "IdListL"_p + ","_t + "id"_t
            >> parser_do(
                vector<string> vec(ast[0].gen().get<vector<string>>());
                vec.push_back(ast.term(1).get<string>());
                return vec;
            )
        |"id"_t
            >> parser_do( return vector<string>({ ast.term(0).get<string>() }); ),
    "IdList"_p = 
        "IdListL"_p
            >> parser_passon()
        |""_t
            >> parser_do( return vector<string>(); ),
    "Function"_p = 
        "fn"_t + "id"_t + "("_t + "IdList"_p + ")"_t + "Block"_p
            >> parser_do(
                string fn = ast.term(1).get<string>();
                if (fnlist[fn].first != "")
                	throw std::logic_error("muntiple function defination.");
               	if (goffset[fn] != 0)
                    throw std::logic_error("function name conflicted with global variable.");
               	global = out.str();
               	out.str("");
                vector<string> vec(ast[0].gen().get<vector<string>>());
                offset = min_offset = -2;
                while (!local.empty())
                    local.pop_back();
                local.push_back(map<string, int>());
                for (auto& s: vec)
                {
                	if (!local.back()[s]) {
                        local.back()[s] = offset--;
                    } else {
                    	throw std::logic_error("parameter name conflicted.");
                    }
                }
                min_offset = min(offset, min_offset);
                Qfnlbl = "Q" + fn;
                if (fn == "main") {
                	out <<  "           LD          R4, #3\n"\
                            "           LD          R5, #3\n"\
                            "           LD          R6, #3\n"
                            "           BR          Fnmain\n"\
                            "           .FILL       STACK\n"\
                            "           .FILL       FUNCTION\n"\
                            "           .FILL       STATIC\n"\
                            "Fnmain     \n";
                    ast[1].gen();
                    out <<  "           AND         R0, R0, #0\n"\
                            "Qmain      TRAP        x25\n";
                } else {
                	string temp = out.str(), tmp0;
                	out.str("");
                        ast[1].gen();
                        tmp0 = out.str();
                    out.str(temp);
                    out <<  "           LDR         R1, R4, #0\n"\
                            "           ADD         R1, R4, R1\n"\
                            "           STR         R1, R4, #" << -min_offset << "\n"\
	                        "           STR         R7, R4, #" << -min_offset - 1 << "\n"\
	                        "           ADD         R4, R4, #" << -min_offset << "\n"
                            << tmp0 <<
                            "           AND         R0, R0, #0\n"\
                            "Q" << fn << "         LDR         R7, R4, #-1\n"\
	                        "           LDR         R4, R4, #0\n"\
	                        "           RET\n";
                }
                fnlist[fn] = fndata(out.str(), fncount++, -min_offset, vec.size());
                out.str(global);
            ),
    "If"_p = 
        "if"_t + "("_t + "Expr"_p + ")"_t + "Stmt"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                int n = lblcount;
                lblcount += 1;
                out <<  read_previous(R0) <<
                        "           BRz         LBL" << n << "\n";
                ast[1].gen();
                out <<  "LBL" << n << "\n";
            )
        |"if"_t + "("_t + "Expr"_p + ")"_t + "Stmt"_p + "else"_t + "Stmt"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                int n = lblcount;
                lblcount += 2;
                out <<  read_previous(R0) <<
                        "           BRz         LBL" << n << "\n";
                ast[1].gen();
                out <<  "           BR          LBL" << (n + 1) << "\n" <<
                        "LBL" << n << "\n";
                ast[2].gen();
                out <<  "LBL" << (n + 1) << "\n";
            ),
    "While"_p = 
        "while"_t + "("_t + "Expr"_p + ")"_t + "Stmt"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                int n = lblcount;
                lblcount += 1;
                out <<  read_previous(R0) <<
                        "           BRz         LBL" << n << "\n";
                ast[1].gen();
                out <<  "LBL" << n << "\n";
            ),
    "Return"_p = 
        "return"_t + "Expr"_p + ";"_t
            >> parser_do(
                eoffset = 1;
                to_rvalue(ast[0].gen());
                out <<  read_previous(R0) <<
                        "           BR          " << Qfnlbl << "\n";
            )
        |"return"_t + ";"_t
            >> parser_do(
                out <<  "           AND         R0, R0, #0\n"\
                        "           BR          " << Qfnlbl << "\n";
            ),
    "Var"_p = 
        "var"_t + "id"_t + "VarR"_p + ";"_t
            >> parser_do(
                string name = ast.term(1).get<string>();
                if (local.back()[name] == 0)
                    local.back()[name] = offset--, min_offset = min(offset, min_offset);
                else
                    throw std::logic_error("multiple defination.");
                ast[0].gen();
            )
        |"var"_t + "id"_t + "="_t + "expr"_p + "VarR"_p + ";"_t
            >> parser_do(
                string name = ast.term(1).get<string>();
                if (local.back()[name] == 0)
                    local.back()[name] = offset--, min_offset = min(offset, min_offset);
                else
                    throw std::logic_error("multiple defination.");
                to_rvalue(ast[0].gen());
                out <<  read_previous(R1) <<
                        "           STR         R1, R4, #" << offset + 1 << "\n";
                ast[1].gen();
            )
        |""_t
            >> parser_do(),
    "VarR"_p = 
        ","_t + "id"_t + "VarR"_p
            >> parser_do(
                string name = ast.term(1).get<string>();
                if (local.back()[name] == 0)
                    local.back()[name] = offset--, min_offset = min(offset, min_offset);
                else
                    throw std::logic_error("multiple defination.");
                ast[0].gen();
            )
        |""_t
            >> parser_do(),
    "Block"_p =
        "{"_t + "Stmts"_p + "}"_t
            >> parser_do(
                int off = offset;
                local.push_back(map<string, int>());
                ast[0].gen();
                offset = off;
                local.pop_back();
            ),
    "Stmts"_p = 
        "Stmts"_p + "Stmt"_p
            >> parser_expand()
        |""_t
            >> parser_do(),
    "Stmt"_p =
        "Block"_p
            >> parser_passon()
        |"Expr"_p + ";"_t
            >> parser_do(
                eoffset = 1;
                return ast[0].gen();
            )
        |"Return"_p
            >> parser_passon()
        |"If"_p
            >> parser_passon()
        |"Var"_p
            >> parser_passon(),
            
            
    "Expr"_p =
        "expr"_p + ","_t + "Expr"_p
            >> parser_do(
                ast[0].gen();
                return ast[1].gen();
            )
        |"expr"_p
            >> parser_passon(),
    "expr"_p = 
        "expr1"_p + "="_t + "expr"_p
            >> parser_do(
                to_lvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           STR         R2, R1, #0\n"
                        << write_current(R2) << "; Store\n";
            )
        |"expr1"_p + "+="_t + "expr"_p
            >> parser_do(
                to_lvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           LDR         R7, R1, #0\n"\
                        "           ADD         R2, R7, R2\n"\
                        "           STR         R2, R1, #0\n"
                        << write_current(R2) << "; Store\n";
            )
        |"expr1"_p + "-="_t + "expr"_p
            >> parser_do(
                to_lvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           LDR         R7, R1, #0\n"\
                        "           NOT         R2, R2\n"\
                        "           ADD         R2, R2, #1\n"\
                        "           ADD         R2, R7, R2\n"\
                        "           STR         R2, R1, #0\n"
                        << write_current(R2) << "; Store\n";
            )
        |"expr1"_p + "*="_t + "expr"_p
            >> parser_do(
            
            )
        |"expr1"_p + "<<="_t + "expr"_p
            >> parser_do(
            
            )
        |"expr1"_p + "&="_t + "expr"_p
            >> parser_do(
                to_lvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           LDR         R7, R1, #0\n"\
                        "           AND         R2, R7, R2\n"\
                        "           STR         R2, R1, #0\n"
                        << write_current(R2) << "; Store\n";
            )
        |"expr1"_p + "|="_t + "expr"_p
            >> parser_do(
                to_lvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           LDR         R7, R1, #0\n"\
                        "           NOT         R2, R2\n"\
                        "           NOT         R7, R7\n"\
                        "           AND         R2, R7, R2\n"\
                        "           NOT         R2, R2\n"\
                        "           STR         R2, R1, #0\n"
                        << write_current(R2) << "; Store\n";
            )
        |"expr1"_p + "^="_t + "expr"_p
            >> parser_do(
            
            )
        |"expr1"_p
            >> parser_passon(),
    "expr1"_p = 
        "expr2"_p + "?"_t + "expr1"_p + ":"_t + "expr1"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                int n = lblcount;
                lblcount += 2;
                out <<  read_previous(R0) <<
                        "           BRz         LBL" << n << "\n";
                to_rvalue(ast[1].gen());
                out <<  "           BR          LBL" << (n + 1) << "\n" <<
                        "LBL" << n << "\n";
                to_rvalue(ast[2].gen());
                out <<  "LBL" << (n + 1) << "\n";
            )
        |"expr2"_p
            >> parser_passon(),
    "expr2"_p = 
        "expr2"_p + "||"_t + "expr3"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                int n = lblcount;
                lblcount += 2;
                out <<  read_previous(R0) <<
                        "           BRnp        LBL" << n << "\n";
                to_rvalue(ast[1].gen());
                out <<  read_previous(R0) <<
                        "           BRnp        LBL" << n << "\n"\
                        "           AND         R1, R1, #0\n"\
                        "           ADD         R1, R1, #1\n"
                        << write_current(R1) << "; a || b\n"\
                        "           BR          LBL" << (n + 1) << "\n"\
                        "LBL" << n << "\n"\
                        "           AND         R1, R1, #0\n"
                        << write_current(R1) << "; a || b\n"\
                        "LBL" << (n + 1) << "\n";
            )
        |"expr3"_p
            >> parser_passon(),
    "expr3"_p =
        "expr3"_p + "&&"_t + "expr4"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                int n = lblcount;
                lblcount += 2;
                out <<  read_previous(R0) <<
                        "           BRz         LBL" << n << "\n";
                to_rvalue(ast[1].gen());
                out <<  read_previous(R0) <<
                        "           BRz         LBL" << n << "\n"\
                        "           AND         R1, R1, #0\n"\
                        "           ADD         R1, R1, #1\n"
                        << write_current(R1) << "; a && b\n"\
                        "           BR          LBL" << (n + 1) << "\n"\
                        "LBL" << n << "\n"\
                        "           AND         R1, R1, #0\n"
                        << write_current(R1) << "; a && b\n"\
                        "LBL" << (n + 1) << "\n";
            )
        |"expr4"_p
            >> parser_passon(),
    "expr4"_p = 
        "expr4"_p + "|"_t + "expr5"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           NOT         R1, R1\n"\
                        "           NOT         R2, R2\n"\
                        "           AND         R1, R1, R2\n"\
                        "           NOT         R1, R1\n"
                        << write_current(R1) << "; a + b\n";
            )
        |"expr5"_p
            >> parser_passon(),
    "expr5"_p = 
        "expr5"_p + "^"_t + "expr6"_p
            >> parser_do(
            
            )
        |"expr6"_p
            >> parser_passon(),
    "expr6"_p = 
        "expr6"_p + "&"_t + "expr7"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           AND         R1, R1, R2\n"
                        << write_current(R1) << "; a + b\n";
            )
        |"expr7"_p
            >> parser_passon(),
    "expr7"_p = 
        "expr7"_p + "!="_t + "expr8"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           NOT         R2, R2\n"\
                        "           ADD         R2, R2, #1\n"\
                        "           AND         R0, R0, #0\n"\
                        "           ADD         R1, R1, R2\n"\
                        "           BRz         LBL" << lblcount << "\n"\
                        "           ADD         R0, R0, #1\n"\
                        "LBL" << lblcount << write_current(R0) << "; a != b\n";
                lblcount++;
            )
        |"expr7"_p + "=="_t + "expr8"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           NOT         R2, R2\n"\
                        "           ADD         R2, R2, #1\n"\
                        "           AND         R0, R0, #0\n"\
                        "           ADD         R1, R1, R2\n"\
                        "           BRnp        LBL" << lblcount << "\n"\
                        "           ADD         R0, R0, #1\n"\
                        "LBL" << lblcount << write_current(R0) << "; a == b\n";
                lblcount++;
            )
        |"expr8"_p
            >> parser_passon(),
    "expr8"_p = 
        "expr8"_p + "<="_t + "expr9"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;      // R1 = a - b <= 0 -> 1 : 0
                out <<  read_previous(R1) <<
                        "           NOT         R2, R2\n"\
                        "           ADD         R2, R2, #1\n"\
                        "           AND         R0, R0, #0\n"\
                        "           ADD         R1, R1, R2\n"\
                        "           BRp         LBL" << lblcount << "\n"\
                        "           ADD         R0, R0, #1\n"\
                        "LBL" << lblcount << write_current(R0) << "; a <= b\n";
                lblcount++;
            )
        |"expr8"_p + "<"_t + "expr9"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;      // 
                out <<  read_previous(R1) <<
                        "           NOT         R2, R2\n"\
                        "           ADD         R2, R2, #1\n"\
                        "           AND         R0, R0, #0\n"\
                        "           ADD         R1, R1, R2\n"\
                        "           BRzp        LBL" << lblcount << "\n"\
                        "           ADD         R0, R0, #1\n"\
                        "LBL" << lblcount << write_current(R0) << "; a < b\n";
                lblcount++;
            )
        |"expr8"_p + ">="_t + "expr9"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R1);
                eoffset--;      // 
                out <<  read_previous(R2) <<
                        "           NOT         R2, R2\n"\
                        "           ADD         R2, R2, #1\n"\
                        "           AND         R0, R0, #0\n"\
                        "           ADD         R1, R1, R2\n"\
                        "           BRp         LBL" << lblcount << "\n"\
                        "           ADD         R0, R0, #1\n"\
                        "LBL" << lblcount << write_current(R0) << "; a >= b\n";
                lblcount++;
            )
        |"expr8"_p + ">"_t + "expr9"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R1);
                eoffset--;      // 
                out <<  read_previous(R2) <<
                        "           NOT         R2, R2\n"\
                        "           ADD         R2, R2, #1\n"\
                        "           AND         R0, R0, #0\n"\
                        "           ADD         R1, R1, R2\n"\
                        "           BRzp        LBL" << lblcount << "\n"\
                        "           ADD         R0, R0, #1\n"\
                        "LBL" << lblcount << write_current(R0) << "; a > b\n";
                lblcount++;
            )
        |"expr9"_p
            >> parser_passon(),
    "expr9"_p = 
        "expr9"_p + "<<"_t + "expr10"_p
            >> parser_do(
            
            )
        |"expr10"_p
            >> parser_passon(),
    "expr10"_p = 
        "expr10"_p + "+"_t + "expr11"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           ADD         R1, R1, R2\n"
                        << write_current(R1) << "; a + b\n";
            )
        |"expr10"_p + "-"_t + "expr11"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                eoffset++;
                to_rvalue(ast[1].gen());
                out <<  read_previous(R2);
                eoffset--;
                out <<  read_previous(R1) <<
                        "           NOT         R2, R2\n"\
                        "           ADD         R2, R2, #1\n"\
                        "           ADD         R1, R1, R2\n"
                        << write_current(R1) << "; a - b\n";
            )
        |"expr11"_p
            >> parser_passon(),
    "expr11"_p = 
        "expr11"_p + "*"_t + "expr12"_p
            >> parser_do(
            
            )
        |"expr12"_p
            >> parser_passon(),
    "expr12"_p = 
        "-"_t + "expr12"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                out <<  read_previous(R1) <<
                        "           NOT         R1, R1\n"\
                        "           ADD         R1, R1, #1\n"
                        << write_current(R1) << "; -a\n";
            )
        |"++"_t + "expr12"_p
            >> parser_do(
                to_lvalue(ast[0].gen());
                out <<  read_previous(R1) << 
                        "           LDR         R2, R1, #0\n"\
                        "           ADD         R2, R2, #1\n"
                        << write_current(R2) << "; ++a\n"\
                        "           STR         R2, R1, #0 ; lexpr++\n";
            )
        |"--"_t + "expr12"_p
            >> parser_do(
                to_lvalue(ast[0].gen());
                out <<  read_previous(R1) <<
                        "           LDR         R2, R1, #0\n"\
                        "           ADD         R2, R2, #-1\n"
                        << write_current(R2) << "; --a\n"\
                        "           STR         R2, R1, #0 ; lexpr--\n";
            )
        |"!"_t + "expr12"_p
            >> parser_do(
            
            )
        |"~"_t + "expr12"_p
            >> parser_do(
                to_rvalue(ast[0].gen());
                out <<  read_previous(R1) <<
                        "           NOT         R1, R1\n"
                        << write_current(R1) << "; ~a\n";
            )
        |"expr13"_p
            >> parser_passon(),
    "exprListL"_p =
        "exprListL"_p + "expr"_p + ","_t
            >> parser_do(
                to_rvalue(ast[1].gen());
                eoffset++;
                short cnt = ast[0].gen().get<short>();
                return short(cnt + 1);
            )
        |""_t
            >> parser_do(
                return short(0);
            ),
    "exprList"_p = 
        "exprListL"_p + "expr"_p
            >> parser_do(
                to_rvalue(ast[1].gen());
                eoffset++;
                short cnt = ast[0].gen().get<short>();
                eoffset -= cnt + 1;
                return short(cnt + 1);
            )
        |""_t
            >> parser_do(
                return short(0);
            ),
    "expr13"_p = 
        "expr13"_p + "++"_t
            >> parser_do(
                to_lvalue(ast[0].gen());
                out <<  read_previous(R1) <<
                        "           LDR         R2, R1, #0\n"
                        << write_current(R2) << "; a++\n"\
                        "           ADD         R2, R2, #1\n"\
                        "           STR         R2, R1, #0 ; lexpr++\n";
            )
        |"expr13"_p + "--"_t
            >> parser_do(
                to_lvalue(ast[0].gen());
                out <<  read_previous(R1) <<
                        "           LDR         R2, R1, #0\n"
                        << write_current(R2) << "; a--\n"\
                        "           ADD         R2, R2, #-1\n"\
                        "           STR         R2, R1, #0 ; lexpr--\n";
            )
        |"id"_t + "("_t + "exprList"_p + ")"_t
            >> parser_do(
                //to_fn(ast[0].gen());
                string name = ast.term(0).get<string>();
                if (fnlist[name].first != "")
                {
                    out <<     "           LDR         R1, R5, #" << fnlist[name].second << "\n"
                	           << write_current(R1) << "; Function\n";
                } else throw std::logic_error("cannot call a non-function object.");
                eoffset++;
                short cnt = ast[0].gen().get<short>();
                eoffset += cnt - 1;
                for (int i = cnt - 1; i != -1; --i) {
                    out <<  read_previous(R1) << "; Param" << i << "\n"
                            "           STR         R1, R4, #" << (eoffset + fnlist[name].offset - fnlist[name].param - 2) << "\n";
                    eoffset--;
                }
                out <<  "           AND         R1, R1, #0\n"\
                        "           STR         R1, R4, #" << (eoffset + fnlist[name].offset) << "\n"
                        <<  read_previous(R7) <<
                        "           AND         R1, R1, #0\n"\
                        "           ADD         R1, R1, #" << -eoffset << "\n"\
                        "           STR         R1, R4, #" << eoffset << "\n"\
                        "           ADD         R4, R4, #" << eoffset << "\n"\
                        "           JSRR        R7      ; call\n"
                        << write_current(R0) << "; ret value\n";
            )
        |"expr13"_p + "["_t + "Expr"_p + "]"_t
            >> parser_do(
                return lvalue;
            )
        |"expr14"_p
            >> parser_passon(),
    "expr14"_p = 
        "id"_t
            >> parser_do(
                auto name = ast.term(0).get<string>();
                for (auto& m: local)
                	if (m[name] != 0)
                    {
                        out << "           ADD         R1, R4, #" << m[name] << "\n"
                               << write_current(R1) << "; Lvalue\n";                    	
                        return lvalue;
                    }
                if (goffset[name] != 0) 
                	out <<     "           ADD         R1, R6, #" << goffset[name] << "\n"
                	           << write_current(R1) << "; Lvalue\n";
                else if (fnlist[name].first != "")
                {
                    out <<     "           LDR         R1, R5, #" << fnlist[name].second << "\n"
                	           << write_current(R1) << "; Function\n";
                    return fn;
                } else 
                    throw std::logic_error("Undeclared identifier.");
                return lvalue;
            )
        |"integer"_t
            >> parser_do(
                short val = ast.term(0).get<short>();
                out <<  "           AND         R1, R1, #0\n"\
                        "           ADD         R1, R1, #" << val << "\n"
                        << write_current(R1) << "; 16-bit literal\n";
                return rvalue;
            )
        |"char"_t
            >> parser_do(
                short val = ast.term(0).get<short>();
                out <<  "           AND         R1, R1, #0\n"\
                        "           ADD         R1, R1, #" << val << "\n"
                        << write_current(R1) << "; 16-bit literal\n";
                return rvalue;
            )
        |"("_t + "Expr"_p + ")"_t
            >> parser_passon()
);

int main(int argc, char *argv[])
{
	/*for (int i = 1; i != argc; ++i)
	{
		ifstream in;
		in.open(argv[i]);

		in.close();
	}*/
	string buffer, ln;
	while (getline(cin, ln)) {
		if (ln == "") break;
		buffer += ln + '\n';
	}
	try {
    	cparser.parse(buffer.c_str());
	} catch(parser_exception<reflected_lexer<ast_type>> e) {
    	cerr << e.what();
    }
    ostream& output = cout;
    output << "           .ORIG       x3000\n";
    if (fnlist["main"].first == "")
    	throw std::logic_error("lack of main function.");
   	else 
   	    output << fnlist["main"].first << endl;
    vector<pair<string, fndata>> fnvec(fnlist.begin(), fnlist.end());
    sort(fnvec.begin(), fnvec.end(), [](const pair<string, fndata>& a, 
                const pair<string, fndata>& b){
                	return a.second < b.second;
                });
    for (auto& s: fnvec)
        if (s.first != "main")
            output << "\n\nFn" << s.first << endl << s.second.first << endl;
    output <<       "FUNCTION               \n";
    for (auto& s: fnvec)
        output <<   "           .FILL       Fn" << s.first << endl;
    output <<       "           .BLKW       x20\n"\
                    "STATIC     .BLKW       x20\n";
    //output << global << endl;
    output <<       "           .BLKW       x30\n"\
                    "STACK      .BLKW       #1\n"\
                    "           .END\n";
    
//	cout << buffer;
} 
