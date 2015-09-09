/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */

#include <SPI.h>
#include <Ethernet.h>
//#include <EEPROM.h>
#include <LiquidCrystal.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,11,177);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2, 3, 6, 7, 8, 9);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  lcd.begin(16, 2);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    String params;
    boolean currentLineIsBlank = true;
    boolean readParams = false;
    boolean first = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (first && c == 'G') {
                      // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<form method=\"POST\">");
            client.println("<input type=text name='msg' />");
            client.println("<input type=submit name=press value=\"submit\" />");
            client.println("woot!");
            client.println("<br />");  
            client.println("</form>");
            client.println("</html>");
            // give the web browser time to receive the data
            delay(1);
            // close the connection:
            client.stop();
          break;
        } else {
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (readParams) {
            params += c;
          } else {
            if (c == '\n' && currentLineIsBlank) {
                readParams = true;
            }
            if (c == '\n') {
              // you're starting a new line
              currentLineIsBlank = true;
            }
            else if (c != '\r') {
              // you've gotten a character on the current line
              currentLineIsBlank = false;
            }
          }
        }  
      } else {
            int startIdx = params.indexOf("msg=") + 4;
            int endIdx = params.indexOf('&', startIdx);
            if (endIdx == -1) {
              endIdx = params.length();
            }
            lcd.clear();
  
            // print the number of seconds since reset:
            lcd.print(params.substring(startIdx, endIdx));
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("woot!");
            client.println("<br />");  
            client.println("</html>");
              
            // give the web browser time to receive the data
            delay(1);
            // close the connection:
            client.stop();
       }
       first = false;
    }

    Serial.println("client disconnected");
  }
}
