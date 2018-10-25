#ifndef Web_h
#define Web_h

#include <WString.h>
#include <ESP8266.h>

#include "index.html.h"
#include "asprintf.h"

ESP8266 wifi(Serial1);

String espWriteln(String command)
{
	Serial1.println(command);
	//uint64_t timeout = 2000UL;
	bool debug = true;

	String response = "";
	//uint64_t time = millis();

	bool done = false;
	int count = 100;
	while (count-- > 0 && !done)
	{
		//Serial.println(count);
		delay(10);
		while (Serial1.available())
		{
			response += (char)Serial1.read(); // read the next character.
			done = true;
			delay(1);
		}
	}
	//Serial.println("_");

	if (debug)
	{
		Serial.print(response);
	}
	return response;
}

String qa(String q)
{
	Serial1.println(q);
	String a = "";
	delay(100);
	while (Serial1.available())
	{
		a += (char)Serial1.read();
	}
	return a;
}

void initWifi()
{
	/*     Serial.println("iniciando wifi");
    espWriteln("AT+RST");                               // reset
    espWriteln("AT+CWJAP=\"GIGANETGIL\",\"34760864\""); //Connect network
    espWriteln("AT+GMR");                               //Firmware version
    espWriteln("AT+CWMODE=1");
    espWriteln("AT+CIFSR");          // IP Address
    espWriteln("AT+CIPMUX=1");       // Multiple connections
    espWriteln("AT+CIPSERVER=1,80"); // start comm port 80 */

	Serial.print("FW Version: ");
	Serial.println(wifi.getVersion().c_str());

	if (wifi.setOprToStation())
	{
		Serial.print("to station ok\r\n");
	}
	else
	{
		Serial.print("to station err\r\n");
	}

	if (wifi.joinAP("GIGANETGIL", "34760864"))
	{
		Serial.print("Join AP success\r\n");
		Serial.print("IP: ");
		Serial.println(wifi.getLocalIP().c_str());
	}
	else
	{
		Serial.print("Join AP failure\r\n");
	}

	if (wifi.enableMUX())
	{
		Serial.print("multiple ok\r\n");
	}
	else
	{
		Serial.print("multiple err\r\n");
	}

	if (wifi.startTCPServer(80))
	{
		Serial.print("start tcp server ok\r\n");
	}
	else
	{
		Serial.print("start tcp server err\r\n");
	}

	if (wifi.setTCPServerTimeout(10))
	{
		Serial.print("set tcp server timout 10 seconds\r\n");
	}
	else
	{
		Serial.print("set tcp server timout err\r\n");
	}

	Serial.print("setup end\r\n");
}

/* uint8_t *getHLength(uint32_t length)
{
	uint8_t *buffer = (uint8_t *)malloc(8);
	itoa(length, (char *)buffer, 10);
	String len = "Content-Length: ";
	len += (char *)buffer;
	len += "\r\n\r\n";
	return buffer;
}

 */

typedef enum HeaderType
{
	Html = 0,
	Json = 1,
	Js = 2,
	Plain = 3
};

void response(uint8_t mux_id, HeaderType type, uint8_t *content, int size)
{
	 const char tStr[4][30] = {"text/html",
							  "application/json",
							  "application/x-javascript",
							  "text/plain"};
 
	const char header[] = "HTTP/1.1 200 OK\r\n"
						  "Content-Length: %d\r\n"
						  "Server: ESP8266 do Gil\r\n"
						  "Content-Type: %s\r\n"
						  "Connection: keep-alive\r\n\r\n";

	char *buf;

	int bufSize = asprintf(&buf, header, size, tStr[type]);

	Serial.print("bufSize: ");
	Serial.println(bufSize);

	wifi.send(mux_id, (uint8_t*)buf, bufSize);
	free(buf);
	wifi.send(mux_id, content, size);
}

void webserver()
{

	uint8_t buffer[128] = {0};
	uint8_t mux_id;
	delay(10);
	uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
	if (len > 0)
	{
		Serial.print("Status:[");
		Serial.print(wifi.getIPStatus().c_str());
		Serial.println("]");

		Serial.print("Received from :");
		Serial.print(mux_id);
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
		}
		else if (strstr(req.c_str(), "angular") != NULL)
		{
			Serial.println("angular requested");
		}
		else if (strstr(req.c_str(), "values") != NULL)
		{
			Serial.println("values requested");
			uint8_t values[] = "{\"value1\": 10.0, \"value2\": 20.0}";
			//response(mux_id, HeaderType::Json, values, sizeof(values));
			wifi.send(values, sizeof(values));
		}
		else
		{
			Serial.println("html requested");
			response(mux_id, HeaderType::Html, html, sizeof(html));
		}

		if (wifi.releaseTCP(mux_id))
		{
			Serial.print("release tcp ");
			Serial.print(mux_id);
			Serial.println(" ok");
		}
		else
		{
			Serial.print("release tcp");
			Serial.print(mux_id);
			Serial.println(" err");
		}

		Serial.print("Status:[");
		Serial.print(wifi.getIPStatus().c_str());
		Serial.println("]");
	}
}

#endif