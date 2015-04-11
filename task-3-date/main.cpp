#ifndef __PROGTEST__
#include <cstdio>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */

class InvalidDateException {};

//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
ios_base & dummy_date_format_manipulator ( ios_base & x )
{
    return x;
}

ios_base & ( * ( date_format ( const char * fmt ) ) ) ( ios_base & x )
{
    return dummy_date_format_manipulator;
}
//=================================================================================================

class CDate
{
public:

    CDate( int y, int m, int d )
    {
        if ( !isDateValid( y, m, d ) ) throw InvalidDateException();
        year = y; month = m; day = d;
    }

    CDate& operator+( int incd )
    {
        increase( incd );
        return *this;
    }

    CDate& operator-( int decd )
    {
        increase( -decd );
        return *this;
    }

    int operator-( CDate decd )
    {
        int days = 0;

        // No difference
        if ( decd == *this ) return 0;

        CDate larger = decd > *this ? decd : *this;
        CDate smaller = decd < *this ? decd : *this;

        // Process full years
        for ( int i = smaller.year + 1 ; i < larger.year ; i++ )
            days += isLeap( i ) ? 366 : 365;

        if ( days != 0 )
        {
            // Process full months at start todo: this is sensitive one
            for ( int i = smaller.month + 1 ; i <= 12; i++ )
                days += getMonthDaysCount( i, smaller.year );

            // Process full months at end todo: aaand this one too
            for ( int i = 1 ; i < larger.month; i++ )
                days += getMonthDaysCount( i, larger.year );
        }
        else
        {
            // Process full months within one year
            for ( int i = smaller.month +1 ; i < larger.month; i++ )
                days += getMonthDaysCount( i, smaller.year );
        }

        // Process start and end days
        days += smaller.year == larger.year && smaller.month == larger.month ?
                larger.day - smaller.day :
                getMonthDaysCount( smaller.month, smaller.year ) - smaller.day + larger.day;

        return days;
    }

    CDate& operator++()
    {
        increase( 1 );
        return *this;
    }

    CDate& operator--()
    {
        increase( -1 );
        return *this;
    }

    CDate operator++( int )
    {
        CDate tmp(*this);
        operator++();
        return tmp;
    }

    CDate operator--( int )
    {
        CDate tmp(*this);
        operator--();
        return tmp;
    }

    bool operator==( CDate date )
    {
        return ( this->year == date.year && this->month == date.month && this->day == date.day );
    }

    bool operator!=( CDate date )
    {
        return !( this->year == date.year && this->month == date.month && this->day == date.day );
    }

    bool operator<=( CDate date )
    {
        return ( this->year == date.year ? (
                this->month == date.month ? (
                        this->day <= date.day
                ) : this->month <= date.month
        ) : this->year <= date.year );
    }

    bool operator<( CDate date )
    {
        return ( this->year == date.year ? (
                this->month == date.month ? (
                        this->day < date.day
                ) : this->month < date.month
        ) : this->year < date.year );
    }

    bool operator>=( CDate date )
    {
        return ( this->year == date.year ? (
                this->month == date.month ? (
                        this->day >= date.day
                ) : this->month >= date.month
        ) : this->year >= date.year );
    }

    bool operator>( CDate date )
    {
        return ( this->year == date.year ? (
                this->month == date.month ? (
                        this->day > date.day
                ) : this->month > date.month
        ) : this->year > date.year );
    }

    friend ostream& operator<<( ostream& os, CDate& d )
    {
        os << setw(4) << setfill('0') << d.year << "-" << setw(2) << d.month << "-" << setw(2) << d.day;
        return (os);
    }

    friend istream& operator>>( istream& is, CDate &d )
    {
        int yr = 0, mt = 0, dy = 0;
        char d1, d2;

        // Get the shit there
        is >> yr >> d1 >> mt >> d2 >> dy;

        // Check if the shit is alrighty
        if ( ! is.eof() || is.fail()
             || ! ( d1 == '-' && d2 == '-' )
             || ! isDateValid( yr, mt, dy ) ) { is.setstate( ios::failbit ); return is; }

        // Get the fowk out of here
        d.year = yr; d.month = mt; d.day = dy;
        return (is);
    }

private:
    int year, month, day;

    void increase( int by )
    {
        day += by;

        // stepping up
        while ( isNextMonth( month, day ) )
        {
            day -= getMonthDaysCount( month );
            month++;

            if ( month > 12 ) { month = 1; year++; }
        }

        // stepping dn
        while ( day < 1 )
        {
            month--;
            if ( month < 1 ) { month = 12; year--; }
            day += getMonthDaysCount( month );
        }
    }

    static bool isDateValid( int y, int m, int d )
    {
        if ( y < 1 ) return false; // ?? not sure, todo
        if ( m < 1 || m > 12 ) return false;
        if ( isNextMonth( y, m, d ) ) return false;
        return true;
    }

    bool isNextMonth( int m, int d )
    {
        return d > getMonthDaysCount( m );
    }

    static bool isNextMonth( int y, int m, int d )
    {
        return d > getMonthDaysCount( m, y );
    }

    bool isLastDay()
    {
        return day == getMonthDaysCount( month );
    }

    bool isLastMonth()
    {
        return ( month == 12 );
    }

    bool isLeap()
    {
        return ( ( !( year % 4 ) && year % 100 ) || !( year % 400 ) ); // todo
    }

    static bool isLeap( int y )
    {
        return ( ( !( y % 4 ) && y % 100 ) || !( y % 400 ) ); // todo
    }

    int getMonthDaysCount( int m )
    {
        int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return isLeap( year ) && m == 2 ? 29 : daysInMonth[ m - 1 ];
    }

    static int getMonthDaysCount( int m, int y )
    {
        int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return isLeap( y ) && m == 2 ? 29 : daysInMonth[ m - 1 ];
    }
};

#ifndef __PROGTEST__

int main( void )
{

    ostringstream oss;
    istringstream iss;


    CDate x(1970, 1, 1);

    iss.str("12-");
    iss >> x;

    CDate a ( 2000, 1, 2 );
    CDate b ( 2010, 2, 3 );
    CDate c ( 2004, 2, 10 );
    oss . str ("");
    oss << a;
    assert ( oss . str () == "2000-01-02" );
    oss . str ("");
    oss << b;
    assert ( oss . str () == "2010-02-03" );
    oss . str ("");
    oss << c;
    assert ( oss . str () == "2004-02-10" );
    a = a + 1500;
    oss . str ("");
    oss << a;
    assert ( oss . str () == "2004-02-10" );
    b = b - 2000;
    oss . str ("");
    oss << b;
    assert ( oss . str () == "2004-08-13" );
    assert ( b - a == 185 );
    assert ( ( b == a ) == false );
    assert ( ( b != a ) == true );
    assert ( ( b <= a ) == false );
    assert ( ( b < a ) == false );
    assert ( ( b >= a ) == true );
    assert ( ( b > a ) == true );
    assert ( ( c == a ) == true );
    assert ( ( c != a ) == false );
    assert ( ( c <= a ) == true );
    assert ( ( c < a ) == false );
    assert ( ( c >= a ) == true );
    assert ( ( c > a ) == false );
    a = ++c;
    oss . str ( "" );
    oss << a << " " << c;
    assert ( oss . str () == "2004-02-11 2004-02-11" );
    a = --c;
    oss . str ( "" );
    oss << a << " " << c;
    assert ( oss . str () == "2004-02-10 2004-02-10" );
    a = c++;
    oss . str ( "" );
    oss << a << " " << c;
    assert ( oss . str () == "2004-02-10 2004-02-11" );
    a = c--;
    oss . str ( "" );
    oss << a << " " << c;
    assert ( oss . str () == "2004-02-11 2004-02-10" );
    iss . clear ();
    iss . str ( "2015-09-03" );
    assert ( ( iss >> a ) );
    oss . str ("");
    oss << a;
    assert ( oss . str () == "2015-09-03" );
    a = a + 70;
    oss . str ("");
    oss << a;
    assert ( oss . str () == "2015-11-12" );

    CDate d ( 2000, 1, 1 );
    try
    {
        CDate e ( 2000, 32, 1 );
        assert ( "No exception thrown!" == NULL );
    }
    catch ( ... ) { }
    iss . clear ();
    iss . str ( "2000-12-33" );
    assert ( ! ( iss >> d ) );
    oss . str ("");
    oss << d;
    assert ( oss . str () == "2000-01-01" );
    iss . clear ();
    iss . str ( "2000-11-31" );
    assert ( ! ( iss >> d ) );
    oss . str ("");
    oss << d;
    assert ( oss . str () == "2000-01-01" );
    iss . clear ();
    iss . str ( "2000-02-29" );
    assert ( ( iss >> d ) );
    oss . str ("");
    oss << d;
    assert ( oss . str () == "2000-02-29" );
    iss . clear ();
    iss . str ( "2001-02-29" );
    assert ( ! ( iss >> d ) );
    oss . str ("");
    oss << d;
    assert ( oss . str () == "2000-02-29" );

//=============================================================================
// bonus tests
//=============================================================================
    CDate f ( 2000, 5, 12 );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2000-05-12" );
    oss . str ("");
    oss << date_format ( "%Y/%m/%d" ) << f;
    assert ( oss . str () == "2000/05/12" );
    oss . str ("");
    oss << date_format ( "%d.%m.%Y" ) << f;
    assert ( oss . str () == "12.05.2000" );
    oss . str ("");
    oss << date_format ( "%m/%d/%Y" ) << f;
    assert ( oss . str () == "05/12/2000" );
    oss . str ("");
    oss << date_format ( "%Y%m%d" ) << f;
    assert ( oss . str () == "20000512" );
    oss . str ("");
    oss << date_format ( "hello kitty" ) << f;
    assert ( oss . str () == "hello kitty" );
    oss . str ("");
    oss << date_format ( "%d%d%d%d%d%d%m%m%m%Y%Y%Y%%%%%%%%%%" ) << f;
    assert ( oss . str () == "121212121212050505200020002000%%%%%" );
    oss . str ("");
    oss << date_format ( "%Y-%m-%d" ) << f;
    assert ( oss . str () == "2000-05-12" );
    iss . clear ();
    iss . str ( "2001-01-02" );
    assert ( ( iss >> date_format ( "%Y-%m-%d" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2001-01-02" );
    iss . clear ();
    iss . str ( "05.06.2003" );
    assert ( ( iss >> date_format ( "%d.%m.%Y" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2003-06-05" );
    iss . clear ();
    iss . str ( "07/08/2004" );
    assert ( ( iss >> date_format ( "%m/%d/%Y" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2004-07-08" );
    iss . clear ();
    iss . str ( "2002*03*04" );
    assert ( ( iss >> date_format ( "%Y*%m*%d" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2002-03-04" );
    iss . clear ();
    iss . str ( "C++09format10PA22006rulez" );
    assert ( ( iss >> date_format ( "C++%mformat%dPA2%Yrulez" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2006-09-10" );
    iss . clear ();
    iss . str ( "%12%13%2010%" );
    assert ( ( iss >> date_format ( "%%%m%%%d%%%Y%%" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2010-12-13" );

    CDate g ( 2000, 6, 8 );
    iss . clear ();
    iss . str ( "2001-11-33" );
    assert ( ! ( iss >> date_format ( "%Y-%m-%d" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "29.02.2003" );
    assert ( ! ( iss >> date_format ( "%d.%m.%Y" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "14/02/2004" );
    assert ( ! ( iss >> date_format ( "%m/%d/%Y" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "2002-03" );
    assert ( ! ( iss >> date_format ( "%Y-%m" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "hello kitty" );
    assert ( ! ( iss >> date_format ( "hello kitty" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "2005-07-12-07" );
    assert ( ! ( iss >> date_format ( "%Y-%m-%d-%m" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "20000101" );
    assert ( ( iss >> date_format ( "%Y%m%d" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-01-01" );
}
#endif /* __PROGTEST__ */