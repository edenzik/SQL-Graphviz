// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#define TABLE_GRAPHVIZ "[label=<<TABLE BORDER='0' CELLBORDER='1' CELLSPACING='0'>%s</TABLE>>]"
#define TABLE_CELL_GRAPHVIZ "<TR><TD PORT='%d'>%s</TD></TR>"
#define TABLE_CELL_KEY_GRAPHVIZ "<TR><TD PORT='%d' BGCOLOR='gray'>%s</TD></TR>"



using namespace std;


std::string trim(std::string const& str)
{
    if(str.empty())
        return str;

    std::size_t first = str.find_first_not_of(' ');
    std::size_t last  = str.find_last_not_of(' ');
    return str.substr(first, last-first+1);
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

class SQLStatement
{
    public:
        string literal;
        SQLStatement(string line){
            literal = line;
        };
        static unsigned statement_end(string str){
            size_t found = str.find_first_of(";");
            if (found!=std::string::npos) return found;
            return 0;
        };
        static bool statement_comment(string str){
            return str.compare(0,2,"--") == 0;
        };
        static string extract_name(string str){
            std::vector<string> x = split(str, ' ');
            return x[2];
        }
        virtual string to_graphviz(){
            return "REGULAR";
        }

};

class SQLCreate : public SQLStatement {
    public:
        SQLCreate(string line) : SQLStatement(line) {
            

        };
        virtual string to_graphviz(){
            std::hash<std::string> hash_fn;
            string name = extract_name(literal);
            string val = to_string(hash_fn(name));
            char buffer [1000];
            sprintf(buffer, TABLE_GRAPHVIZ, name.c_str());
            val+=buffer;
            return val;
        };
    private:
        string table_name;


};

class SQLAlter : public SQLStatement {

    public:
        SQLAlter(string line) : SQLStatement(line) {
            

        };
        virtual string to_graphviz(){
            return "ALTER";
        }

};

SQLStatement* parse_statement(string statement){
    size_t found = statement.find(' ');
    string type = statement.substr(0, found);
    //cout << type << endl;
    if (type == "CREATE" ) return new SQLCreate(statement);
    if (type=="ALTER") return new SQLAlter(statement);
    return nullptr;
    cout << statement.substr(0, found) << endl;
}

typedef map<string, SQLStatement> statementMap;
typedef pair<string, string> tableNamePair;

int main () {
    string statement_buffer;
    string line;
    unsigned end_position;
    while ( getline(std::cin, line) ){
        //line = trim(line);
        if (line.empty()) continue;
        if ( SQLStatement::statement_comment(line) ) continue;
        //cout << line.length() << statement_buffer << endl;
        if ( ( end_position = SQLStatement::statement_end(line)) ) {
            statement_buffer += line.substr(0,end_position);
            line.erase(0,end_position);
            SQLStatement *statement = parse_statement(statement_buffer);
            if (statement){
                cout << statement->to_graphviz() << endl;
                free(statement);
            }
            statement_buffer.clear();
            continue;
        }
        statement_buffer += line;
        //statement_buffer += " ";
    }

    return 0;
}

