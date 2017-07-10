#pragma once

"GlobalDecl"_p = 
    "GlobalVar"_p
        >> parser_passon()
    |"Function"_p
        >> parser_passon(),
"GlobalVar"_p = 
    "var"_t + "id"_t + "GVarR"_p + ";"_t
        >> parser_do(
            auto name = ast.term(1).get<string>();
            //cout << name << endl;
            if (goffset[name] != 0)
                throw std::logic_error("global variable redefined.");
            if (fnlist[name].first != "")
                throw std::logic_error("global variable conflicted with function.");
            if ((goffset[name] = glbcount++) > 0x1F)
                throw std::logic_error("too many global variable.");
            ast[0].gen();
        )
    |"var"_t + "id"_t + "["_t + "integer"_t + "]"_t + "GVarR"_p + ";"_t
        >> parser_do(
            auto name = ast.term(1).get<string>();
            //cout << name << endl;
            if (goffset[name] != 0)
                throw std::logic_error("global variable redefined.");
            if (fnlist[name].first != "")
                throw std::logic_error("global variable conflicted with function.");
            if ((goffset[name] = glbcount) > 0x1F)
                throw std::logic_error("too many global variable.");
            glbcount += ast.term(3).get<short>();
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
    |","_t + "id"_t + "["_t + "integer"_t + "]"_t + "GVarR"_p
        >> parser_do(
            auto name = ast.term(1).get<string>();
            if (goffset[name] != 0)
                throw std::logic_error("global variable redefined.");
            if (fnlist[name].first != "")
                throw std::logic_error("global variable conflicted with function.");
            if ((goffset[name] = glbcount) > 0x1F)
                throw std::logic_error("too many global variable.");
            glbcount += ast.term(3).get<short>();
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
            //cout << " "<< fn << endl;
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
            auto tmp = offset;
            set_offset = true;
            ast[1].gen();
            set_offset = false;
            offset = tmp;
            fnlist[fn] = fndata("?", fncount++, -min_offset, vec.size());
            while (!lblbreak.empty())
                lblbreak.pop();
            while (!lblcontinue.empty())
                lblcontinue.pop();
            if (out.str() != "")
                throw std::logic_error("Compiler has reported a bug!");
            out.str("");
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
                out <<  "           LDR         R1, R4, #0\n"\
                        "           ADD         R1, R4, R1\n"\
                        "           STR         R1, R4, #" << -min_offset << "\n"\
                      "           STR         R7, R4, #" << -min_offset - 1 << "\n"\
                      "           ADD         R4, R4, #" << -min_offset << "\n";
                ast[1].gen();
                out <<  "           AND         R0, R0, #0\n"\
                        "Q" << fn << "         LDR         R7, R4, #-1\n"\
                      "           LDR         R4, R4, #0\n"\
                      "           RET\n";
            }
            fnlist[fn] = fndata(out.str(), fnlist[fn].second, fnlist[fn].offset, fnlist[fn].param);
            out.str(global);
        ),
