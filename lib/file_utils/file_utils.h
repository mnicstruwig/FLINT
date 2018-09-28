#include <SdFat.h>
int get_file_number(char * filename);
void createFileName(char * baseFilename, int filenumber, char * newFilename);
void setNextFilename(SdFatSdioEX sdEx);
bool writeBufferToSd(File open_file, int16_t * buffer, int buffer_size);