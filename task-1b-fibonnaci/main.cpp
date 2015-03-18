#include <cmath>
#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <iomanip>
using namespace std;
#endif /* __PROGTEST__ */

/*
long long int *decodeUtf8( int *byteArray );
int *convertUtf8ToFibonacci( long long int *utf8String );

long long int *decodeFibonacci( int *byteArray );

int *toggleBytesSignificancy( int *bytes );

int *readBytesFromFile( const char *filename );
bool writeBytesToFile( const char * filename, int *content );

bool Utf8ToFibonacci( const char *inFile, const char *outFile );
bool FibonacciToUtf8( const char *inFile, const char *outFile );

void dump( const int *bytes );*/

/**
*  Reads specified file into 1-byte array
*/
bool readBytesFromFile( const char *filename, int **byteArray, int *fileLength )
{
   // Open file for reading
   fstream fs;
   fs.open( "testfile.utf8", ios::in | ios::out | ios::binary );

   // Is file ok?
   if ( fs.fail() )
      return false;

   // Get file size
   fs.seekg( 0, fs.end );
   *fileLength = (int)fs.tellg();
   fs.seekg( 0, fs.beg );

   // Allocate memory
   *byteArray = new int[ *fileLength + 1 ];

   char c;
   while ( fs.get( c ) ) {
      (*byteArray)[ (int)fs.tellg() - 1 ] = (int)(unsigned char)c;
   }

   // Everything ok?
   if ( ! fs.eof() )
   {
      fs.close(); return false;
   }

   fs.close(); return true;
}

/**
*  Reads the 1-byte array and decode from UTF-8, saves to 4-byte array per-character
*/
bool decodeUtf8( int *byteArray, long long int *utf8CharArray, int length )
{
   int charBytesLeft = 0, charBytesTotal = 0, charPos = -1;
   int byteCountSigns[] = { 0x0, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
   int bitmasks[] = { 0x0, 0x7F, 0x1F, 0xF, 0x7, 0x3, 0x1 };

   for ( int i = 0; i < length; i++ )
   {
      // Character start
      if ( charBytesLeft == 0 )
      {
         // Determine the length of character
         for ( int o = 6; o > 0; o-- )
            if ( byteArray[i] >= byteCountSigns[ o - 1 ] ) { charBytesLeft = o; break; }

         charBytesTotal = charBytesLeft; charPos++;
      }

      // Initial byte?
      if ( charBytesLeft == charBytesTotal )
         utf8CharArray[ charPos ] = ( byteArray[ i ] & bitmasks[ charBytesTotal ] ) << ( 6 * ( charBytesLeft - 1 ) );
      else
      // Other char bytes...
         utf8CharArray[ charPos ] += ( byteArray[ i ] & 0x3F ) << ( 6 * ( charBytesLeft - 1 ) );

      charBytesLeft--;

      // TODO: Dynamic array allocation

   }

   return true;
}

void dump( const int *bytes, int length = -1 )
{
   char byte;

   for ( int i = 0; i < ( length >= 0 ? length : sizeof(bytes) / sizeof(*bytes) ); i++ )
   {
      if ( i % 16 == 0 )
         cout << hex << setw( 8 ) << setfill( '0' ) << i;

      cout << " " << hex << setw( 2 ) << (int)(unsigned char)bytes[i];

      if ( i % 16 == 15 )
         cout << endl;
   }

   cout << endl << endl;
}

void dump( const long long int *utf8Bytes, int length = -1 )
{
   char byte;

   for ( int i = 0; i < ( length >= 0 ? length : sizeof(utf8Bytes) / sizeof(*utf8Bytes) ); i++ )
   {
      cout << hex << setw( 4 ) << setfill('0') << utf8Bytes[i] << " ";

      if ( i % 4 == 3 )
         cout << endl;
   }

   cout << endl;
}

bool Utf8ToFibonacci( const char * inFile, const char * outFile )
{
   return false;
}

bool FibonacciToUtf8( const char * inFile, const char * outFile )
{
 // todo
}

#ifndef __PROGTEST__
int main ( int argc, char * argv [] )
{
   int *data = new int[128], fileLength = 0;
   long long int *utf8Data = new long long int[65535];

   readBytesFromFile( "testfile.utf8", &data, &fileLength );

   dump( data, fileLength );

   decodeUtf8( data, utf8Data, fileLength );


   dump( utf8Data, 64 );

   return 0;
}
#endif /* __PROGTEST__ */