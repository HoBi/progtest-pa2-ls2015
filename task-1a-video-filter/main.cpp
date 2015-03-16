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

bool filterFile( const char * srcFileName, const char * dstFileName,const char * lang );

bool filterFile(const char *srcFileName, const char *dstFileName, const char *lang) {
    return false;
}

int main()
{
    return 0;
}