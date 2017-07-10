#pragma once

parser<ast_type> cparser( lex, 
	"S"_p =
		"S"_p + "GlobalDecl"_p
			>> parser_expand()
		|""_t
            >> parser_do(),
#   include "clcc_global.h"
#   include "clcc_control_flow.h"
#   include "clcc_statement.h"
#   include "clcc_expression.h"
);
