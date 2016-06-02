#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

const uint8_t led = 13;
const uint8_t burnerA = 6, burnerB = 7, burnerC = 11, burnerD = 12;
const uint8_t pullA = A0, pullB = A1, pullC = A2, pullD = A3;

const long baudRate = 9600;

AltSoftSerial xBee; // TX:9, RX:8, PWM Unusuable:10 **These pins cannot be changed - this is built into the AltSoftSerial Library**
                    // If using Arduino Uno with SparkFun XBee shield, make sure switch is set to DLINE (not UART).
                    // This tells the XBee to communicate with pins 2 and 3 which must be jumped to pins 8 and 9 respectively.
                    // Make sure pins 2 and 3 are not inserted into the GPS shield (bend them).

SoftwareSerial s(5, 4); // TX:5, RX:4 **Pins may be changed to reflect wiring**
                        // Using Ultimate GPS Breakout v3

//  GPS Declaration  //
Adafruit_GPS GPS(&s);
String  gpsData;
#define GPSECHO  false
boolean usingInterrupt = false;
void useInterrupt(boolean);

//  regular updates/box behavior  //
boolean gpsUpdates = false;
boolean statusUpdates = false;
boolean autonomous = true;
long flightTimer = 0;

void setup () {
  //  Initialize Burners  //
    pinMode(burnerA, OUTPUT);
    pinMode(burnerB, OUTPUT);
    pinMode(burnerC, OUTPUT);
    pinMode(burnerD, OUTPUT);

  pinMode(led, OUTPUT); // initialize LED for testing

  //  Initialize Pulls  //
    pinMode(pullA, INPUT);
    pinMode(pullB, INPUT);
    pinMode(pullC, INPUT);
    pinMode(pullD, INPUT);

  //  Initialize Serials  //
  Serial.begin(baudRate);
  xBee.begin(baudRate);
  GPS.begin(baudRate);

  //  GPS Setup //
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //  selects data type to recieve
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);    //  sets update rate
  GPS.sendCommand(PGCMD_ANTENNA);               //  updates antenna status
  useInterrupt(true);
  delay(1000);
  s.println(PMTK_Q_RELEASE);

}

void fireBurner(int burnerPin) {
  delay(30000);                   //  recharge capacitors for thirty seconds
  digitalWrite(burnerPin, HIGH);  //  fire burner
  delay(2000);                    //  wait for two seconds
  digitalWrite(burnerPin, LOW);   //  stop burner
}

void testLED() {
//  delay(3000);
  digitalWrite(led, HIGH);
  delay(1500);
  digitalWrite(led, LOW);
}

void queryStatus() {
  String status;
  int minutes = flightTimer/60;
  int seconds = flightTimer%60;
  status += String(minutes) + ":" + String(seconds) + " ";
  if(digitalRead(pullA) == LOW) status += "AO";
  else status += "AX";
  if(digitalRead(pullB) == LOW) status += "BO";
  else status += "BX";
  if(digitalRead(pullC) == LOW) status += "CO";
  else status += "CX";
  if(digitalRead(pullD) == LOW) status += "DO";
  else status += "DX";
  Serial.println(status);
  xBee.println(status);
}

//  Interrupt is called once a millisecond, looks for any new GPS data, and stores it  //
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
}

//  GPS interrupt function  //
void useInterrupt(boolean v)
{
  if (v)
  {
    //  Timer0 is already used for millis() - we'll just interrupt somewhere in the middle and call the "Compare A" function above  //
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  }
  else
  {
    //  Do not call the interrupt function COMPA anymore  //
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

//  Transmit GPS string: "hours,minutes,seconds,lat,long,alt,satellites"  //
void transmitGPS() {
  gpsData = String(GPS.hour) + "," + String(GPS.minute) + "," + String(GPS.seconds) + "," +
            String(GPS.latitudeDegrees, 6) + "," + String(GPS.longitudeDegrees, 6) + "," + String(GPS.altitude, 1) + "," + String(GPS.satellites);
  Serial.println(gpsData);
  xBee.println(gpsData);
}

// Checks for and responds to any messages sent via XBee //
void xBeeCommand() {
  if (xBee.available()) {

    //  Grab transmission and parse command  //
    int leng, num;
    String transmission, id, command;
    transmission = xBee.readStringUntil('!');
    leng = transmission.length();
    num = transmission.indexOf('?');
    id = transmission.substring(0, num);
    command = transmission.substring(num + 1, leng);

    Serial.println(transmission);

    //  Verify id and perform valid command //
    if (id == "01") {
      Serial.println("Command acknowledged");
      xBee.println("A\n");                                  //  Command acknowledged
      if (command == "01") {                                //  "01": Burn A
        Serial.println("Burning A in thirty seconds");
        xBee.println("BA");
        testLED();
        fireBurner(burnerA);
        Serial.println("Burned A");
        xBee.println("OK");
      } else if (command == "02") {                         //  "02": Burn B
        Serial.println("Burning B in thirty seconds");
        xBee.println("BB");
        testLED();
        fireBurner(burnerB);
        Serial.println("Burned B");
        xBee.println("OK");
      } else if (command == "03") {                         //  "03": Burn C
        Serial.println("Burning C in thirty seconds");
        xBee.println("BC");
        testLED();
        fireBurner(burnerC);
        Serial.println("Burned C");
        xBee.println("OK");
      } else if (command == "04") {                         //  "04": Burn D
        Serial.println("Burning D in thirty seconds");
        xBee.println("BD");
        testLED();
        fireBurner(burnerD);
        Serial.println("Burned D");
        xBee.println("OK");
      } else if (command == "05") {                         //  "05": Transmit GPS
        transmitGPS();
      } else if (command == "06") {                         //  "06": Turn on/off automatic GPS transmission
        gpsUpdates = !gpsUpdates;
        if (gpsUpdates)  {
          Serial.println("GPS updates ON");
          xBee.println("GOn");
        } else {
          Serial.println("GPS udpates OFF");
          xBee.println("GOff");
        }
      } else if (command == "07") {                         //  "07": Query Status
        queryStatus();
      } else if (command == "08") {                         //  "08": Turn on/off automatic status transmission
        statusUpdates = !statusUpdates;
        if (statusUpdates)  {
          Serial.println("Status updates ON");
          xBee.println("SOn");
        } else {
          Serial.println("Status updates OFF");
          xBee.println("SOff");
        }
      } else if (command == "09") {                         //  "09": Turn on/off autonomous behavior redundancies
        autonomous = !autonomous;
        if (autonomous) {
          Serial.println("Autonomous behavior ON");
          xBee.println("AOn");
        } else {
          Serial.println("Autonomous behavior OFF");
          xBee.println("AOff");
        }
      } else if (command == "10") {                         //  "10": Test LED
        Serial.println("LED test");
        xBee.println("LED");
        testLED();
      } else if (command == "99") {                         //  "99": Burn all modules
        Serial.println("Burning all modules");
        xBee.println("BAll");
        fireBurner(burnerA);
        Serial.println("Burned A");
        xBee.println("OKA");
        fireBurner(burnerB);
        Serial.println("Burned B");
        xBee.println("OKB");
        fireBurner(burnerC);
        Serial.println("Burned C");
        xBee.println("OKC");
        fireBurner(burnerD);
        Serial.println("Burned D");
        xBee.println("OKD");
        Serial.println("Burned all modules");
        xBee.println("OK");
      } else {
        Serial.println("Command not recognized");
        xBee.println("Fail");
      }
    }
  }
}

void loop ()
{
  xBeeCommand();                              //  respond to XBee commands if any
  if (GPS.newNMEAreceived())
  {
    if (!GPS.parse(GPS.lastNMEA()))           //  update GPS
      return;
  }
  if (gpsUpdates) transmitGPS();              //  transmit GPS automatically if turned on

  if(statusUpdates && flightTimer%60==0) {    //  transmit box status by minute if turned on
    queryStatus();
  }

  //  Autonomous Behavior Redundancies //
    if(GPS.altitude >= 40000) fireBurner(burnerA);     //  altitude cutdown (40,000 ft)
    if(flightTimer >= 5400) fireBurner(burnerA);       //  timer cutdown (90 min)
  
  delay(3000);
  flightTimer+=3;         //  track flightTime
}

