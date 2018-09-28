#include <SPI.h>
#include <Arduino.h>
#include <IntervalTimer.h>
#include <SdFat.h>
#include <accel.h>
#include <file_utils.h>
#include <dualBufferHandler.h>

#define BUFFER_SIZE 4092
bool myFlag = false;
DualBuffer dualbuffer(&myFlag);
int num_written = 0;
uint8_t accValues[6]; // Holds accelerometer values read from ADXL345
int16_t x,y,z;
double xg, yg, zg;
const int read_period = 100000; //625; // sampling rate in microseconds
IntervalTimer timer0;
int CS = 10;
SPISettings adxl345spi(4000000, MSBFIRST, SPI_MODE3);
ADXL345_Sensor adxl345(CS, ADXL345_RANGE_4_G, ADXL345_DATARATE_1600_HZ, adxl345spi);

SdFatSdioEX sdEx;
File file; // File to write to


int16_t combine_bytes(unsigned char lsb, unsigned char msb){
  return (msb << 8 ) | (lsb);
}


void timer0_callback(void){
  adxl345.readAccel(accValues); // Get raw accelerometer readings
  dualbuffer.recordValues(accValues, 6);
}

/*Get serial input from the user.*/
char get_user_input(){
  if(Serial.available() > 0){
    return Serial.read();
  }
  return 0;
}

/*Set-up*/
void setup(void) 
{
 //Set-up SPI first
  SPI.begin();
  adxl345.setUpSensor();
  delay(1000);

  Serial.println("Initializing file system...");
  if(!sdEx.begin()){
    Serial.println("Failed.");
  }

  sdEx.chvol();
  dualbuffer.init_buffers(BUFFER_SIZE);
  Serial.print("Buffers initiated with size: ");
  Serial.println(dualbuffer.buffer_size);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Setup complete!");
}

void loop(void) 
{

  char user_input = get_user_input();


  if (user_input == 'b'){
    char *  test_filename = "testing_002.txt";
    file.open(test_filename, O_CREAT | O_WRITE);
    timer0.begin(timer0_callback, read_period);
    delayMicroseconds(10);
    Serial.println("Timer started...");
  }
  else if(user_input == 's'){
    Serial.println("Stopping...");
    timer0.end();
    if(file.isOpen()){
      file.close();
      Serial.println("File closed.");
    }
  }

  if(myFlag){
    Serial.println("=====");
    dualbuffer.printBuffer('A');
    Serial.println("=====");
    Serial.print("Buffer full: ");
    Serial.println(dualbuffer.buffer_to_disk);
    Serial.println("Writing to disk...");
    dualbuffer.writeToDisk(&file);
    Serial.println(dualbuffer.buffer_to_disk);
    myFlag = false;
}

}