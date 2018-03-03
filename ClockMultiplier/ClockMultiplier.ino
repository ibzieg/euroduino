/*
  Arduino EuroRack Kit #1
 
 Dual LFO
 
 
 created May 22, 2014
 Tim Ressel
 
 This example code is in the public domain.
 
 */

// These constants won't change.  They're used to give names
// to the pins used:
const int analogIn1Pin   = A0; // Analog Input 1
const int analogIn2Pin   = A1; // Analog Input 2
const int analogPot1Pin  = A2; // Pot 1
const int analogPot2Pin  = A3; // Pot 2
const int analogOut1Pin  = 5;  // Analog Output 1
const int analogOut2Pin  = 6;  // Analog Output 2
const int DigitalIn1Pin  = 8;  // Digital Input 1
const int DigitalIn2Pin  = 9;  // Digital Input 2
const int DigitalOut1Pin = 3;  // Digital Output 1
const int DigitalOut2Pin = 4;  // Digital Output 2
const int Switch1Up      = A4; // Switch 1 Up
const int Switch1Dwn     = A5; // Switch 1 Dwn
const int Switch2Up      = 7;  // Switch 2 Up
const int Switch2Dwn     = 2;  // Switch 2 Dwn
    
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


unsigned long cyclesPerBeat;
unsigned long cyclesSinceLastClkEdge;
unsigned long cyclesSinceLastOutEdge;
unsigned long lastClkState;

void setup() {
  /* Set up I/O pins */
  pinMode(DigitalIn1Pin,  INPUT);
  pinMode(DigitalIn2Pin,  INPUT);
  pinMode(DigitalOut1Pin, OUTPUT); 
  pinMode(DigitalOut2Pin, OUTPUT); 
  pinMode(Switch1Up,      INPUT_PULLUP);
  pinMode(Switch1Dwn,     INPUT_PULLUP);
  pinMode(Switch2Up,      INPUT_PULLUP);
  pinMode(Switch2Dwn,     INPUT_PULLUP);

  cyclesPerBeat          = 0;
  cyclesSinceLastClkEdge = 0;
  cyclesSinceLastOutEdge = 0;
  lastClkState           = LOW;

}

unsigned long DCO1;
unsigned long Inc1;
unsigned char Out1;
unsigned char ch1_ClkIn;
unsigned char ch1_TriSw;

unsigned long DCO2;
unsigned long Inc2;
unsigned char Out2;
unsigned char Rst2;
unsigned char Sw2;


void loop() {
	////////////////////////////////
	// Clock Multiplier

	// 1M cycles is 1/16sec at 16Mhz cycle speed 
	int edgeLength = 4; 

	if (cyclesPerBeat < 0) {
		cyclesPerBeat = 1;
	}


	////
	// Determine the Clock Multiplier Factor

	int clockMultFactor = 2; /* 2, 4, 8 */

    // Read Switch 1
    if (digitalRead(Switch1Up) == LOW) {
    	// Up
    	clockMultFactor = 2;
    } else if (digitalRead(Switch1Dwn) == LOW) {
    	// Down
    	clockMultFactor = 8;
    } else {
    	// Center
    	clockMultFactor = 4;
    }

    ////
    // Read Clock
    ch1_ClkIn = digitalRead(DigitalIn1Pin);

    if (ch1_ClkIn != lastClkState &&
    	ch1_ClkIn == HIGH) {
    	// Edge event

    	// Capture cycles per beat
    	cyclesPerBeat = cyclesSinceLastClkEdge;

    	cyclesSinceLastClkEdge = 0;
        cyclesSinceLastOutEdge = 0;

    } else {

    	unsigned long cyclesPerMultiple = cyclesPerBeat / clockMultFactor;

    	if ((cyclesSinceLastClkEdge % cyclesPerMultiple) == 0) {
    		// Interval has passed for us to send another multiple
    		cyclesSinceLastOutEdge = 0;
    	}
    }

    if (cyclesSinceLastOutEdge < edgeLength) {
   		digitalWrite(DigitalOut1Pin,HIGH);
   	} else {
   		digitalWrite(DigitalOut1Pin,LOW);   		
   	}

   	cyclesSinceLastClkEdge++;
   	cyclesSinceLastOutEdge++;
   	lastClkState = ch1_ClkIn;

   	//digitalWrite(DigitalOut1Pin, ch1_ClkIn);
  
  // LFO #2:
  // read dig input (reset in)
  Rst2 = digitalRead(DigitalIn2Pin);
  // read the value from the input
  Inc2 = analogRead(analogIn2Pin);    
  // read the value from the input
  Inc2 += analogRead(analogPot2Pin);  
  // add offset for non-zero freq at lowest setting
  Inc2 += 1;
  // update the DCO
  DCO2 += (Inc2*65536);  
  // process reset
  if (Rst2 == HIGH)
  {
    DCO2 = 0xFFFFFFFF;
  }
  // update output
  Out2 = DCO2 >> 24;
  // process switches
  if (digitalRead(Switch2Up)==LOW)
  {
    Out2 = sines[Out2];
  }
  if (digitalRead(Switch2Dwn)==LOW)
  {
    if (Out2 > 127)
    {
      Out2 = 255-Out2;
    }
    Out2 = Out2<<1;
  }
  analogWrite(analogOut2Pin, Out2);
  if (Out2>127)
  {
    digitalWrite(DigitalOut2Pin,HIGH);
  }
  else
  {
    digitalWrite(DigitalOut2Pin,LOW);
  }

  delay(2);
}




