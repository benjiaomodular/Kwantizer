#include <avr/pgmspace.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <MCP4725.h>
#include <ADS1X15.h>
#include "definitions.h"
MCP4725 MCP(0x60);
ADS1115 ADS(0x48);


bool tuning_dac = false;
bool tuning_adc = false;

uint16_t current_scale_mask = 0;
uint8_t nb_notes_in_scale = 0;

uint8_t current_root_semitone = 1; // Force refresh at start

// in_scale_cv_mode = 0 ->  1 semitone / 83 mV  (non-constant CV difference to change note in scale)
//                  = 1 ->  1 note in_scale / 83 mV (constant CV difference to change note in scale
bool volatile in_scale_cv_mode = true;

uint8_t last_semitone_index = 0;
long int trigger_out_width = 10;
bool reset_trigger_out = false;
long int last_trigger_out_millis = millis();

void setup() {
  cli();
  PCICR |= 0b00000101;
  PCMSK0 |= 0b00001100;
  PCMSK2 |= 0b11000000;
  sei();

  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TUNING_PIN, INPUT);
  pinMode(NOTE_PIN_C, INPUT);
  pinMode(NOTE_PIN_CS, INPUT);
  pinMode(NOTE_PIN_D, INPUT);
  pinMode(NOTE_PIN_DS, INPUT);
  pinMode(NOTE_PIN_E, INPUT);
  pinMode(NOTE_PIN_F, INPUT);
  pinMode(NOTE_PIN_FS, INPUT);
  pinMode(NOTE_PIN_G, INPUT);
  pinMode(NOTE_PIN_GS, INPUT);
  pinMode(NOTE_PIN_A, INPUT);
  pinMode(NOTE_PIN_AS, INPUT);
  pinMode(NOTE_PIN_B, INPUT);

  compute_scale_mask();
  
  Wire.begin();
  Wire.setClock(400000);

  MCP.begin();
  ADS.begin();
  ADS.setGain(0);

}

void loop() {

  uint8_t semitones_above_root_in_scale;
  uint16_t temp_scale_mask = compute_scale_mask();
  uint8_t note_in_scale;
  uint8_t root_semitone;
  int16_t cv_to_quantize = ADS.readADC(0);

//  Serial.print("CV In: ");
//  Serial.print(cv_to_quantize);
//  Serial.print("\t");

  if (reset_trigger_out && millis() - last_trigger_out_millis >= trigger_out_width) {
//    digitalWrite(TRIGGER_OUT_PIN, LOW);
    reset_trigger_out = false;
  }

  nb_notes_in_scale = 12;

  note_in_scale = map(
    cv_to_quantize,
    CV_0V_BOUNDARY_INCLUSIVE, CV_ABOVE_5V_BOUNDARY_EXCLUSIVE,
    0, 5 * nb_notes_in_scale + 1);

  semitones_above_root_in_scale = 0;
  
  for (uint8_t semitones_above_root = 0, note = 0; semitones_above_root < MAX_DAC_SEMITONE; semitones_above_root++) {

    if (bitRead(temp_scale_mask, 0)) {
      semitones_above_root_in_scale = semitones_above_root;
      note += 1;
      if (note >= note_in_scale) {
        break;
      }
    }

    temp_scale_mask = rotate12Right(temp_scale_mask, 1);

  }

  uint8_t semitone_index = current_root_semitone + semitones_above_root_in_scale;

  semitone_index = min(semitone_index, MAX_DAC_SEMITONE - 1);

//  Serial.print(" Note Out: ");
//  Serial.println((int16_t)pgm_read_word_near(semitone_cvs_dac + semitone_index));

  if (digitalRead(TUNING_PIN)){
    MCP.setValue(4095);
  } else {
    MCP.setValue((int16_t)pgm_read_word_near(semitone_cvs_dac + semitone_index));
  }
  
  if (last_semitone_index != semitone_index) {
    reset_trigger_out = true;
    last_semitone_index = semitone_index;
    last_trigger_out_millis = millis();
  }

}


uint16_t compute_scale_mask(){
  uint8_t note_count = 0;
  uint16_t scale_mask = 0;

  if(digitalRead(NOTE_PIN_C)) { scale_mask = scale_mask | 0b100000000000; note_count++;}
  if(digitalRead(NOTE_PIN_CS)){ scale_mask = scale_mask | 0b010000000000; note_count++;}
  if(digitalRead(NOTE_PIN_D)) { scale_mask = scale_mask | 0b001000000000; note_count++;}
  if(digitalRead(NOTE_PIN_DS)){ scale_mask = scale_mask | 0b000100000000; note_count++;}
  if(digitalRead(NOTE_PIN_E)) { scale_mask = scale_mask | 0b000010000000; note_count++;}
  if(digitalRead(NOTE_PIN_F)) { scale_mask = scale_mask | 0b000001000000; note_count++;}
  if(digitalRead(NOTE_PIN_FS)){ scale_mask = scale_mask | 0b000000100000; note_count++;}
  if(digitalRead(NOTE_PIN_G)) { scale_mask = scale_mask | 0b000000010000; note_count++;}
  if(digitalRead(NOTE_PIN_GS)){ scale_mask = scale_mask | 0b000000001000; note_count++;}
  if(digitalRead(NOTE_PIN_A)) { scale_mask = scale_mask | 0b000000000100; note_count++;}
  if(digitalRead(NOTE_PIN_AS)){ scale_mask = scale_mask | 0b000000000010; note_count++;}
  if(digitalRead(NOTE_PIN_B)) { scale_mask = scale_mask | 0b000000000001; note_count++;}

  current_scale_mask = scale_mask;
  nb_notes_in_scale = note_count;
//  Serial.println(current_scale_mask);
//  Serial.println(note_count);
  
  return scale_mask;
}

uint16_t rotate12Left(uint16_t n, uint16_t d) {
  return 0xfff & ((n << (d % 12)) | (n >> (12 - (d % 12))));
}

uint16_t rotate12Right(uint16_t n, uint16_t d) {
  return 0xfff & ((n >> (d % 12)) | (n << (12 - (d % 12))));
}
