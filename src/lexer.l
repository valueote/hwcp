%option noyywrap
%{
    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    #include "parser.h"
    #include <ostream>
    #include <fstream>
    #include <iostream>
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;
    extern bool dump_tokens;
    int chars = 0;
    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
    #include <iostream>
%}
DECIMIAL ([1-9][0-9]*|0)
OCTAL (0[0-7]+)
HEXADECIMAL (0[xX][0-9A-Fa-f]+)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
BLOCKCOMMENTBEGIN \/\*
BLOCKCOMMENTELEMENT .
BLOCKCOMMENTEND \*\/
%x BLOCKCOMMENT
LINECOMMENT \/\/[^\n]*


%%
"int" {
    if(dump_tokens)
    printf("(INT,\'%s\')\n",yytext);
    return INT;
}

"void" {
    if(dump_tokens)
        printf("(VOID,\'%s\')\n", yytext);
    return VOID;
}
"const" {
    if(dump_tokens)
        printf("(CONST,\'%s\')\n", yytext);
    return CONST;
}
"if" {
    if(dump_tokens)
        printf("(IF,\'%s\')\n", yytext);
    return IF;
}
"else" {
    if(dump_tokens)
        printf("(ELSE,\'%s\')\n", yytext);
    return ELSE;
}
"return" {
    if(dump_tokens)
        printf("(RETURN,\'%s\')\n", yytext);
    return RETURN;
}
"while" {
    if(dump_tokens)
        printf("(WHILE,\'%s\')\n", yytext);
    return WHILE;
}
"for" {
    if(dump_tokens)
        printf("(FOR,\'%s\')\n", yytext);
    return FOR;
}
"break" {
    if(dump_tokens)
        printf("(BREAK,\'%s\')\n", yytext);
    return BREAK;
}
"continue" {
    if(dump_tokens)
        printf("(CONTINUE,\'%s\')\n", yytext);
    return CONTINUE;
}
"==" {
    if(dump_tokens)
        printf("(EQUAL,\'%s\')\n", yytext);
    return EQUAL;
}
"!=" {
    if(dump_tokens)
        printf("(NOTEQUAL,\'%s\')\n", yytext);
    return NOTEQUAL;
}
"=" {
    if(dump_tokens)
        printf("(ASSIGN,\'%s\')\n", yytext);
    return ASSIGN;
}
"<" {
    if(dump_tokens)
        printf("(LESS,\'%s\')\n", yytext);
    return LESS;
}
"<=" {
    if(dump_tokens)
        printf("(LESSEQUAL,\'%s\')\n", yytext);
    return LESSEQUAL;
}
">" {
    if(dump_tokens)
        printf("(GREATER,\'%s\')\n", yytext);
    return GREATER;
}
">=" {
    if(dump_tokens)
        printf("(GREATEREQUAL,\'%s\')\n", yytext);
    return GREATEREQUAL;
}
"+" {
    if(dump_tokens)
        printf("(ADD,\'%s\')\n", yytext);
    return ADD;
}
"-" {
    if(dump_tokens)
        printf("(SUB,\'%s\')\n", yytext);
    return SUB;
}
"*" {
    if(dump_tokens)
        printf("(MUL,\'%s\')\n", yytext);
    return MUL;
}
"/" {
    if(dump_tokens)
        printf("(DIV,\'%s\')\n", yytext);
    return DIV;
}
"%" {
    if(dump_tokens)
        printf("(MOD,\'%s\')\n", yytext);
    return MOD;
}
"++" {
    if(dump_tokens)
        printf("(INCREMENT,\'%s\')\n", yytext);
    return INCREMENT;
}
"--" {
    if(dump_tokens)
        printf("(DECREMENT,\'%s\')\n", yytext);
    return DECREMENT;
}
"&&" {
    if(dump_tokens)
        printf("(AND,\'%s\')\n", yytext);
    return AND;
}
"||" {
    if(dump_tokens)
        printf("(OR,\'%s\')\n", yytext);
    return OR;
}
"!" {
    if(dump_tokens)
        printf("(NOT,\'%s\')\n", yytext);
    return NOT;
}
";" {
    if(dump_tokens)
        printf("(SEMICOLON,\'%s\')\n", yytext);
    return SEMICOLON;
}
"(" {
    if(dump_tokens)
        printf("(LPAREN,\'%s\')\n", yytext);
    return LPAREN;
}
")" {
    if(dump_tokens)
        printf("(RPAREN,\'%s\')\n", yytext);
    return RPAREN;
}
"{" {
    if(dump_tokens)
        printf("(LBRACE,\'%s\')\n", yytext);
    return LBRACE;
}
"}" {
    if(dump_tokens)
        printf("(RBRACE,\'%s\')\n", yytext);
    return RBRACE;
}
"[" {
    if(dump_tokens)
        printf("(LBRACKET,\'%s\')\n", yytext);
    return LBRACKET;
}
"]" {
    if(dump_tokens)
        printf("(RBRACKET,\'%s\')\n", yytext);
    return RBRACKET;
}
"," {
    if(dump_tokens)
        printf("(COMMA,\'%s\')\n", yytext);
    return COMMA;
}
{DECIMIAL} {
    if(dump_tokens)
       printf("(DECIMIAL,\'%s\')\n",yytext);
    chars += strlen(yytext);
    yylval.itype = atoi(yytext);
    return INTEGER;
}
{OCTAL} {
    int temp;
    sscanf(yytext, "%o", &temp);
    if(dump_tokens)
        DEBUG_FOR_LAB4(string("NUMBER\t") + to_string(temp));
    yylval.itype = temp;
    return INTEGER;
}
{HEXADECIMAL} {
    int temp;
    sscanf(yytext, "%x", &temp);
    if(dump_tokens)
        DEBUG_FOR_LAB4(string("NUMBER\t") + to_string(temp));
    yylval.itype = temp;
    return INTEGER;
}
"putint" {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    vec.push_back(TypeSystem::intType);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec,vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    st->install(yytext, se);
    return ID;
}

"getint" {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    Type* funcType = new FunctionType(TypeSystem::intType, vec,vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    st->install(yytext, se);
    return ID;
}
"putch" {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    vec.push_back(TypeSystem::intType);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec,vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    st->install(yytext, se);
    return ID;
}
"getch" {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    std::vector<SymbolEntry*> vec1;
    Type* funcType = new FunctionType(TypeSystem::intType, vec,vec1);
    SymbolTable* st = identifiers;
    while(st->getPrev())
        st = st->getPrev();
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext, st->getLevel(), -1, true);
    st->install(yytext, se);
    return ID;
}


\"(\\.|[^\\"])*\" {
    if(dump_tokens)
        DEBUG_FOR_LAB4(string("STRING\t") + yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return STRING;
}



{ID} {
    if(dump_tokens)

    printf("(id,\'%s\')\n",yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return ID;
}
<*>{EOL} {
if(dump_tokens)
printf("Line: %d over!\n\n",yylineno);
    chars += strlen(yytext);
    yylineno++;
    chars = 0;
}
{WHITE}
{LINECOMMENT} {}
{BLOCKCOMMENTBEGIN} {BEGIN BLOCKCOMMENT;}
<BLOCKCOMMENT>{BLOCKCOMMENTELEMENT} {}
<BLOCKCOMMENT>{BLOCKCOMMENTEND} {BEGIN INITIAL;}


%%
