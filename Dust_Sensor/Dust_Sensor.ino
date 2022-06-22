/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>
#include "SDS011.h"

// Replace with your network credentials
const char* ssid = "CircuitLaunch";
const char* password = "makinghardwarelesshard";

float p10i, p25i, p10o, p25o;
int err;
int airPin = 23;
int airDutyCycle = 2000;
int airCount = 0;

SDS011 my_sds_input;
SDS011 my_sds_output;

HardwareSerial port0(0);
HardwareSerial port2(2);

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  //Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  // Print local IP address and start web server
  //Serial.println("");
  //Serial.println("WiFi connected.");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
  server.begin();

  my_sds_input.begin(&port2, 17, 16);
  my_sds_output.begin(&port0, 2, 20);

  pinMode(airPin, OUTPUT);
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    //Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              //Serial.println("GPIO 26 on");
              output26State = "on";
            } else if (header.indexOf("GET /26/off") >= 0) {
              //Serial.println("GPIO 26 off");
              output26State = "off";
            } else if (header.indexOf("GET /27/on") >= 0) {
              //Serial.println("GPIO 27 on");
              output27State = "on";
            } else if (header.indexOf("GET /27/off") >= 0) {
              //Serial.println("GPIO 27 off");
              output27State = "off";
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Cyclone Tester</h1>");

            client.println("<p><a href=\"/26/on\"><button class=\"button\">Update</button></a></p>");
            client.println("<p>Input P25: " + String(p25i) + "</p>");
            client.println("<p>Input P10: " + String(p10i) + "</p>");
            client.println("<p>Output P25: " + String(p25o) + "</p>");
            client.println("<p>Output P10: " + String(p10o) + "</p>");
            // If the output27State is off, it displays the ON button       
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
      err = my_sds_input.read(&p25i, &p10i);
      err = my_sds_output.read(&p25o, &p10o);
      if(airCount > airDutyCycle){
        airCount = 0;
        digitalWrite(airPin, HIGH);
      }
      else{
        airCount++;
        digitalWrite(airPin, LOW);
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
}
