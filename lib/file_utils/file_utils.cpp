#include <SdFat.h>
#include <Arduino.h>

/* Get the number at the end of a string*/
int get_file_number(char * filename){
    char * num;
    int val = 0;

    strtok(filename, "_."); // We don't need the filename, just the number.
    num = strtok(NULL, "_."); // Get the number from the next split.

    // If a number exists, extract it.
    if(num != NULL){
        val = atoi(num);
    }
    return val;
}

void createFileName(char * baseFilename, int filenumber, char * newFilename){
    char str_filenumber[10];
    itoa(filenumber, str_filenumber, 10);
    strcpy(newFilename, baseFilename);

    if(filenumber < 10){
       strcat(newFilename, "0"); // Append a zero if single-digit file number
    }

    // Format new filename
    strcat(newFilename, str_filenumber);
    strcat(newFilename, ".bin");
}

void setNextFilename(SdFatSdioEX sdEx, char * baseFilename, int filenumber, char * newFilename){
    File current_file;
    char name[20];
    int highest_filenumber = 0;
    int current_filenumber = 0;

    sdEx.vwd()->rewind(); // Beginning of directory

    // Scan through directory and find the highest file number.
    while(current_file.openNext(sdEx.vwd(), O_READ)){
        current_file.getName(name, 20);
        current_filenumber = get_file_number(name);

        if(current_filenumber > highest_filenumber){
            highest_filenumber = current_filenumber;
        }

        current_file.close(); // Close and continue
    }

    highest_filenumber++;

    createFileName(baseFilename, highest_filenumber, newFilename);
}