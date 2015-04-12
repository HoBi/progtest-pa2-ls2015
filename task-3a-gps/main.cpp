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

#define M_PI           3.14159265358979323846
#define degtorad(deg) ((deg) * M_PI / 180.0)
#define radtodeg(rad) ((rad) * 180.0 / M_PI)

class WrongParamsException {};
class NonExistingIndexException {};
class WrongFormatException {};

/**
 * @class CCoord
 */
class CCoord
{
public:
    double lat, lon, latDeg, latMin, latSec, lonDeg, lonMin, lonSec;
    bool latOri, lonOri;

    CCoord( double lat, double lon )
    {
        this->lat = lat;
        this->lon = lon;

        convertDecimalToComponental( lat, &latDeg, &latMin, &latSec );
        convertDecimalToComponental( lon, &lonDeg, &lonMin, &lonSec );

        checkRangeValidity( lat, lon );

        this->latOri = !signbit( lat );
        this->lonOri = !signbit( lon );
    }

    CCoord( const char * latloncoord )
    {
        char sepLat[256], sepLon[256];
        separateCoords( latloncoord, sepLat, sepLon );

        string ssLat = sepLat, ssLon = sepLon;

        this->lat = parseCoordString( ssLat, &latDeg, &latMin, &latSec );
        this->lon = parseCoordString( ssLon, &lonDeg, &lonMin, &lonSec );

        checkRangeValidity( lat, lon );

        this->latOri = !signbit( lat );
        this->lonOri = !signbit( lon );
    }

    CCoord( const char * latcoord, const char * loncoord )
    {
        this->lat = parseCoordString( latcoord, &latDeg, &latMin, &latSec );
        this->lon = parseCoordString( loncoord, &lonDeg, &lonMin, &lonSec );

        checkRangeValidity( lat, lon );

        this->latOri = !signbit( lat );
        this->lonOri = !signbit( lon );
    }

    friend ostream & operator<<( ostream &os, const CCoord &coord )
    {
        os << "(";
        os << setprecision( coord.latSec > 10 ? 5 : 4 );
        os << coord.latDeg << " " << coord.latMin << "'" << coord.latSec << '"' << ( coord.latOri ? "N" : "S" ) << ", ";
        os << setprecision( coord.lonSec > 10 ? 5 : 4 );
        os << coord.lonDeg << " " << coord.lonMin << "'" << coord.lonSec << '"' << ( coord.lonOri ? "E" : "W" );
        os << ")";

        return os;
    }

    /**
     * Converts degrees value from decimal to componental ( deg, min, sec ) format
     */
    void convertDecimalToComponental( double decimalDegrees, double * degrees, double * minutes, double * seconds )
    {
        decimalDegrees = fabs( decimalDegrees );

        *degrees = (int)decimalDegrees;
        *minutes = (int)(( decimalDegrees - *degrees ) * 60.0);
        *seconds = ( decimalDegrees - *degrees - ( *minutes / 60.0 ) ) * 3600.0;

        if ( *seconds >= 60 || fabs( *seconds - 60 ) < 0.00001 ) { (*minutes)++; *seconds = 0; }
        if ( *minutes >= 60 || fabs( *minutes - 60 ) < 0.00001  ) { (*degrees)++; *minutes = 0; }
    }

    /**
     * Converts degrees value from componental format to decimal number
     */
    static double convertComponentalToDecimal( double degrees, double minutes, double seconds )
    {
        return degrees + ( minutes / 60.0 ) + ( seconds / 3600.0 );
    }

private:
    /**
     * Separates coordinates in format ( XXX, XXX ) into two strings
     */
    void separateCoords( string coords, char * oLat, char * oLon )
    {
        istringstream ss;
        ss.str( coords );

        char dum = ' ';
        ss >> dum;
        if (dum != '(') throw WrongFormatException();
        ss.getline( oLat, 256, ',' );
        ss.getline( oLon, 256, ')' );
    }

    /**
     * Parse coordinates to decimal and componental values.
     * Input format can be XX.XXX N/S or XX XX'XX.XXX" E/W ( various float point lengths )
     */
    double parseCoordString( const string & coostring, double * rcd = NULL, double * rcm = NULL, double * rcs = NULL )
    {
        unsigned char orientation = 'X';
        double compd, compm, comps, degrees;
        string newString = coostring;

        // Get rid of "E" sticked to number
        size_t efound = newString.rfind( 'E' );
        if ( efound != string::npos )
            newString.replace( efound, 1, " E" );

        char const * coord = newString.c_str();

        if ( sscanf( coord, " %lf %lf'%lf\" %c", &compd, &compm, &comps, &orientation ) != 4 )
        {
            if ( sscanf( coord, "%lf %c", &degrees, &orientation ) != 2 ) throw WrongFormatException();
            else
            {
                // Search for '.' in coordinate
                string hstck = coord; if (
                        hstck.find( '.' ) == string::npos ||
                                hstck.find( ". " ) != string::npos || hstck.find( ".N" ) != string::npos ||
                                hstck.find( ".S" ) != string::npos || hstck.find( ".E" ) != string::npos ||
                                hstck.find( ".W" ) != string::npos
                        ) throw WrongFormatException();

                // Format: XX.XXXX O
                if ( rcd != NULL && rcm != NULL && rcs != NULL )
                {
                    convertDecimalToComponental( degrees, rcd, rcm, rcs );
                }
                return ( orientation == 'S' || orientation == 'W' ? -1 : 1) * degrees;
            }
        }
        else
        {
            // Format: XX XX'XX.XX" O

            if ( compd < 0 || compm < 0 || comps < 0 ) throw WrongFormatException();

            if ( rcd != NULL && rcm != NULL && rcs != NULL )
            {
                *rcd = compd; *rcm = compm; *rcs = comps;
            }
            return ( orientation == 'S' || orientation == 'W' ? -1 : 1) * convertComponentalToDecimal( compd, compm, comps );
        }
    }

    /**
     * Checks if latitude and longitude values are in geoloc range
     */
    void checkRangeValidity( double lat, double lon )
    {
        if ( lat < -90 || lat > 90 || lon < -180 || lon > 180 ) throw WrongFormatException();
    }
};

/**
 * @class CGPS
 */
class CGPS
{
public:

    /**
     * Adds new coordinates to GPS log
     */
    CGPS& Add( const CCoord coord )
    {
        log.insert( log.end(), coord );
        return *this;
    }

    /**
     * Adds new coordinates to GPS log
     */
    CGPS& Add( const char * latcoord, const char * loncoord )
    {
        return Add( CCoord( latcoord, loncoord ) );
    }

    /**
     * Adds new coordinates to GPS log
     */
    CGPS& Add( const char * latloncoord )
    {
        return Add( CCoord( latloncoord ) );
    }

    CGPS& operator+=( const CCoord &coord )
    {
        return Add( coord );
    }

    CGPS& operator+=( const char * latloncoord )
    {
        return Add( latloncoord );
    }

    CCoord operator[]( int index ) const
    {
        if ( index < 0 || index > (int)log.size() ) throw NonExistingIndexException();
        return log.at( index );
    }

    CGPS operator()( int startIndex, int endIndex ) const
    {
        CGPS ng = CGPS();

        if ( startIndex < 0 || endIndex > (int)log.size() || endIndex < startIndex )
            throw NonExistingIndexException();

        for ( int i = startIndex; i <= endIndex; i++ )
            ng.Add( log.at( i ) );

        return ng;
    }

    friend ostream& operator<<( ostream& os, const CGPS& obj )
    {
        os << "[";
        int i = 0;

        for ( CCoord coo : obj.getLog() )
        {
            if ( i != 0 ) os << " > ";
            os << coo;
            i++;
        }

        os << "]";

        return os;
    }

    friend istream& operator>>( istream& is, CGPS& obj )
    {
        char dum = ' ';
        CGPS newInst = CGPS();

        // [
        is >> dum; if ( dum != '[' ) { is.setstate( ios::failbit ); return is; }

        while ( !is.eof() )
        {
            char lats[256], lons[256];

            if ( dum == ']' ) break; // tady to muze bejt divny

            is >> dum; if ( dum != '(' ) { is.setstate( ios::failbit ); return is; }        // (
            is.getline( lats, 256, ',' );                                                   //  XXX.XXX X,
            is.getline( lons, 256, ')' );                                                   //             XXX.XXX X)

            is >> dum;
            if ( dum != ']' && dum != '>' ) { is.setstate( ios::failbit ); return is; }

            try { newInst.Add( lats, lons ); } catch ( WrongFormatException ex ) { is.setstate( ios::failbit ); return is; }
        }

        // ]
        if ( dum != ']' ) { is.setstate( ios::failbit ); return is; }

        obj = newInst;

        return is;
    }

    /**
     * Returns distance between each point in GPS log
     */
    double Distance() const
    {
        double distance = 0;

        for ( unsigned int i = 0; i < log.size() - 1; i++ )
            distance += twoPointsDistance( log.at( i ), log.at( i+1 ) );

        return distance;
    }

    vector<CCoord> getLog() const
    {
        return log;
    }

private:
    vector<CCoord> log;

    /**
     * Calculates distance between two coordinates
     */
    double twoPointsDistance( CCoord from, CCoord to ) const
    {
        double earthRadius = 6371000;

        double dLat = degtorad( to.lat - from.lat );
        double dLon = degtorad( to.lon - from.lon );
        double lat1 = degtorad( from.lat );
        double lat2 = degtorad( to.lat );

        double a = sin( dLat / 2.0 ) * sin( dLat / 2.0) + sin( dLon / 2.0 ) * sin( dLon / 2.0 ) *
                    cos( lat1 ) * cos( lat2 );
        double c = 2.0 * atan2( sqrt( a ), sqrt( 1 - a ) );
        double d = earthRadius * c;

        return d;
    }
};

#ifndef __PROGTEST__
int main( void )
{


    return 0;
}
#endif /* __PROGTEST__ */