// Burner Variables
unsigned long burnertimer = 0;
boolean activatedA = false;
boolean activatedB = false;
boolean activatedC = false;
boolean activatedD = false;
//============================================================================================================================

// Fires the burner specified in the argument   **Added timer to check the time between burns. Code won't wait 30 seconds everytime anymore.**
void fireBurner(int burner) {
  sync_LEDs();
  unsigned long diff, temp;
  diff = millis() - burnertimer;
  if(diff > 30000)
  {
    digitalWrite(burner, HIGH);  //  fire burner
    delay(3000);                 //  wait for three seconds
    digitalWrite(burner, LOW);   //  stop burner
  }
  else
  {
    delay(30000 - diff);
    digitalWrite(burner, HIGH);  //  fire burner
    delay(3000);                 //  wait for three seconds
    digitalWrite(burner, LOW);   //  stop burner
  }
  burnertimer = millis();
}

// Fires the burner specificed in the first argument one time and then up to two more times if pull pin is still present
void fireAutonomousBurner(int burner, int pull) {
  fireBurner(burner);
  delay(1000);
  if(digitalRead(pull) == HIGH) {
    fireBurner(burner);
    delay(1000);
  } 
  if(digitalRead(pull) == HIGH) {
    fireBurner(burner);
  }
}

//  Return status string: "minutes:seconds A?B?C?D?" (where '?' will be O if pull is present or X if not present)
String getPullStatus() {
  String status = "";
  if(digitalRead(pullA) == LOW){
    activatedA = false;
    status += "AX";
  }
  else{
    if(not activatedA){
      activateIndicatorLED("A","P");
    }
    activatedA = true;
    status += "AO";
  }
  if(digitalRead(pullB) == LOW){
    activatedB = false;
    status += "BX";
  }
  else{
    if(not activatedB){
      activateIndicatorLED("B","P");
    }
    activatedB = true;
    status += "BO";
  }
  if(digitalRead(pullC) == LOW){
    activatedC = false;
    status += "CX";
  }
  else{
    if(not activatedC){
      activateIndicatorLED("C","P");
    }
    activatedC = true;
    status += "CO";
  }
  if(digitalRead(pullD) == LOW){
    activatedD = false;
    status += "DX";
  }
  else{
    if(not activatedD){
      activateIndicatorLED("D","P");
    }
    activatedD = true;
    status += "DO";
  }
  return status;
}
