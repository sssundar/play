/*
 -- Motivation --
 My eventual goal is to use echolocation to detect whether I am sitting at my desk for periods 
 of longer than 30 minutes at a stretch.

 For now I just wanted to get familiar with Arduino, its IDE, its power capacity, its I/O voltage specs,
 and the quality & speed of its ADC, because I've only handled FPGAs and 8088s before this. 

 So, for now...
 
 -- Description --
 Burst 100 samples of ambient sound for every 1 second, for 6 seconds. For each burst, the maximum sample is level shifted and
 the absolute value is computed. This is thresholded, and an incrementor remembers how many of the 1-second interval bursts in the set
 were above the thresold.
 
 If this number is more than 50% of the possible intervals (6),
 the program fades an LED in and out rapidly until the system is reset.
 
 -- Analog Pre-amp --
 An electret mic sampling ambient sounds is amplified 200x using an LM386N-1.
 The electret is powered using a 2.2k resistors from Vcc which connects to another 2.2k to the positive terminal of the electret, and also to a 10uF electrolytic cap bypassing to GND .
 The other terminal of the electret goes to GND.
 The electret positive terminal is fed into a 10uF electrolytic coupling cap in series with a 100k to GND. 
 At the node intermediate to these two, we link to the negative input of the op-amp.
 The positive input of the op-amp goes to GND, and the op-amp is powered from 0-Vcc, from the Arduino.
 Pins 1/8 of the LM386N-1 are linked by a 10nF tantalum cap to boost gain to about 200x.
 The output of the pre-amp is level shifted using an even 2k resistive divider and a coupling cap (10nF tantalum) to about 2.5 V DC.
 The measured AC amplitude for loud sounds is roughly 40mV in amplitude. Plenty for what we need.
 
 -- Analog Input --
 The on-board ADC samples the level shifted output of the pre-amp on Pin A0.

 -- Digital PWM Output --
 The on-board PWM controller outputs on Pin 9 to fade an LED grounded by a 1k resistor.
  
 -- Target Board --
 Arduino Uno
 
 -- Revision History --
 Revised by Sushant Sundaresh on 10 October 2015.
  Notes:
    Built pre-amp
    Wrote script
    Debugged (int is 16 bits)
    Serial.println is not enough - need debugger for scalable code. Atmel Studio!
    Tested for typing & youtube - works for both. 
    Need to reshape flow to include burst buffering & to count intervals, not time
    Need to tweak for other ambient sounds; youtube doesn't flag as often as typing.
 
 -- References --
  I cobbled together much of this code from the following sources:
    Example | Fade
    Sampling | https://www.arduino.cc/en/Tutorial/ReadAnalogVoltage
    Seeing Samples | http://electronics.stackexchange.com/questions/54/saving-arduino-sensor-data-to-a-text-file      
*/





// Constants //

// Control Flags //
int ledBlinkFlag = 0;       
int shouldLogSamples = 0;          // must be set manually, on upload
int shouldLogDebugFlags = 0;       // send info like 'crossed threshold' to serial logger

// LED Fade Constants //
int led = 9;                       // the pin that the LED is attached to
int brightness = 0;                // how bright the LED is
int fadeAmount = 5;                // how many points to fade the LED by

// Sampling & Thresholding Constants
int sensorValue = 0;                 // allocate for the sensor stream

int msTillNextInterval = 1000;       // ms to wait until next sampling burst
int msIntoThisInterval = 0;          // counter 

int samplesPerBurst = 100;           // burst samples per interval (at max rate, roughly 10kHz for a 16MHz UNO)
int whereInThisBurst = 0;            // counter
int maxSampleThisBurst = 0;          // the maximum sample from the current burst

int defaultDelay = 50;               // delay if we're just supposed to be waiting. not exact.

int intervalsPerCtrlCycle = 6;       // msTillEvalCtrlFlags must fit in 16 bits (for now)
                                     // the number of ms till the ledBlinkFlag is re-evaluated
int intervalCount = 0;               // counter                                    
int numAboveThresholdThisCtrlCycle = 0; // the number of samples > threshold this control cycle
int cutoffSamplesAboveThreshold = 3;    // if more than 50 % of the intervals have a typing sound, I'm typing.

int msTillEvalCtrlFlags = intervalsPerCtrlCycle * msTillNextInterval;                                        
int timeInCtrlCycle = 0;             // counter

int levelShift = 510;                // base level from ADC is about 510. level shift and take magnitude of input data.
int threshold = 6;                   // This is the residual level past which sound is 'too loud,' an indication I am nearby
                                     // and typing on my computer. See attached TypingSoundLevels.jpeg for a graph of levels while I type.




 
// Program //

void clearStateForNextControlCycle () {  
  timeInCtrlCycle = 0;
  numAboveThresholdThisCtrlCycle = 0;
  intervalCount = 0;
  msIntoThisInterval = 0;   
  
  whereInThisBurst = 0;
  maxSampleThisBurst = 0;    
}

void clearStateForNextInterval () {    
  msIntoThisInterval = 0;   
  whereInThisBurst = 0;
  maxSampleThisBurst = 0;
}

// Setup Runs on Reset //
void setup() {
 // initialize serial communication at 9600 bits per second:
 Serial.begin(9600);
 
 // declare pin 9 to be an output:
 pinMode(led, OUTPUT);

 // clear led  
 analogWrite(led, 0);

 // reset control flag
 ledBlinkFlag = 0;

 // reset all state
 clearStateForNextControlCycle(); 
}

// Main Loop //
void loop() {      
  if (ledBlinkFlag == 0) {        
    // The control cycle is still active    
    if (timeInCtrlCycle > msTillEvalCtrlFlags) {      
      if (numAboveThresholdThisCtrlCycle >= cutoffSamplesAboveThreshold) {
        // I was typing in this control cycle
        ledBlinkFlag = 1;
      } else {
        clearStateForNextControlCycle(); 
      }      
    } else {      
      // I am either in an interval, idling or bursting

      if (msIntoThisInterval > msTillNextInterval) {        
        clearStateForNextInterval();
      } else {                
        if (whereInThisBurst == samplesPerBurst) {
          // update threshold-crossings, just finished burst
          intervalCount = intervalCount + 1;
          if (maxSampleThisBurst >= threshold) {
            numAboveThresholdThisCtrlCycle = numAboveThresholdThisCtrlCycle + 1;            
            if (shouldLogDebugFlags == 1) {
               Serial.println("-----------------------------------");  
               Serial.println("Sound threshold crossed. For this control cycle, this is crossing #");  
               Serial.println(numAboveThresholdThisCtrlCycle);  
               Serial.println("We are currently at Interval #");
               Serial.println(intervalCount);  
               Serial.println("-----------------------------------");  
            }
          }          
          whereInThisBurst = whereInThisBurst + 1;
        } 
        
        if (whereInThisBurst > samplesPerBurst) {
          // idling, just delay and add to all timers
          delay(defaultDelay);
          msIntoThisInterval = msIntoThisInterval + defaultDelay;
          timeInCtrlCycle = timeInCtrlCycle + defaultDelay;
        } else {
            // burst sampling
            // This should be restructured to simply store to a buffer, then process later.
            // For now it's fine for testing.
            while (whereInThisBurst < samplesPerBurst) {
              // read the input on analog pin 0:
              sensorValue = analogRead(A0);                
              sensorValue = sensorValue - levelShift;
              if (sensorValue < 0) {
                sensorValue = 0 - sensorValue;
              }
              if (sensorValue > maxSampleThisBurst) {
                maxSampleThisBurst = sensorValue;
              }
              if (shouldLogSamples == 1) {                
                Serial.println(sensorValue);  
              }            
              whereInThisBurst = whereInThisBurst + 1;   
            }               
        }        
      }
    }     
  } else {
    // Blink the LED until a reset    
    // set the brightness of pin 9:
    analogWrite(led, brightness);
  
    // change the brightness for next time through the loop:
    brightness = brightness + fadeAmount;
  
    // reverse the direction of the fading at the ends of the fade:
    if (brightness == 0 || brightness == 255) {
      fadeAmount = -fadeAmount ;
    }
    // wait for 5 milliseconds to see the fast faded blinking effect
    delay(5);  
  }
}
