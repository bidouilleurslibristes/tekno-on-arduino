#include "pitches.h"
#include "Color.h"

int btn1 = 0;
int spk1 = 8;
int pot1 = A1;
int pot2 = A3;

int led_g = 11;
int led_b = 10;
int led_r = 9;

int last_val1 = 1;
int last_note = 0;

int note_from = NOTE_B0;
int note_to = NOTE_DS8;

unsigned long tremolo_tick = 0;
unsigned long tremolo_delay = 1;
unsigned long tremolo_threshold = 2;
int tremolo_state = 1;

int state = 0;

void setColor(int red, int green, int blue)
{
  analogWrite(led_r, red);
  analogWrite(led_g, green);
  analogWrite(led_b, blue);  
}

void setColorFromNote(int note) {
  
  Serial.println(11111111);
  
  Color colors[] = {
    Color(0,255,0),
    Color(255,255,0),
    Color(255,0,0),
    Color(0,255,255),
    Color(0,0,255),
    Color(255,0,255)
  };
  

  int nstep = sizeof(colors) / sizeof(Color); 
  int step_size = (note_to - note_from) / (nstep - 1);
  int step_index = (note - note_from) / step_size;

  Serial.println(step_index);
  
  Color color_from = colors[step_index];
  Color color_to = colors[step_index + 1];
  
  float pos = (float)((note - note_from) % step_size) / (float)step_size;
  Serial.println(pos);
  Color color = Color::valInterm(color_from, color_to, pos);
  Serial.println(color.red);
  Serial.println(color.green);
  Serial.println(color.blue);
  setColor(color.red, color.green, color.blue);
}

void setup() {
  Serial.begin(9600);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
}

void loop() {
  int val1 = digitalRead(btn1);
  int ana1 = analogRead(pot1);
  int ana2 = analogRead(pot2);

  if (val1 == 0 && last_val1 == 1) {
    state = !state;
  }

  int note = map(ana1, 0, 1024, note_from, note_to);
  tremolo_delay = map(ana2, 0, 1024, 0, 200);

  unsigned long tick = millis();
  
/*  Serial.println(1111111);
  Serial.println(val1);
  Serial.println(ana1);
  Serial.println(ana2);
  Serial.println(note);
  Serial.println(tremolo_delay);*/
  
  if (tremolo_delay < tremolo_threshold) {
    tremolo_state = 1;
  }
  else if (tick - tremolo_tick > tremolo_delay) {
    tremolo_tick = tick;
    tremolo_state = !tremolo_state;
  }
  
  if (state && tremolo_state) {
    tone(spk1, note);
    setColorFromNote(note);
  }
  else {
    noTone(spk1);
    note = 0;
    setColor(0,0,0);
  }

  last_val1 = val1;
  last_note = note;
}
