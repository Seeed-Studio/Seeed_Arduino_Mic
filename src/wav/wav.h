

struct soundhdr {
char  riff[4];        /* "RIFF"                                  */
long  flength;        /* file length in bytes                    */
char  wave[4];        /* "WAVE"                                  */
char  fmt[4];         /* "fmt "                                  */
long  chunk_size;     /* size of FMT chunk in bytes (usually 16) */
short format_tag;     /* 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM */
short num_chans;      /* 1=mono, 2=stereo                        */
long  srate;          /* Sampling rate in samples per second     */
long  bytes_per_sec;  /* bytes per second = srate*bytes_per_samp */
short bytes_per_samp; /* 2=16-bit mono, 4=16-bit stereo          */
short bits_per_samp;  /* Number of bits per sample               */
char  data[4];        /* "data"                                  */
long  dlength;        /* data length in bytes (filelength - 44)  */
} wavh;
