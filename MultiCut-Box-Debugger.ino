#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

const uint8_t led = 13;
//const uint8_t burnerA = 9, burnerB = 10, burnerC = 11, burnerD = 12;
//const uint8_t pullA = 0, pullB = 1, pullC = 6, pullD = 7;

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
boolean alwaysTransmit = true;

void setup () {
  //  Initialize Burners  //
  //  pinMode(burnerA, OUTPUT);
  //  pinMode(burnerB, OUTPUT);
  //  pinMode(burnerC, OUTPUT);
  //  pinMode(burnerD, OUTPUT);

  pinMode(led, OUTPUT); // initialize LED for testing

  //  Initialize Pulls  //
  //  pinMode(pullA, INPUT);
  //  pinMode(pullB, INPUT);
  //  pinMode(pullC, INPUT);
  //  pinMode(pullD, INPUT);

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

void fireBurner(int burnerPin, int pullPin) {
  delay(30000);                   //  recharge capacitors for thirty seconds
  digitalWrite(burnerPin, HIGH);  //  fire burner
  delay(2000);                    //  wait for two seconds
  digitalWrite(burnerPin, LOW);   //  stop burner
}

void testLED() {
  delay(3000);
  digitalWrite(led, HIGH);
  delay(1500);
  digitalWrite(led, LOW);
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

//  Transmit GPS string: "GPS: hours,minutes,seconds,lat,long,alt,satellites"  //
void transmitGPS() {
  xBee.println("Transmitting GPS");
  gpsData = "GPS: " + String(GPS.hour) + "," + String(GPS.minute) + "," + String(GPS.seconds) + "," +
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

    //  Verify id and perform valid command //
    if (id == "01") {
      xBee.println("Command received by MultiCut Box");     //  Command received alert
      if (command == "01") {                                //  "01": Burn A
        xBee.println("Burning A in 30 seconds");
        testLED();
        //fireBurner(burnerA, pullA);
        xBee.println("Burned A");
      } else if (command == "02") {                         //  "02": Burn B
        xBee.println("Burning B in 30 seconds");
        testLED();
        //fireBurner(burnerB, pullB);
        xBee.println("Burned B");
      } else if (command == "03") {                         //  "03": Burn C
        xBee.println("Burning C in 30 seconds");
        testLED();
        //fireBurner(burnerC, pullC);
        xBee.println("Burned C");
      } else if (command == "04") {                         //  "04": Burn D
        xBee.println("Burning D in 30 seconds");
        testLED();
        //fireBurner(burnerD, pullD);
        xBee.println("Burned D");
      } else if (command == "05") {                         //  "05": Transmit GPS
        transmitGPS();
      } else if (command == "06") {                         //  "06": Turn on/off automatic GPS transmission
        alwaysTransmit = !alwaysTransmit;
        if (alwaysTransmit)  xBee.println("GPS automatic transmission is turned on");
        else xBee.println("GPS automatic transmission is turned off");
      } else {
        xBee.println("Command not recognized");
      }
    }
  }
}

void loop ()
{
  xBeeCommand();                      //  respond to XBee commands if any
  if (GPS.newNMEAreceived())
  {
    if (!GPS.parse(GPS.lastNMEA()))   //  update GPS
      return;
  }
  if (alwaysTransmit) transmitGPS();  //  transmit GPS automatically if turned on
  delay(3000);
}

