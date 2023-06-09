%skeleton "lalr1.cc"
%require "3.0"
%debug
%defines
%define api.namespace{drewgon}
%define api.parser.class {Parser}
%define parse.error verbose
%output "parser.cc"
%token-table

%code requires{
	#include <list>
	#include "tokens.hpp"
	#include "ast.hpp"
	namespace drewgon {
		class Scanner;
	}

//The following definition is required when 
// we don't use the %locations directive (which we won't)
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

//End "requires" code
}

%parse-param { drewgon::Scanner &scanner }
%parse-param { drewgon::ProgramNode** root }
%code{
   // C std code for utility functions
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   // Our code for interoperation between scanner/parser
   #include "scanner.hpp"
   #include "ast.hpp"
   #include "tokens.hpp"

  //Request tokens from our scanner member, not 
  // from a global function
  #undef yylex
  #define yylex scanner.yylex
}

/*
The %union directive is a way to specify the 
set of possible types that might be used as
translation attributes on a symbol.
For this project, only terminals have types (we'll
have translation attributes for non-terminals in the next
project)
*/
%union {
   drewgon::Token* lexeme;
   drewgon::Token* transToken;
   drewgon::IDToken*                       transIDToken;
   drewgon::IntLitToken *			   	   transIntToken;
   drewgon::StrToken *					   transStrToken;
   drewgon::ProgramNode*                   transProgram;
   std::list<drewgon::DeclNode *> *        transDeclList;
   drewgon::VarDeclNode *                  transVarDecl;
   drewgon::TypeNode *                     transType;
   drewgon::IDNode *                       transID;
   drewgon::FnDeclNode *				   transFnDecl;
   drewgon::StmtNode *					   transStmt;
   std::list<drewgon::StmtNode *> *		   transStmtList;
   drewgon::FormalDeclNode *			   transFormal;
   std::list<drewgon::FormalDeclNode *> *  transFormalList;
   drewgon::FnTypeNode *				   transFnType;
   std::list<drewgon::TypeNode *> *		   transTypeList;
   drewgon::AssignExpNode *				   transAssignExp;
   drewgon::ExpNode *					   transExp;
   drewgon::CallExpNode *				   transCallExp;
   std::list<drewgon::ExpNode *> *		   transActuals;
   drewgon::StmtNode *					   transBlockStmt;

   
}

%define parse.assert

/* Terminals 
 *  No need to touch these, but do note the translation type
 *  of each node. Most are just "transToken", which is defined in
 *  the %union above to mean that the token translation is an instance
 *  of drewgon::Token *, and thus has no fields (other than line and column).
 *  Some terminals, like ID, are "transIDToken", meaning the translation
 *  also has a name field. 
*/
%token                   END	   0 "end file"
%token	<transToken>     AND
%token	<transToken>     ARROW
%token	<transToken>     ASSIGN
%token	<transToken>     BOOL
%token	<transToken>     COMMA
%token	<transToken>     DIVIDE
%token	<transToken>     ELSE
%token	<transToken>     EQUALS
%token	<transToken>     FALSE
%token	<transToken>     FN
%token	<transToken>     FOR
%token	<transToken>     GREATER
%token	<transToken>     GREATEREQ
%token	<transIDToken>   ID
%token	<transToken>     IF
%token	<transToken>     INPUT
%token	<transToken>     INT
%token	<transIntToken>  INTLITERAL
%token	<transToken>     LCURLY
%token	<transToken>     LESS
%token	<transToken>     LESSEQ
%token	<transToken>     LPAREN
%token	<transToken>     MINUS
%token	<transToken>     MAYHEM
%token	<transToken>     NOT
%token	<transToken>     NOTEQUALS
%token	<transToken>     OR
%token	<transToken>     OUTPUT
%token	<transToken>     PLUS
%token	<transToken>     POSTDEC
%token	<transToken>     POSTINC
%token	<transToken>     RETURN
%token	<transToken>     RCURLY
%token	<transToken>     RPAREN
%token	<transToken>     SEMICOL
%token	<transStrToken>  STRINGLITERAL
%token	<transToken>     TIMES
%token	<transToken>     TRUE
%token	<transToken>     VOID
%token	<transToken>     WHILE

/* Nonterminals
*  The specifier in angle brackets
*  indicates the type of the translation attribute using
*  the names defined in the %union directive above
*  TODO: You will need to add more attributes for other nonterminals
*  to this list as you add productions to the grammar
*  below (along with indicating the appropriate translation
*  attribute type).
*/
/*    (attribute type)    (nonterminal)    */
%type <transProgram>    program
%type <transDeclList>   globals
%type <transVarDecl>    varDecl
%type <transType>       type
%type <transType>       primType
%type <transID>         id
%type <transStmt>		stmt
%type <transStmtList>	stmtList
%type <transFormalList>	formals
%type <transFormal>		formalDecl
%type <transFnDecl>		fnDecl
%type <transFnType>		fnType
%type <transTypeList>	typeList
%type <transAssignExp>	assignExp
%type <transExp>		exp
%type <transExp>		term
%type <transCallExp>	callExp
%type <transActuals>	actualsList
%type <transBlockStmt>	blockStmt

%right ASSIGN
%left OR
%left AND
%nonassoc LESS GREATER LESSEQ GREATEREQ EQUALS NOTEQUALS
%left MINUS PLUS
%left TIMES DIVIDE
%left NOT 

%%

program 	: globals
		  {
		  $$ = new ProgramNode($1);
		  *root = $$;
		  }

globals 	: globals varDecl SEMICOL
	  	  { 
	  	  $$ = $1; 
	  	  DeclNode * declNode = $2;
		  $$->push_back(declNode);
	  	  }
		| globals fnDecl
		  {
		  //TODO: like most of the SDD clauses,
		  // you'll need to add some code here
		  // to make the LHS symbol have a reasonable
		  // translation to create an AST

		  $$ = $1;
		  DeclNode * declNode = $2;
		  $$->push_back(declNode);
		  }
		| /* epsilon */
		  {
		  $$ = new std::list<DeclNode * >();
		  }

varDecl 	: type id
		  {
		  const Position * p = new Position($1->pos(), $2->pos());
		  $$ = new VarDeclNode(p, $1, $2);
		  }

type		: primType
		  { $$ = $1;}
		| FN fnType
		  { $$ = $2; }
primType 	: INT
	  	  { $$ = new IntTypeNode($1->pos()); }
		| BOOL
		  { $$ = new BoolTypeNode($1->pos());}
		| VOID
		  { $$ = new VoidTypeNode($1->pos());}

fnType		: LPAREN typeList RPAREN ARROW type
		  { 
			const Position * p = new Position($1->pos(), $5->pos());
			$$ = new FnTypeNode(p, $2, $5);
		  }
		| LPAREN RPAREN ARROW type
		  { 
			const Position * p = new Position($1->pos(), $4->pos());
			$$ = new FnTypeNode(p, nullptr, $4);
		  }

typeList	: type
		  { 
			$$ = new std::list<TypeNode* >();
		  	$$->push_back($1);
		  }
		| typeList COMMA type
		  { 
			$$ = $1;
		  	$$->push_back($3);
		  }

fnDecl 		: type id LPAREN RPAREN LCURLY stmtList RCURLY
		  { 
			const Position * p = new Position($1->pos(), $7->pos());

			$$ = new FnDeclNode(p, $1, $2, nullptr, $6); 
			
			}
		| type id LPAREN formals RPAREN LCURLY stmtList RCURLY
		  { 
			const Position * p = new Position($1->pos(), $8->pos());

			$$ = new FnDeclNode(p, $1, $2, $4, $7); 
		  }

formals 	: formalDecl
		  { $$ = new std::list<FormalDeclNode *>();
		  	$$->push_back($1); 
			}
		| formals COMMA formalDecl
		  { 
			$$ = $1;
		  	$$->push_back($3);
		  }

formalDecl 	: type id
		  { 
			const Position * p = new Position($1->pos(), $2->pos());
			$$ = new FormalDeclNode(p, $1, $2);
		  }

stmtList 	: /* epsilon */
	   	  {$$ = new std::list<StmtNode * >(); }
		| stmtList stmt SEMICOL
	  	  {
			$$ = $1; 
		  	$$->push_back($2);
		   }
		| stmtList blockStmt
		{ 
			$$ = $1; 
		  	$$->push_back($2);
		}


blockStmt	: WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
		  { 
			Position * p = new Position($1->pos(), $7->pos());
			$$ = new WhileStmtNode(p, $3, $6);
		  }
		| FOR LPAREN stmt SEMICOL exp SEMICOL stmt RPAREN LCURLY stmtList RCURLY
		  { 
			Position * p = new Position($1->pos(), $11->pos());
			$$ = new ForStmtNode(p, $5, $3, $7, $10);
		  }
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY
		  { 
			Position * p = new Position($1->pos(), $7->pos());
			$$ = new IfStmtNode(p, $3, $6);
		  }
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
		  { 
			Position * p = new Position($1->pos(), $11->pos());
			$$ = new IfElseStmtNode(p, $3, $6, $10);
		  }

stmt		: varDecl
		  {$$ = $1; }
		| assignExp
		  {
			const Position * pos = $1->pos();
		  	$$ = new AssignStmtNode(pos, $1);
		  }
		| id POSTDEC
		  {
			const Position * pos = $1->pos();
			$$ = new PostDecStmtNode(pos, $1);
		   }
		| id POSTINC
		  { 
			const Position * pos = $1->pos();
			$$ = new PostIncStmtNode(pos, $1);
			}
		| INPUT id
		  { 
			const Position * p = new Position($1->pos(), $2->pos());
			$$ = new InputStmtNode(p, $2);
		  }
		| OUTPUT exp
		  { 
			const Position * p = new Position($1->pos(), $2->pos());
			$$ = new OutputStmtNode(p, $2);
		  }
		| RETURN exp
		  { 
			const Position * p = new Position($1->pos(), $2->pos());
			$$ = new ReturnStmtNode(p, $2);
		  }
		| RETURN
		  { 
			const Position * p = $1->pos();
			$$ = new ReturnStmtNode(p, nullptr);
		  }
		| callExp
		  {$$ = new CallStmtNode($1->pos(), $1);}

exp		: assignExp 
		  { $$ = $1;}
		| exp MINUS exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new MinusNode(p, $1, $3);
			}
		| exp PLUS exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new PlusNode(p, $1, $3);
		  }
		| exp TIMES exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new TimesNode(p, $1, $3);
		  }
		| exp DIVIDE exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new DivideNode(p, $1, $3);
		  }
		| exp AND exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new AndNode(p, $1, $3);
		  }
		| exp OR exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new OrNode(p, $1, $3);
		  }
		| exp EQUALS exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new EqualsNode(p, $1, $3);
		  }
		| exp NOTEQUALS exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new NotEqualsNode(p, $1, $3);
		  }
		| exp GREATER exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new GreaterNode(p, $1, $3);
			}
		| exp GREATEREQ exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new GreaterEqNode(p, $1, $3);
		  }
		| exp LESS exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new LessNode(p, $1, $3);
		  }
		| exp LESSEQ exp
	  	  { 
			const Position * p = new Position($1->pos(), $3->pos());
			$$ = new LessEqNode(p, $1, $3);
		  }
		| NOT exp
	  	  { 
			const Position * p = new Position($1->pos(), $2->pos());
			$$ = new NotNode(p, $2);
		  }
		| MINUS term
	  	  { 
			const Position * p = new Position($1->pos(), $2->pos());
			$$ = new NegNode(p, $2);
		  }
		| term
	  	  { $$ = $1; }

assignExp	: id ASSIGN exp
		  { 
			const Position * p = new Position($1->pos(), $3->pos());

			$$ = new AssignExpNode(p, $1, $3); 
		  }

callExp		: id LPAREN RPAREN
		  {
				const Position * p = new Position($1->pos(), $3->pos());
				$$ = new CallExpNode(p, $1, nullptr);
			}
		| id LPAREN actualsList RPAREN
		  { 
			const Position * p = new Position($1->pos(), $4->pos());
			$$ = new CallExpNode(p, $1, $3);
			}

actualsList	: exp
		  { 
			$$ = new std::list<ExpNode *>();
			$$->push_back($1);
		  }
		| actualsList COMMA exp
		  { 
			$$ = $1;
			$$->push_back($3);
		  }

term 		: id
		  {$$ = $1; }
		| INTLITERAL 
		  {
			
			$$ = new IntLiteralNode($1->pos(), $1->num());

			}
		| STRINGLITERAL 
		  { 
			$$ = new StringLiteralNode($1->pos(), $1->str());

		  }
		| TRUE
		  { $$ = new TrueNode($1->pos());}
		| FALSE
		  { $$ = new FalseNode($1->pos());}
		| LPAREN exp RPAREN
		  { $$ = $2;}
		| MAYHEM
		  { $$ = new MayhemNode($1->pos());}
		| callExp
		  { $$ = $1;}

id		: ID
		  { 
		  const Position * pos = $1->pos();
		  $$ = new IDNode(pos, $1->value());
		  }
%%

void drewgon::Parser::error(const std::string& msg){
	std::cout << msg << std::endl;
	std::cerr << "syntax error" << std::endl;
}
