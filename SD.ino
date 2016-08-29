boolean datalogOpen = false;
boolean eventlogOpen = false;
//============================================================================================================================

void recordEvent(String event) {
  openEventlog();
  eventlog.println(flightTimeStr() + "  " + event);
  closeEventlog();
}

void openDatalog() {
  if (!datalogOpen) {
    datalog = SD.open(datafile, FILE_WRITE);
    datalogOpen = true;
  }
}

void closeDatalog() {
  if (datalogOpen) {
    datalog.close();
    datalogOpen = false;
  }
}

void openEventlog() {
  if (!eventlogOpen) {
    eventlog = SD.open(eventfile, FILE_WRITE);
    eventlogOpen = true;
  }
}

void closeEventlog() {
  if (eventlogOpen) {
    eventlog.close();
    eventlogOpen = false;
  }
}
