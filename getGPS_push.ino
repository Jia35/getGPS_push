#include <ESP8266WiFi.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define BaudRrate 9600
#define GPSBaud 9600
#define SSID "Asus_J"
#define PASS "12347890"
#define HOST "140.125.32.146"
#define PORT 8088

String dog_name = "testDog";

static const int RXPin = 0, TXPin = 2;
static const int ledPin = 16;

String GET = "GET /getGPS/?";

double gps_lat, gps_lng, gps_date, gps_time;
bool isGet;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
    pinMode(4, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    Serial.begin(BaudRrate);
    ss.begin(GPSBaud);

    Serial.println( "ESP8266 Ready!" );
    Serial.print("Connect to ");
    Serial.println( SSID );
    WiFi.begin( SSID, PASS );

    while( WiFi.status() != WL_CONNECTED )
    {
        delay(500);
        Serial.print( "." );
    }
    Serial.println( "" );

    Serial.println( "WiFi connected" );
    Serial.println( "IP address: " );
    Serial.println( WiFi.localIP() );
    Serial.println( "" );
    
    Serial.print(F("Testing TinyGPS++ library v. "));
    Serial.println(TinyGPSPlus::libraryVersion());
    Serial.println();

    delay(1000);
}

void loop()
{
    while (ss.available() > 0)
    {
        if (gps.encode(ss.read()))
        {
            isGet = false;
            displayInfo();      //顯示GPS資訊
            if (isGet)
            {
            	updateGPS();    //上傳GPS資料
            	digitalWrite(ledPin, LOW);
				delay(300);
				digitalWrite(ledPin, HIGH);
            }
            delay(15000);
        }
    }

    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
        Serial.println(F("No GPS detected: check wiring."));
        while(true);
    }
}

void updateGPS()
{
    // 設定 ESP8266 作為 Client 端
    WiFiClient client;
    if( !client.connect( HOST, PORT ) )
    {
        Serial.println( "connection failed" );
        return;
    }
    else
    {
        char gps_lat_char[12];
        char gps_lng_char[12];
		dtostrf(gps_lat,3,6,gps_lat_char);
		dtostrf(gps_lng,3,6,gps_lng_char);
		
        String getStr = GET +
						"dog_name=" + String(dog_name) + 
						"&latitude=" + String(gps_lat_char) + 
						"&longitude=" + String(gps_lng_char) +
						"&ok=ok" +
						" HTTP/1.1\r\n" +
						"Host: " + HOST +
						"\r\nConnection: close\r\n\r\n";
        client.print( String(getStr) );
        //Serial.println( String(getStr) );
        delay(10);
        //client.stop();
    }
}

void displayInfo()
{
    Serial.print(F("Location: ")); 
    if (gps.location.isValid())
    {
        isGet = true;
        gps_lat = gps.location.lat();
        gps_lng = gps.location.lng();
        Serial.print(gps_lat,6);
        Serial.print(F(","));
        Serial.print(gps_lng,6);
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F("  Date/Time: "));
    if (gps.date.isValid())
    {
        //gps_date = 
        Serial.print(gps.date.month());
        Serial.print(F("/"));
        Serial.print(gps.date.day());
        Serial.print(F("/"));
        Serial.print(gps.date.year());
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F(" "));
    if (gps.time.isValid())
    {
        //gps_time = 
        if (gps.time.hour() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        if (gps.time.minute() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        if (gps.time.second() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.second());
        Serial.print(F("."));
        if (gps.time.centisecond() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.centisecond());
    }
    else
    {
        Serial.print(F("INVALID"));
    }
	/*
	Serial.println("Satellite Count:");
    Serial.println(gps.satellites.value());
    Serial.println("Latitude:");
    Serial.println(gps.location.lat(), 6);
    Serial.println("Longitude:");
    Serial.println(gps.location.lng(), 6);
    Serial.println("Speed MPH:");
    Serial.println(gps.speed.mph());
    Serial.println("Altitude Feet:");
    Serial.println(gps.altitude.feet());
	*/
    Serial.println();
}



