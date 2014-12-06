/*
Jenkins Monitor
Light Sequencer that responds to Jenkins build states
*/
#include <Arduino.h>
#include <SoftwareSerial.h>

#include "patterns.h"

#define numChannels 10
#define firstChannel 2
#define DEMO_MAX_LOOPS 2

#define JENKINS_BUILD_MONITOR "Jenkins Build Monitor"

// Setup the bluetooth software serial
SoftwareSerial btSerial(12, 13); // RX, TX

byte byteRead;

// State information
typedef enum {
	STOPPED , STOPPING , STARTED
}
State_t;

// The state of the current sequence that is being played
State_t currentState = STOPPED;

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
void btSerialEvent();
void processBTCmd(String command);
LightSequence *findSequence( String name );
void setSequence( LightSequence **seq , String name );

boolean myCompare(const String str1 , const String str2 );

// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	Serial.println(JENKINS_BUILD_MONITOR);

	// set the data rate for the SoftwareSerial port
	btSerial.begin(9600);
	btSerial.println(JENKINS_BUILD_MONITOR);
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

	//  check if data has been received over BT
	btSerialEvent();

	// print the string when a newline arrives:
	if (stringComplete) {
		processBTCmd( inputString );

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
* Process BT commands
*/
void processBTCmd(String command) {
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
			btSerial.println(lseq->name);
		}
		
		btSerial.print("Left: ");
		btSerial.println(leftSeq->name);
		btSerial.print("Right: ");
		btSerial.println(rightSeq->name);
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
		setSequence( &leftSeq, seq1Str );
		setSequence( &rightSeq, seq2Str );
		startLights();
	}
	else if (command.startsWith("SEQR:") ) {
		
		// Restart the sequences
		stopLights();
		setSequence( &rightSeq, params );
		startLights();
	}
	else if (command.startsWith("SEQL:") ) {
		
		// Restart the sequences
		stopLights();
		setSequence( &leftSeq, params );
		startLights();
	} else {
		Serial.println("Unrecognised command");
	}

}

void setSequence( LightSequence **seq , String name ) {
	if( name.length() == 0 ) {
		return;
	}
	name.trim();
	
	LightSequence *foundSeq = findSequence( name );
	if ( NULL != foundSeq ) {
		*seq = foundSeq;
	}
}

/*
* Find the sequence with the given name
*/
LightSequence *findSequence( String name ) {
	for ( int i = 0 ; i < ARRAY_LENGTH(definedSequences) ; i++ ) {
		LightSequence *lseq = definedSequences[i];

		if( myCompare(name , lseq->name) ) {
			return lseq;
		}
	}
	btSerial.println("Sequence not Found");
	return NULL;
}

/*
SerialEvent occurs whenever a new data comes in the
hardware serial RX.  This routine is run between each
time loop() runs, so using delay inside loop can delay
response.  Multiple bytes of data may be available.
*/
void btSerialEvent() {
	while (btSerial.available()) {

		// get the new byte:
		char inChar = (char)btSerial.read();
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

/*	Serial.print(str1);
	Serial.print("=");
	Serial.print(str2);
	Serial.println(":");
 */
	if( str1.length() != str2.length() ) {
		return false;
	}
	
	for(int i=0;i<str1.length();i++) {
		if( str2[i] != str1[i] ) {
			return false;
		}
	}
	
	return true;
}

