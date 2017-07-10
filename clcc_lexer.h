#pragma once

reflected_lexer<ast_type> lex(
	"+"_t, "-"_t, "~"_t,
	"*"_t, "/"_t, "%"_t,
    "<"_t, ">"_t,
	"<<"_t, ">>"_t,
	"<="_t, ">="_t, 
	"&"_t, "|"_t, "^"_t, "?"_t, ":"_t, 
	"&&"_t, "||"_t, 
	","_t, ";"_t,
	"="_t, "+="_t, "-="_t, "*="_t, "/="_t, "&="_t, "|="_t, "%="_t
	"^="_t, "<<="_t, ">>="_t,
	"("_t, ")"_t, "["_t, "]"_t, "{"_t, "}"_t,
	"!"_t, "++"_t, "--"_t, "sizeof"_t,
	"=="_t, "!="_t,
	"integer"_t = "(0[xX][0-9a-fA-F]+)|([1-9]\\d*)|(0[0-7]*)"_r
		>> lexer_reflect<ast_type>([](const string& src)->rtype{
			size_t idx; long long result = stoll(src, &idx, 0);
			if (result & overflow_mask) {
				warning("integer literal overflow.");
			}
			return short(result);
		}),
	"chliteral"_t = "\\'("
                "(\\\\("
                    "([0-7]+)"
                    "|([xX][0-9A-Fa-f]+)"
                    "|([\\\\abfnrtv\\\'\\\"\\?])"
                "))"
            "|([^\\\\'\\n\\\\]))*\\'"_r
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
    "strliteral"_t = "\\\"("
                "(\\\\"
                    "[\\\\abfnrtv\\\'\\\"\\?]"
                ")"
            "|([^\\\\\"\\n\\\\]))*\\\""_r
	    >> lexer_reflect<ast_type>([](const string& src)->rtype{	
            return string(src);
        }),
	"id"_t = "[A-Za-z_]\\w*"_r
        >> lexer_reflect<ast_type>([](const string& src)->rtype{
       	    return string(src);
        }),
	"var"_t, "fn"_t,
	"break"_t, "else"_t, "switch"_t, "case"_t, "enum"_t, "register"_t,
	"return"_t, "union"_t, "const"_t, "continue"_t,
	"for"_t, "default"_t, "goto"_t, "sizeof"_t, "do"_t,
	"if"_t, "while"_t, 
    "printd"_t, "printc"_t, "prints"_t, 
    "scanc"_t, "scand"_t, "scans"_t
);
