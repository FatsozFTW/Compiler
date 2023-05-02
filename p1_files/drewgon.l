%{
#include <string>
#include <limits.h>

/* Get our custom yyFlexScanner subclass */
#include "lexer.hpp"
#include "tokens.hpp"
#undef  YY_DECL
#define YY_DECL int drewgon::Scanner::yylex( drewgon::Parser::semantic_type * const lval )

/* define yyterminate as this instead of NULL */
#define yyterminate() return( TokenKind::END )

/* Exclude unistd.h for Visual Studio compatability. */
#define YY_NO_UNISTD_H

/* typedef to make the reference to token kinds shorter */
using TokenKind = drewgon::Parser::token;

%}

%option debug
%option nodefault
%option yyclass="drewgon::Scanner"
%option noyywrap
%option c++

DIGIT [0-9]
ATOZ [a-z]
CAPAZ [A-Z]

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

"+"		{
		// Keep track of the "kind" of token we matched. In this case,
		// it's a PLUS (aka the plus symbol). The full list of token
		// kinds is declared around line 197 of grammar.hh. The testing
		// functionScanner::outputTokens also indicates what each token
		// represents (declared in scanner.cpp line 9)
		int tokenKind = TokenKind::PLUS;

		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);

		//Instatiate a token. There are 5 classes of tokens defined based on
		// - a literal string (StringLitToken)
		// - a literal integer (IntLitToken)
		// - a literal short (ShortLitToken)
		// - an identifier (IDToken)
		// - a base Token (any other type)
		// The token object is a handy place to keep information about the
		// token (i.e. token metadata) such as the line and column on which
		// the token character started (which can be used later in debug
		// messages, etc).
		Token * token = new Token(pos, tokenKind);

		//Assign the token to a instance variable of the scanner.
		// (When Flex processes a .l file in C++ mode, it creates
		// a class called Scanner, which is a subclass of yyFlexScanner)
		// By assigning a value to this field, we put the token
		// metadata in a place where scanner's client program can find it.
		this->yylval->lexeme = token;

		//Since we consumed 1 character, increase the column number by 1
		colNum++;

		//Let the scanner's client program know that a PLUS token
		// has been matched
		return tokenKind;
		}

"-" {
		//Essentially the same thing as PLUS, but without the verbose
		// comments so it doesn't look so scary.
		int tokenKind = TokenKind::MINUS;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		return tokenKind;
		}

"*" {
		int tokenKind = TokenKind::TIMES;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		return tokenKind;
		}

"/" {
		int tokenKind = TokenKind::DIVIDE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		return tokenKind;
		}

[/][/].* {
		colNum+= strlen(yytext);

		}


"<" {
		int tokenKind = TokenKind::LESS;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		return tokenKind;
		}
">" {
		int tokenKind = TokenKind::GREATER;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		return tokenKind;
		}
"<=" {
		int tokenKind = TokenKind::LESSEQ;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		colNum++;
		return tokenKind;
		}
">=" {
		int tokenKind = TokenKind::GREATEREQ;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		colNum++;
		return tokenKind;
		}
"and" {
		int tokenKind = TokenKind::AND;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+3;

		return tokenKind;
		}
"->" {
		int tokenKind = TokenKind::ARROW;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+2;

		return tokenKind;
		}
"=" {
		int tokenKind = TokenKind::ASSIGN;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;

		return tokenKind;
		}
" " {
		colNum++;
		}

"bool" {
		int tokenKind = TokenKind::BOOL;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+4;

		return tokenKind;
		}
"," {
		int tokenKind = TokenKind::COMMA;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;

		return tokenKind;
		}
"else" {
		int tokenKind = TokenKind::ELSE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+4;

		return tokenKind;
		}
"==" {
		int tokenKind = TokenKind::EQUALS;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+2;

		return tokenKind;
		}
"false" {
		int tokenKind = TokenKind::FALSE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+5;

		return tokenKind;
		}
"fn" {
		int tokenKind = TokenKind::FN;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+2;

		return tokenKind;
		}
"for" {
		int tokenKind = TokenKind::FOR;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+3;

		return tokenKind;
		}
"if" {
		int tokenKind = TokenKind::IF;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+2;

		return tokenKind;
		}
"input" {
		int tokenKind = TokenKind::INPUT;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+5;

		return tokenKind;
		}
"int" {
		int tokenKind = TokenKind::INT;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+3;

		return tokenKind;
		}
"{" {
		int tokenKind = TokenKind::LCURLY;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;

		return tokenKind;
		}
"}" {
		int tokenKind = TokenKind::RCURLY;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;

		return tokenKind;
		}
"(" {
		int tokenKind = TokenKind::LPAREN;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;

		return tokenKind;
		}
")" {
		int tokenKind = TokenKind::RPAREN;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;

		return tokenKind;
		}
"mayhem" {
		int tokenKind = TokenKind::MAYHEM;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum = colNum+6;

		return tokenKind;
		}
"!" {
		int tokenKind = TokenKind::NOT;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;

		return tokenKind;
		}
"!=" {
		int tokenKind = TokenKind::NOTEQUALS;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		colNum++;
		return tokenKind;
		}
"or" {
		int tokenKind = TokenKind::OR;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		colNum++;
		return tokenKind;
		}
"output" {
		int tokenKind = TokenKind::OUTPUT;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum=colNum+6;

		return tokenKind;
		}
"return" {
		int tokenKind = TokenKind::RETURN;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum=colNum+6;

		return tokenKind;
		}
";" {
		int tokenKind = TokenKind::SEMICOL;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;

		return tokenKind;
		}
"true" {
		int tokenKind = TokenKind::TRUE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum=colNum+4;

		return tokenKind;
		}
"void" {
		int tokenKind = TokenKind::VOID;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum=colNum+4;

		return tokenKind;
		}
"while" {
		int tokenKind = TokenKind::WHILE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum=colNum+5;

		return tokenKind;
		}

"--" {
		int tokenKind = TokenKind::POSTDEC;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		colNum++;
		return tokenKind;
		}
"++" {
		int tokenKind = TokenKind::POSTINC;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->lexeme = new Token(pos, tokenKind);
		colNum++;
		colNum++;
		return tokenKind;
		}

({ATOZ}+|{CAPAZ}+)(({ATOZ}+|{CAPAZ}+)*({DIGIT}+)*)*	{
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->lexeme = new IDToken(pos, yytext);

		colNum +=strlen(yytext);

		return TokenKind::ID;

	}



["].*[\\][^ nt\\"](.*[\\][^ nt\\"].*)*["] {
    Position * pos = new Position(
    lineNum, colNum, lineNum, colNum+strlen(yytext));
    errStrEsc(pos);
    }


["](\\(.|\n)|[^\\"\n])*["]	{
  		Position * pos = new Position(
  			lineNum, colNum, lineNum, colNum+strlen(yytext));
  		this->yylval->lexeme = new StrToken(pos, yytext);

  		colNum +=strlen(yytext);

  		return TokenKind::STRINGLITERAL;

  	}

["].*[\\].* {
    Position * pos = new Position(
    lineNum, colNum, lineNum, colNum+strlen(yytext));
    errStrEscAndUnterm(pos);
    }


["].* {
    Position * pos = new Position(
    lineNum, colNum, lineNum, colNum+strlen(yytext));
    errStrUnterm(pos);
    }

{DIGIT}+	{
		//TODO: The following computation of the integer
		// value does not check for overflow. It should do so.
		int intVal = atoi(yytext);

		//Note that the IntLitToken does not take a TokenKind argument
		// since it's implicitly known that the token represents an
		// INTLITERAL
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		if(intVal<0)
		{
			errIntOverflow(pos);
			intVal = 0;
		}
		yylval->lexeme = new IntLitToken(pos, intVal);

		//TODO: update the colNum count according to the number of
		// characters consumed.
		colNum +=strlen(yytext);
		//As before, alert the client program that an INTLITERAL
		// token was matched

		return TokenKind::INTLITERAL;
		}

\n		{
		//Update the line number, reset the columnNumber
		lineNum++;
		colNum = 1;
		}


.   {
		//TODO: Add the rest of the rules above this point so that legal characters
		// are properly accounted for.
		std::string msg = "Illegal character ";
		msg += yytext;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		Report::fatal(pos,msg);
		colNum += strlen(yytext);
    }
%%
