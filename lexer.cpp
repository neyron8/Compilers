#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <regex>
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

using namespace std;

int checkEOL(string sub)
{
    for (int i = 0; i <= sub.length(); i++)
    {
        if (sub[i] == '\0' || sub[i] == '\n' || sub[i] == ' ')
            return 1;
    }
    return 0;
}

int checkContLetSign8sys(string sub)
{
    for (int i = 0; i < sub.length(); i++)
        if (!isdigit(sub[i]) || sub[i] == '9')
        {
            if (sub[i] == '\0' || sub[i] == '\n')
                return 1;
            else
                return -1;
        }
    return 0;
}

int checkContLetSigns(string sub)
{
    int i;
    for (i = 0; i < sub.length(); i++)
    {
        if (!isdigit(sub[i]))
            return -1;
    }
    return 0;
}

int check16sys(string sub)
{
    for (int i = 2; i < sub.length(); i++)
        if ((sub[i] > 'F' && sub[i] <= 'Z') || (sub[i] >= 'a' && sub[i] <= 'z'))
        {
            if (sub[i] == '\0' || sub[i] == '\n')
                return 1;
            else
                return -1;
        }
    return 0;
}

int checkDelimiter(char sym)
{
    if (sym == '{' || sym == '}' || sym == '+' || sym == ' ' || sym == '-' || sym == '*' ||
        sym == '/' || sym == '[' || sym == ']' || sym == '(' || sym == ')' || sym == '!' ||
        sym == '=' || sym == '<' || sym == '>' || sym == ';' || sym == '\"' || sym == '|' ||
        sym == '.' || sym == ',')
        return 0;
    return 1;
}

class Token
{

public:
    string token_class;
    string lexem;
    int line, col;

    void setter(string tok, string lex)
    {
        this->token_class = tok;
        this->lexem = lex;
    }
    void position(int linenum, int colnum)
    {
        this->line = linenum;
        this->col = colnum;
    }
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
    int line = 0, col = 0;
    int endline = 0;

    int IsId()
    {
        regex regular("([\\w]*)");
        if (regex_match(sub, regular))
        {
            token.setter("'id'", sub);
            return 0;
        }
        else
        {
            return -1;
        }
    }

    int IsWord()
    {
        if (sub == "public")
        {
            token.setter("'mod_access'", "public");
            return 0;
        }
        else if (sub == "main")
        {
            token.setter("'access_point'", "main");
            return 1;
        }
        else if (sub == "void")
        {
            token.setter("'void'", "void");
            return 2;
        }
        else if (sub == "static")
        {
            token.setter("'modificator'", "static");
            return 3;
        }
        else if (sub == "String")
        {
            token.setter("'string'", "string");
            return 4;
        }
        else if (sub == "int")
        {
            token.setter("'int'", "int");
            return 5;
        }
        else if (sub == "System")
        {
            token.setter("'System class'", "System");
            return 6;
        }
        else if (sub == "new")
        {
            token.setter("'new'", "new");
            return 7;
        }
        else if (sub == "if")
        {
            token.setter("'if'", "if");
            return 9;
        }
        else if (sub == "else")
        {
            token.setter("'else'", "else");
            return 10;
        }
        else if (sub == "while")
        {
            token.setter("'cycle_while'", "while");
            return 11;
        }
        else if (sub == "class")
        {
            token.setter("'class'", "class");
            return 12;
        }
        else if (sub == "charAt")
        {
            token.setter("'method_to_char'", "charAt");
            return 13;
        }
        else if (sub == "out")
        {
            token.setter("'out'", "out");
            return 14;
        }
        else if (sub == "println")
        {
            token.setter("'print method'", "println");
            return 15;
        }
        else
            return -1;
    }

    int IsSign()
    {
        if (sub == "{")
        {
            token.setter("'{'", "{");
            return 0;
        }
        else if (sub == "}")
        {
            token.setter("'}'", "}");
            return 1;
        }
        else if (sub == "+")
        {
            token.setter("'+'", "+");
            return 2;
        }
        else if (sub == "-")
        {
            token.setter("'-'", "-");
            return 3;
        }
        else if (sub == ".")
        {
            token.setter("'.'", ".");
            return 4;
        }
        else if (sub == "*")
        {
            token.setter("'*'", "*");
            return 5;
        }
        else if (sub == "/")
        {
            token.setter("'/'", "/");
            return 6;
        }
        else if (sub == "(")
        {
            token.setter("'('", "(");
            return 7;
        }
        else if (sub == ")")
        {
            token.setter("')'", ")");
            return 8;
        }
        else if (sub == ";")
        {
            token.setter("';'", ";");
            return 9;
        }
        else if (sub == ">")
        {
            token.setter("'>'", ">");
            return 10;
        }
        else if (sub == "<")
        {
            token.setter("'<'", "<");
            return 11;
        }
        else if (sub == "=")
        {
            token.setter("'='", "=");
            return 12;
        }
        else if (sub == "!=")
        {
            token.setter("'!='", "!=");
            return 13;
        }
        else if (sub == "[")
        {
            token.setter("'['", "[");
            return 14;
        }
        else if (sub == "]")
        {
            token.setter("']'", "]");
            return 15;
        }
        else if (sub == "\"")
        {
            token.setter("\"", "\"");
            return 16;
        }
        else if (sub == "!")
        {
            token.setter("'!'", "!");
            return 17;
        }
        else if (sub == "&&")
        {
            token.setter("'&&'", "&&");
            return 18;
        }
        else if (sub == "||")
        {
            token.setter("'||'", "||");
            return 19;
        }
        else if (sub == "|")
        {
            token.setter("'|'", "|");
            return 20;
        }
        else if (sub == "==")
        {
            token.setter("'=='", "==");
            return 21;
        }
        else if (sub == "++")
        {
            token.setter("'++'", "++");
            return 22;
        }
        else if (sub == "--")
        {
            token.setter("'--'", "--");
            return 23;
        }
        else if (sub == "[]")
        {
            token.setter("'[]'", "[]");
            return 24;
        }
        else if (sub == ",")
        {
            token.setter("','", ",");
            return 25;
        }

        else
            return -1;
    }
    int IsNumber()
    {
        string subs;
        subs = sub;
        if (subs == "0")
        {
            token.setter("'decimal_number'", "0");
            return 0;
        } //10
        else if (subs == "0x0")
        {
            token.setter("'hex_number'", "0x0");
            return 1;
        } //16
        if (subs == "00")
        {
            token.setter("'octal_number'", "00");
            return 2;
        } //8
        else
        {
            if (subs[0] != '0' && checkContLetSigns(subs) == 0 && stoi(subs, 0, 10) != 0)
            {
                token.setter("'decimal_number'", sub);
                return 0;
            }
            else
            {
                string s = sub.substr(1, sub.length());
                if (subs[0] == '0' && checkContLetSign8sys(s) == 0 && stoi(sub, 0, 8) != 0)
                {
                    token.setter("'octal_number'", sub);
                    return 2;
                }
                else
                {
                    if (subs[0] == '0' && subs[1] == 'x' && check16sys(subs) == 0 && stoi(subs, 0, 16) != 0)
                    {
                        token.setter("'hex_number'", sub);
                        return 1;
                    }
                    else
                        return -1;
                }
            }
        }
    }

    int getWord()
    {
        int current_pos = col;
        int not_space = 1;
        while (not_space)
        {
            if (file_string[current_pos] == '\n')
            {
                file_string.clear();
                col = 0;
                line++;
                endline = 0;
                return -1;
            }
            if (file_string[current_pos] == ' ' || file_string[current_pos] == '\t')
                current_pos++;
            else
            {
                not_space = 0;
            }
        }
        ascii_code_sym = file_string[current_pos];
        if (file_string[current_pos] == '\0' || endline == 1 || ascii_code_sym == 13)
        { //|| file_string[i + 1] == '\0' || ( (i + 1) == file_string.length())
            file_string.clear();
            col = 0;
            line++;
            endline = 0;
            return -1; //end of line
        }

        if (checkDelimiter(file_string[current_pos]) == 0) //symbol
        {
            if (file_string[current_pos] == '\"') // beginning of string
            {
                start = current_pos;
                current_pos++;
                for (current_pos; file_string[current_pos] != '\"' && file_string[current_pos] != '\0'; current_pos++)
                    ;
                if (file_string[current_pos] == '\"') // end of string -> string
                {
                    end = current_pos;
                    col = end + 1;
                    sub = file_string.substr(start, current_pos + 1 - start);
                    token.setter("'String'", sub);
                    token.position(line, start);
                    return 1;
                }
                else
                {
                    sub = file_string.substr(start, current_pos - start - 1);
                    token.setter("'Unknown token'", sub);
                    token.position(line, start);
                    endline = 1;
                    return 1;
                }
            }
            if (file_string[current_pos] == '/' && file_string[current_pos + 1] == '/') // beginning of string
            {
                file_string.clear();
                col = 0;
                line++;
                endline = 0;
                return -1; //end of line
            }
            if (file_string[current_pos + 1] != '\0' && checkDelimiter(file_string[current_pos + 1]) == 0) //pair delimiter
            {
                sub = file_string.substr(current_pos, 2);
                if (IsSign() >= 0) // pair sign
                {
                    start = current_pos;
                    end = current_pos + 1;
                    token.position(line, start);
                    if (file_string[end] == '\0')
                        endline = 1;
                    else
                        col = end + 1;
                    return 1;
                }
                else
                {
                    start = current_pos;
                    end = current_pos;
                    token.position(line, start);
                    sub = file_string.substr(current_pos, 1);
                    IsSign();
                    if (file_string[end] == '\0')
                    {
                        endline = 1;
                        return 1;
                    }
                    else
                    {
                        col = end + 1;
                    }
                    return 1;
                }
            }
            else
            {
                sub = file_string.substr(current_pos, 1);
                IsSign();
                start = current_pos;
                end = current_pos;
                token.position(line, start);
                //changes here ~!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~~
                if ((end + 2) == file_string.length())
                {
                    endline = 1;
                    return 1;
                }
                else
                {
                    col = end + 1;
                }
                return 1;
            }
        }
        else
        {
            start = current_pos;
            while ((checkDelimiter(file_string[current_pos]) == 1) && file_string[current_pos] != '\0')
            {
                current_pos++;
            }
            sub = file_string.substr(start, current_pos - start);
            if (IsWord() >= 0 || IsNumber() >= 0 || IsSign() >= 0 || IsId() >= 0)
            {
                end = current_pos;
                token.position(line, start);
                if (file_string[end] == '\0')
                {
                    endline = 1;
                    return 1;
                }
                else
                    col = end;
                return 1;
            }
            else
            {
                sub = file_string.substr(start, current_pos - start - 1);
                if (IsWord() >= 0)
                {
                    end = current_pos;
                    token.position(line, start);
                    if (file_string[end] == '\0')
                    {
                        endline = 1;
                        return 1;
                    }
                    else
                        col = end;

                    return 1;
                }
                if (IsNumber() >= 0)
                {
                    end = current_pos;
                    token.position(line, start);
                    if (file_string[end] == '\0')
                    {
                        endline = 1;
                        return 1;
                    }
                    else
                        col = end;
                    return 1;
                }
                if (IsId() >= 0)
                {
                    end = current_pos;
                    token.position(line, start);
                    if (file_string[end] == '\0')
                    {
                        endline = 1;
                        return 1;
                    }
                    else
                        col = end;

                    return 1;
                }
                token.setter("unknown token", sub);
                end = current_pos;
                token.position(line, start);
                if (file_string[end] == '\0')
                {
                    endline = 1;
                    return 1;
                }
                else
                    col = end;
                return 1;
            }
        }
        return 1;
    }

    int getTokens()
    {
        if (file.is_open())
        {
            getline(file, file_string);
            while (1)
            {
                if (file.eof())
                {
                    exit(0);
                }
                if (file_string.empty())
                {
                    getline(file, file_string);
                }
                else
                {
                    if (getWord() != -1)
                        cout << "Line: " << token.line << ", "
                             << "Col: " << token.col << ", "
                             << "Lexem: < " << token.lexem << " > "
                             << "Token Class: " << token.token_class << endl;
                }
            }
        }
        else
        {
            cout << "Error: file cannot be open" << endl;
            return -3;
        }
    }
};

/*int main(int argc, char *argv[])
{
    string s;
    Lexer lex;

    lex.file.open(argv[1]);
    lex.getTokens();
    lex.file.close();

}*/

TEST_CASE("Gonna meet <public>", "First")
{
    Lexer lex;
	lex.sub = "public";
	REQUIRE(lex.IsWord() == 0);
    
}

TEST_CASE("Gonna meet not a word", "Second")
{
    Lexer lex;
	lex.sub = "123";
	REQUIRE(lex.IsWord() == -1);
    
}

TEST_CASE("Gonna meet decimal number", "Third")
{
    Lexer lex;
	lex.sub = "123";
	REQUIRE(lex.IsNumber() == 0);
    
}

TEST_CASE("Gonna meet sign <{>", "Fourth")
{
    Lexer lex;
	lex.sub = "{";
	REQUIRE(lex.IsSign() == 0);
    
}

TEST_CASE("Gonna meet sign <{> but there is decimal number", "Fifth")
{
    Lexer lex;
	lex.sub = "45";
	REQUIRE(lex.IsSign() == 0);
    
}


