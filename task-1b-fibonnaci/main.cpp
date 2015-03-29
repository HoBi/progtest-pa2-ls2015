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

bool debug = true;

int byteCountSigns[] = { 0x0, 0x0, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
int bitmasks[] = { 0x0, 0xFF, 0x3F, 0x1F, 0xF, 0x7, 0x3 };
int numOfBitsInUtf8Char[] = { 0, 7, 11, 16, 21, 26, 31 };
long long int utf8BitLengths[] = { 0, 0x80, 0x800, 0x10000, 0x200000, 0x4000000, 0x7FFFFFFF };
long long int fibonacci[] = { 0x1, 0x2, 0x3, 0x5, 0x8, 0xd, 0x15, 0x22, 0x37, 0x59, 0x90, 0xe9, 0x179, 0x262, 0x3db, 0x63d,
        0xa18, 0x1055, 0x1a6d, 0x2ac2, 0x452f, 0x6ff1, 0xb520, 0x12511, 0x1da31, 0x2ff42, 0x4d973, 0x7d8b5, 0xcb228, 0x148add };

/**
* Opens a file for reading and writing in binary mode
* @param *filename      name of file to open
* @param *fileStream    output var for opened fileStream
* @param *fileLength    length of file in bytes
*/
bool openFiles( const char *inFilename, fstream *inFileStream, const char *outFilename, fstream *outFileStream )
{
   // Open file for reading
   (*inFileStream).open( inFilename, ios::in | ios::binary );

   // Is file ok?
   if ( (*inFileStream).fail() )
      return false;

   // Open file for writing
   (*outFileStream).open( outFilename, ios::out | ios::binary | ios::trunc );

   // Is file ok?
   if ( (*outFileStream).fail() )
      return false;

   return true;
}

/**
*  Read byte and tries to decode an utf-8 character
*  @param nextByte            byte for processing
*  @param *character          output variable of resulting character (but return value must be 1)
*  @param *lastCharBytesLeft  bytes left for processing of actual character
*  @param *lastCharBytesTotal total count of bytes of actual processed character
*/
int decodeUtf8( unsigned char nextByte, long long int *character, int *charBytesLeft, int *charBytesTotal )
{
   // Character start
   if ( *charBytesLeft == 0 )
   {

      // Determine the length of character
      for ( int o = 6; o > 0; o-- )
         if ( nextByte >= byteCountSigns[ o ] )
         {

            *charBytesLeft = o;
            *charBytesTotal = *charBytesLeft;
             break;
         }
   }

   if ( *charBytesLeft == *charBytesTotal ) // Initial byte?
   {
      if ( ( nextByte & 0xC0 ) == 0x80 ) return -1;
      *character = ( ( nextByte & bitmasks[ *charBytesTotal ] ) << ( 6 * ( *charBytesLeft - 1 ) ) );
   }
   else // Other char bytes...
   {
      if ( ( nextByte & 0xC0 ) != 0x80 ) return -1;
      *character += ( nextByte & 0x3F ) << ( 6 * ( *charBytesLeft - 1 ) );
   }

   (*charBytesLeft)--;

   // Character is decoded
   if ( *charBytesLeft == 0 )
      return *charBytesTotal == -1 ? false : true;

   return 0;
}

void printBuffer( char const * name, bool * writeBuffer, int writeBufferPos )
{
   cout << " [ " << name << " " << dec << setw(4) << setfill(' ') << writeBufferPos << " ] : ";
   for ( int i = 0; i < 48; i++) cout << (writeBuffer[ i ] ? "1" : "0") << ( i+1 == writeBufferPos ? "| " : ", " ) << ( i > 0 && (i+1) % 8 == 0 ? "   " : "" );
   cout << endl;
}

bool writeBufferToFile( fstream * file, bool * writeBuffer, int * writeBufferPos )
{
   unsigned char byteToWrite;

   while ( *writeBufferPos >= 8 )
   {
      // Construct number
      byteToWrite = 0;
      for ( int i = 7; i >= 0; i-- )
      {
         byteToWrite <<= 1;
         byteToWrite += writeBuffer[i] ? 1 : 0;
      }

      if ( debug ) cout << " # Written: " << dec << (int)byteToWrite << endl;

      // Write to file
      // TODO: osetrit zapis
      (*file).put( byteToWrite );

      // Shift array
      for ( int i = 8; i < 128; i++ )
      {
         writeBuffer[ i - 8 ] = writeBuffer[ i ];
      }
      *writeBufferPos -= 8;

      return true;
   }

   return true;
}

bool emptyBuffer( fstream * file, bool * writeBuffer, int * writeBufferPos )
{
   while ( *writeBufferPos < 8 )
   {
      writeBuffer[ *writeBufferPos ] = false;
      (*writeBufferPos)++;
   }

   if ( ! writeBufferToFile( file, writeBuffer, writeBufferPos )) return false;

   if ( *writeBufferPos > 0 )  return emptyBuffer( file, writeBuffer, writeBufferPos ); else return true;
}

bool toFibonacciAndWrite( long long int character, bool * writeBuffer, int * writeBufferPos, fstream * file )
{
   long long int charLeft = character + 1;
   bool foundMsb = false;
   int writeoffset = *writeBufferPos;

   // TODO: ostranit printy bufferu

   // Find Fibonacci code
   for ( int i = 29; i >= 0; i-- ) {
      if ( fibonacci[ i ] <= charLeft )
      {
         foundMsb = true;
      }

      if ( foundMsb ) {
         writeBuffer[ i + writeoffset ] = fibonacci[ i ] <= charLeft;
         (*writeBufferPos)++;
      }

      if ( fibonacci[ i ] <= charLeft )
      {
         charLeft -= fibonacci[ i ];
      }
   }

   // Add extra one to end
   writeBuffer[ *writeBufferPos ] = true;
   (*writeBufferPos)++;


   printBuffer( "fb", writeBuffer, *writeBufferPos );

   // Write bytes
   writeBufferToFile( file, writeBuffer, writeBufferPos );

   printBuffer( "aw", writeBuffer, *writeBufferPos );

   return true;
}

bool Utf8ToFibonacci( const char * inFile, const char * outFile )
{
   int charBytesLeft = 0, charBytesTotal = -1, writeBufferPos = 0;
   long long int character;
   fstream inFileStream, outFileStream;
   char byte;
   bool *writeBuffer = new bool[128];

   // Open a file for reading
   if ( ! openFiles( inFile, &inFileStream, outFile, &outFileStream ) )
   {
      if ( debug ) cout << " # Error while opening file " << inFile << endl;
      return false;
   }

   // Read all bytes from file
   while ( inFileStream.get( byte ) )
   {
      // Decode one utf-8 character
      int decodingResult = decodeUtf8( (unsigned char)byte, &character, &charBytesLeft, &charBytesTotal );

      if ( decodingResult == 1 ) // Found a char
      {
         if ( debug ) cout << " # Read: " << dec << (int)character << endl;
         // Check if character is in utf8 range
         if ( character < 0x0 || character > 0x10ffff ) return false;

         // Convert to Fib and write to file
         if ( ! toFibonacciAndWrite( character, writeBuffer, &writeBufferPos, &outFileStream ) ) return false;

         character = 0x0;
      }
      else if ( decodingResult == -1 ) // Error while decoding
      {
         if ( debug ) cout << " # Failed decoding: " << hex << byte << endl;
         return false;
      }
   }

   // All bytes decoded?
   if ( charBytesLeft > 0 ) return false;

   // Fill rest of bytes with zeroes and write
   emptyBuffer( &outFileStream, writeBuffer, &writeBufferPos );

   // Make sure that everything went ok
   if ( ! inFileStream.eof() ) {
      if ( debug ) cout << " # Error while reading " << endl;
      inFileStream.close();
      outFileStream.close();
      return false;
   }
   inFileStream.close(); outFileStream.close(); return true;
}





// ====================================================================================================================












long long int getCharacterFromBuffer( bool *bitsBuffer, int *bitsBufferPos, bool ** charBits, int * charBitsLength )
{
   long long int character = 0;

   for ( int i = 0; i < *bitsBufferPos; i++ )
   {
      character += bitsBuffer[i] ? fibonacci[i] : 0;
   }

   character--;

   (*bitsBufferPos) = 0;

   (*charBits) = new bool[48];
   for ( int i = 6; i >= 0; i-- ) if ( utf8BitLengths[i] <= character ) { (*charBitsLength) = (i+1)*8; break; }

   for (int i=0; i < (*charBitsLength); i++) {
      ( *charBits )[ i ] = ( character & ( 1 << i ) ) != 0;
   }

   return character;
}

bool toUtf8AndWrite( bool * charBits, int bitsLength, fstream *file )
{
   int bytes[] = { 0, 0, 0, 0, 0, 0 };
   int bytesCount = bitsLength/8;
   int numBitsInFirstB[] = { 0, 7, 5, 4, 3, 2, 1 };

   // Construct bytes
   for ( int i = 0; i < bytesCount; i++ )
   {
      // First byte
      if ( i == 0 )
      {
         for ( int o = numOfBitsInUtf8Char[bytesCount]-1; o >= numOfBitsInUtf8Char[bytesCount] - numBitsInFirstB[bytesCount]; o-- )
         {
            bytes[i] <<= 1;
            bytes[i] += charBits[o] ? 1 : 0;
         }
         bytes[i] += byteCountSigns[ bytesCount ];
      }
      // Other bytes
      else
      {
         int starts = numOfBitsInUtf8Char[bytesCount] - 1 - numBitsInFirstB[bytesCount] - ((i-1)*6);
         int ends = starts - 5; if (ends < 0) ends = 0;
         for ( int o = starts ; o >= ends ; o-- )
         {
            bytes[i] <<= 1;
            bytes[i] += charBits[o];
         }
         bytes[i] += 0x80;
      }
   }

   // Write bytes to file
   for ( int i = 0; i < bytesCount; i++ )
   {
      if ( ! (*file).put( (unsigned char)bytes[i] ) ) return false;
      if ( debug ) cout << " # |- Written: " << (int)bytes[i] << endl;
   }

   return true;
}

/**
*  Read byte and tries to decode an utf-8 character
*  @param nextByte            byte for processing
*  @param *character          output variable of resulting character (but return value must be 1)
*  @param *lastCharBytesLeft  bytes left for processing of actual character
*  @param *lastCharBytesTotal total count of bytes of actual processed character
*/
int decodeFibo( unsigned char nextByte, bool *bitsBuffer, int *bitsBufferPos, fstream *file )
{
   // Convert uns. char to bits array
   bool *bits = new bool[8];

   for ( int i=0; i < 8; i++ )
      bits[ i ] = (nextByte & (1<<i)) != 0;

   for ( int i = 0; i < 8; i++ )
   {
      bitsBuffer[ *bitsBufferPos ] = bits[ i ];

      if ( *bitsBufferPos != 0 && bitsBuffer[ *bitsBufferPos ] == true && bitsBuffer[ *bitsBufferPos - 1 ] == true )
      {
         bool *bits; int charBitsLength;
         long long int character = getCharacterFromBuffer( bitsBuffer, bitsBufferPos, &bits, &charBitsLength );

         // Check if character is in utf8 range
         if ( character < 0x0 || character > 0x10ffff ) return false;

         if ( debug ) cout << " # Read: " << (int)character << endl;
         if ( ! toUtf8AndWrite( bits, charBitsLength, file ) ) return false;
      }
      else (*bitsBufferPos)++;
   }

   return true;

}

bool FibonacciToUtf8( const char * inFile, const char * outFile )
{
   int writeBufferPos = 0;
   fstream inFileStream, outFileStream;
   char byte;
   bool *writeBuffer = new bool[48];

   // Open a file for reading
   if ( ! openFiles( inFile, &inFileStream, outFile, &outFileStream ) )
   {
      if ( debug ) cout << " # Error while opening file " << inFile << endl;
      return false;
   }

   // Read all bytes from file
   while ( inFileStream.get( byte ) )
   {
      // Decode character and immediately write to file
      if ( ! decodeFibo( (unsigned char)byte, writeBuffer, &writeBufferPos, &outFileStream ) ) return false;
   }

   // Last byte is 0 ?
   if ( byte == 0 ) return false;

   // Make sure that everything went ok
   if ( ! inFileStream.eof() ) {
      if ( debug ) cout << " # Error while reading " << endl;
      inFileStream.close();
      outFileStream.close();
      return false;
   }
   inFileStream.close(); outFileStream.close(); return true;
}

#ifndef __PROGTEST__
int main ( int argc, char * argv [] )
{
   cout << "test : " << ( FibonacciToUtf8( "test_4.fib", "result.txt" ) ? "ok" : "nok" ) << endl;

   //FibonacciToUtf8( "test_1.fib", "result-file.txt" );

   return 0;
}
#endif /* __PROGTEST__ */