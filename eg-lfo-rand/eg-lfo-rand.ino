/*
  Arduino EuroRack Kit #1
 
 Dual LFO
 
 
 created May 22, 2014
 Tim Ressel
 
 This example code is in the public domain.
 
 */

// These constants won't change.  They're used to give names
// to the pins used:
const int analogIn1Pin = A0;  // Analog Input 1
const int analogIn2Pin = A1; // Analog Input 2
const int analogPot1Pin = A2;  // Pot 1
const int analogPot2Pin = A3; // Pot 2
const int analogOut1Pin = 5;  // Analog Output 1
const int analogOut2Pin = 6; // Analog Output 2
const int DigitalIn1Pin = 8;  // Digital Input 1
const int DigitalIn2Pin = 9;  // Digital Input 2
const int DigitalOut1Pin = 3;  // Digital Output 1
const int DigitalOut2Pin = 4;  // Digital Output 2
const int Switch1Up = A4;  // Switch 1 Up
const int Switch1Dwn = A5;  // Switch 1 Dwn
const int Switch2Up = 7;  // Switch 2 Up
const int Switch2Dwn = 2;  // Switch 2 Dwn

const unsigned char sines[] = {128,131,134,137,140,143,146,149,152,156,159,162,165,168,171,174,
                              176,179,182,185,188,191,193,196,199,201,204,206,209,211,213,216,
                              218,220,222,224,226,228,230,232,234,235,237,239,240,242,243,244,
                              246,247,248,249,250,251,251,252,253,253,254,254,254,255,255,255,
                              255,255,255,255,254,254,253,253,252,252,251,250,249,248,247,246,
                              245,244,242,241,239,238,236,235,233,231,229,227,225,223,221,219,
                              217,215,212,210,207,205,202,200,197,195,192,189,186,184,181,178,
                              175,172,169,166,163,160,157,154,151,148,145,142,138,135,132,129,
                              126,123,120,117,113,110,107,104,101,98,95,92,89,86,83,80,77,74,
                              71,69,66,63,60,58,55,53,50,48,45,43,40,38,36,34,32,30,28,26,24,
                              22,20,19,17,16,14,13,11,10,9,8,7,6,5,4,3,3,2,2,1,1,0,0,0,0,0,0,
                              0,1,1,1,2,2,3,4,4,5,6,7,8,9,11,12,13,15,16,18,20,21,23,25,27,29,
                              31,33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,79,81,
                              84,87,90,93,96,99,103,106,109,112,115,118,121,124,127 };


unsigned char ch1_LastGateState;
unsigned char ch1_GateState;
unsigned long ch1_Value;
unsigned long ch1_AnalogOutput;
unsigned long ch1_DigitalOutput;
unsigned long ch1_PotValue;
unsigned long ch1_CVInput;
unsigned long ch1_CV;
unsigned long ch1_CyclesSinceReset;
unsigned long ch1_ResetClockCount;
unsigned long ch1_CyclesSinceRandPulse;
unsigned long ch1_RandPulseDiv;

unsigned char ch2_LastGateState;
unsigned char ch2_GateState;
unsigned long ch2_Value;
unsigned long ch2_AnalogOutput;
unsigned long ch2_DigitalOutput;
unsigned long ch2_PotValue;
unsigned long ch2_CVInput;
unsigned long ch2_CV;
unsigned long ch2_CyclesSinceReset;
unsigned long ch2_ResetClockCount;
unsigned long ch2_CyclesSinceRandPulse;
unsigned long ch2_RandPulseDiv;

unsigned long DCO1;
unsigned long DCO2;
unsigned long Inc1;
unsigned long Inc2;
unsigned char Out1;
unsigned char Out2;
unsigned char Rst1;
unsigned char Rst2;
unsigned char Sw1;
unsigned char Sw2;



void setup() {
  /* Set up I/O pins */
  pinMode(DigitalIn1Pin, INPUT);
  pinMode(DigitalIn2Pin, INPUT);
  pinMode(DigitalOut1Pin, OUTPUT); 
  pinMode(DigitalOut2Pin, OUTPUT); 
  pinMode(Switch1Up, INPUT_PULLUP);
  pinMode(Switch1Dwn, INPUT_PULLUP);
  pinMode(Switch2Up, INPUT_PULLUP);
  pinMode(Switch2Dwn, INPUT_PULLUP);

  ch1_LastGateState = LOW;
  ch1_CyclesSinceReset = 1;
  ch1_ResetClockCount = 1;
  ch1_CyclesSinceRandPulse = 1;

  ch2_LastGateState = LOW;
  ch2_CyclesSinceReset = 1;
  ch2_ResetClockCount = 1;
  ch2_CyclesSinceRandPulse = 1;

  randomSeed(420);
}



void loop() {

    ch1_GateState = digitalRead(DigitalIn1Pin);
    ch1_CVInput = analogRead(analogIn1Pin);    
    ch1_PotValue = analogRead(analogPot1Pin);  
    ch1_CV = ch1_CVInput + ch1_PotValue + 1;

    if (digitalRead(Switch1Up) == LOW) {
      ////////////////////////////////
      // Up: Envelope Generator
      ////////////////////////////////

      // Update Value
      if (ch1_Value > ch1_CV*65536) {
        ch1_Value -= (ch1_CV*65536);  
      } else {
        ch1_Value = 0;
      }

      // Detect Reset Event
      if (ch1_GateState == HIGH && ch1_LastGateState == LOW) {
        ch1_Value = 0xFFFFFFFF;
      }

      // Update Outputs
      ch1_AnalogOutput = 255 - (ch1_Value >> 24);
      if (ch1_AnalogOutput == 255) {
        ch1_DigitalOutput = HIGH;
      } else {
        ch1_DigitalOutput = LOW;
      }

    } else if (digitalRead(Switch1Dwn) == LOW) {
      ////////////////////////////////
      // Down: Random
      ////////////////////////////////

      // Detect reset event
      if (ch1_GateState == HIGH && ch1_LastGateState == LOW) {
        ch1_AnalogOutput = random(1,255);
        ch1_ResetClockCount = ch1_CyclesSinceReset;
        ch1_CyclesSinceReset = 0;
        ch1_CyclesSinceRandPulse = 0;
      } else {
        ch1_CyclesSinceReset++;
        ch1_CyclesSinceRandPulse++;
      }

      // Toggle the random pulse
      if (ch1_CyclesSinceRandPulse >= ch1_RandPulseDiv) {
        if (ch1_DigitalOutput = HIGH) {
          ch1_DigitalOutput = LOW;
        } else {
          ch1_DigitalOutput = HIGH;
        }
        ch1_CyclesSinceRandPulse = 0;
        ch1_RandPulseDiv = ch1_ResetClockCount / (2^random(1,4));
      }

    } else {
      ////////////////////////////////
      // Center: LFO
      ////////////////////////////////

      // Update Value
      ch1_Value += (ch1_CV*65536);  

      // Detect reset event
      if (ch1_GateState == HIGH && ch1_LastGateState == LOW) {
        ch1_Value = 0xFFFFFFFF;
      }

      // Update Outputs
      ch1_AnalogOutput = ch1_Value >> 24;
      if (ch1_AnalogOutput > 127) {
        ch1_AnalogOutput = 255-ch1_AnalogOutput;
      }
      ch1_AnalogOutput = ch1_AnalogOutput<<1;

      if (ch1_AnalogOutput > 127) {
        ch1_DigitalOutput = HIGH;
      } else {
        ch1_DigitalOutput = LOW;
      }

    }

    // Write outputs
    ch1_LastGateState = ch1_GateState;
    analogWrite(analogOut1Pin, ch1_AnalogOutput);
    digitalWrite(DigitalOut1Pin,ch1_DigitalOutput);
  
  ////////////////////////////////////

   ch2_GateState = digitalRead(DigitalIn2Pin);
    ch2_CVInput = analogRead(analogIn2Pin);    
    ch2_PotValue = analogRead(analogPot2Pin);  
    ch2_CV = ch2_CVInput + ch2_PotValue + 1;

    if (digitalRead(Switch2Up) == LOW) {
      ////////////////////////////////
      // Up: Envelope Generator
      ////////////////////////////////

      // Update Value
      if (ch2_Value > ch2_CV*65536) {
        ch2_Value -= (ch2_CV*65536);  
      } else {
        ch2_Value = 0;
      }

      // Detect Reset Event
      if (ch2_GateState == HIGH && ch2_LastGateState == LOW) {
        ch2_Value = 0xFFFFFFFF;
      }

      // Update Outputs
      ch2_AnalogOutput = 255 - (ch2_Value >> 24);
      if (ch2_AnalogOutput == 255) {
        ch2_DigitalOutput = HIGH;
      } else {
        ch2_DigitalOutput = LOW;
      }

    } else if (digitalRead(Switch2Dwn) == LOW) {
      ////////////////////////////////
      // Down: Random
      ////////////////////////////////

      // Detect reset event
      if (ch2_GateState == HIGH && ch2_LastGateState == LOW) {
        ch2_AnalogOutput = random(1,255);
        ch2_ResetClockCount = ch2_CyclesSinceReset;
        ch2_CyclesSinceReset = 0;
        ch2_CyclesSinceRandPulse = 0;
      } else {
        ch2_CyclesSinceReset++;
        ch2_CyclesSinceRandPulse++;
      }

      // Toggle the random pulse
      if (ch2_CyclesSinceRandPulse >= ch2_RandPulseDiv) {
        if (ch2_DigitalOutput = HIGH) {
          ch2_DigitalOutput = LOW;
        } else {
          ch2_DigitalOutput = HIGH;
        }
        ch2_CyclesSinceRandPulse = 0;
        ch2_RandPulseDiv = ch2_ResetClockCount / (2^random(1,4));
      }

    } else {
      ////////////////////////////////
      // Center: LFO
      ////////////////////////////////

      // Update Value
      ch2_Value += (ch2_CV*65536);  

      // Detect reset event
      if (ch2_GateState == HIGH && ch2_LastGateState == LOW) {
        ch2_Value = 0xFFFFFFFF;
      }

      // Update Outputs
      ch2_AnalogOutput = ch2_Value >> 24;
      if (ch2_AnalogOutput > 127) {
        ch2_AnalogOutput = 255-ch2_AnalogOutput;
      }
      ch2_AnalogOutput = ch2_AnalogOutput<<1;

      if (ch2_AnalogOutput > 127) {
        ch2_DigitalOutput = HIGH;
      } else {
        ch2_DigitalOutput = LOW;
      }

    }

    // Write outputs
    ch2_LastGateState = ch2_GateState;
    analogWrite(analogOut2Pin, ch2_AnalogOutput);
    digitalWrite(DigitalOut2Pin,ch2_DigitalOutput);
  

  delay(2);
}



