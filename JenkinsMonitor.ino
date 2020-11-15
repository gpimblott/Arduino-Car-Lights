/*
  Car light controller
  Controls the light sequences via high level commands sent via serial.
*/
#include <Arduino.h>
#include <SoftwareSerial.h>

#include "patterns.h"

#define numChannels 10
#define firstChannel 2
#define DEMO_MAX_LOOPS 2

#define BANNER "Car Light Controller"

// Setup the serial channel being used for control messages
SoftwareSerial controlSerial(12, 13); // RX, TX

byte byteRead;

// State information
typedef enum {
  STOPPED , STOPPING , STARTED
}
State_t;

// The state of the current sequence that is being played
State_t currentState = STOPPED;

// Create structures for custom light sequences that can be sent
Pattern customLeft [] = {{ {0,0,0,0,0}, 1000 }};
Pattern customRight [] = {{ {0,0,0,0,0}, 1000 }};
LightSequence customLeftSeq = {  "CUSTOM_L", 1 , customLeft };
LightSequence customRightSeq = {  "CUSTOM_R", 1 , customRight };

LightSequence *leftSeq;
LightSequence *rightSeq;
int nextLeftPattern = 0;
int nextRightPattern = 0;
unsigned long leftTime = 0;
unsigned long rightTime = 0;

int speed = 1;

// Buffer for serial commands
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// Prototypes for the methods
void displayPattern( );
void stopLights();
void startLights();
void controlSerialEvent();
void processSerialCmd(String command);
LightSequence *findSequence( String name );
void setSequenceByName( LightSequence **seq , String name );

boolean myCompare(const String str1 , const String str2 );

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  Serial.println(BANNER);

  // set the data rate for the SoftwareSerial port
  controlSerial.begin(9600);
  controlSerial.println(BANNER);
  inputString.reserve(100);

  // Set the default sequence and position
  leftSeq = &cylonSeq;
  rightSeq = &cylonRightSeq;
  nextLeftPattern = 0;
  nextRightPattern = 0;

  // initialize the pins as outputs
  for ( int i = firstChannel; i < (firstChannel + numChannels) ; i++ ) {
    pinMode(i, OUTPUT);
  }

  startLights();
}

// the loop routine runs over and over again forever:
void loop() {

  // check if any commands have been received
  //controlSerialEvent(Serial);
  controlSerialEvent(controlSerial);

  // process a command when new line is detected
  if (stringComplete) {
    processSerialCmd( inputString );

    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  switch ( currentState ) {
    case STARTED :
      displayPattern( );
      break;
    case STOPPING :
      stopLights();
      break;
    case STOPPED :
      break;
  }

}

/**
  Process commands
*/
void processSerialCmd(String command) {
  if ( command == "ON" ) {
    startLights();
    return;
  }
  else if ( command == "OFF" ) {
    stopLights();
    return;
  } else if ( command == "HELP" ) {

    for ( int i = 0 ; i < ARRAY_LENGTH(definedSequences) ; i++ ) {
      LightSequence *lseq = definedSequences[i];
      Serial.println(lseq->name);
    }

    Serial.print("Left: ");
    Serial.println(leftSeq->name);
    Serial.print("Right: ");
    Serial.println(rightSeq->name);
    return;
  }

  unsigned int pos = command.indexOf(':');
  if ( -1 == pos ) {
    return;
  }

  pos++;
  String params = command.substring(pos);

  if ( command.startsWith("SPEED:" ) ) {
    stopLights();
    speed = (unsigned long)params.toInt();
    startLights();
    Serial.print("Speed:");
    Serial.println(speed);
    return;
  }
  else if (command.startsWith("SEQ:") ) {
    int pos2 = params.indexOf(' ');
    String seq1Str;
    String seq2Str;

    if ( -1 == pos2 ) {
      seq1Str = params;
      seq2Str = params;
    } else {
      seq1Str = params.substring(0, pos2);
      seq2Str = params.substring(pos2);
    }

    // Restart the sequences
    stopLights();
    setSequenceByName( &leftSeq, seq1Str );
    setSequenceByName( &rightSeq, seq2Str );
    startLights();
  }
  else if (command.startsWith("SEQR:") ) {
    // Restart the sequences
    stopLights();
    setSequenceByName( &rightSeq, params );
    startLights();
  }
  else if (command.startsWith("SEQL:") ) {
    // Restart the sequences
    stopLights();
    setSequenceByName( &leftSeq, params );
    startLights();
  } 
  else if (command.startsWith("BIN:") ) {
    setSequenceByBits(params);
    
    // Restart the sequences
    stopLights();
    leftSeq = &customLeftSeq;
    rightSeq = &customRightSeq;
    startLights();

    //outputPattern( leftSeq );
    //outputPattern( rightSeq );
  }
  else {
    Serial.println("Unrecognised command");
  }
}

// Process some bit patterns
void setSequenceByBits( String params ) {
  params.trim();
  int numChars = params.length();
  for(int pos=0,index=0;pos<numChars;pos++) {
    char theChar = params.charAt(pos);
    if( theChar !='0' && theChar !='1') {
      continue;
    }
    boolean value = (theChar=='1');

    if( index<5) {
      customLeft[0].bits[index] = value;
    } else {
      customRight[0].bits[index-5] = value;
    }
    index++;
  }
}

void setSequenceByName( LightSequence **seq , String name ) {
  if ( name.length() == 0 ) {
    return;
  }
  name.trim();

  LightSequence *foundSeq = findSequence( name );
  if ( NULL != foundSeq ) {
    *seq = foundSeq;
  }
}

/*
  Find the sequence with the given name
*/
LightSequence *findSequence( String name ) {
  for ( int i = 0 ; i < ARRAY_LENGTH(definedSequences) ; i++ ) {
    LightSequence *lseq = definedSequences[i];

    if ( myCompare(name , lseq->name) ) {
      return lseq;
    }
  }
  controlSerial.println("Sequence not Found");
  return NULL;
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void controlSerialEvent(Stream & channel) {
  while (channel.available()) {

    // get the new byte:
    char inChar = (char)channel.read();
    inputString += (char)(toupper(inChar));

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if ((inChar == '\n') || (inChar == '\r')) {
      inputString.trim();
      stringComplete = true;
    }
  }
}

// Start the lights from the beginning
void startLights() {
  nextLeftPattern = 0;
  nextRightPattern = 0;
  currentState = STARTED;
  leftTime = rightTime = millis();
}

// Stop the light sequences if they are running
void stopLights() {
  currentState = STOPPED;
  for ( int j = 0; j < numChannels ; j++) {
    digitalWrite(j + firstChannel, false );
  }
  nextLeftPattern = 0;
  nextRightPattern = 0;
}

// Display the next step in the current pattern
void displayPattern( ) {
  Pattern *leftPattern = leftSeq->pattern;
  Pattern *rightPattern = rightSeq->pattern;

  // use the same base time for both sequences so that they stay in step
  unsigned long currentTime = millis();

  if ( currentTime > leftTime ) {
    // Display the left pattern
    Pattern pl = leftPattern[nextLeftPattern++];
    if ( nextLeftPattern >= leftSeq->items ) {
      nextLeftPattern = 0;
    }

    for ( int j = 0; j < 5 ; j++) {
      digitalWrite(j + firstChannel, pl.bits[j]);
    }

    leftTime = currentTime + ((pl.delay ) * speed);
  }

  if ( currentTime > rightTime ) {
    // Display the right pattern
    Pattern pr = rightPattern[nextRightPattern++];
    if ( nextRightPattern >= rightSeq->items ) {
      nextRightPattern = 0;
    }

    for ( int j = 0; j < 5 ; j++) {
      digitalWrite(j + firstChannel + 5, pr.bits[j]);
    }

    rightTime = currentTime + ((pr.delay ) * speed);
  }
}

boolean myCompare(const String str1 , const String str2 ) {

  if ( str1.length() != str2.length() ) {
    return false;
  }

  for (int i = 0; i < str1.length(); i++) {
    if ( str2[i] != str1[i] ) {
      return false;
    }
  }

  return true;
}

void outputPattern( LightSequence *sequence ) {
  Serial.print("Debug - ");
  Serial.println( sequence->name );

  for(int x=0;x<5;x++) {
    Serial.print( sequence->pattern->bits[x] );
    Serial.print( " ");
  }

  Serial.println();
  
  
}
