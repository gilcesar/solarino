/*
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

#ifndef Storage_h
#define Storage_h

#include <SPI.h>
#include <SD.h>

void initStorage()
{
    Serial.print("Initializing SD card...");

    if (!SD.begin(4))
    {
        Serial.println("initialization failed!");
        while (1);//?
    }
    Serial.println("initialization done.");
}

File openFile(const char * path){
    File f = SD.open(path, FILE_READ);
    
}

void closeFile(File f){
    f.close();
}

#endif