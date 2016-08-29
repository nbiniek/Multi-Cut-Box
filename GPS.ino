unsigned long timer = millis();
int logRate = 1000;   //interval in seconds between cycles of datalogging
//============================================================================================================================

void updateGPS() {
  while (gpsSerial.available() > 0) {
    GPS.read();
    if (GPS.newNMEAreceived()) {
      GPS.parse(GPS.lastNMEA());
      break;
    }
  }
  if (millis() - timer > logRate) {
    timer = millis();
    openDatalog();
    if (GPS.fix) {
      datalog.print(flightTimeStr() + "," + String(GPS.latitudeDegrees) + "," + String(GPS.longitudeDegrees) + ",");
      datalog.print(String(GPS.altitude * 3.28048) + ",");    //convert meters to feet for datalogging
      datalog.print(String(GPS.month) + "/" + String(GPS.day) + "/" + String(GPS.year) + ",");
      datalog.println(String(GPS.hour) + ":" + String(GPS.minute) + ":" + String(GPS.seconds) + ",");
    }
    else
      datalog.println(flightTimeStr() + ",No fix");
    closeDatalog();
  }
}

//  Return GPS string: "hours,minutes,seconds,lat,long,alt,satellites"
String getGPS() {
  return  flightTimeStr() + "," + String(GPS.latitudeDegrees) + "," + String(GPS.longitudeDegrees) + ","
          + String(GPS.altitude * 3.28048) + ","    //convert meters to feet for datalogging
          + String(GPS.month) + "/" + String(GPS.day) + "/" + String(GPS.year) + ","
          + String(GPS.hour) + ":" + String(GPS.minute) + ":" + String(GPS.seconds) + ",";
}

