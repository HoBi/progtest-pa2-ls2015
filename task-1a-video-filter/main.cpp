#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdint.h>

using namespace std;

const uint32_t BIG_ENDIAN_ID    = 0x4d6d6d4d;
const uint32_t LITTLE_ENDIAN_ID = 0x49696949;
const uint32_t FRAME_ID         = 0x46727246;
const uint32_t VIDEO_DATA_ID    = 0x56696956;
const uint32_t AUDIO_LIST_ID    = 0x416c6c41;
const uint32_t AUDIO_DATA_ID    = 0x41757541;

#endif /* __PROGTEST__ */

/**
* Nacte ze souboru startovaci hlavicku
*/
bool readHeader ( fstream & file )
{
    char buffer[8];
    file.read( buffer, 8 );

    for ( int i = 0; i < 8; i ++ )
    {
        cout << buffer[i] << " ";
    }

    return false;
}

/**
* Cte ze souboru jeden frame
*/
bool readFrame( fstream & source, int offset)
{
    return false;
}

/**
* Otevre soubor a vyfiltruje specifikovany jazyk
*/
bool filterFile(const char *srcFileName, const char *dstFileName, const char *lang)
{
    /* Operace se souborem */
    fstream file;
    file.open( srcFileName, ios::binary );
    if ( file.fail() ) { cout << "Soubor " << srcFileName << " neexistuje" << endl; return false; }

    /* Zpracování hlavièky */
    readHeader( file );

    file.close();

    return true;
}

int main()
{
    cout << "=== task-1a-video-filter === " << endl << endl;

    filterFile( "input.bin", "output.bin", "cs" );

    return 0;
}