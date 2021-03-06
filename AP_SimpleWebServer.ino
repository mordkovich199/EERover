/*
  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.
  This sketch will create a new access point (with no password).
  It will then launch a new server and print out the IP address
  to the Serial monitor. From there, you can open that address in a web browser
  to turn on and off the LED on pin 13.

  If the IP address of your shield is yourAddress:
    http://yourAddress/H turns the LED on
    http://yourAddress/L turns it off

  created 25 Nov 2012
  by Tom Igoe
  adapted to WiFi AP by Adafruit
*/

#include <SPI.h>
#include <WiFi101.h>
#include <stdio.h>
#include "arduino_secrets.h"






char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int led =  LED_BUILTIN;

String URL;
String QueryKey;
int keyValue;
int Speed = 0;
int SpeedTurn;
int Turn = 0;

const int PWML_pin = 12;
const int DIRL_pin = 11;
const int PWMR_pin = 8;
const int DIRR_pin = 9;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {

  //Initialize Serial and wait for port to open:
  Serial.begin(9600);

      while (!Serial) {
        // wait for Serial port to connect. Needed for native USB port only
      }

  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);      // set the LED pin mode

  pinMode(PWML_pin, OUTPUT);
  pinMode(DIRL_pin, OUTPUT);
  pinMode(PWMR_pin, OUTPUT);
  pinMode(DIRR_pin, OUTPUT);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // by default the local IP address of will be 192.168.1.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();

}


void loop() {
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      byte remoteMac[6];

      // a device has connected to the AP
      Serial.print("Device connected to AP, MAC address: ");
      WiFi.APClientMacAddress(remoteMac);
      printMacAddress(remoteMac);
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the Serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      Serial.println("beginnign");



      //Handle incomplete or empty currentLines in such a way that no function gets triggered with it.
      int time_a = micros();
      int time_limit =  50000;
      while (micros() - time_a < time_limit) {
        if (client.available()) {
          char c = client.read();
          currentLine += c;
        }
      }

      if (currentLine == " ") {
        client.println("HTTP/1.1 200 OK");
        break;

      }

      Serial.println(currentLine);

      URL = findURL(currentLine);
      QueryKey = findQuery_key(currentLine);

      Serial.println(URL);
      Serial.println(QueryKey);





      if (URL == "led") {
        if (QueryKey == "on") {
          digitalWrite(led, HIGH);
          client.println("HTTP/1.1 200 OK");

        }

        if (QueryKey.endsWith("off")) {
          digitalWrite(led, LOW);
          client.println("HTTP/1.1 200 OK");

        }


      }

      else if (URL == "speed") {

        if (isDigit(QueryKey[QueryKey.length() - 1])) {
          int keyValue = QueryKey.toInt();

          if (keyValue > 0) {
            digitalWrite(DIRL_pin, LOW);
            digitalWrite(DIRR_pin, LOW);
            Speed = keyValue;
            if (Turn == 0) {
              analogWrite(PWML_pin, keyValue);
              analogWrite(PWMR_pin, keyValue);

            }
            else if (Turn > 0) {
              SpeedTurn = keyValue - (keyValue * Turn / 100);
              analogWrite(PWML_pin, keyValue);
              analogWrite(PWMR_pin, SpeedTurn);
            }
            else {
              SpeedTurn = keyValue - (keyValue * abs(Turn) / 100);
              analogWrite(PWML_pin, SpeedTurn);
              analogWrite(PWMR_pin, keyValue);

            }
          }

          if (keyValue < 0) {
            digitalWrite(DIRL_pin, HIGH);
            digitalWrite(DIRR_pin, HIGH);
            Speed = abs(keyValue);

            if (Turn == 0) {
              analogWrite(PWML_pin, abs(keyValue));
              analogWrite(PWMR_pin, abs(keyValue));

            }
            else if (Turn > 0) {
              SpeedTurn = abs(keyValue) - (abs(keyValue) * Turn / 100);
              analogWrite(PWML_pin, abs(keyValue));
              analogWrite(PWMR_pin, SpeedTurn);
            }
            else {
              SpeedTurn = abs(keyValue) - (abs(keyValue) * abs(Turn) / 100);
              analogWrite(PWML_pin, SpeedTurn);
              analogWrite(PWMR_pin, abs(keyValue));

            }

          }

          if (keyValue == 0) {
            Speed = 0;
            analogWrite(PWML_pin, 0);
            analogWrite(PWMR_pin, 0);
            digitalWrite(DIRL_pin, HIGH);
            digitalWrite(DIRR_pin, HIGH);
            client.println("HTTP/1.1 200 OK");
          }
        }

        else {
          analogWrite(PWML_pin, Speed);
          analogWrite(PWMR_pin, Speed);
          client.println("HTTP/1.1 200 OK");
        }
        client.println("HTTP/1.1 200 OK");

      }

      else if (URL == "turn") {
        if (isDigit(QueryKey[QueryKey.length() - 1])) {
          keyValue = QueryKey.toInt();   //For now, keyValue is -100 to 100 in % for simplicity
          Turn = keyValue;
          if (keyValue >= 0) {    //turn right

            //turning on the spot//
            if (Speed == 0) {

              analogWrite(PWML_pin, (keyValue * 255 / 100));
              analogWrite(PWMR_pin, (keyValue * 255 / 100));
              digitalWrite(DIRL_pin, LOW);
              digitalWrite(DIRR_pin, HIGH);
            }
            else {
              SpeedTurn = Speed - (Speed * keyValue / 100);
              analogWrite(PWML_pin, Speed);
              analogWrite(PWMR_pin, SpeedTurn);
            }


            client.println("HTTP/1.1 200 OK");
          }

          if (keyValue < 0) {    //turn left

            if (Speed == 0) {

              analogWrite(PWML_pin, (abs(keyValue) * 255 / 100));
              analogWrite(PWMR_pin, (abs(keyValue) * 255 / 100));
              digitalWrite(DIRL_pin, HIGH);
              digitalWrite(DIRR_pin, LOW);
            }
            else {
              SpeedTurn = Speed - (Speed * abs(keyValue) / 100);
              analogWrite(PWML_pin, SpeedTurn);
              analogWrite(PWMR_pin, Speed);
            }

          }

        }
        else {
          Serial.println("unsopported value");
          client.println("HTTP/1.1 200 OK");

        }
      }


      Serial.println("end");

      break;
    }
    client.stop();
    Serial.println("client disconnected");
  }


}






void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

int findURL_first (const String rqst) {

  int i = 4;
  while (rqst[i] == '/') {
    i++;
  }
  return i;

}


int findURL_last(const String rqst) {
  int i = findURL_first(rqst);

  while ((rqst[i] != '/') && (rqst[i] != ' ')) {
    i++;
  }

  return i - 1;
}

String findURL(const String rqst) {
  int first = findURL_first(rqst);
  int last = findURL_last(rqst);

  String output = "";
  int j = 0;
  for (int i = first; i <= last; i++) {
    char c = rqst[i];
    output +=  c;

  }
  return output;
}

String findQuery_key (const String rqst) {
  int first = findURL_last(rqst) + 2;
  int j = first;
  while ((rqst[j] != ' ') && (j != rqst.length() - 10)) {
    j++;
  }

  int last = j - 1;

  String output = "";
  for (int i = first; i <= last; i++) {
    char c = rqst[i];
    output += c;
  }
  return output;
}
