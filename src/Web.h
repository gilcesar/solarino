#ifndef Web_h
#define Web_h

#include <WString.h>
#include <ESP8266.h>
#include <SPI.h>
#include <SD.h>

#include "asprintf.h"
#include "State.h"

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


/*
SOLARINO/     2018-11-03 23:28:10
  ANGULA~1.JS   2018-10-23 16:47:40 166352
  BOOTST~1.CSS  2018-01-18 13:33:30 144877
  FAVICON.ICO   2017-02-01 00:06:26 32038
  INDEX~1.HTM   2018-11-03 21:37:32 594

*/
typedef enum HeaderType
{
	Html = 0,
	Json = 1,
	Js = 2,
	Plain = 3,
	Icon = 4,
	CSS = 5
};

ESP8266 wifi(Serial1);
String path = "/";

void initWifi(String appPath)
{
	path = appPath;
	//Serial.print("FW Version: ");
	//Serial.println(wifi.getVersion().c_str());
	Serial.println("Initializing wifi...");
	delay(100);
	bool res = wifi.setOprToStation();
	Serial.println(res ? "to station ok" : "to station err");

	res = wifi.joinAP("GIGANETGIL", "34760864");
	Serial.println(res ? ("Join AP success. IP: " + wifi.getLocalIP()).c_str() : "Join AP failure");
	if (res)
	{
		res = wifi.enableMUX();
		Serial.println(res ? "multiple ok" : "multiple err");

		res = wifi.startTCPServer(80);
		Serial.println(res ? "start tcp server ok" : "tcp server start err");

		res = wifi.setTCPServerTimeout(10);
		Serial.println(res ? "set tcp server timout 10 seconds" : "set tcp server timout err");
		Serial.println("setup wifi end.");
	}
	else
	{
		Serial.println("setup wifi failed!");
	}
}

void sendHeader(uint8_t mux_id, HeaderType type, uint32_t size)
{
	const char tStr[6][25] = {"text/html",
							  "application/json",
							  "application/x-javascript",
							  "text/plain",
							  "image/x-icon",
							  "text/css"};

	const char header[] = "HTTP/1.1 200 OK\r\n"
						  "Content-Length: %lu\r\n"
						  "Connection: keep-alive\r\n"
						  "X-Powered-By: ESP8266\r\n"
						  "Vary: Origin\r\n"
						  "Access-Control-Allow-Origin: *\r\n"
						  //"Access-Control-Allow-Methods: POST,GET,PUT,DELETE,OPTIONS\r\n"
						  //"Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept \r\n"
						  "Content-Type: %s\r\n"
						  "\r\n";

	char *buf;

	int bufSize = asprintf(&buf, header, size, tStr[type]);
	Serial.print("size = ");
	Serial.println(size);

	Serial.println(buf);

	wifi.send(mux_id, (uint8_t *)buf, bufSize);
	free(buf);
}

void responseFile(uint8_t mux_id, HeaderType type, const char *fileName)
{
	File f = SD.open(fileName, FILE_READ);
	//delay(50);
	if (f)
	{
		Serial.print("Send file init. Size ");
		Serial.println(f.size());
		uint8_t buf[256] = {0};
		size_t len = 0;
		sendHeader(mux_id, type, f.size());
		while ((len = f.readBytes(buf, sizeof(buf))) > 0)
		{
			Serial.print("-");
			wifi.send(mux_id, buf, len);
		}
		f.close();
		Serial.println("Send file end.");
	}
	else
	{
		Serial.println("Arquivo não enviado!");
	}
}

void responseJson(uint8_t mux_id, const char * json, size_t size)
{
	sendHeader(mux_id, HeaderType::Json, size);
	wifi.send(mux_id, (uint8_t*)json, size);
}

void webserver()
{

	uint8_t buffer[64] = {0};
	uint8_t mux_id;
	//delay(10);
	uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer));
	if (len > 0)
	{
		//Serial.println("Status:[" + wifi.getIPStatus() + "]");
		Serial.println("Received from : " + String(mux_id));

		String req = "";
		Serial.print("[");
		for (uint32_t i = 0; i < len; i++)
		{
			req += (char)buffer[i];
		}
		Serial.print(req.c_str());
		Serial.print("]\r\n");

		if (strstr(req.c_str(), "favicon") != NULL)
		{
			Serial.println("favicon requested");
			String fullPath = path + "/FAVICON.ICO";
			responseFile(mux_id, HeaderType::Icon, fullPath.c_str());
		}
		else if (strstr(req.c_str(), "angular") != NULL)
		{
			Serial.println("angular requested");
			String fullPath = path + "/ANGULA~1.JS";
			//String fullPath = path + "/CHANGE.LOG";
			responseFile(mux_id, HeaderType::Js, fullPath.c_str());
		}
		else if (strstr(req.c_str(), "bootstrap") != NULL)
		{
			Serial.println("bootstrap requested");
			String fullPath = path + "/BOOTST~1.CSS";
			responseFile(mux_id, HeaderType::Js, fullPath.c_str());
		}
		else if (strstr(req.c_str(), "stats") != NULL)
		{
			Serial.println("stats requested");
			char stats[200] = {0};
			size_t size = state.getJsonStats(stats);
			Serial.println(stats);
    		responseJson(mux_id, stats, size);
		}
		else
		{
			Serial.println("html requested");
			String fullPath = (path + "/INDEX~1.HTM");
			responseFile(mux_id, HeaderType::Html, fullPath.c_str());
		}

		bool res = wifi.releaseTCP(mux_id);
		Serial.println(res ? "release tcp " + String(mux_id) + " ok"
						   : "release tcp " + String(mux_id) + " err");

		Serial.print("Status:[");
		Serial.print(wifi.getIPStatus().c_str());
		Serial.println("]");
	}
	else
	{
		Serial.print(".");
	}
}


#endif