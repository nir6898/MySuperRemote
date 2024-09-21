
#include <WiFiS3.h>

#include "wifi_secrets.h" 
#include "HTML_style.h" 
#include "HTML_page.h" 

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

String readString;

boolean alreadyConnected = false; // whether or not the client was connected previously

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // start the server:
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
  
}


void loop() {
  bool acOnFlag = 0;
  int fanState = 0;

  // wait for a new client:
  WiFiClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    while (client.connected()) {   
      if (client.available()) {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 1000) {
          //store characters to string
          readString += c;
          // Serial.print(c);
        }

        //if HTTP request has ended
        if ((c == '\n') && (readString.indexOf("GET /?") == 0)) {          
          Serial.println(readString); //print to serial monitor for debuging

          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println();     
          client.println(HTML_page);
           
          //  client.println("<HTML>");
          //  client.println("<HEAD>");
          //  client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
          //  client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");          
          //  client.println("<style>");          
          //  client.println(HTML_style);          
          //  client.println("</style>");          
          //  client.println("<TITLE>Nir & Romi's super smart home controller</TITLE>");
          //  client.println("</HEAD>");
          //  client.println("<BODY>");
          //  client.println("<H1>Nir & Romi's super-smart-home controller</H1>");
          //  client.println("<hr />");
          //  client.println("<hr />");
          //  client.println("<form action='' method='post'>");
          //  client.println("<input type='submit' name='upvote' value='Upvote' />");
          //  client.println("</form>");
          //  client.println("<br />");
          //  client.println("<H2>Air Condition Control</H2>");  
          //  client.println("<br />");
          //  client.println("<button class='button-on' role='button'>AC On</button>");
          //  client.println("<br />");  
          //  client.println("<br />");  
          // //  if (acOnFlag){
          // //   client.println("<a href=\"/?AC_on_off\"\">AC On/Off</a>");
          // //  }else{
          // //   client.println("<a href=\"/?AC_on_off\"\">AC On/Off</a>");
          // //  }
          //  client.println("<hr />");
          //  client.println("<H2>Ceileing Fan Control</H2>");  
          //  client.println("<br />");  
          // //  client.println("<a_on href=\"/?FAN_on_off\"\">FAN On/Off</a_on>");
          //  client.println("</BODY>");
          //  client.println("</HTML>");
     
          delay(1);
          //stopping client
          client.stop();
          //controls the Arduino if you press the buttons
          if (readString.indexOf("?AC_on_off") >0){
            Serial.println("got AC on-off command!");
            if (acOnFlag){
              acOnFlag = 0;
            }else{
              acOnFlag = 1;
            }
          } else if (readString.indexOf("Fan_Value")){
            if (readString.indexOf("Fan_low")){
              fanState = 1;
            } else if (readString.indexOf("Fan_med")){
              fanState = 2;
            } else if (readString.indexOf("Fan_high")){
              fanState = 3;
            } else if (readString.indexOf("Fan_off")){
              fanState = 0;
            }
            Serial.print("changed fan state to: ");
            Serial.println(fanState);
          }
          //  else if (readString.indexOf("?FAN_on_off") >0){
          //    Serial.println("got FAN on-off command!");
          //  }
          // clearing string for next read
          readString="";     
        }
      }
    }
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
