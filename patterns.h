
#ifndef Patterns_h_
#define Patterns_h_

#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))

// The structure that holds a single pattern
typedef struct Patterns {
  boolean bits[5];
  int delay;
} Pattern;

typedef struct LightSequences {
  String name;
  int items;
  Pattern *pattern;
} LightSequence;

/**
 * Define the default patterns
 */
Pattern fullBlink [] = {
  { {1,1,1,1,1}, 1000 },
  { {0,0,0,0,0}, 1000 }
};

LightSequence fullBlinkSeq = { 
        "BLINK", 
        ARRAY_LENGTH(fullBlink) , fullBlink };

/**
 * Define the default patterns
 */
Pattern allOn [] = {
  { {1,1,1,1,1}, 1000 }
};

LightSequence allOnSeq = { 
        "ALL", 
        ARRAY_LENGTH(allOn) , allOn };

// Hazards
Pattern fhazards [] = {
  { {1,0,0,0,0}, 1000 },
  { {0,0,0,0,0}, 500 }
};
LightSequence fhazardsSeq = { "FHAZARDS", ARRAY_LENGTH(fhazards) , fhazards };

Pattern hazards [] = {
  { {1,0,0,0,0}, 1000 },
};
LightSequence hazardsSeq = { "HAZARDS", ARRAY_LENGTH(hazards) , hazards };

// Reverse
Pattern freverse [] = {
  { {0,0,0,0,1}, 1000 },
  { {0,0,0,0,0} , 500 }
};
LightSequence freverseSeq = { "FREVERSE" , ARRAY_LENGTH(freverse) , freverse };

Pattern reverse [] = {
  { {0,0,0,0,1}, 1000 }
};
LightSequence reverseSeq = { "REVERSE" , ARRAY_LENGTH(reverse) , reverse };


// Breaks
Pattern fbreaks [] = {
  { {0,1,1,1,0} ,1000 },
  { {0,0,0,0,0}, 500 }
};
LightSequence fbreaksSeq = { "FBREAKS" , ARRAY_LENGTH(fbreaks) , fbreaks };

Pattern breaks [] = {
  { {0,1,1,1,0} ,1000 },
};
LightSequence breaksSeq = { "BREAKS" , ARRAY_LENGTH(breaks) , breaks };

Pattern breaksToggle [] = {
  { {0,1,0,1,0} ,1000 },
  { {0,0,1,0,0} ,1000 },
};
LightSequence breaksToggleSeq = { "BREAKSTOGGLE" , ARRAY_LENGTH(breaksToggle) , breaksToggle };

// Cylon Sequence
Pattern cylon [] = {
  { {0,1,0,0,0},100 },
  { {0,0,1,0,0},100 },
  { {0,0,0,1,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,1,0},100 },
  { {0,0,1,0,0},100 },
  { {0,1,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 }
};
LightSequence cylonSeq = { "CYLON" , ARRAY_LENGTH(cylon) , cylon };

Pattern cylonRight [] = {
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,1,0,0,0},100 },
  { {0,0,1,0,0},100 },
  { {0,0,0,1,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,1,0},100 },
  { {0,0,1,0,0},100 },
  { {0,1,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 },
  { {0,0,0,0,0},100 }
};
LightSequence cylonRightSeq = { "CYLON_R" , ARRAY_LENGTH(cylonRight) , cylonRight };

// Figure of 8 Sequence
Pattern eight [] = {
	{ {0,0,0,0,1},100 },
	{ {1,0,0,0,0},100 },
	{ {0,1,0,0,0},100 },
	{ {0,0,1,0,0},100 },
	{ {0,0,0,1,0},100 },
	{ {0,0,0,0,0},500 }
};
LightSequence eightSeq = { "EIGHT" , ARRAY_LENGTH(eight) , eight };

Pattern eightRight [] = {
	{ {0,0,0,0,0},500 },
	{ {0,0,0,0,1},100 },
	{ {1,0,0,0,0},100 },
	{ {0,0,0,1,0},100 },
	{ {0,0,1,0,0},100 },
	{ {0,1,0,0,0},100 }
};
LightSequence eightRightSeq = { "EIGHT_R" , ARRAY_LENGTH(eightRight) , eightRight };

// LightRun Sequence
Pattern lightRun [] = {
  { {1,0,0,0,0},100 },
  { {0,1,0,0,0},100 },
  { {0,0,1,0,0},100 },
  { {0,0,0,1,0},100 },
  { {0,0,0,0,1},100 }
};
LightSequence lightRunSeq = { "LIGHTRUN" , ARRAY_LENGTH(lightRun) , lightRun };

// Police Sequence
Pattern jackpot [] = {
	{ {1,0,0,0,0},100 },
	{ {0,1,0,0,1},100 },
	{ {1,0,1,0,0},100 },
	{ {0,0,0,1,1},100 }
};
LightSequence jackpotSeq = { "JACKPOT" , ARRAY_LENGTH(jackpot) , jackpot };


LightSequence *definedSequences [] = { 
                    &cylonSeq , &cylonRightSeq, 
                    &fbreaksSeq, &breaksSeq, &freverseSeq, &reverseSeq , 
                    &fhazardsSeq , &hazardsSeq, &fullBlinkSeq, &breaksToggleSeq,
                    &allOnSeq,
						//&topBottomSeq,
					&eightSeq,&eightRightSeq,&jackpotSeq ,&lightRunSeq };

#endif
