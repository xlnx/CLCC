#pragma once

"If"_p = 
    "if"_t + "("_t + "Expr"_p + ")"_t + "Stmt"_p
        >> parser_do(
            if (!set_offset) {
              to_rvalue(ast[0].gen());
              int n = lblcount;
              lblcount += 1;
              out <<  read_previous(R0) <<
                      "           BRz         LBL" << n << "\n";
              ast[1].gen();
              out <<  "LBL" << n << "\n";
            } else {
            	ast[1].gen();
            }
        )
    |"if"_t + "("_t + "Expr"_p + ")"_t + "Stmt"_p + "else"_t + "Stmt"_p
        >> parser_do(
            if (!set_offset) {
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
            } else {
            	ast[1].gen(); ast[2].gen();
            }
        ),
"Loop"_p = 
    "while"_t + "("_t + "Expr"_p + ")"_t + "Stmt"_p
        >> parser_do(
            if (!set_offset) {
              int n = lblcount;   
              lblcount += 2;
              lblcontinue.push(n + 1);
              lblbreak.push(n);
              out <<  "           AND         R0, R0, #0\n"\
                      "LBL" << (n + 1) << "\n";
              to_rvalue(ast[0].gen());
              out <<  read_previous(R0) <<
                      "           BRz         LBL" << n << "\n";
              ast[1].gen();
              out <<  "           BR          LBL" << (n + 1) << "\n"\
                        "LBL" << n << "\n";
                lblcontinue.pop();
                lblbreak.pop();
            } else {
             	ast[1].gen();
            }
        )
    |"do"_t + "Stmt"_p + "while"_t + "("_t + "Expr"_p + ")"_t + ";"_t
        >> parser_do(
            if (!set_offset) {
              int n = lblcount;   
              lblcount += 2;
              lblcontinue.push(n);
              lblbreak.push(n + 1);
                out <<  "           AND         R0, R0, #0\n"\
                        "LBL" << n << "\n";
              ast[0].gen();
                to_rvalue(ast[1].gen());
              out <<  read_previous(R0) <<
                      "           BRnp        LBL" << n << "\n";
                out <<  "LBL" << (n + 1) << "\n";
                lblcontinue.pop();
                lblbreak.pop();
            } else {
             	ast[0].gen();
            }
        )
    |"for"_t + "("_t + "FStmt"_p + "Expr"_p + ";"_t + "Expr"_p + ")"_t + "Stmt"_p
        >> parser_do(
            int off = offset;
            local.push_back(map<string, int>());
            if (!set_offset) {
            	int n = lblcount;   
              lblcount += 2;
              lblcontinue.push(n + 1);
              lblbreak.push(n);
                ast[0].gen();
              out <<    "           AND         R0, R0, #0\n"\
                        "LBL" << (n + 1) << "\n";
              to_rvalue(ast[1].gen());
              out <<  read_previous(R0) <<
                      "           BRz         LBL" << n << "\n";
              ast[3].gen();
                ast[2].gen();
              out <<  "           BR          LBL" << (n + 1) << "\n"\
                        "LBL" << n << "\n";
                lblcontinue.pop();
                lblbreak.pop();
            } else {
            	ast[0].gen();
            	ast[3].gen();
            }
            offset = off;
            local.pop_back();
        ),
