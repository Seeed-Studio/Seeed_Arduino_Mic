#include "wav.h"
/*
void create_template(file sFile)
{
    strncpy(wavh.riff,"RIFF", 4);
    strncpy(wavh.wave,"WAVE", 4);
    strncpy(wavh.fmt,"fmt ", 4);
    strncpy(wavh.data,"data", 4);
    
    // size of FMT chunk in bytes
    wavh.chunk_size = 16;
    wavh.format_tag = 1; // PCM
    wavh.num_chans = 1; // mono
    wavh.srate = 16000;
    wavh.bytes_per_sec = (16000 * 1 * 16 * 1)/8;
    wavh.bytes_per_samp = 2;
    wavh.bits_per_samp = 16;
    wavh.dlength = 16000 * 2 *  1 * 16/2;
    
    sFile.seek(0);
    sFile.write((byte *)&wavh, 44);
}

    void finalize_template(file sFile)
{
    unsigned long fSize = sFile.size()-8;
    sFile.seek(4); 
    byte data[4] = {lowByte(fSize), highByte(fSize), fSize >> 16, fSize >> 24};
    sFile.write(data,4);
    byte tmp;
    sFile.seek(40);
    fSize = fSize - 36;
    data[0] = lowByte(fSize);
    data[1]= highByte(fSize);
    data[2]= fSize >> 16;
    data[3]= fSize >> 24;
    sFile.close();
}
 */