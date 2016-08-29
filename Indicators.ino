/*  Class to support blinking LEDs that will not interrupt main loop
    By Austin Langford
*/
class IndicatorLED {
  private:
    uint8_t _ledPin;
    boolean _ledOn;
    boolean _burned;
    boolean _pulled;

  public:
    IndicatorLED(uint8_t pin);
    int getPin();
    void setOn();
    void setOff();
    // Turn the led on for a given amount of time (relies
    // on a call to check() in the main loop()).
    void burned();
    void pulled();
    void burnCheck();
    void pullCheck();
    boolean isOn();
};
//============================================================================================================================

// Initialize LEDs
IndicatorLED burnerALED(33);
IndicatorLED burnerBLED(35);
IndicatorLED burnerCLED(37);
IndicatorLED burnerDLED(39);

// LED Variables
long timeCheck = 0;
long interval = 500;
boolean flash1 = true;
boolean burn = false;
boolean flash2 = true;
boolean pull = false;
boolean resetTimeCheck = false;
boolean on = true;
//============================================================================================================================

// Takes a string as an argument, and uses it to adjust the correct indicator LED
void activateIndicatorLED(String arg1, String arg2) {
  if (arg1 == "A") {
    if (arg2 == "B") {
      recordEvent("Turning on Burner A burn indicator light");
      burnerALED.burned();
    }
    else if (arg2 == "P") {
      recordEvent("Turning on Burner A pull indicator light");
      burnerALED.pulled();
    }
  }
  else if (arg1 == "B") {
    if (arg2 == "B") {
      recordEvent("Turning on Burner B burn indicator light");
      burnerBLED.burned();
    }
    else if (arg2 == "P") {
      recordEvent("Turning on Burner B pull indicator light");
      burnerBLED.pulled();
    }
  }
  else if (arg1 == "C") {
    if (arg2 == "B") {
      recordEvent("Turning on Burner C burn indicator light");
      burnerCLED.burned();
    }
    else if (arg2 == "P") {
      recordEvent("Turning on Burner C pull indicator light");
      burnerCLED.pulled();
    }
  }
  else if (arg1 == "D") {
    if (arg2 == "B") {
      recordEvent("Turning on Burner D burn indicator light");
      burnerDLED.burned();
    }
    else if (arg2 == "P") {
      recordEvent("Turning on Burner D pull indicator light");
      burnerDLED.pulled();
    }
  }
}
//============================================================================================================================

void indicateWithLEDs() {
  if (flash1) {
    if (on) {
      burnerALED.setOn();
      burnerBLED.setOn();
      burnerCLED.setOn();
      burnerDLED.setOn();
    }
    if (millis() - timeCheck > interval) {
      if (resetTimeCheck) {
        timeCheck = millis();
        resetTimeCheck = false;
      }
      burnerALED.setOff();
      burnerBLED.setOff();
      burnerCLED.setOff();
      burnerDLED.setOff();
      on = false;
    }
    if (millis() - timeCheck > interval) {
      timeCheck = millis();
      flash1 = false;
      burn = true;
      resetTimeCheck = true;
      on = true;
    }
  }
  if (burn) {
    if (on) {
      burnerALED.burnCheck();
      burnerBLED.burnCheck();
      burnerCLED.burnCheck();
      burnerDLED.burnCheck();
    }
    if (millis() - timeCheck > interval) {
      if (resetTimeCheck) {
        timeCheck = millis();
        resetTimeCheck = false;
      }
      burnerALED.setOff();
      burnerBLED.setOff();
      burnerCLED.setOff();
      burnerDLED.setOff();
      on = false;
    }
    if (millis() - timeCheck > interval) {
      timeCheck = millis();
      burn = false;
      flash2 = true;
      resetTimeCheck = true;
      on = true;
    }
  }
  if (flash2) {
    if (on) {
      burnerALED.setOn();
      delay(100);
      burnerBLED.setOn();
      delay(100);
      burnerCLED.setOn();
      delay(100);
      burnerDLED.setOn();
      delay(100);
    }
    if (millis() - timeCheck > interval) {
      if (resetTimeCheck) {
        timeCheck = millis();
        resetTimeCheck = false;
      }
      burnerALED.setOff();
      burnerBLED.setOff();
      burnerCLED.setOff();
      burnerDLED.setOff();
      on = false;
    }
    if (millis() - timeCheck > interval) {
      timeCheck = millis();
      flash2 = false;
      pull = true;
      resetTimeCheck = true;
      on = true;
    }
  }
  if (pull) {
    if (on) {
      burnerALED.pullCheck();
      burnerBLED.pullCheck();
      burnerCLED.pullCheck();
      burnerDLED.pullCheck();
    }
    if (millis() - timeCheck > interval) {
      if (resetTimeCheck) {
        timeCheck = millis();
        resetTimeCheck = false;
      }
      burnerALED.setOff();
      burnerBLED.setOff();
      burnerCLED.setOff();
      burnerDLED.setOff();
      on = false;
    }
    if (millis() - timeCheck > interval) {
      timeCheck = millis();
      pull = false;
      flash1 = true;
      resetTimeCheck = true;
      on = true;
    }
  }
}

// Constructor for flashing LED class
IndicatorLED::IndicatorLED(uint8_t ledPin) : _ledPin(ledPin) {
  pinMode(_ledPin, OUTPUT);
  _ledOn = false;
}

int IndicatorLED::getPin() {
  return _ledPin;
}

boolean IndicatorLED::isOn() {
  return _ledOn;
}

// Enables the LED
void IndicatorLED::pulled() {
  _pulled = true;
}

// Disables the LED
void IndicatorLED::burned() {
  _burned = true;
}

// Turns the LED on
void IndicatorLED::setOn() {
  if (not _ledOn) {
    digitalWrite(_ledPin, HIGH);
    _ledOn = true;
  }
}

// Turns the LED off
void IndicatorLED::setOff() {
  if (_ledOn) {
    digitalWrite(_ledPin, LOW);
    _ledOn = false;
  }
}


// Checks and toggles the LED state when needed
void IndicatorLED::burnCheck() {
  if (_burned) {
    setOn();
  }
}
void IndicatorLED::pullCheck() {
  if (_pulled) {
    setOn();
  }
  if (not _pulled) {
    setOff();
  }
}
void sync_LEDs() {
  burnerALED.setOff();
  burnerBLED.setOff();
  burnerCLED.setOff();
  burnerDLED.setOff();
}
