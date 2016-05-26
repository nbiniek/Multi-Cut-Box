/*
  Multicut Box (Timed)
  By Noah Biniek
      
      This program is designed to work with four multicut modules. Each module will fire after
      heat up nichrome wires in order to cut away strings at set times.

      Compatible with Arduino Uno
      
 */


//  assign pinouts  //
const int led = 13; // arduino led

const int burnerA = 10, burnerB = 12, burnerC = 11, burnerD = 9;
const int pullA = 0, pullB = 1, pullC = 4, pullD = 8;

const int startPull = 7;


bool runLoop = true;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pins as output
  pinMode(led, OUTPUT);
  pinMode(burnerA, OUTPUT);
  pinMode(burnerB, OUTPUT);
  pinMode(burnerC, OUTPUT);
  pinMode(burnerD, OUTPUT);
  pinMode(pullA, INPUT);
  pinMode(pullB, INPUT);
  pinMode(pullC, INPUT);
  pinMode(pullD, INPUT);
  pinMode(startPull, INPUT);
}

void fireBurner(int burnerPin, int pullPin, int numberOfAttempts) {
  if(numberOfAttempts > 0) {
      delay(30000);                   //  recharge capacitors for thirty seconds
      digitalWrite(burnerPin, HIGH);  //  fire burner
      delay(2000);                    //  wait for two seconds
      digitalWrite(burnerPin, LOW);   //  fire burner
      delay(5000);                    //  wait for the weight to fall and pull on the circuit for five seconds
      
      if (digitalRead(pullPin) == LOW) fireBurner(burnerPin, pullPin, numberOfAttempts-1);  //  if pullPin is not released, recurse burn
  }
}

void loop() {

    //  Standby blink Arduino LED 13 //
    
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);              // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);              // wait for a second
    
    if (runLoop) {
        if (digitalRead(startPull) == LOW) {
          digitalWrite(led, HIGH);      //  led is on while operational
          
           fireBurner(burnerA, pullA, 2);
           fireBurner(burnerB, pullB, 2);
           fireBurner(burnerC, pullC, 2);
           fireBurner(burnerD, pullD, 2);

           runLoop = false;
        }
    }
}

