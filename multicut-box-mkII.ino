/*  Multi-Cut Box current implementation as of 8/22/16
    https://github.umn.edu/MNSGC-Ballooning/multi-cut-box

    Lead programmer Noah Biniek (binie005@umn.edu),
    LED implementation by Austin Langford
    Assistance from Benjamin Geadelmann (GPS fixes) and Ryan Bowers (XBee communications)

    Based on original multi-cut-box (MkI) by Noah Biniek and VentArrow-MkII (https://github.umn.edu/MNSGC-Ballooning/VentArrow-MkII) by Ryan Bowers and Danny Toth

    Currently supports use of GPS, SD datalogging, and xBee communications.
    Can be used as both an autonomous system and/or commanded from ground via RFD relay.
*/

// Libraries
#include <AltSoftSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GPS.h>

// Pin Declarations
#define burnerA 9
#define burnerB 5
#define burnerC 7
#define burnerD 11
#define pullA 8
#define pullB 4
#define pullC 6
#define pullD 10
#define chipSelect 53

const String xBeeID = "MC";

#define gpsSerial Serial1
Adafruit_GPS GPS(&gpsSerial);
AltSoftSerial xBee;

File datalog, eventlog;
char datafile[13], eventfile[13];

String filename = "MCut";

boolean startup = true;
unsigned long flightStart = 0;

boolean usingInterrupt = false;
void useInterrupt(boolean);

//============================================================================================================================

void setup() {
  //set up pin modes
  pinMode(burnerA, OUTPUT);
  pinMode(burnerB, OUTPUT);
  pinMode(burnerC, OUTPUT);
  pinMode(burnerD, OUTPUT);
  pinMode(pullA, INPUT);
  pinMode(pullB, INPUT);
  pinMode(pullC, INPUT);
  pinMode(pullD, INPUT);
  pinMode(chipSelect, OUTPUT);

  //begin all serial lines
  GPS.begin(9600);
  gpsSerial.begin(9600);
  xBee.begin(9600);

  //GPS setup and config
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  //initialize SD card
  SD.begin(chipSelect);

  for (int i = 0; i < 100; i++) {                 //check for existing files from previous runs of program...
    (filename + String(i / 10) + String(i % 10) + ".csv").toCharArray(datafile, sizeof(datafile));
    if (!SD.exists(datafile)) {                   //...and make sure a new file is opened each time
      openDatalog();
      (filename + String(i / 10) + String(i % 10) + ".txt").toCharArray(eventfile, sizeof(eventfile));
      openEventlog();
      break;
    }
  }
  String Header = "Flight Time, Lat, Long, Altitude, Date, Hour:Min:Sec";
  datalog.println(Header);  //set up datalog format

  sendXBee("Setup Complete");
  sendXBee("Awaiting Startup");

  closeDatalog();
  closeEventlog();

  while (true) {
    xBeeCommand();
    if (!startup) break;
    delay(100);
  }
}

//============================================================================================================================

void loop() {
  indicateWithLEDs();                            // Blink the status of the LEDs
  updateGPS();                                   // Update and log GPS
  xBeeCommand();                                 // Check for and respond to any commands received via XBee
  autonomous();
}

