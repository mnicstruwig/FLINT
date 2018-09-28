#include <Arduino.h>
#include <dualBufferHandler.h>
#include <SdFat.h>


DualBuffer::DualBuffer(bool * bufferFullflag){
    buffer_size = 0;
    cursor = 0;
    record_to = 'A';
    buffer_to_disk = 'N';
    this->bufferFullFlag = bufferFullflag;
}

void DualBuffer::init_buffers(int size){
    bufferA = new uint8_t[size];
    bufferB = new uint8_t[size];
    buffer_size = size;
}

bool DualBuffer::getBufferFullFlag(){
    return * bufferFullFlag;
}

void DualBuffer::_printOutBuffer(uint8_t * buffer){
    for(int i=0; i<buffer_size;i++){
        Serial.print(i);
        Serial.print(": ");
        Serial.println(buffer[i]);
    }
}

void DualBuffer::setBufferFullFlag(){
   * bufferFullFlag = true; // Must be set to False externally
}

void DualBuffer::printBuffer(char which_buffer){
    if(which_buffer == 'A'){
        Serial.println("BUFFER A:");
        _printOutBuffer(bufferA);
    }
    else if (which_buffer == 'B'){
        Serial.println("Buffer B:");
        _printOutBuffer(bufferB);
    }
    else{
        Serial.println("INCORRECT BUFFER SPECIFIED FOR PRINTOUT");
    }
}

bool DualBuffer::_writeBufferToDisk(File * open_file, uint8_t * buffer){
    int num_bytes_written = open_file->write(buffer, this->buffer_size);
    if(num_bytes_written == this->buffer_size){
        return true;
    }
    else{
        return false;
    }
    open_file->sync();
}

/* Write the correct full buffer to disk.*/
void DualBuffer::writeToDisk(File * open_file){
    if(buffer_to_disk == 'A'){
        if(_writeBufferToDisk(open_file, bufferA)){
            Serial.println("Write successful.");
            buffer_to_disk = 'N';
        }
        else{
            Serial.println("Write failure. Stopping...");
        }
    }
    else if (buffer_to_disk == 'B'){
        if(_writeBufferToDisk(open_file, bufferB)){
            Serial.println("Write succesfful.");
            buffer_to_disk = 'N';
        }
        else{
            Serial.println("Write failure. Stopping ...");
        }
    }
}

void DualBuffer::evaluateBuffers(){
    if(cursor >= buffer_size && record_to == 'A'){
        buffer_to_disk = 'A';
        setBufferFullFlag();
        record_to = 'B';
        cursor = 0;
    }
    else if (cursor >= buffer_size && record_to == 'B'){
        buffer_to_disk = 'B';
        setBufferFullFlag();

        record_to = 'A';
        cursor = 0;
    }
}

void DualBuffer::writeToBuffer(uint8_t * values, int size, uint8_t * buffer){
    for(int i=0; i<size; i++){
        buffer[cursor] = values[i];
        cursor = cursor + 1;
    }
}

void DualBuffer::recordValues(uint8_t * values, int size){
    if(record_to == 'A'){
        writeToBuffer(values, size, bufferA);
        evaluateBuffers();
    }
    else if(record_to == 'B'){
        writeToBuffer(values, size, bufferB);
        evaluateBuffers();
    }
}