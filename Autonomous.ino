//////////////////// Autonomous Behavior Variables ////////////////////

boolean autoA = false, autoB = false, autoC = false, autoD = false;

// At a certain altitude, burn specific payloads
long burnAAlt = 40000;         // Burn A at this altitude (in meters, 0 for never)
long burnBAlt = 55000;         // Burn B at this altitude (in meters, 0 for never)
long burnCAlt = 80000;         // Burn C at this altitude (in meters, 0 for never)
long burnDAlt = 0;             // Burn D at this altitude (in meters, 0 for never)

// After a certain amount of time (from start of flight), burn specific payloads
long burnATime = 30;           // Burn A after this much time has elapsed (in seconds, 0 for never)
long burnBTime = 3900;         // Burn B after this much time has elapsed (in seconds, 0 for never)
long burnCTime = 5400;         // Burn C after this much time has elapsed (in seconds, 0 for never)
long burnDTime = 7200;         // Burn D after this much time has elapsed (in seconds, 0 for never)

//============================================================================================================================

void autonomous() {
  if(autoA) {                                          //  Module A
    if(burnAAlt != 0){
      if(GPS.altitude >= burnAAlt) {
        recordEvent("AUTO  Burn A (Altitude)");
        fireAutonomousBurner(burnerA,pullA);
        autoA = false;
      }
    }
    if(burnATime != 0){
      if(((millis()/1000)-flightStart) >= burnATime) {
        recordEvent("AUTO  Burn A (Time)");
        fireAutonomousBurner(burnerA,pullA);
        autoA = false;
      }
    }
  }
  if(autoB) {                                           // Module B
    if(burnBAlt != 0){
      if(GPS.altitude >= burnBAlt) {
        recordEvent("AUTO  Burn B (Altitude)");
        fireAutonomousBurner(burnerB,pullB);
        autoB = false;
      }
    }
    if(burnBTime != 0){
      if(((millis()/1000)-flightStart) >= burnBTime){
        recordEvent("AUTO  Burn B (Time)");
        fireAutonomousBurner(burnerB,pullB);
        autoB = false;
      }
    }
  }
  if(autoC) {                                          //  Module C
    if(burnCAlt != 0){
      if(GPS.altitude >= burnCAlt) {
        recordEvent("AUTO  Burn C (Altitude)");
        fireAutonomousBurner(burnerC,pullC);
        autoC = false;
      }
    }
    if(burnCTime != 0){
      if(((millis()/1000)-flightStart) >= burnCTime){
        recordEvent("AUTO  Burn C (Time)");
        fireAutonomousBurner(burnerC,pullC);
        autoC = false;
      }
    }
  }
  if(autoD) {                                          //  Module D
    if(burnDAlt  != 0){
      if((GPS.altitude >= burnDAlt)) {
        recordEvent("AUTO  Burn D (Altitude)");
        fireAutonomousBurner(burnerD,pullD);
        autoD = false;
      }
    }
    if(burnDTime != 0){
      if(((millis()/1000)-flightStart) >= burnDTime) {
        recordEvent("AUTO  Burn D (Time)");
        fireAutonomousBurner(burnerD,pullD);
        autoD = false;
      }
    }
  }
}

void turnAutoOn() {
  autoA = true;
  autoB = true;
  autoC = true;
  autoD = true;
}

void turnAutoOff() {
  autoA = false;
  autoB = false;
  autoC = false;
  autoD = false;
}

