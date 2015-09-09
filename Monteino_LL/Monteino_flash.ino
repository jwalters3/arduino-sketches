#include <RFM69.h> 
#include <SPI.h>
#include <SPIFlash.h>

#define LED           9 // Moteinos have LEDs on D9
#define BUTTON        4
#define NODEID        2    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     1
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY   RF69_868MHZ
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!

#define SERIAL_BAUD   115200

char openPayload[] = "Open:2";
char closePayload[] = "Close:2";
char buff[20];
RFM69 radio;


int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 500;    // the debounce time; increase if the output flickers


// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(SERIAL_BAUD);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(ENCRYPTKEY);
  
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

  // set initial LED state
  digitalWrite(LED, LOW);
}

void loop() {
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 'r') //d=dump register values
      radio.readAllRegs();
    //if (input == 'E') //E=enable encryption
    //  radio.encrypt(KEY);
    //if (input == 'e') //e=disable encryption
    //  radio.encrypt(null);
  }
  
    //check for any received packets
  if (radio.receiveDone())
  {
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.print(" - ACK sent");
    }
    Blink(LED,3,3);
    Serial.println();
  }
  
  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
 
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
   
    // if the button state has changed:
    if (reading != buttonState) {

      buttonState = reading;
        if (buttonState == HIGH) {
            Serial.print("Sending[");
            Serial.print(6);
            Serial.print("]: ");
            for(byte i = 0; i < 6; i++)
              Serial.print((char)openPayload[i]);
          
            if (radio.sendWithRetry(GATEWAYID, openPayload, 6))
              Serial.print(" ok!");
            else Serial.print(" nothing...");
            Serial.println();
            Blink(LED,30,3);
        } else {
            Serial.print("Sending[");
            Serial.print(7);
            Serial.print("]: ");
            for(byte i = 0; i < 7; i++)
              Serial.print((char)closePayload[i]);
          
            if (radio.sendWithRetry(GATEWAYID, closePayload, 7))
              Serial.print(" ok!");
            else Serial.print(" nothing...");
            Serial.println();
            Blink(LED,30,3);
        }

        // set the LED:
        digitalWrite(LED, buttonState);
    }
    
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;

}

void Blink(byte PIN, int DELAY_MS, byte repeats)
{
  while(repeats-->0)
  {
    pinMode(PIN, OUTPUT);
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}

