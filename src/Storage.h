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

const int chipSelect = 53; //Mega
//const int chipSelect = ;//Nano

File root;
void printDirectory(File dir, int numTabs)
{
    while (true)
    {
        //Serial.println("root");
        File entry = dir.openNextFile();
        if (!entry)
        {
            Serial.println("no more files");
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            // files have sizes, directories do not
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}

void initStorage()
{
    Serial.println("Initializing SD card...");

    if (!SD.begin(chipSelect))
    {
        Serial.println("initialization failed!");
        //delay(100); //?
    }
    Serial.println("initialization done.");

    root = SD.open("/");

    printDirectory(root, 0);
}

File openFile(const char * path){
    File f = SD.open(path, FILE_READ);
    
}

void closeFile(File f){
    f.close();
}

/* int readFile(const char *fileName, const char * buf)
{
    String res = "";
    //uint8_t buf[128] = {0};
    size_t len = 0;
    File f = SD.open(fileName);
    if (f)
    {
        Serial.print("File name = ");
        Serial.println(fileName);
        Serial.print("File size = ");
        Serial.println(f.size());
        while ((len = f.readBytes(buf, sizeof(buf))) > 0)
        {
            //Serial.print((const char *)buf);
            res += (const char *)buf;
        }
        f.close();
    }
    else
    {
        Serial.print("Erro abrindo arquivo ");
        Serial.println(fileName);
    }
    //closeFile(f);
    return res;
} */

void testIndex()
{
   // readFile("/SOLARINO/INDEX~1.HTM");
    //readFile("/SOLARINO/INDEX~1.HTM");
    /* File myFile = SD.open("/SOLARINO/INDEX~1.HTM");
  if (myFile) {
    Serial.println("/solarino/index.html:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  } */
}

/*
SOLARINO/     2018-11-03 23:28:10
  ANGULA~1.JS   2018-10-23 16:47:40 166352
  BOOTST~1.CSS  2018-01-18 13:33:30 144877
  FAVICON.ICO   2017-02-01 00:06:26 32038
  INDEX~1.HTM   2018-11-03 21:37:32 594

*/
#endif