#ifndef Web_h
#define Web_h

#include <WString.h>
#include <ESP8266.h>

#include "index.html.h"
#include "asprintf.h"
#include "Storage.h"

typedef enum HeaderType
{
	Html = 0,
	Json = 1,
	Js = 2,
	Plain = 3
};

ESP8266 wifi(Serial1);
String path = "/";

void initWifi(String appPath)
{
	path = appPath;
	Serial.print("FW Version: ");
	Serial.println(wifi.getVersion().c_str());
	bool res = wifi.setOprToStation();
	Serial.println(res ? "to station ok" : "to station err");

	res = wifi.joinAP("GIGANETGIL", "34760864");
	Serial.println(res ? ("Join AP success. IP: " + wifi.getLocalIP()).c_str() : "Join AP failure");

	res = wifi.enableMUX();
	Serial.println(res ? "multiple ok" : "multiple err");

	res = wifi.startTCPServer(80);
	Serial.println(res ? "start tcp server ok" : "tcp server start err");

	res = wifi.setTCPServerTimeout(10);
	Serial.println(res ? "set tcp server timout 10 seconds" : "set tcp server timout err");

	Serial.println("setup end");
}

void response(uint8_t mux_id, HeaderType type, uint8_t *content, int size)
{
	const char tStr[4][32] = {"text/html; charset=utf-8",
							  "application/json; charset=utf-8",
							  //"application/vnd.api+json",
							  "application/x-javascript",
							  "text/plain; charset=utf-8"};

	const char header[] = "HTTP/1.1 200 OK\r\n"
						  "Content-Length: %d\r\n"
						  "Connection: keep-alive\r\n";
						  "X-Powered-By: ESP8266\r\n"
						  "Vary: Origin\r\n"
						  "Access-Control-Allow-Origin: *\r\n"
						  //"Access-Control-Allow-Methods: POST,GET,PUT,DELETE,OPTIONS\r\n"
						  //"Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept \r\n"
						  "Content-Type: %s\r\n"
						  "\r\n";

	char *buf;

	int bufSize = asprintf(&buf, header, size, tStr[type]);

	Serial.print(">>bufSize: ");
	Serial.println(bufSize);
	Serial.print(">>buf: ");
	Serial.println(buf);
	Serial.print(">>content: ");
	Serial.println((char*)content);
 
	wifi.send(mux_id, (uint8_t *)buf, bufSize);
	free(buf);
	wifi.send(mux_id, content, size);
}

void sendFile(uint8_t mux_id, const char *fileName)
{
	/* uint8_t buf[128] = {0};
	size_t len = 0;
	File f = openFile(fileName);
	while ((len = f.readBytes(buf, sizeof(buf))) > 0)
	{
		wifi.send(mux_id, buf, len);
	}
	closeFile(f); */
}

void webserver()
{

	uint8_t buffer[128] = {0};
	uint8_t mux_id;
	delay(10);
	uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
	if (len > 0)
	{
		Serial.println("Status:[" + wifi.getIPStatus() + "]");
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
			sendFile(mux_id, "favicon.ico");
		}
		else if (strstr(req.c_str(), "angular") != NULL)
		{
			Serial.println("angular requested");
			sendFile(mux_id, "angular.min.js");
		}
		else if (strstr(req.c_str(), "values") != NULL)
		{
			Serial.println("values requested");
			uint8_t values[] = "{\"value1\": \"10.0\", \"value2\": \"20.0\"}";
			response(mux_id, HeaderType::Json, values, sizeof(values));
		}
		else
		{
			Serial.println("html requested");
			response(mux_id, HeaderType::Html, html, sizeof(html));
		}

		bool res = wifi.releaseTCP(mux_id);
		Serial.println(res ? "release tcp " + String(mux_id) + " ok"
						   : "release tcp " + String(mux_id) + " err");

		Serial.print("Status:[");
		Serial.print(wifi.getIPStatus().c_str());
		Serial.println("]");
	}
}

#endif