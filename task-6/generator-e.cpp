#include <iostream>      /* printf, scanf, puts, NULL */
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */
#include <string>

using namespace std;

int fRand(int min, int max){
    return min + (rand() % (int)(max - min + 1));
}

string randomString( const int len) {
    static const char alphanum[] =
            "ABCDEFGH ";
    string s;
    for (int i = 0; i < 5; ++i) {
        s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return s;
}

string getImg(){
    string result;
    int len = fRand(5,15);
    result += "CImage() ";
    for(int i = 0 ; i < fRand(5,15); i++){
        result += ".AddRow(\"";
        result += randomString(len);
        result += "\\n\")";
    }
    return result;
}

string getEmpt(){
    string result;
    result += "CEmpty()";
    return result;
}

string getTxt(){
    string result;
    result += "CText(\"";
    result += randomString(fRand(5,15));
    result += "\",";
    result += to_string( fRand(0,1) );
    result += ")";
    return result;
}


string getObject(int func){
    string result;
    switch(func){
        case 0: result = getImg(); break;
        case 1: result = getTxt(); break;
        case 2: result = getEmpt(); break;
    }
    return result;
}


string SetCell(int i){
    string s;
    s += "SetCell(";
    s += to_string(fRand(0,110));
    s += ", ";
    s += to_string(fRand(0,110));
    s += ", ";
    s += getObject((rand() % (int)(3)));
    s += ");";
    return s;
}

string GetCell(int i){
    string s;
    s += "GetCell(";
    s += to_string(fRand(0,110));
    s += ", ";
    s += to_string(fRand(0,110));
    s += ");";
    return s;
}


string getFunction(int i ){
    switch(fRand(0,1)){
        case 0: return SetCell(i);
        default: return GetCell(i);

    }
}



int main (){
    int k = 80;
    int col = 100;
    int row = 100;

    for(int j=10;j<k;j++){
        cout<<"CTable x"<<j<<" ("<<col<<", "<<row<<" );"<<endl;
        for (int i = 0; i < k; ++i){
            cout<<"x"<<j<<"."<<getFunction(i)<<endl;
        }

    }
    return 0;
}