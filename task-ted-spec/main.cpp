#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include <stack>

#define INPUT cin
#define LINEEND "\n"

using namespace std;

string trim( string& str )
{
    size_t start = str.find_first_not_of( ' ' );
    size_t end = str.find_last_not_of( ' ' );
    return str.substr( 0, ( end + 1 ) );
}

int main() {

    ostringstream output;

    // --- file
/*
    ifstream file;
    file.open( "data.txt" );

    if ( file.fail() )
    {
        cerr << "File does not exists." << LINEEND;
        return 74;
    }*/

    int depthlevel = 0, lastdepthlevel = 1, id = 1;
    stack<int> parents;
    parents.push( 0 );
    char c;
    map<string,string> shapes = {
            {"house", "house"},
            {"attribute", "diamond"},
            {"cdata-section", "trapezium"},
            {"comment", "note"},
            {"element", "circle"},
            {"entity-reference", "oval"},
            {"processing-instruction", "triangle"},
            {"text", "plaintext"}
    };
    map<int, list<int>> children;

    output << "digraph {" << LINEEND;
    output << "\t0 [shape=house label=\"/\"]" << LINEEND;

    while ( ! INPUT.eof() )
    {
        INPUT.get( c );

        if ( c == ' ' )
        {
            depthlevel++;
        }

        else if ( c == '#' ) {
            string type, data;
            getline(INPUT, type, ' ');
            getline(INPUT, data);

            //data = trim(data);

            if (type.length() != 0 && data.length() != 0) {
                if (type == "attribute") data.replace(data.find('='), 1, "\\n");
                if (type == "processing-instruction") data.replace(data.find(' '), 1, "\\n");

                output << "\t" << id << " [shape=" << shapes[type] << " label=\"" << data << "\"]" << LINEEND;
            }
            else {
                if ( INPUT.eof() )
                    break;
                else {
                    cerr << "Not able to found type and data attributes at " << INPUT.tellg() << "." << LINEEND;
                    return 1;
                }
            }

            // ----------------

            if (depthlevel == lastdepthlevel) {
                int parent = parents.top();
                children[ parent ].insert( children[ parent ].end(), id );
            }
            else if (depthlevel > lastdepthlevel)
            {
                int parent = id - 1;
                parents.push( parent );
                children[ parent ].insert( children[ parent ].end(), id );
            }
            else if (depthlevel < lastdepthlevel)
            {
                for ( int i = 0; i < lastdepthlevel - depthlevel && parents.size() != 0; i++ )
                    parents.pop();
                int parent = parents.top();
                children[ parent ].insert( children[ parent ].end(), id );
            }

            lastdepthlevel = depthlevel;
            depthlevel = 0;
            id++;
        }

        else
        {
            cerr << "Oops, wrong character found at " << INPUT.tellg() << ", expected ' ' or '#', '" << c << "' found." << LINEEND;
            return 74;
        }
    }

    output << LINEEND;

    for ( const auto &it : children ) {
        int i = 0;
        for ( const auto &it2 : it.second ) {
            output << "\t" << it.first << " -> " << it2 << LINEEND;
            i++;
        }
    }

    output << "}" << LINEEND;
/*
    ofstream outfil;
    outfil.open( "output.txt", ios::binary );


    outfil << output.str();

    outfil.close();
*/

    cout << output.str();

    return 0;
}