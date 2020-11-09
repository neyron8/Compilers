#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <regex>
#include<vector>

using namespace std;

int checkEOL(string sub);
int checkContLetSign8sys(string sub);
int checkContLetSigns(string sub);
int check16sys(string sub);
int checkDelimiter(char sym);

class Token
{

public:
    string token_class;
    string lexem;
    int line, col;
	Token(string& token_cl, string& token_lex);
	Token(string& token_cl, string& token_lex, int l, int c);
	Token();
    void setter(string tok, string lex);
    void position(int linenum, int colnum);

};

class Lexer
{
public:
    ifstream file;
    int length;
    int ascii_code_sym;
    int start, end;
    string file_string, sub;
    Token token;
	vector<Token> tokens;
    int line = 0, col = 0;
    int endline = 0;

    int IsId();
    int IsWord();
    int IsSign();
    int IsNumber();
    int getWord();
    int getTokens();
    Token getToken();
};
