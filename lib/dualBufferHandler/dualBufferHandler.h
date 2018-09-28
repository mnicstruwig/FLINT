#include <Arduino.h>
#include <SdFat.h>

class DualBuffer{
    public:
        int buffer_size;
        int cursor;
        char record_to;
        char buffer_to_disk;
        uint8_t * bufferA;
        uint8_t * bufferB;
        bool * bufferFullFlag;
        DualBuffer(bool * bufferFullFlag);
        void init_buffers(int size);
        void writeToBuffer(uint8_t * values, int size, uint8_t * buffer);
        void recordValues(uint8_t * values, int size);
        void printBuffer(char which_buffer);
        void evaluateBuffers();
        void setBufferFullFlag();
        bool getBufferFullFlag();
        void writeToDisk(File * open_file);

    private:
        void _printOutBuffer(uint8_t * buffer);
        bool _writeBufferToDisk(File * open_file, uint8_t * buffer);
};
