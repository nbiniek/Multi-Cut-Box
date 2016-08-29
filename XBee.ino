void sendXBee(String out) {
  xBee.println(xBeeID + ";" + out + "!");
  recordEvent("TX  " + out);
}

void logCommand(String com, String command) {
  recordEvent("RX  " + com + "  " + command);
}

void acknowledge() {
  xBee.println(xBeeID + "\n");
}
//============================================================================================================================

String lastCommand = "";
unsigned long commandTime = 0;

void xBeeCommand() {
  boolean complete = false;
  String command = "";
  char inChar;
  while (xBee.available() > 0) {
    inChar = (char)xBee.read();
    if (inChar != ' ') {
      command += inChar;
      if (inChar == '!') {
        complete = true;
        break;
      }
    }
    delay(10);
  }
  if (!complete) return;
  if (command.equals(lastCommand) && (millis() - commandTime < 30000)) return;
  int split = command.indexOf('?');
  if (!(command.substring(0, split)).equals(xBeeID)) return;
  lastCommand = command;
  String Com = command.substring(split + 1, command.length() - 1);
  acknowledge();
  commandTime = millis();
  
  if (startup && Com.equals("GO")) {                     //  "GO": Initial flight start command
    logCommand(Com,"Begin Flight");
    flightStart = millis();
    sendXBee("Flight Start");
    startup = false;
    return;
    
  } else if (Com.equals("GO")) {                         //  "GO": Reset flight clock; same as flight start command
    logCommand(Com,"Flight Clock Reset");
    flightStart = millis();
    sendXBee("Flight Reset");
    
  } else if (Com.equals("BA")) {                         //  "BA": Burn A
    logCommand(Com,"Burn A");
    fireBurner(burnerA);
    activateIndicatorLED("A", "B");
    sendXBee("BA OK");
    
  } else if (Com.equals("BB")) {                         //  "BB": Burn B
    logCommand(Com,"Burn B");
    fireBurner(burnerB);
    activateIndicatorLED("B", "B");
    sendXBee("BB OK");
    
  } else if (Com.equals("BC")) {                         //  "BC": Burn C
    logCommand(Com,"Burn C");
    fireBurner(burnerC);
    activateIndicatorLED("C", "B");
    sendXBee("BC OK");
    
  } else if (Com.equals("BD")) {                         //  "BD": Burn D
    logCommand(Com,"Burn D");
    fireBurner(burnerD);
    activateIndicatorLED("D", "B");
    sendXBee("BD OK");
    
  } else if (Com.equals("TG")) {                         //  "TG": Transmit GPS
    logCommand(Com,"Transmit GPS");
    String gpsString = getGPS();
    sendXBee(gpsString);
    
  } else if (Com.equals("TP")) {                         //  "TP": Transmit Pull Status
    logCommand(Com,"Transmit Pull Status");
    String pullString = getPullStatus();
    sendXBee(pullString);

  } else if (Com.equals("TT")) {                         //  "TT": Transmit Flight Time
    logCommand(Com,"Transmit Flight Time");
    sendXBee(flightTimeStr());
    
  } else if (Com.equals("A1")) {                         //  "A1": Turn autonomous controls on
    logCommand(Com,"Autonomous Control On");
    turnAutoOn();
    sendXBee("Auto On");
    
  } else if (Com.equals("A0")) {                         //  "A0": Turn autonomous controls off
    logCommand(Com,"Autonomous Control Off");
    turnAutoOff();
    sendXBee("Auto Off");
    
  } else if (Com.equals("TEST")) {                       //  "TEST": Test XBee
    logCommand(Com,"TEST");
    sendXBee("TEST SUCCESS");

  } else if (Com.equals("XX")) {                         //  "XX": Auto fire all burners
    logCommand(Com,"Auto Fire All Burners");
    fireAutonomousBurner(burnerA,pullA);
    fireAutonomousBurner(burnerB,pullB);
    fireAutonomousBurner(burnerC,pullC);
    fireAutonomousBurner(burnerD,pullD);
    sendXBee("XX OK");
    
  } else {                                               //  If no recognizable command was received, inform ground station
    logCommand(Com,"**Command Not Recognized**");          
    sendXBee(Com + "Command Not Recognized");
  }
  lastCommand = command;
  commandTime = millis();
}
