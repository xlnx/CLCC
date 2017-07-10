#pragma once

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
            throw std::logic_error("not implemented.");
        )
    |"expr1"_p + "<<="_t + "expr"_p
        >> parser_do(
            throw std::logic_error("not implemented.");
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
            throw std::logic_error("not implemented.");
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
                    << write_current(R1) << "; a || b\n"\
                    "           BR          LBL" << (n + 1) << "\n"\
                    "LBL" << n << "\n"\
                    "           AND         R1, R1, #0\n"
                    "           ADD         R1, R1, #1\n"
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
            throw std::logic_error("not implemented.");
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
            throw std::logic_error("not implemented.");
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
            throw std::logic_error("not implemented.");
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
                    "           STR         R2, R1, #0 ; ++lexpr\n";
        )
    |"--"_t + "expr12"_p
        >> parser_do(
            to_lvalue(ast[0].gen());
            out <<  read_previous(R1) <<
                    "           LDR         R2, R1, #0\n"\
                    "           ADD         R2, R2, #-1\n"
                    << write_current(R2) << "; --a\n"\
                    "           STR         R2, R1, #0 ; --lexpr\n";
        )
    |"!"_t + "expr12"_p
        >> parser_do(
            to_rvalue(ast[0].gen());
            out <<  "           AND         R7, R7, #0\n" <<
                    read_previous(R1) <<
                    "           BRnp        LBL" << lblcount << "\n"\
                    "           ADD         R7, R7, #1\n"\
                    "LBL" << lblcount << "\n"
                    << write_current(R7) << "; !a\n";
            lblcount++;
        )
    |"~"_t + "expr12"_p
        >> parser_do(
            to_rvalue(ast[0].gen());
            out <<  read_previous(R1) <<
                    "           NOT         R1, R1\n"
                    << write_current(R1) << "; ~a\n";
        )
    |"*"_t + "expr12"_p
        >> parser_do(
            to_rvalue(ast[0].gen());
            return lvalue;
        )
    |"&"_t + "expr12"_p
        >> parser_do(
            to_lvalue(ast[0].gen());
            return rvalue;
        )
    |"expr13"_p
        >> parser_passon(),
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
            to_rvalue(ast[0].gen());
            eoffset++;
            to_rvalue(ast[1].gen());
            out <<  read_previous(R2);
            eoffset--;
            out <<  read_previous(R1) <<
                    "           ADD         R1, R1, R2\n"\
                    << write_current(R1) << "; a[i]\n";
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
            out <<  "           LD          R1, #1\n"\
                    "           BR          LBL" << lblcount << "\n"\
                    "           .FILL       #" << val << "      ; 16-bit literal\n"\
                    "LBL" << lblcount << write_current(R1);
            lblcount++;
            return rvalue;
        )
    |"chliteral"_t
        >> parser_do(
            short val = ast.term(0).get<short>();
            out <<  "           LD          R1, #1\n"\
                    "           BR          LBL" << lblcount << "\n"\
                    "           .FILL       #" << val << "      ; 16-bit literal\n"\
                    "LBL" << lblcount << write_current(R1);
            lblcount++;
            return rvalue;
        )
    |"strliteral"_t
        >> parser_do(
            string val = ast.term(0).get<string>();
            out <<  "           LEA         R1, #1\n"\
                    "           BR          LBL" << lblcount << "\n"\
                    "           .STRINGZ    " << val << "      ; string literal\n"\
                    "LBL" << lblcount << write_current(R1);
            lblcount++;
            return rvalue;
        )
    |"("_t + "Expr"_p + ")"_t
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
        )
