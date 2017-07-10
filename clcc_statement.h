#pragma once

"Var"_p = 
    "var"_t + "id"_t + "VarR"_p
        >> parser_do(
            string name = ast.term(1).get<string>();
            if (local.back()[name] == 0)
                local.back()[name] = offset--, min_offset = min(offset, min_offset);
            else
                throw std::logic_error("multiple defination.");
            ast[0].gen();
        )
    |"var"_t + "id"_t + "["_t + "integer"_t + "]"_t + "VarR"_p
        >> parser_do(
            string name = ast.term(1).get<string>();
            if (local.back()[name] == 0) {
                short size = ast.term(3).get<short>();
                local.back()[name] = offset -= size - 1, min_offset = min(offset, min_offset);
                out <<  "           AND         R0, R0, #0\n"
                        "           ADD         R0, R4, #" << offset + 1 << "\n"
                        "           STR         R0, R4, #" << offset << "\n";
                offset--;
            } else
                throw std::logic_error("multiple defination.");
            ast[0].gen();
        )
    |"var"_t + "id"_t + "="_t + "expr"_p + "VarR"_p
        >> parser_do(
            string name = ast.term(1).get<string>();
            if (local.back()[name] == 0)
                local.back()[name] = offset--, min_offset = min(offset, min_offset);
            else
                throw std::logic_error("multiple defination.");
            if (!set_offset) {
                to_rvalue(ast[0].gen());
                out <<  read_previous(R1) <<
                    "           STR         R1, R4, #" << offset + 1 << "\n";
            }
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
    |","_t + "id"_t + "["_t + "integer"_t + "]"_t + "VarR"_p
        >> parser_do(
            string name = ast.term(1).get<string>();
            if (local.back()[name] == 0) {
                short size = ast.term(3).get<short>();
                local.back()[name] = offset -= size, min_offset = min(offset, min_offset);
                out <<  "           AND         R0, R0, #0\n"
                        "           ADD         R0, R4, #" << offset + 1 << "\n"
                        "           STR         R0, R4, #" << offset << "\n";
                offset--;
            } else
                throw std::logic_error("multiple defination.");
            ast[0].gen();
        )
    |","_t + "id"_t + "="_t + "expr"_p + "VarR"_p
        >> parser_do(
            string name = ast.term(1).get<string>();
            if (local.back()[name] == 0)
                local.back()[name] = offset--, min_offset = min(offset, min_offset);
            else
                throw std::logic_error("multiple defination.");
            if (!set_offset) {
                to_rvalue(ast[0].gen());
                out <<  read_previous(R1) <<
                    "           STR         R1, R4, #" << offset + 1 << "\n";
            }
            ast[1].gen();
        )
    |""_t
        >> parser_do(),
"Block"_p =
    "{"_t + "Stmts"_p + "}"_t
        >> parser_do(
            eoffset = 1;
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
"FStmt"_p = 
    "Expr"_p + ";"_t
        >> parser_do(
            if (!set_offset) {
                eoffset = 1;
                return ast[0].gen();
            }
        )
    |"Var"_p + ";"_t
        >> parser_passon(),
"Control"_p = 
    "break"_t
        >> parser_do(
            if (lblbreak.empty())
                throw std::logic_error("cannot break because no loop.");
            out << "            BR          LBL" << lblbreak.top() << "\n";
        )
    |"continue"_t
        >> parser_do(
            if (lblcontinue.empty())
                throw std::logic_error("cannot continue because no loop.");
            out << "            BR          LBL" << lblcontinue.top() << "\n";
        )
    |"return"_t + "Expr"_p
        >> parser_do(
            eoffset = 1;
            to_rvalue(ast[0].gen());
            out <<  read_previous(R0) <<
                    "           BR          " << Qfnlbl << "\n";
        )
    |"return"_t
        >> parser_do(
            out <<  "           AND         R0, R0, #0\n"\
                    "           BR          " << Qfnlbl << "\n";
        ),
"ScancR"_p =
    ","_t + "expr"_p + "ScancR"_p
        >> parser_do(
            eoffset = 1;
            to_lvalue(ast[0].gen());
            out <<  read_previous(R1) <<
                    //"           TRAP        x23\n"
                    "           LDR         R7, R6, #-1\n"\
                    "           JSRR        R7\n"\
                    "           STR         R0, R1, #0\n";
            ast[1].gen();
        )
    |""_t
        >> parser_do(),
"ScandR"_p = 
    ","_t + "expr"_p + "ScandR"_p
        >> parser_do(
            eoffset = 1;
            to_lvalue(ast[0].gen());
            out <<  read_previous(R1) <<
                    //"           TRAP        x23\n"
                    "           LDR         R7, R6, #-2\n"\
                    "           JSRR        R7\n"\
                    "           STR         R0, R1, #0\n";
            ast[1].gen();
        )
    |""_t
        >> parser_do(),
"PrintcR"_p = 
    ","_t + "Expr"_p + "PrintcR"_p
        >> parser_do(
            eoffset = 1;
            to_rvalue(ast[0].gen());
            out <<  read_previous(R0) <<
                    "           TRAP        x21\n";
            ast[1].gen();
        )
    |""_t
        >> parser_do(),
"PrintdR"_p =
    ","_t + "Expr"_p + "PrintdR"_p
        >> parser_do(
            eoffset = 1;
            to_rvalue(ast[0].gen());
            out <<  read_previous(R0) <<
                    "           LDR         R7, R6, #-3\n"\
                    "           JSRR        R7\n";
            ast[1].gen();
        )
    |""_t
        >> parser_do(),
"PrintsR"_p = 
    ","_t + "Expr"_p + "PrintsR"_p
        >> parser_do(
            eoffset = 1;
            to_rvalue(ast[0].gen());
            out <<  read_previous(R0) <<
                    "           TRAP        x22\n";
            ast[1].gen();
        )
    |""_t
        >> parser_do(),
"ScansR"_p = 
    ","_t + "Expr"_p + "ScansR"_p
        >> parser_do(
            eoffset = 1;
            to_rvalue(ast[0].gen());
            out <<  read_previous(R0) <<
                    "           LDR         R7, R6, #-4\n"\
                    "           JSRR        R7\n";
            ast[1].gen();
        )
    |""_t
        >> parser_do(),
"IO"_p = 
    "scanc"_t + "expr"_p + "ScancR"_p
        >> parser_do(
            eoffset = 1;
            to_lvalue(ast[0].gen());
            out <<  read_previous(R1) <<
                    //"           TRAP        x23\n"
                    "           LDR         R7, R6, #-1\n"\
                    "           JSRR        R7\n"\
                    "           STR         R0, R1, #0\n";
            ast[1].gen();
        )
    |"scand"_t + "expr"_p + "ScandR"_p
        >> parser_do(
            eoffset = 1;
            to_lvalue(ast[0].gen());
            out <<  read_previous(R1) <<
                    //"           TRAP        x23\n"
                    "           LDR         R7, R6, #-2\n"\
                    "           JSRR        R7\n"\
                    "           STR         R0, R1, #0\n";
            ast[1].gen();
        )
    |"scans"_t + "Expr"_p + "ScansR"_p
        >> parser_do(
            eoffset = 1;
            to_rvalue(ast[0].gen());
            out <<  read_previous(R0) <<
                    "           LDR         R7, R6, #-4\n"\
                    "           JSRR        R7\n";
            ast[1].gen();
        )
    |"printc"_t + "Expr"_p + "PrintcR"_p
        >> parser_do(
            eoffset = 1;
            to_rvalue(ast[0].gen());
            out <<  read_previous(R0) <<
                    "           TRAP        x21\n";
            ast[1].gen();
        )
    |"printd"_t + "Expr"_p + "PrintdR"_p
        >> parser_do(
            eoffset = 1;
            to_rvalue(ast[0].gen());
            out <<  read_previous(R0) <<
                    "           LDR         R7, R6, #-3\n"\
                    "           JSRR        R7\n";
            ast[1].gen();
        )
    |"prints"_t + "Expr"_p + "PrintsR"_p
        >> parser_do(
            eoffset = 1;
            to_rvalue(ast[0].gen());
            out <<  read_previous(R0) <<
                    "           TRAP        x22\n";
            ast[1].gen();
        ),
"Stmt"_p =
    "IO"_p + ";"_t
        >> parser_do(
            if (!set_offset) {
            	return ast[0].gen();
            }
        )
    |"Block"_p
        >> parser_passon()
    |"Expr"_p + ";"_t
        >> parser_do(
            if (!set_offset) {
                eoffset = 1;
                return ast[0].gen();
            }
        )
    |"If"_p
        >> parser_passon()
    |"Var"_p + ";"_t
        >> parser_passon()
    |"Loop"_p
        >> parser_passon()
    |"Control"_p + ";"_t
        >> parser_do(
            if (!set_offset) {
            	return ast[0].gen();
            }
        ),
