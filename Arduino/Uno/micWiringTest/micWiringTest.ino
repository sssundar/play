// Intended to be a test of whether the mic could discriminate a click
// at 450Hz from an onboard speaker (to see whether it could be used for
// ranging). 

// Demonstrated some sort of mic wiring issue; still debugging.

int spk = 3;  
int adc0 = 0;

void setup()  { 
  Serial.begin(9600);
  pinMode(spk, OUTPUT);   // PWM 500 Hz
  analogWrite(spk, 220);  // ~ 450 Hz
} 

void loop()  { 
  adc0 = analogRead(A0);
  Serial.print(adc0, DEC); // 0-1023
  Serial.print("\n");
  delay(30); // ms
}

