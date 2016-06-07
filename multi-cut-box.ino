//  Multi-Cut Box current implementation as of 6/6/16
//  By Noah Biniek (binie005@umn.edu)

#include <AltSoftSerial.h>
#include <Adafruit_GPS.h>
#include <SPI.h>
#include <SD.h>

const uint8_t led = 13;
const uint8_t burnerA = 10, burnerB = 11, burnerC = 12, burnerD = 9;
const uint8_t pullA = 6, pullB = 8, pullC = 7, pullD = 5;
const int chipSelect = 53;

const long baudRate = 9600;

AltSoftSerial xBee; // TX:46, RX:48, PWM Unusuable:44,45 **These pins cannot be changed - this is built into the AltSoftSerial Library**
                    // If using Arduino Mega with SparkFun XBee shield, make sure switch is set to DLINE (not UART).
                    // This tells the XBee to communicate with pins 2 and 3 which must be jumped to pins 48 and 46 respectively.
                    // Make sure pins 2 and 3 are not inserted into the GPS shield (bend them).
HardwareSerial s = Serial1;
//SoftwareSerial s(5, 4); // TX:14, RX:15 **Pins may be changed to reflect wiring**
                        // Using Ultimate GPS Breakout v3
                        
//  SD Breakout Pinouts (with Arduino Mega):
//    DI - Pin 51
//    DO - Pin 50
//    CLK - Pin 52
//    CS - Pin 53

//  GPS Declaration  //
Adafruit_GPS GPS(&Serial1);
String  gpsData;
#define GPSECHO  false
boolean usingInterrupt = false;
void useInterrupt(boolean);

//  regular updates/box behavior  //
boolean gpsUpdates = false;
boolean statusUpdates = false;
boolean autoA = false, autoB = false, autoC = false, autoD = false;
long timerA = 3000, timerB = 3900, timerC = 5400, timerD = 7200;    //  default times to burn: A-50min, B-65min, C-90min, D-120min
long altA = 40000, altB = 55000, altC = 80000;                      //  default altitudes to burn: A-40,000ft, B-55,000ft, C-80,000ft
long flightTimer = 0;
long peakAlt = 0;
boolean descending = false;
long descentTime = 0;

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
  while (!Serial){;}      //  wait for serial port to connect
  xBee.begin(baudRate);
  GPS.begin(baudRate);

  //  SD Data Log Setup //
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {    //  Check if SD card is present
    Serial.println("Card failed, or not present");
    return;
  } Serial.println("Card initialized");
  File logFile = SD.open("MC-LOG.txt", FILE_WRITE);
  if (logFile) {
    logFile.println("Multi-cut Box Data Log:\n");
    logFile.close();
  } else Serial.println("Error opening MC-LOG.txt");
  
  //  GPS Setup //
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //  selects data type to recieve
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);    //  sets update rate
  GPS.sendCommand(PGCMD_ANTENNA);               //  updates antenna status
  useInterrupt(true);
  delay(1000);
  s.println(PMTK_Q_RELEASE);

}

void logData(String text) {
  File logFile = SD.open("MC-LOG.txt", FILE_WRITE);
  if (logFile) {
    logFile.println(String(flightTimer) + ": " + text);
    logFile.close();
  }
}
void fireBurner(int burner) {
  delay(30000);                //  recharge capacitors for thirty seconds
  digitalWrite(burner, HIGH);  //  fire burner
  delay(2000);                 //  wait for two seconds
  digitalWrite(burner, LOW);   //  stop burner
}

void fireAutonomousBurner(int burner, int pull) {
  fireBurner(burner);
  if(digitalRead(pull) == LOW) {
    delay(30000);
    fireBurner(burner);
  } if(digitalRead(pull) == LOW) {
    delay(30000);
    fireBurner(burner);
  }
}

void testLED() {
  digitalWrite(led, HIGH);
  delay(1500);
  digitalWrite(led, LOW);
}

//  Return status string: "minutes:seconds A?B?C?D?" where ? may be O if pull is present or X if not present
String getPullStatus() {
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
  return status;
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

//  Return GPS string: "hours,minutes,seconds,lat,long,alt,satellites"  //
String getGPS() {
  String gpsData = String(GPS.hour) + "," + String(GPS.minute) + "," + String(GPS.seconds) + "," +
            String(GPS.latitudeDegrees, 6) + "," + String(GPS.longitudeDegrees, 6) + "," + String(GPS.altitude, 1) + "," + String(GPS.satellites);
  return gpsData;
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
    if (id == "MC") {
      Serial.println("Command acknowledged");
      xBee.print("A\n");                                  //  Command acknowledged
      logData("Command: " + command + " acknowledged");
      if (command == "BA") {                                //  "BA": Burn A
        Serial.println("Burning A in thirty seconds");
        xBee.print("BA");
        testLED();
        fireBurner(burnerA);
        Serial.println("Burned A");
        xBee.print("OK");
        logData("Attempt Burn A");
      } else if (command == "BB") {                         //  "BB": Burn B
        Serial.println("Burning B in thirty seconds");
        xBee.print("BB");
        testLED();
        fireBurner(burnerB);
        Serial.println("Burned B");
        xBee.print("OK");
        logData("Attempt Burn B");
      } else if (command == "BC") {                         //  "BC": Burn C
        Serial.println("Burning C in thirty seconds");
        xBee.print("BC");
        testLED();
        fireBurner(burnerC);
        Serial.println("Burned C");
        xBee.print("OK");
        logData("Attempt Burn C");
      } else if (command == "BD") {                         //  "BD": Burn D
        Serial.println("Burning D in thirty seconds");
        xBee.print("BD");
        testLED();
        fireBurner(burnerD);
        Serial.println("Burned D");
        xBee.print("OK");
        logData("Attempt Burn D");
      } else if (command == "TG") {                         //  "TG": Transmit GPS
        String gpsString = getGPS();
        xBee.print(gpsString);
        Serial.println(gpsString);
        logData("Attempt manual GPS data transmission: " + gpsString);
      } else if (command == "AG") {                         //  "AG": Turn on/off automatic GPS transmission
        gpsUpdates = !gpsUpdates;
        if (gpsUpdates)  {
          logData("Automatic GPS updates turned on");
          Serial.println("GPS updates ON");
          xBee.print("GOn");
        } else {
          logData("Automatic GPS updates turned off");
          Serial.println("GPS udpates OFF");
          xBee.print("GOff");
        }
      } else if (command == "TP") {                         //  "TP": Transmit Pull Status
        String pullStatus = getPullStatus();
        xBee.print(pullStatus);
        Serial.println(pullStatus);
        logData("Attempt manual pull status transmission: " + pullStatus);
      } else if (command == "AS") {                         //  "AS": Turn on/off automatic status transmission
        statusUpdates = !statusUpdates;
        if (statusUpdates)  {
          logData("Automatic status updates turned on");
          Serial.println("Status updates ON");
          xBee.print("SOn");
        } else {
          logData("Automatic status updates turned off");
          Serial.println("Status updates OFF");
          xBee.print("SOff");
        }
      } else if (command == "AA") {                         //  "AA": Turn on/off autonomous A behavior
        autoA = !autoA;
        if (autoA) {
          logData("Autonomous A behavior turned on");
          Serial.println("Autonomous A behavior ON");
          xBee.print("AAOn");
        } else {
          logData("Autonomous A behavior turned off");
          Serial.println("Autonomous A behavior OFF");
          xBee.print("AAOff");
        }
      } else if (command == "AB") {                         //  "AB": Turn on/off autonomous B behavior
        autoB = !autoB;
        if (autoB) {
          logData("Autonomous B behavior turned on");
          Serial.println("Autonomous B behavior ON");
          xBee.print("ABOn");
        } else {
          logData("Autonomous B behavior turned off");
          Serial.println("Autonomous B behavior OFF");
          xBee.print("ABOff");
        }
      } else if (command == "AC") {                         //  "AC": Turn on/off autonomous behavior
        autoC = !autoC;
        if (autoC) {
          logData("Autonomous C behavior turned on");
          Serial.println("Autonomous C behavior ON");
          xBee.print("ACOn");
        } else {
          logData("Autonomous C behavior turned off");
          Serial.println("Autonomous C behavior OFF");
          xBee.print("ACOff");
        }
      } else if (command == "AD") {                         //  "AD": Turn on/off autonomous behavior
        autoD = !autoD;
        if (autoD) {
          logData("Autonomous D behavior turned on");
          Serial.println("Autonomous D behavior ON");
          xBee.print("ADOn");
        } else {
          logData("Autonomous D behavior turned off");
          Serial.println("Autonomous D behavior OFF");
          xBee.print("ADOff");
        }
      } else if (command == "TL") {                         //  "TL": Test LED
        Serial.println("LED test");
        xBee.print("LED");
        testLED();
        logData("LED test");
      } else if (command == "XX") {                         //  "XX": Burn all modules
        logData("Attempt burn all modules");
        Serial.println("Burning all modules");
        xBee.print("BAll");
        fireBurner(burnerA);
        Serial.println("Burned A");
        xBee.print("OKA");
        fireBurner(burnerB);
        Serial.println("Burned B");
        xBee.print("OKB");
        fireBurner(burnerC);
        Serial.println("Burned C");
        xBee.print("OKC");
        fireBurner(burnerD);
        Serial.println("Burned D");
        xBee.print("OKD");
        Serial.println("Burned all modules");
        xBee.println("OK");
      } else {
        Serial.println("Command not recognized");
        xBee.print("Fail");
        logData("Command not recognized");
      }
      xBee.println("!");                                    //  Transmission delimiter for raspberryPI
    }
  }
}

void loop ()
{
  logData(getGPS());
  xBeeCommand();                              //  respond to XBee commands if any
  if (GPS.newNMEAreceived())
  {
    if (!GPS.parse(GPS.lastNMEA()))           //  update GPS
      return;
  }
  if (gpsUpdates) {
    xBee.print(getGPS());              //  transmit GPS automatically if turned on
    xBee.println("!");
  }

  if(statusUpdates && flightTimer%60==0) {    //  transmit box status by minute if turned on
    xBee.print(getPullStatus());
    xBee.println("!");
  }

  if(GPS.altitude>peakAlt) peakAlt = GPS.altitude;
  if(GPS.altitude != 0 && GPS.altitude<peakAlt-100) descending = true;  //  if altitude is over 100ft less than the peak altitude, payload is declared descending
  
  //  Autonomous Behavior //
  if(autoA) {                                          //  Module A
    if(GPS.altitude >= altA || flightTimer >= timerA) {
      fireAutonomousBurner(burnerA,pullA);     //  altitude cutdown (40,000 ft), timer cutdown (50 min)
      autoA = false;
    }
  }
  if(autoB) {                                          //  Module B
    if(GPS.altitude >= altB || flightTimer >= timerB) {
      fireAutonomousBurner(burnerB,pullB);     //  altitude cutdown (55,000 ft), timer cutdown (65 min)
      autoB = false;
    }
  }
  if(autoC) {                                          //  Module C
    if(GPS.altitude >= altC || flightTimer >= timerC) {
      fireAutonomousBurner(burnerC,pullC);     //  altitude cutdown (80,000 ft), timer cutdown (90 min)
      autoC = false;
    }
  }
  if(autoD) {                                          //  Module D
    if((descending && descentTime >= 300) || flightTimer >= timerD) {
      fireAutonomousBurner(burnerD,pullD);     //  time descending cutdown(5 min), timer cutdown (120 min)
      autoD = false;
    }
  }
  delay(3000);
  if(descending) descentTime+=3;
  flightTimer+=3;         //  track flightTime
}

