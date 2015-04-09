#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

class CCoord
{
public:
    CCoord( double lat, double lon )
    {
        this->lat = lat;
        this->lon = lon;
    }

    CCoord( const string & latlon )
    {

    }

private:
    double lat;
    double lon;
};

class CGPS
{
public:
    CGPS()
    {

    }

    ~CGPS()
    {

    }

    CGPS& Add( CCoord coordinates )
    {
        log.insert( log.end(), &coordinates );
    }

    CGPS& operator+=( const string & coords )
    {

    }

    CCoord operator[]( size_t index )
    {

    }

    ostream& operator<<( ostream& os, const CGPS& obj )
    {

    }

    istream& operator>>( istream& is, CGPS& obj )
    {

    }

    int Distance()
    {
        return 0; // todo
    }

private:
    vector<CCoord*> log;
};

int main( void )
{
    double d;
    CGPS x0;
    cout << x0;
    // []

    x0 += "(50 6\'16.5\"N, 14 23\'20.25\" E)";
    cout << x0;
    // [(50 6'16.5"N, 14 23'20.25"E)]

    x0 . Add ( CCoord ( "(50.1003781N, 14.3925125E)" ) ) . Add ( CCoord ( 50.084202, 14.423357 ) );
    cout << x0;
    // [(50 6'16.5"N, 14 23'20.25"E) > (50 6'1.361"N, 14 23'33.045"E) > (50 5'3.127"N, 14 25'24.085"E)]

    d = x0 . Distance ();
    // d = 3373.886483

    x0 += "(50.0843122N, 14.417463 E)";
    cout << x0;
    // [(50 6'16.5"N, 14 23'20.25"E) > (50 6'1.361"N, 14 23'33.045"E) > (50 5'3.127"N, 14 25'24.085"E) > (50 5'3.524"N, 14 25'2.867"E)]

    cout << x0;
    // [(50 6'16.5"N, 14 23'20.25"E) > (50 6'1.361"N, 14 23'33.045"E) > (50 5'3.127"N, 14 25'24.085"E) > (50 5'3.524"N, 14 25'2.867"E)]

    d = x0 . Distance ();
    // d = 3794.598230

    CCoord tmp0 = x0 [ 1 ];
    cout << tmp0;
    // (50 6'1.361"N, 14 23'33.045"E)

    CGPS tmp1 = x0 ( 1, 2 );
    cout << tmp1;
    // [(50 6'1.361"N, 14 23'33.045"E) > (50 5'3.127"N, 14 25'24.085"E)]


    CGPS x1;
    istringstream tmp2 ( "[(50 6\'16.5\"N, 14 23\'20.25\" E) > (51.5N, 0.0E) > (33.9S, 151.2E) > (37.42190N, 122.08405W) > (0.00N, 50.000W)]" );
    tmp2 >> x1;
    // tmp2 . fail () = false

    cout << x1;
    // [(50 6'16.5"N, 14 23'20.25"E) > (51 30'0"N, 0 0'0"E) > (33 54'0"S, 151 12'0"E) > (37 25'18.84"N, 122 5'2.58"W) > (0 0'0"N, 50 0'0"W)]

    cout << x1;
    // [(50 6'16.5"N, 14 23'20.25"E) > (51 30'0"N, 0 0'0"E) > (33 54'0"S, 151 12'0"E) > (37 25'18.84"N, 122 5'2.58"W) > (0 0'0"N, 50 0'0"W)]

    d = x1 . Distance ();
    // d = 38401662.588282

    CGPS tmp3 = x1 ( 1, 3 );
    cout << tmp3;
    // [(51 30'0"N, 0 0'0"E) > (33 54'0"S, 151 12'0"E) > (37 25'18.84"N, 122 5'2.58"W)]

    istringstream tmp4 ( "[(51.5N, 0.0E) > (33.9S, 151.2E) > (37.42190N, 122.08405W) > (100.00N, 50.000W)]" );
    tmp4 >> x1;
    // tmp4 . fail () = true
    cout << x1;
    // [(50 6'16.5"N, 14 23'20.25"E) > (51 30'0"N, 0 0'0"E) > (33 54'0"S, 151 12'0"E) > (37 25'18.84"N, 122 5'2.58"W) > (0 0'0"N, 50 0'0"W)]

    CCoord tmp5 = x1 [ 1 ];
    cout << tmp5;
    // (51 30'0"N, 0 0'0"E)

    CCoord tmp6 = x1 [ -1 ];
    // exception thrown

    CCoord tmp7 = x1 [ 5 ];
    // exception thrown

    CGPS tmp8 = x1 ( 2, 4 );
    cout << tmp8;
    // [(33 54'0"S, 151 12'0"E) > (37 25'18.84"N, 122 5'2.58"W) > (0 0'0"N, 50 0'0"W)]

    CGPS tmp9 = x1 ( 2, 5 );
    // exception thrown

    CGPS tmp10 = x1 ( 3, 1 );
    // exception thrown

    istringstream tmp11 ( "[(51.5N, 0.0E) > (33.9S, 151.2E) > (37.42190N, 122.08405W) > (80.00N, 50.000W)]" );
    tmp11 >> x1;
    // tmp11 . fail () = false
    cout << x1;
    // [(51 30'0"N, 0 0'0"E) > (33 54'0"S, 151 12'0"E) > (37 25'18.84"N, 122 5'2.58"W) > (80 0'0"N, 50 0'0"W)]

    cout << x1;
    // [(51 30'0"N, 0 0'0"E) > (33 54'0"S, 151 12'0"E) > (37 25'18.84"N, 122 5'2.58"W) > (80 0'0"N, 50 0'0"W)]
}