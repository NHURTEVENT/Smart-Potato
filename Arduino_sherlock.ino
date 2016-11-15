#define __ASSERT_USE_STDERR
#include <assert.h> /* defines assert(); uses __assert() defined below */


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

enum Note_Freq {
  NOTE_B0  = 31,
  NOTE_C1  = 33,
  NOTE_CS1 = 35,
  NOTE_D1  = 37,
  NOTE_DS1 = 39,
  NOTE_E1  = 41,
  NOTE_F1  = 44,
  NOTE_FS1 = 46,
  NOTE_G1  = 49,
  NOTE_GS1 = 52,
  NOTE_A1  = 55,
  NOTE_AS1 = 58,
  NOTE_B1  = 62,
  NOTE_C2  = 65,
  NOTE_CS2 = 69,
  NOTE_D2  = 73,
  NOTE_DS2 = 78,
  NOTE_E2  = 82,
  NOTE_F2  = 87,
  NOTE_FS2 = 93,
  NOTE_G2  = 98,
  NOTE_GS2 = 104,
  NOTE_A2  = 110,
  NOTE_AS2 = 117,
  NOTE_B2  = 123,
  NOTE_C3  = 131,
  NOTE_CS3 = 139,
  NOTE_D3  = 147,
  NOTE_DS3 = 156,
  NOTE_E3  = 165,
  NOTE_F3  = 175,
  NOTE_FS3 = 185,
  NOTE_G3  = 196,
  NOTE_GS3 = 208,
  NOTE_A3  = 220,
  NOTE_AS3 = 233,
  NOTE_B3  = 247,
  NOTE_C4  = 262,
  NOTE_CS4 = 277,
  NOTE_D4  = 294,
  NOTE_DS4 = 311,
  NOTE_E4  = 330,
  NOTE_F4  = 349,
  NOTE_FS4 = 370,
  NOTE_G4  = 392,
  NOTE_GS4 = 415,
  NOTE_A4  = 440,
  NOTE_AS4 = 466,
  NOTE_B4  = 494,
  NOTE_C5  = 523,
  NOTE_CS5 = 554,
  NOTE_D5  = 587,
  NOTE_DS5 = 622,
  NOTE_E5  = 659,
  NOTE_F5  = 698,
  NOTE_FS5 = 740,
  NOTE_G5  = 784,
  NOTE_GS5 = 831,
  NOTE_A5  = 880,
  NOTE_AS5 = 932,
  NOTE_B5  = 988,
  NOTE_C6  = 1047,
  NOTE_CS6 = 1109,
  NOTE_D6  = 1175,
  NOTE_DS6 = 1245,
  NOTE_E6  = 1319,
  NOTE_F6  = 1397,
  NOTE_FS6 = 1480,
  NOTE_G6  = 1568,
  NOTE_GS6 = 1661,
  NOTE_A6  = 1760,
  NOTE_AS6 = 1865,
  NOTE_B6  = 1976,
  NOTE_C7  = 2093,
  NOTE_CS7 = 2217,
  NOTE_D7  = 2349,
  NOTE_DS7 = 2489,
  NOTE_E7  = 2637,
  NOTE_F7  = 2794,
  NOTE_FS7 = 2960,
  NOTE_G7  = 3136,
  NOTE_GS7 = 3322,
  NOTE_A7  = 3520,
  NOTE_AS7 = 3729,
  NOTE_B7  = 3951,
  NOTE_C8  = 4186,
  NOTE_CS8 = 4435,
  NOTE_D8  = 4699,
  NOTE_DS8 = 4978,
};
enum {
  PIN_BUZZER = 13,
  PIN_LED    = 1,

};

enum Note_Type {
  NT_NOTE,
  NT_REST,
  NT_MM /**< metronome mark */
};

/** Default metronome mark, crotchets per minute. */
enum { DEFAULT_MM = 120 };

/** The duration of the whole note, miliseconds. */
static unsigned int whole_note;

struct Note {
  enum Note_Type n_type;
  /** See enum Note_Freq for values. */
  unsigned int n_frequency;
  /**
   * Inverted note value.
   *
   * 1 - whole (semibreve),
   * 2 - half (minim),
   * 4 - quarter (crotchet),
   * 8 - eighth (quaver),
   * etc.
   *
   * If .n_type == NT_MM, then .n_value specifies the number
   * of crotchets per minute.
   */
  unsigned int n_value;
  /** Whether to "join" this note with the next one. */
  bool n_slur;
};

static bool note_invariant(const struct Note *note)
{
  return note != NULL &&
    (note->n_type >= NT_NOTE && note->n_type <= NT_MM) &&
    note->n_value != 0 &&
    /* only NOTEs can have .n_slur set */
    (!note->n_slur || note->n_type == NT_NOTE) &&
    /* only NOTEs can have non-zero .n_frequency */
    (note->n_frequency == 0 || note->n_type == NT_NOTE);
}

#define n(pitch, value)  { NT_NOTE, NOTE_ ## pitch, (value), false }
#define ns(pitch, value) { NT_NOTE, NOTE_ ## pitch, (value), true }
#define r(value)         { NT_REST, 0, (value), false }
#define mm(value)        { NT_MM, 0, (value), false }

/* https://en.wikipedia.org/wiki/Note_value */
static unsigned int dot(unsigned int inverted_value)
{
  /* Add 1 to compensate for integer division. */
  return (inverted_value + 1) * 2 / 3;
}

/* https://youtu.be/P5L6Qgmcjfw */
static struct Note sherlock[] = {
  mm(200),
#define XXX_LEFTY 0
#if XXX_LEFTY
  n(C3,4), n(G3,4), n(G2,4), n(G3,4),
  n(C3,4), n(G3,4), n(G2,4), n(G3,4),
#else
  r(1),
  r(1),
#endif
  n(C4,4), n(G4,2), n(G4,4),
  n(FS4,8), n(G4,8), n(GS4,2), n(G4,4),
  n(F4,4), ns(C5,dot(2)),
  n(C5,1),

  n(F4,4), n(C5,2), n(C5,4),
  n(B4,8), n(C5,8), n(D5,2), n(C5,4),
  ns(DS5,1),
  n(DS5,1),
  n(G5,4), n(C5,2), n(C5,4),
  n(D5,4), n(DS5,4), n(D5,4), n(C5,4),

  n(F5,4), n(C5,4), r(2),
  r(2), r(4), n(C5,8), n(D5,8),
  n(DS5,4), n(DS5,8), n(F5,8), n(D5,4), n(D5,8), n(DS5,8),

  n(C5,4), n(C5,8), n(D5,8), n(B4,4), n(GS4,8), n(G4,8),
  n(C5,1),
};

#if 0 /* XXX -------------------------------------------------------- */
/* http://www.8notes.com/scores/1110.asp */
/* https://youtu.be/xAMsLDQi2b0 */

#endif /* XXX ------------------------------------------------------- */

#undef mm
#undef r
#undef s
#undef n

static void _play(const struct Note *score, size_t score_len, const char *name)
{
  const struct Note *note;
  unsigned int duration;

  whole_note = 240000 / DEFAULT_MM;
  if (name != NULL)
    Serial.println(name);
  for (size_t i = 0; i < score_len; ++i) {
    note = &score[i];
    assert(note_invariant(note));
    if (note->n_type == NT_MM) {
      /* MM * t/4 = 60s ==> t = 240s / MM */
      whole_note = 240000 / note->n_value;
      continue;
    }
    duration = whole_note / note->n_value;
    if (note->n_type == NT_REST) {
      delay(duration);
      continue;
    }
    assert(note->n_type == NT_NOTE);
    if (note->n_slur) {
      buzz(note->n_frequency, duration);
    } else {
      buzz(note->n_frequency, 0.7 * duration);
      delay(0.3 * duration); /* pause between notes */
    }
  }
}
#define play(score) _play(score, ARRAY_SIZE(score), #score)

/*
 * `frequency' is the number of cycles per second.
 * `duration' is measured in miliseconds.
 */
static void buzz(long frequency, long duration)
{
  assert(frequency != 0);
  digitalWrite(PIN_LED, HIGH);
  /*
   * Delay between transitions =
   * 1 second's worth of microseconds / frequency / 2,
   * where 2 is number of phases (HIGH and LOW) per cycle.
   */
  long delay = 1000000 / frequency / 2;
  long ncycles = frequency * duration / 1000;
  for (long i = 0; i < ncycles; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delayMicroseconds(delay);
    digitalWrite(PIN_BUZZER, LOW);
    delayMicroseconds(delay);
  }
  digitalWrite(PIN_LED, LOW);
}

/** Handle diagnostic informations given by assertion and abort program. */
void __assert(const char *__func, const char *__file, int __lineno,
        const char *__sexp)
{
  Serial.print(__file);
  Serial.print(':');
  Serial.print(__lineno, DEC);
  Serial.print(" (");
  Serial.print(__func);
  Serial.print("): ");
  Serial.println(__sexp);
  Serial.flush();
  abort();
}

//****************************************************************************************
// Illutron take on Disney style capacitive touch sensor using only passives and Arduino
// Dzl 2012
//****************************************************************************************


//                              10n
// PIN 9 --[10k]-+-----10mH---+--||-- OBJECT
//               |            |
//              3.3k          |
//               |            V 1N4148 diode
//              GND           |
//                            |
//Analog 0 ---+------+--------+
//            |      |
//          100pf   1MOmhm
//            |      |
//           GND    GND



#define SET(x,y) (x |=(1<<y))				//-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))       		// |
#define CHK(x,y) (x & (1<<y))           		// |
#define TOG(x,y) (x^=(1<<y))            		//-+



#define N 160  //How many frequencies

float results[N];            //-Filtered result buffer
float freq[N];            //-Filtered result buffer
int sizeOfArray = N;

 
   
   

void setup()
{
  
  
  TCCR1A=0b10000010;        //-Set up frequency generator
  TCCR1B=0b00011001;        //-+
  ICR1=110;
  OCR1A=55;

  pinMode(9,OUTPUT);        //-Signal generator pin
  pinMode(8,OUTPUT);        //-Sync (test) pin

  Serial.begin(115200);

  for(int i=0;i<N;i++)      //-Preset results
    results[i]=0;         //-+
    pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(11, OUTPUT);
}

void loop()
{
  unsigned int d;

  int counter = 0;
  for(unsigned int d=0;d<N;d++)
  {
    int v=analogRead(0);    //-Read response signal
    CLR(TCCR1B,0);          //-Stop generator
    TCNT1=0;                //-Reload new frequency
    ICR1=d;                 // |
    OCR1A=d/2;              //-+
    SET(TCCR1B,0);          //-Restart generator

    results[d]=results[d]*0.5+(float)(v)*0.5; //Filter results
   
    freq[d] = d;

 //   plot(v,0);              //-Display
 //   plot(results[d],1);
  // delayMicroseconds(1);
  }


//PlottArray(1,freq,results); 
//int valeur=analogRead(A0);
/*
Serial.print("50: ");
Serial.println(results[50]);
Serial.print("87: ");
Serial.println(results[87]);
Serial.print("90: ");
Serial.println(results[90]);*/


if(results[50] > results[87]){

digitalWrite(10, HIGH);
digitalWrite(12, LOW);
digitalWrite(11, LOW);
Serial.println("pas touché");
}
else if(results[87] > results[90]){
  
digitalWrite(10, LOW);
digitalWrite(12, HIGH);
digitalWrite(11, LOW);

Serial.println("un doigt");
}
else {
  
digitalWrite(10, LOW);
digitalWrite(12, LOW);
digitalWrite(11, HIGH);
  play(sherlock);

Serial.println("la main");
}

 

  TOG(PORTB,0);            //-Toggle pin 8 after each sweep (good for scope)
}
   
