#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <regex>
#include "header.h"
#include <vector>

using namespace std;

void write_ast(int spaces, string type, string value ){
    ofstream fout;
    fout.open("Ast.txt", ios::app);
    int k = 0;
    string pref = "**";
    while(k < spaces){
        pref+="**";
        k++;
    }
    fout << pref <<" " << type <<" " << value << endl;
    fout.close();
}

class Node
{
    public:
    string type;
    string value;
    int line;
    int col;
    vector<Node> params;

	Node(string& type, string& value, int l, int c){
        this->type = type;
        this->value = value;
        this->line = l;
        this->col = c;
    }

	Node(){
        this->type = "Empty";
        this->value = "Empty";
    }
};

class Table
{
    public:
    string visibility;
    int nestlvl;
    string value;
    string type;

	Table(string& vi, int nest, string& v, string& t ){
        this->type = t;
        this->value = v;
        this->nestlvl = nest;
        this->visibility = vi;
    }

	Table(){
        this->type = "Empty";
        this->value = "Empty";
        this->nestlvl = 0;
        this->visibility = "Empty";
    }
};


class Parser
{
    public:
    int nestlvl = 0;
    int current = 0;
    Lexer lex;
	Lexer for_tok;
    vector<Token> var_table;
    vector<string> visibility;
    vector<Table> table;
    Node ast;
    Node sem_tree;

	int nest()
    {
        for_tok.token = lex.getToken();
        if(for_tok.token.token_class == "Unknown token"){
            cout << "ERROR: Unknown token |line: " << for_tok.token.line << " |col: " << for_tok.token.col  << endl;
            cout << for_tok.token.lexem << endl;
            //for_tok.token = lex.getToken();
            //cout << for_tok.token.lexem << endl;
            exit(-1);
        }
        if(for_tok.token.lexem != "Empty")
            for_tok.tokens.push_back(Token(for_tok.token.token_class ,for_tok.token.lexem, for_tok.token.line , for_tok.token.col));
		return 0;
    }
    int create_var_tables(){
        int i = 0;
        string arr_char = "'Array'";
        for(i ; i < for_tok.tokens.size(); i++){
            if(for_tok.tokens[i].token_class == "'int'" || for_tok.tokens[i].token_class == "'string'" || for_tok.tokens[i].token_class == "'char'" ){
                if(((i + 2) < for_tok.tokens.size()) && for_tok.tokens[i + 1].token_class == "'id'"){
                    if(for_tok.tokens[i + 2].token_class == "';'"){// not array
                        var_table.push_back(Token(for_tok.tokens[i].token_class ,for_tok.tokens[i+1].lexem));
                    } else {
                        cout << "ERROR! There is no ; after init |line: " << for_tok.tokens[i].line<< endl;
                        exit (-1);
                    }
                    //cout << for_tok.tokens[i].token_class << " " << for_tok.tokens[i + 1].lexem << endl;
                }
                if(((i + 2) < for_tok.tokens.size()) && for_tok.tokens[i + 1].lexem == "'[]'" &&
                    for_tok.tokens[i + 2].token_class == "'id'" && for_tok.tokens[i].token_class== "'int'" ){ //array
                        //Maybe here
                        var_table.push_back(Token(arr_char,for_tok.tokens[i+2].lexem));
                }
                if(((i + 2) < for_tok.tokens.size()) && for_tok.tokens[i + 1].token_class== "'[]'" &&
                    for_tok.tokens[i].token_class== "'int'" ){
                    string arr = "int[]";//array
                    var_table.push_back(Token(arr,for_tok.tokens[i+2].lexem));
                }
            }
        }
        return 0;
    }
    int parse(){
        while(!lex.file.eof())
	      nest();
          /*for(int i = 0 ; i < for_tok.tokens.size(); i++){
              cout << for_tok.tokens[i].token_class << " " << for_tok.tokens[i].lexem  << endl;
          }*/
        create_var_tables();
        ast.type = "Main";
        visibility.push_back("Program");
        //cout <<"SIZE " << for_tok.tokens.size()<< endl;
        while (current < for_tok.tokens.size()) { //current < for_tok.tokens.size()
            Parser pushing;
            pushing.ast = step();
            //cout << "TYPE IS" <<pushing.ast.type << endl;
            ast.params.push_back(pushing.ast);
        }
        return 0;
    }
    int ast_to_sem(){
        int i = 0;
        sem_tree.type = "Main";
        //cout << "SEM TREE" << endl;
        while(i < ast.params.size()){
            Parser pushing;
            //cout << "Current i " << i << endl;
            pushing.sem_tree = transf(ast.params[i]);
            if(pushing.sem_tree.type == "Empty"){
                //cout << "Empty" << endl;
                i++;
                continue;
            }
            sem_tree.params.push_back(pushing.sem_tree);
            i++;
        }
        return 0;
    }
    int checking_types(Node tree_node){
        if((tree_node.value == "=" || tree_node.value =="==") && !tree_node.params.empty()){
            int i = 0;
            while(i < table.size()){
                if(tree_node.params[0].value == table[i].value){
                    string type = table[i].type;
                    //cout << "IT IS TYPE " << type << endl;
                    if(type == "'int[]'"){
                        if(tree_node.params[1].type == "'{'"){
                            int j = 2;
                            while(j <  tree_node.params.size()){
                                if (tree_node.params[j].type == "'String'" ||
                                    tree_node.params[j].type == "'char'"){
                                        cout << "Error: There is not equal types. Expected 'int' not char or String |line: " << tree_node.params[j].line << endl;
                                        exit(-1);
                                    }
                                    j++;
                            }
                            break;
                        }
                    }
                    if(type == "'int'"){
                        if(tree_node.params[1].type == "Number"){
                                return  1; // INT (Number)
                        }
                        if(tree_node.params[1].type == "'id'"){
                                int a = 0;
                                while(a < table.size()){
                                  if(tree_node.params[1].value == table[a].value && table[a].type == "'int'" ){
                                    //cout << "Second argument is int -> EQUAL" << endl;
                                    //cout << tree_node.params[1].value << endl;
                                    return  1;
                                  }
                                  a++;
                                }
                                cout << "Error: Second argument is not int " << tree_node.params[1].value <<" line: " << tree_node.params[1].line << endl;
                                exit(-1);
                        }
                        if(tree_node.params[1].type == "Math"){
                                //cout << "IT IS MATH" << endl;
                                int j = checking_types(tree_node.params[1]);
                                if(j==1){
                                    //cout << "Int + Int equal no errors YEAH" << endl;
                                    return 1;
                                } else {
                                    cout << "Error: Not equal types in MATH " << tree_node.params[1].params[0].value << " and " <<  tree_node.params[1].params[1].value<< " |line: " <<tree_node.params[1].line << " operation: " << tree_node.params[1].value << endl;
                                    exit(-1);
                                }
                        }
                        if(tree_node.params[1].type == "Array"){
                                return 1; // INT
                        } else {
                            cout << "Error: types are not equal - " << tree_node.params[1].type << " != " << type << " line: "<< tree_node.params[1].line << endl;
                            exit(-1);
                        }
                    }
                    if(type == "'string'"){
                        if(tree_node.params[1].type == "'String'"){
                          return 2;
                        }
                    }
                    if(type == "'char'"){
                        if(tree_node.params[1].type == "String + method"){
                            //cout << "Char and string[i] are equal" << endl;
                            return 2;
                        } else {
                          cout << "Error. Not equal type with char "<< endl;
                          exit(-1);
                        }
                    } else {
                        cout << "ERROR Unknown operation with " << tree_node.params[0].value << endl;
                        exit(-1);
                    }
                    break;
                }
                i++;
            }
            return 0;
        }
        if(tree_node.type == "Math" && !tree_node.params.empty()){
            int i = 0;
            string type1, type2;
            if(tree_node.params[1].type == "'decimal_number'"){
                string type;
                while(i < table.size()){
                    if(tree_node.params[0].value == table[i].value){
                        type = table[i].type;
                        break;
                    }
                    i++;
                }
                if (type == "'int'"){
                    return 1;
                } else {
                    cout << "Error! Types are not valid for operation |line: " << tree_node.params[0].line <<" col: "<< tree_node.params[0].col <<endl;
                    exit(-1);
                }
            }
            i = 0;
            while(i < table.size()){
                if(tree_node.params[0].value == table[i].value){
                    type1 = table[i].type;
                    break;
                }
                i++;
            }
            if(tree_node.params[1].type == "Number"){
              return 1;
            }
            i = 0;
            while(i < table.size()){
                if(tree_node.params[1].value == table[i].value){
                    type2 = table[i].type;
                    break;
                }
                i++;
            }
            if(type1 == type2){
                if(type1 == "'String'"){
                    return 2; //string
                }
                if(type1 == "'int'"){
                    return 1; //int
                }
                if(type1 == "'char'"){
                    return 3;// char
                }
            } else {
                cout << "Error: Types are not equal " << type1 << " " << type2  << " |line: " << tree_node.params[1].line << endl;
                exit(-1);
            }
        }
        if((tree_node.value == ">" || tree_node.value == "<" || tree_node.value == "!=") && !tree_node.params.empty()){
          int i = 0;
          if(tree_node.params[0].type == "'decimal_number'" && tree_node.params[1].type == "Math"){
            if(checking_types(tree_node.params[1]) == 1){
              //cout << "Types are equal Number and int" << endl;
              return 1;
            }
          }
          i = 0;
          while(i < table.size()){
              if(tree_node.params[0].value == table[i].value){
                  //cout << "I FOUND " << endl;
                  string type = table[i].type;
                  if(type == "'int'"){
                    if(tree_node.params[1].type == "Number"){
                        return 1;
                    }
                    i = 0;
                    while(i < table.size()){
                    if(tree_node.params[1].value == table[i].value){
                      string type2 = table[i].type;
                      //cout << "I FOUND " << endl;
                      if (type2 == "'int[]'"){
                        return 1;
                      }
                      if (type == type2){
                        return 1;
                      }
                    }
                    i++;
                  }
                  }
              }
              i++;
          }
          cout << "Error! Not equal types " << tree_node.params[0].value << " and " << tree_node.params[1].value  << " |line: " << tree_node.params[0].line<< endl;
          exit(-1);
        }
        return 0;
    }
    Node transf(Node tree_node){
        if(!tree_node.params.empty()){
            int i = 0;
            Node node(tree_node.type,tree_node.value, tree_node.line, tree_node.col);
            //cout << "Sem tree " << node.value << endl;
            while (i < tree_node.params.size()){
                Parser check;
                check.sem_tree = transf(tree_node.params[i]);
                if(check.sem_tree.type == "Empty"){
                    i++;
                    continue;
                } else {
                    node.params.push_back(check.sem_tree);
                }
                i++;
            }
            return node;
            //cout << "PARAMS INSIDE" << endl;
            cout << tree_node.type << endl;
        }
        if(tree_node.type == "mod_access + class"){
            //cout << "SDSDDSDDSD" << endl;
            return Node();
        }
        if(tree_node.type == "';'"){
            //cout << ";;;;;;;;;;;;;;" << endl;
            return Node();
        }
        if(tree_node.type == "'{'"){
            //cout << "{{{{{{{{{" << endl;
            return Node();
        }
        if(tree_node.type == "'}'"){
            //cout << "}}}}}}}}}}" << endl;
            return Node();
        }
        if(tree_node.type == "')'"){
            //cout << ")))))))))))" << endl;
            return Node();
        }
        if(tree_node.type == "'('"){
            //cout << "((((((" << endl;
            return Node();
        }
        if(tree_node.type == "'['"){
            //cout << "[[[[[[[" << endl;
            return Node();
        }
        if(tree_node.type == "']'"){
            //cout << "]]]]]]]" << endl;
            return Node();
        }
        return tree_node;
    }
    Node step(){
        Lexer first_tok;
        first_tok.token = for_tok.tokens[current];
        Lexer second_tok;
        if(current < for_tok.tokens.size() && current + 1 != for_tok.tokens.size()){
            second_tok.token = for_tok.tokens[current+1];
        }
        if (current + 1 == for_tok.tokens.size()) {
            second_tok.token = for_tok.tokens[current];
        }
        //cout << "F " << for_tok.tokens[for_tok.tokens.size()].token_class << endl;
        /*if(current >= for_tok.tokens.size()){
            string type,value;
            value = "Eof";
            type = "Eof";
            Node node(type,value,0,0);
            return node;
        }*/
        if (first_tok.token.token_class == "'mod_access'" && second_tok.token.token_class== "'class'"){ // BEGINNING OR MAIN FUNCTION
            current = current + 2;
            string type, value;
            //cout << "mod_access + class"<< endl;
            type = "mod_access + class";
            value = "Public class";
            Node node(type, value, first_tok.token.line , first_tok.token.col);
            //cout << "MOD ACCESS" << endl;
            write_ast(nestlvl,node.type,node.value);
            return node;
        }
        if(first_tok.token.token_class == "'id'" && second_tok.token.token_class == "'{'"){
            string type = "'Main class'";
            string value = first_tok.token.lexem;
            //cout << "'Main class'" << endl;
            visibility.push_back("Main Class");
            current = current + 2;
            Node node(type, value, first_tok.token.line , first_tok.token.col);
            write_ast(nestlvl,node.type,node.value);
            int nest = nestlvl;
            //cout << "nest " << nest << endl;
            nestlvl++;
            //cout <<"Current nestlvl " << nestlvl << endl;
            node.params.push_back(Node(second_tok.token.token_class,second_tok.token.lexem, second_tok.token.line, second_tok.token.col));
            write_ast(nestlvl,second_tok.token.lexem,second_tok.token.token_class);
            while(nest < nestlvl){
                Parser pushing;
                pushing.ast = step();
                node.params.push_back(pushing.ast);
            }
            return node;
        }
        if(first_tok.token.token_class == "'mod_access'" && second_tok.token.token_class == "'modificator'"){
            current = current + 2;
            string type, value;
            //cout << "mod_access + static" << endl;
            type = "mod_access + static";
            value = "Main Function";
            Node node(type, value, first_tok.token.line , first_tok.token.col);
            write_ast(nestlvl,node.type,node.value);
            return node;
        }
        if(first_tok.token.token_class == "'void'" && second_tok.token.token_class == "'access_point'"){
            current = current + 2;
            string type, value;
            type = "void + main";
            //cout << "void + main" << endl;
            visibility.push_back("Main Function");
            value = "Access point";
            Node node(type, value, first_tok.token.line , first_tok.token.col);
            write_ast(nestlvl,node.type,node.value);
            int nest = nestlvl;
            nestlvl++;
            while(nest < nestlvl){
                node.params.push_back(step());
            }
            return node;
        }
        if(first_tok.token.token_class == "'string'" && second_tok.token.token_class == "'[]'"){
            current = current + 2;
            string type, value;
            type = "Arguments";
            value = "Args from main func";
            //cout << "String + []" << endl;
            Node node(type, value, first_tok.token.line , first_tok.token.col);
            write_ast(nestlvl,node.type,node.value);
            return node;
        }
        if(first_tok.token.token_class == "'string'" && second_tok.token.token_class == "'id'"){
            current = current + 2;
            Node node(first_tok.token.token_class,second_tok.token.lexem, first_tok.token.line , first_tok.token.col);
            int o = 0;
            while (o < table.size()){
              if(table[o].value == second_tok.token.lexem){
                cout << "Error: This variable already exist '" << second_tok.token.lexem <<"' |line: " << second_tok.token.line << endl;
                exit(-1);
              }
              o++;
            }
            table.push_back(Table(visibility[nestlvl], nestlvl, second_tok.token.lexem, first_tok.token.token_class ));
            write_ast(nestlvl,node.type,node.value);
            return node;
        }
        if(first_tok.token.lexem == "args" && second_tok.token.token_class == "')'"){
            current++;
            //cout << "args " << endl;
            return step();
        }
        if (first_tok.token.token_class == "'{'"){ // for later // {
            current++;
            //cout << "{ " << endl;
            write_ast(nestlvl,first_tok.token.token_class ,first_tok.token.lexem);
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }
        if (first_tok.token.token_class == "'}'" && second_tok.token.token_class == "';'" ){ // for later // }
            current++;
            write_ast(nestlvl,first_tok.token.lexem,first_tok.token.token_class);
            //cout << "Nestlvl " << nestlvl << " TOken " << first_tok.token.token_class << endl;
            //cout <<"WAT " << current << endl;
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }
        if (first_tok.token.token_class == "'}'"){ // for later // }
            current++;
            nestlvl--;
            write_ast(nestlvl,first_tok.token.lexem,first_tok.token.token_class);
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }

        if (first_tok.token.token_class == "'('"){ // for (
            current++;
            //cout << "(" << endl;
            string type,value;
            value = "Expr";
            type = "CallExpr";
            //cout << "Nestlvl " << nestlvl << " TOken " << first_tok.token.token_class << endl;
            Node node(type,value,first_tok.token.line, first_tok.token.col);
            write_ast(nestlvl,node.type,node.value);
            node.params.push_back(Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col));
            write_ast(nestlvl,first_tok.token.token_class ,first_tok.token.lexem);
            first_tok.token = for_tok.tokens[current];
            int nest = nestlvl;
            nestlvl++;
            //cout << "Nestlvl in ((((( " << nestlvl << endl;
            while (nestlvl > nest){
                node.params.push_back(step());
                //first_tok.token = for_tok.tokens[current];
            }
            return node;
        }
        if (first_tok.token.token_class == "')'"){ // for later // }
            current++;
            nestlvl--;
            write_ast(nestlvl,first_tok.token.token_class ,first_tok.token.lexem);
            //cout << "Nestlvl " << nestlvl << " TOken " << first_tok.token.token_class << endl;
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }
        if (first_tok.token.token_class == "']'"){ // for later // }
            current++;
            write_ast(nestlvl,first_tok.token.token_class ,first_tok.token.lexem);
            nestlvl--;
            //cout << "Nestlvl " << nestlvl << " TOken " << first_tok.token.token_class << endl;
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }
        if (first_tok.token.token_class == "'['"){ // for later // }
            current++;
            write_ast(nestlvl,first_tok.token.token_class ,first_tok.token.lexem);
            nestlvl++;
            //cout << "Nestlvl " << nestlvl << " TOken " << first_tok.token.token_class << endl;
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }
        if (first_tok.token.token_class== "','"){ // for later // }
            if(second_tok.token.token_class != "'decimal_number'"){
                cout << "Error: there is incorrect sign after ',' |line: " << first_tok.token.line << endl;
                exit(-1);
            }
            current++;
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            //cout << "it was ," << endl;
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }
        if (first_tok.token.token_class == "'String'"){ // for later // }
            current++;
            write_ast(nestlvl,first_tok.token.token_class ,first_tok.token.lexem);
            //cout << "it was String literal" << endl;
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }

        if (first_tok.token.token_class == "'int'" && second_tok.token.token_class == "'id'" ){  // for int smth;
            current = current + 2;
            int o = 0;
            while (o < table.size()){
              if(table[o].value == second_tok.token.lexem){
                cout << "Error: This variable already exist '" << second_tok.token.lexem << "' |line: " << first_tok.token.line  << endl;
                exit(-1);
              }
              o++;
            }
            table.push_back(Table(visibility[nestlvl], nestlvl, second_tok.token.lexem, first_tok.token.token_class ));
            write_ast(nestlvl,first_tok.token.token_class,second_tok.token.lexem);
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }

        if (first_tok.token.token_class == "';'"){ // for ;
            current++;
            write_ast(nestlvl,first_tok.token.token_class ,first_tok.token.lexem);
            //cout <<"It was ;" << endl;
            return Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
        }
        if (first_tok.token.token_class == "'id'" && second_tok.token.token_class == "'='"){ // for x = number; and new int[x]; add checking errors (invalid input char for int)!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            int j = 0;
            //cout << "!@!@!@" << endl;
            while(j < var_table.size())
            {
                if(first_tok.token.lexem == var_table[j].lexem){
                    string type = "CallExpr";
                    write_ast(nestlvl,type,second_tok.token.token_class);
                    Node node(type,second_tok.token.lexem, second_tok.token.line, second_tok.token.col); // pushing '='
                    node.params.push_back(Node(first_tok.token.token_class,first_tok.token.lexem,first_tok.token.line,first_tok.token.col));
                    write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem); //pushing first argument
                    current = current + 2;
                    first_tok.token = for_tok.tokens[current];
                    while (first_tok.token.token_class != "';'"){
                        //cout << "Current is " << current << endl;
                        if(current >= for_tok.tokens.size()){
                            cout << "ERROR THERES IS NO ; after = ... |line: " << first_tok.token.line << endl;
                            exit(-1);
                        }
                        if(first_tok.token.token_class == "'int'" ||
                            first_tok.token.token_class == "'char'"){
                                cout << "ERROR THERES IS NO ; after = ... |line: " << first_tok.token.line << endl;
                                exit(-1);
                        }
                        node.params.push_back(step());
                        first_tok.token = for_tok.tokens[current];
                    }
                    checking_types(node);
                    //cout << "End of checking types" << endl;
                    //cout <<
                    return node;
                    //current++;
                } else {
                    //cout << first_tok.token.lexem << " and " << var_table[j].lexem << endl;
                }
                j++;
            }
            cout << "ERROR : Didn`t find id: '" << first_tok.token.lexem << "' |line: "<< first_tok.token.line <<endl;
            exit(-1);
            return Node();
        }
        if (first_tok.token.token_class == "'id'" && (second_tok.token.token_class == "'+'" || second_tok.token.token_class == "'-'" ||
            second_tok.token.token_class == "'*'" || second_tok.token.token_class == "'/'"  ) ) { //Math operations
                string type;
                type = "Math";
                //cout << "Math"<< endl;
                write_ast(nestlvl,type,second_tok.token.token_class);
                Node node(type,second_tok.token.lexem, second_tok.token.line, second_tok.token.col); //pushing math
                node.params.push_back(Node(first_tok.token.token_class, first_tok.token.lexem, first_tok.token.line, first_tok.token.col)); // pushing  first argument
                write_ast(nestlvl,first_tok.token.lexem,first_tok.token.token_class);
                //cout << "Math";
                current = current + 2;
                first_tok.token = for_tok.tokens[current];
                //cout <<"It is ;" <<for_tok.tokens[current + 1].lexem << endl;
                while(first_tok.token.token_class != "';'" && first_tok.token.token_class != "')'" ){
                    node.params.push_back(step());
                    first_tok.token = for_tok.tokens[current];
                    //cout << "FIRST " << first_tok.token.lexem << endl;
                }
                checking_types(node);
                return node;
        }
        if (first_tok.token.token_class == "'decimal_number'" && (second_tok.token.token_class == "'+'" || second_tok.token.token_class == "'-'" ||
            second_tok.token.token_class == "'*'" || second_tok.token.token_class == "'/'"  ) ) { //Math operations
                string type;
                type = "Math";
                //cout << "Math"<< endl;
                write_ast(nestlvl,type,second_tok.token.token_class);
                Node node(type,second_tok.token.lexem, second_tok.token.line, second_tok.token.col); //pushing math
                node.params.push_back(Node(first_tok.token.token_class, first_tok.token.lexem, first_tok.token.line, first_tok.token.col)); // pushing  first argument
                write_ast(nestlvl,"Number",first_tok.token.lexem);
                //cout << "Math";
                current = current + 2;
                first_tok.token = for_tok.tokens[current];
                //cout <<"It is ;" <<for_tok.tokens[current + 1].lexem << endl;
                while(first_tok.token.token_class != "';'" && first_tok.token.token_class != "')'" ){
                    node.params.push_back(step());
                    first_tok.token = for_tok.tokens[current];
                    //cout << "FIRST " << first_tok.token.lexem << endl;
                }
                checking_types(node);
                return node;
        }
        if ((first_tok.token.token_class == "'id'" || first_tok.token.token_class == "'decimal_number'" ) && (second_tok.token.token_class == "'<'" || second_tok.token.token_class == "'>'" )) { // x != y
            string type;
            type = "Comparison";
            //cout << "id ><" << endl;
            Node node(type,second_tok.token.lexem, second_tok.token.line, second_tok.token.col); //pushing < >
            write_ast(nestlvl,type,second_tok.token.token_class);
            node.params.push_back(Node(first_tok.token.token_class, first_tok.token.lexem, first_tok.token.line, first_tok.token.col)); //pushing id
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            //cout << current << endl;
            current = current + 2;
            first_tok.token = for_tok.tokens[current];
            while(first_tok.token.token_class != "')'"){
                node.params.push_back(step());
                first_tok.token = for_tok.tokens[current];
            }
            //cout <<"***************************" << endl;
            //cout << node.value << endl;
            checking_types(node);
            //cout << "END <>"<< endl;
            return node;
        }
        if (first_tok.token.token_class == "'decimal_number'") { // decimal nnumber for j = 0; continue
            string type;
            type = "Number";
            //cout << "Nestlvl " << nestlvl << " TOken " << first_tok.token.token_class << endl;
            Node node(type,first_tok.token.lexem, first_tok.token.line, first_tok.token.col);
            write_ast(nestlvl,type,first_tok.token.lexem);
            //cout << current << endl;
            current++;
            //cout << "it was number " << endl;
            //cout << first_tok.token.lexem << endl;
            return node;
        }
        if (first_tok.token.token_class == "'int'" && second_tok.token.token_class == "'[]'") { // array[dd] = smth
            string type;
            type = "Array(alloc)";
            Node node(type,first_tok.token.lexem ,first_tok.token.line, first_tok.token.col);
            write_ast(nestlvl,type,first_tok.token.token_class);
            type = "'int[]'";
            int o = 0;
            while (o < table.size()){
              if(table[o].value == for_tok.tokens[current + 2].lexem){
                cout << "Error: This variable already exist '" << for_tok.tokens[current + 2].lexem <<"' |line: " << for_tok.tokens[current + 2].line << endl;
                exit(-1);
              }
              o++;
            }
            table.push_back(Table(visibility[nestlvl], nestlvl, for_tok.tokens[current + 2].lexem, type));
            //cout << current << endl;
            //cout << "It was array" << endl;
            current = current + 2;
            first_tok.token = for_tok.tokens[current];
            //cout << first_tok.token.lexem << endl;
            while (first_tok.token.token_class != "';'"){
                node.params.push_back(step());
                first_tok.token = for_tok.tokens[current];
            }
            return node;
        }
        if (first_tok.token.token_class == "'id'" && (second_tok.token.token_class== "'++'"|| second_tok.token.token_class== "'--'")) {
            string type;
            type = "Increment";
            Node node(type,second_tok.token.lexem,second_tok.token.line, second_tok.token.col);
            write_ast(nestlvl,type,second_tok.token.token_class);
            node.params.push_back(Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col));
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            current = current + 2;
            //cout << "Next token " << for_tok.tokens[current].token_class << endl;
            return node;
        }
        if (first_tok.token.token_class == "'id'" && second_tok.token.token_class == "'['") {
            string type;
            type = "Array";
            Node node(type,first_tok.token.lexem,first_tok.token.line, first_tok.token.col); //pushing id
            write_ast(nestlvl,type,first_tok.token.token_class);
            int nest = nestlvl;
            nestlvl++;
            //if(first_tok.token.lexem == table[0].value){
            //}
            node.params.push_back(Node(second_tok.token.token_class, second_tok.token.lexem ,second_tok.token.line, second_tok.token.col)); //pushing [
            write_ast(nestlvl,second_tok.token.lexem,second_tok.token.token_class);
            current = current + 2;
            first_tok.token = for_tok.tokens[current];
            while(nest < nestlvl){ //first_tok.token.lexem != "';'" && first_tok.token.lexem != "')'"
                node.params.push_back(step());
            }
            return node;
        }
        if (first_tok.token.token_class == "'id'" && second_tok.token.token_class == "'!='") { // x != y
            string type;
            type = "CallExpr";
            //cout << "id !=" << endl;
            Node node(type,second_tok.token.lexem,second_tok.token.line, second_tok.token.col); //pushing !=
            write_ast(nestlvl,type,second_tok.token.token_class);
            node.params.push_back(Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col)); //pushing id
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            //cout << current << endl;
            current = current + 2;
            first_tok.token = for_tok.tokens[current];
            while(first_tok.token.token_class != "')'"){
                node.params.push_back(step());
                first_tok.token = for_tok.tokens[current];
            }
            checking_types(node);
            return node;
        }
        if (first_tok.token.token_class == "'id'" && second_tok.token.token_class == "'=='") { // x != y
            string type;
            type = "CallExpr";
            //cout << "id == " <<first_tok.token.token_class <<  endl;
            Node node(type,second_tok.token.lexem,second_tok.token.line, second_tok.token.col); //pushing ==
            write_ast(nestlvl,type,second_tok.token.token_class);
            node.params.push_back(Node(first_tok.token.token_class, first_tok.token.lexem,first_tok.token.line, first_tok.token.col)); //pushing id
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            //cout << current << endl;
            current = current + 2;
            first_tok.token = for_tok.tokens[current];
            while(first_tok.token.token_class != "')'"){
                node.params.push_back(step());
                first_tok.token = for_tok.tokens[current];
            }
            checking_types(node);
            //cout <<"After id ==" << for_tok.tokens[current].lexem << endl;
            return node;
        }
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! add checking for string
        if (first_tok.token.token_class == "'id'" && second_tok.token.token_class == "'.'") { // add checking for string
            string type;
            int i = 0;
            int isTrue = 0;
            while(i < table.size()){
                if(first_tok.token.lexem == table[i].value && table[i].type == "'string'"){
                    //cout << "BINGO" << endl;
                    isTrue = 1;
                }
                i++;
            }
            if(isTrue == 0){
              cout << "Error: calling method char_at not from string |line: " << first_tok.token.line << " |col: "<< second_tok.token.col<<  endl;
              exit(-1);
            }
            type = "String + method";
            Node node(type,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            write_ast(nestlvl,type,first_tok.token.token_class);
            current = current + 2;
            //cout << "it was id + . " << endl;
            return node;
        }
        if (first_tok.token.token_class == "'method_to_char'" && second_tok.token.token_class == "'('") { // add checking for string
            Node node(first_tok.token.token_class,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            current = current + 2;
            node.params.push_back(Node(second_tok.token.token_class, second_tok.token.lexem,second_tok.token.line, second_tok.token.col));
            write_ast(nestlvl,second_tok.token.token_class,second_tok.token.lexem);
            //cout << "it was Char at" << endl;
            int nest = nestlvl;
            nestlvl++;
            //first_tok.token = for_tok.tokens[current];
            while(nest < nestlvl){
                node.params.push_back(step());
                //first_tok.token = for_tok.tokens[current];
            }
            return node;
        }
        if (first_tok.token.token_class == "'id'") { // decimal nnumber for j = 0; continue
            Node node(first_tok.token.token_class,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            current++;
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            //cout << "it was number " << endl;
            return node;
        }

        if (first_tok.token.token_class == "'System class'" && second_tok.token.token_class == "'.'") { // decimal nnumber for j = 0; continue
            Node node(first_tok.token.token_class,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            node.params.push_back(Node(second_tok.token.token_class, second_tok.token.lexem,second_tok.token.line, second_tok.token.col));
            write_ast(nestlvl,second_tok.token.token_class,second_tok.token.lexem);
            current = current + 2;
            first_tok.token = for_tok.tokens[current];
            while(first_tok.token.token_class != "';'"){
                node.params.push_back(step());
                first_tok.token = for_tok.tokens[current];
            }
            //cout << "it System " << endl;
            return node;
        }
        if (first_tok.token.token_class == "'.'") { // decimal nnumber for j = 0; continue
            Node node(first_tok.token.token_class,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            current++;
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            //cout << "it was '.' " << endl;
            return node;
        }
        if (first_tok.token.token_class  == "'out'") { // decimal nnumber for j = 0; continue
            Node node(first_tok.token.token_class,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            current++;
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            //cout << "it was number " << endl;
            return node;
        }
        if (first_tok.token.token_class == "'char'" && second_tok.token.token_class == "'id'" ) { // decimal nnumber for j = 0; continue
            Node node(first_tok.token.token_class,second_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            int o = 0;
            while (o < table.size()){
              if(table[o].value == second_tok.token.lexem){
                cout << "Error: This variable already exist '" << second_tok.token.lexem << "' line: " << second_tok.token.line <<endl;
                exit(-1);
              }
              o++;
            }
            table.push_back(Table(visibility[nestlvl], nestlvl, second_tok.token.lexem, first_tok.token.token_class ));
            current = current + 2;
            write_ast(nestlvl,first_tok.token.token_class,second_tok.token.lexem);
            //cout << "Char + id " << endl;
            return node;
        }
        if (first_tok.token.token_class == "'print method'" && second_tok.token.token_class == "'('") { // decimal nnumber for j = 0; continue
            Node node(first_tok.token.token_class,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            write_ast(nestlvl,first_tok.token.token_class,first_tok.token.lexem);
            nestlvl++;
            node.params.push_back(Node(second_tok.token.token_class, second_tok.token.lexem,second_tok.token.line, second_tok.token.col));
            write_ast(nestlvl,second_tok.token.token_class,second_tok.token.lexem);
            current = current + 2;
            first_tok.token = for_tok.tokens[current];
            while(first_tok.token.token_class != "';'"){
                node.params.push_back(step());
                first_tok.token = for_tok.tokens[current];
            }
            //cout << "it was number " << endl;
            return node;
        }
        if (first_tok.token.token_class == "'cycle_while'") { //WHILE
            string type;
            type = "CallExpr";
            visibility.push_back("While");
            current++;
            if(second_tok.token.token_class != "'('")
            {
                cout << "ERROR! There is invalid token after 'While' |line: " << first_tok.token.line << " |col: "  << second_tok.token.col << endl;
                exit(-1);
            }
            Node node(type,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            write_ast(nestlvl,type,first_tok.token.token_class);
            first_tok.token = for_tok.tokens[current];
            int nest = nestlvl;
            nestlvl++;
            //cout << "while " << nestlvl - 1<< endl;
            while (nestlvl > nest){
                if(current >= (for_tok.tokens.size() - 1)){
                  cout << "ERROR! There is no closing '}' after 'While' |line: " << first_tok.token.line << " |col: "  << first_tok.token.col << endl;
                  exit(-1);
                }
                //cout << "current " << current << " " << for_tok.tokens.size() << endl;
                node.params.push_back(step());
                //first_tok.token = for_tok.tokens[current];
            }
            //node.params.push_back(Node(first_tok.token.lexem, first_tok.token.token_class));
            //cout << "END WHILE" << endl;
            //current = current + 2;
            //cout << "Next token " << for_tok.tokens[current].token_class << endl;
            return node;
        }
        if (first_tok.token.token_class == "'if'") { //WHILE
            string type;
            type = "CallExpr";
            visibility.push_back("If");
            write_ast(nestlvl,type,first_tok.token.token_class);
            current++;
            if(second_tok.token.token_class != "'('")
            {
                cout << "ERROR! There is invalid token after 'if' |line: " << second_tok.token.line << " |col: " << second_tok.token.col << endl;
                exit(-1);
            }
            Node node(type,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            first_tok.token = for_tok.tokens[current];
            int nest = nestlvl;
            nestlvl++;
            //cout << "if"<< endl;
            while (nestlvl > nest){
                node.params.push_back(step());
                //first_tok.token = for_tok.tokens[current];
            }
            return node;
        }
        if (first_tok.token.token_class == "'else'") { //WHILE
            string type;
            type = "CallExpr";
            visibility.push_back("Else");
            current++;
            if(second_tok.token.token_class != "'{'")
            {
                cout << "ERROR! There is invalid token after 'else' |line: " << second_tok.token.line << " |col: " << second_tok.token.col << endl;
                exit(-1);
            }
            write_ast(nestlvl,type,first_tok.token.token_class);
            Node node(type,first_tok.token.lexem,first_tok.token.line, first_tok.token.col);
            first_tok.token = for_tok.tokens[current];
            int nest = nestlvl;
            nestlvl++;
            while (nestlvl > nest){
                node.params.push_back(step());
            }
            return node;
        } else {
            if(second_tok.token.lexem != "Empty"){
                cout << "ERROR! There is invalid combination of tokens " << first_tok.token.lexem << " and " << second_tok.token.lexem  <<
                    " |line: " << second_tok.token.line << " |col: " << second_tok.token.col << endl;
                exit(0);
            } else {
              cout << "ERROR! There is no closing } " << endl;
              exit(0);
            }
            // for testing
        }
        return Node();
    }
};


int main(int argc, char *argv[]) //FOR CHECKING EMPTY PARAMS USE cout << "Before push" << ast.params.empty() << endl; // 1 if empty
{
    Parser parser;
    Parser jack;
    parser.lex.file.open(argv[1]);
    parser.parse();
    //parser.create_var_tables();
    int i = 0;
    /*for(i = 0 ; i < parser.var_table.size(); i++){
        cout << parser.var_table[i].lexem << " " << parser.var_table[i].token_class << endl;
    }*/
    /*for(i ; i < parser.for_tok.tokens.size(); i++){
        cout << parser.for_tok.tokens[i].token_class << " " << parser.for_tok.tokens[i].lexem  <<" " << i<< endl;
    }*/

    /*cout << "parser " << endl;
    for(i = 0 ; i < parser.ast.params.size(); i++){
        cout << parser.ast.params[i].type << " " << parser.ast.params[i].value  <<" " << i<< endl;
    }*/
    /*for(i = 0 ; i < parser.ast.params.size(); i++){
        cout << parser.atable[i].value << " " << parser.table[i].type <<" " << i<< endl;
    }
    int k = 0;*/
    //parser.ast_to_sem();
    /*while(k < 4){
        write_ast(k,parser.ast.params[0].type,parser.ast.params[0].value);
        k++;
    }*/
    //cout << parser.var_table[0].lexem << " " << parser.var_table[0].token_class << endl;
    /*for(i = 0 ; i < parser.var_table.size(); i++){
        cout << parser.var_table[i].lexem << " " << parser.var_table[i].token_class << endl;
    }*/
    //FROM HERE
    /*cout << endl << "table" << endl << endl;
    for(i = 0 ; i < parser.table.size(); i++){
        cout << parser.table[i].value << " " << parser.table[i].type << " " << parser.table[i].nestlvl << " " << parser.table[i].visibility << endl;
    }*/
    //cout << parser.visibility[0] << endl;
    /*cout <<  parser.ast.params[0].type << " " << parser.ast.params[0].value  << endl;
    cout <<  parser.ast.params[1].params[0].type << " " << parser.ast.params[1].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[1].type << " " << parser.ast.params[1].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].type << " " << parser.ast.params[1].params[2].value  << endl;*/
    /*cout << "SEMANTIC" << endl;
    cout << parser.sem_tree.params[0].type << endl;
    cout << parser.sem_tree.params[0].params[0].type << endl;*/
    /*cout <<  parser.ast.params[1].params[2].params[0].type << " " << parser.ast.params[1].params[2].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[0].params[0].type << " " << parser.ast.params[1].params[2].params[0].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[0].params[1].type << " " << parser.ast.params[1].params[2].params[0].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[0].params[2].type << " " << parser.ast.params[1].params[2].params[0].params[2].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[1].type << " " << parser.ast.params[1].params[2].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[2].type << " " << parser.ast.params[1].params[2].params[2].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[3].type << " " << parser.ast.params[1].params[2].params[3].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[4].type << " " << parser.ast.params[1].params[2].params[4].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[5].type << " " << parser.ast.params[1].params[2].params[5].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[6].type << " " << parser.ast.params[1].params[2].params[6].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[7].type << " " << parser.ast.params[1].params[2].params[7].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[8].type << " " << parser.ast.params[1].params[2].params[8].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[9].type << " " << parser.ast.params[1].params[2].params[9].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].type << " " << parser.ast.params[1].params[2].params[10].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[11].type << " " << parser.ast.params[1].params[2].params[11].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[12].type << " " << parser.ast.params[1].params[2].params[12].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[13].type << " " << parser.ast.params[1].params[2].params[13].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[14].type << " " << parser.ast.params[1].params[2].params[14].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[15].type << " " << parser.ast.params[1].params[2].params[15].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[16].type << " " << parser.ast.params[1].params[2].params[16].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[17].type << " " << parser.ast.params[1].params[2].params[17].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].type << " " << parser.ast.params[1].params[2].params[18].value  << endl;*/
    //while()
    //cout << "While" << endl;
    //cout <<  parser.ast.params[1].params[2].params[19].type << " " << parser.ast.params[1].params[2].params[19].value  << endl;
    /*cout <<  parser.ast.params[1].params[2].params[18].params[0].type << " " << parser.ast.params[1].params[2].params[18].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[1].type << " " << parser.ast.params[1].params[2].params[18].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[2].type << " " << parser.ast.params[1].params[2].params[18].params[2].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[3].type << " " << parser.ast.params[1].params[2].params[18].params[3].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[6].type << " " << parser.ast.params[1].params[2].params[18].params[6].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[7].type << " " << parser.ast.params[1].params[2].params[18].params[7].value  << endl;*/
    /*cout <<  parser.ast.params[1].params[2].params[18].params[0].params[0].type << " " << parser.ast.params[1].params[2].params[18].params[0].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[0].params[1].type << " " << parser.ast.params[1].params[2].params[18].params[0].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[0].params[1].params[0].type << " " << parser.ast.params[1].params[2].params[18].params[0].params[1].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[0].params[1].params[1].type << " " << parser.ast.params[1].params[2].params[18].params[0].params[1].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[0].params[1].params[1].params[0].type << " " << parser.ast.params[1].params[2].params[18].params[0].params[1].params[1].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[0].params[1].params[1].params[1].type << " " << parser.ast.params[1].params[2].params[18].params[0].params[1].params[1].params[1].value  << endl;*/
    //cout <<  parser.ast.params[1].params[2].params[18].params[0].params[0].params[0].type << " " << parser.ast.params[1].params[2].params[18].params[0].params[0].params[0].value  << endl;
    //cout <<  parser.ast.params[1].params[2].params[18].params[0].params[2].type << " " << parser.ast.params[1].params[2].params[18].params[0].params[2].value  << endl;
    //cout <<  parser.ast.params[1].params[2].params[18].params[1].type << " " << parser.ast.params[1].params[2].params[18].params[1].value  << endl;
    //cout <<  parser.ast.params[1].params[2].params[18].params[2].type << " " << parser.ast.params[1].params[2].params[18].params[2].value  << endl;
    /*cout <<  parser.ast.params[1].params[2].params[18].params[2].params[0].type << " " << parser.ast.params[1].params[2].params[18].params[2].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[2].params[1].type << " " << parser.ast.params[1].params[2].params[18].params[2].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[2].params[2].type << " " << parser.ast.params[1].params[2].params[18].params[2].params[2].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[2].params[2].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[2].params[2].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[2].params[2].params[2].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[2].params[1].value  << endl;*/
    /*cout <<  parser.ast.params[1].params[2].params[18].params[3].type << " " << parser.ast.params[1].params[2].params[18].params[3].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[4].type << " " << parser.ast.params[1].params[2].params[18].params[4].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[5].type << " " << parser.ast.params[1].params[2].params[18].params[5].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[6].type << " " << parser.ast.params[1].params[2].params[18].params[6].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[7].type << " " << parser.ast.params[1].params[2].params[18].params[7].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[8].type << " " << parser.ast.params[1].params[2].params[18].params[8].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[18].params[9].type << " " << parser.ast.params[1].params[2].params[18].params[9].value  << endl;
    //cout <<  parser.ast.params[1].params[2].params[18].params[10].type << " " << parser.ast.params[1].params[2].params[18].params[10].value  << endl;*/
    //To here
    //cout <<  parser.ast.params[1].params[2].params[18].params[3].type << " " << parser.ast.params[1].params[2].params[18].params[3].value  << endl;
    /*cout <<  parser.ast.params[1].params[2].params[1].type << " " << parser.ast.params[1].params[2].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[2].type << " " << parser.ast.params[1].params[2].params[2].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[3].type << " " << parser.ast.params[1].params[2].params[3].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].type << " " << parser.ast.params[1].params[2].params[10].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].params[0].type << " " << parser.ast.params[1].params[2].params[10].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].params[1].type << " " << parser.ast.params[1].params[2].params[10].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].params[2].type << " " << parser.ast.params[1].params[2].params[10].params[2].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].params[2].params[0].type << " " << parser.ast.params[1].params[2].params[10].params[2].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].params[2].params[2].type << " " << parser.ast.params[1].params[2].params[10].params[2].params[2].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].params[2].params[2].params[0].type << " " << parser.ast.params[1].params[2].params[10].params[2].params[2].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].params[2].params[2].params[1].type << " " << parser.ast.params[1].params[2].params[10].params[2].params[2].params[1].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].params[2].params[2].params[1].params[0].type << " " << parser.ast.params[1].params[2].params[10].params[2].params[2].params[1].params[0].value  << endl;
    cout <<  parser.ast.params[1].params[2].params[10].params[2].params[2].params[1].params[1].type << " " << parser.ast.params[1].params[2].params[10].params[2].params[2].params[1].params[1].value  << endl;
    /*cout <<  parser.ast.params[1].params[3].type << " " << parser.ast.params[1].params[3].value  << endl;
    /*cout <<  parser.ast.params[0].params[1].type << " " << parser.ast.params[0].params[1].value  << endl;
    cout <<  parser.ast.params[0].params[2].type << " " << parser.ast.params[0].params[2].value  << endl;
    cout <<  parser.ast.params[0].params[3].type << " " << parser.ast.params[0].params[3].value  << endl;
    cout <<  parser.ast.params[0].params[3].params[0].type << " " << parser.ast.params[0].params[3].params[0].value  << endl;
    cout <<  parser.ast.params[0].params[3].params[1].type << " " << parser.ast.params[0].params[3].params[1].value  << endl;
    cout <<  parser.ast.params[0].params[3].params[2].type << " " << parser.ast.params[0].params[3].params[2].value  << endl;
    cout <<  parser.ast.params[2].params[1].params[0].type << " " << parser.ast.params[2].params[1].params[0].value  << endl;
    cout <<  parser.ast.params[2].params[1].params[1].type << " " << parser.ast.params[2].params[1].params[1].value  << endl;*/
    /*cout <<  parser.ast.params[0].params[0].type << " " << parser.ast.params[0].params[0].value  << endl;
    cout <<  parser.ast.params[0].params[1].type << " " << parser.ast.params[0].params[1].value  << endl;
    cout <<  parser.ast.params[0].params[1].params[0].type << " " << parser.ast.params[0].params[1].params[0].value  << endl;
    cout <<  parser.ast.params[0].params[1].params[1].type << " " << parser.ast.params[0].params[1].params[1].value  << endl;
    cout <<  parser.ast.params[0].params[1].params[1].params[0].type << " " << parser.ast.params[0].params[1].params[1].params[0].value  << endl;
    cout <<  parser.ast.params[0].params[1].params[1].params[1].type << " " << parser.ast.params[0].params[1].params[1].params[1].value  << endl;
    cout <<  parser.ast.params[0].params[1].params[1].params[2].type << " " << parser.ast.params[0].params[1].params[1].params[2].value  << endl;*/
    /*cout <<  parser.ast.params[0].params[0].params[1].type << " " << parser.ast.params[0].params[0].params[1].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[2].type << " " << parser.ast.params[0].params[0].params[2].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[3].type << " " << parser.ast.params[0].params[0].params[3].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[4].type << " " << parser.ast.params[0].params[0].params[4].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[5].type << " " << parser.ast.params[0].params[0].params[5].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[6].type << " " << parser.ast.params[0].params[0].params[6].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[7].type << " " << parser.ast.params[0].params[0].params[7].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[8].type << " " << parser.ast.params[0].params[0].params[8].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[9].type << " " << parser.ast.params[0].params[0].params[9].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[10].type << " " << parser.ast.params[0].params[0].params[10].value  << endl;
    cout <<  parser.ast.params[0].params[0].params[11].type << " " << parser.ast.params[0].params[0].params[11].value  << endl;
    //cout <<  parser.ast.params[0].params[0].params[12].type << " " << parser.ast.params[0].params[0].params[12].value  << endl;
    /*cout <<  parser.ast.params[0].params[1].params[0].type << " " << parser.ast.params[0].params[1].params[0].value  << endl;
    cout <<  parser.ast.params[0].params[1].params[1].type << " " << parser.ast.params[0].params[1].params[1].value  << endl;
    cout <<  parser.ast.params[0].params[1].params[2].type << " " << parser.ast.params[0].params[1].params[2].value  << endl;*/
    //cout <<  parser.ast.params[0].params[2].type << " " << parser.ast.params[0].params[2].value  << endl;
    //cout <<  parser.ast.params[6].params[3].type << " " << parser.ast.params[6].params[2].value  << endl;
    parser.lex.file.close();

}
