#define OLED_RESET  4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define TUNING_PIN A0
#define NOTE_PIN_C 2
#define NOTE_PIN_D 3
#define NOTE_PIN_E 4
#define NOTE_PIN_F 5
#define NOTE_PIN_G 6
#define NOTE_PIN_A 7
#define NOTE_PIN_B 8

#define NOTE_PIN_CS 9
#define NOTE_PIN_DS 10
#define NOTE_PIN_FS 11
#define NOTE_PIN_GS 12
#define NOTE_PIN_AS 13

#define MAX_DAC_SEMITONE 61
const int16_t PROGMEM semitone_cvs_dac[MAX_DAC_SEMITONE] = {
// 0     83.3  166.6  250   333   416.6  500  583.3  666.6  750   833.3  916.6
  6,     65,   131,   201,  268,  337,   405,  474,  540,   609,  677,   745,
  814,   882,  950,  1016, 1084, 1151,  1223, 1290, 1359,  1427, 1496,  1563, 
  1631, 1699, 1768,  1838, 1904, 1974,  2041, 2110, 2177,  2248, 2315,  2386,
  2455, 2524, 2592,  2660, 2729, 2798,  2867, 2934, 3004,  3071, 3141,  3206,
  3278, 3344, 3412,  3480, 3549, 3617,  3685, 3754, 3822,  3892, 3960,  4026,
  4093
};



// 0  83  167  250  333  417  500  583  667  750  833  917
// Techically the ADC can distinguish amongst 63 semitones, but the DAC can
// only produce 57 because of the power of my USB port

#define CV_0V_BOUNDARY_INCLUSIVE -32
#define CV_ABOVE_5V_BOUNDARY_EXCLUSIVE 26832
#define NB_ADC_BOUNDARIES 62

// ADS1015 maps 6.14 at 32767, but we stop at 5V to stay within standards.
// These are the inter CVs ADC reading, i.e. we round CVs by placing boundaries
// at 1/24V left and right of the CV associated with a semitone,
// in other words if you are within k*83.3mV ± 41.6mV then bin CV into k
const int16_t PROGMEM inter_semitonecv_to_ADC16read[NB_ADC_BOUNDARIES] = {
        -240,   208,   656,  1104,  1544,  1984,  2432,  2880,  3328,  3776,  4216,  4656,
        5096,  5544,  5992,  6432,  6872,  7320,  7776,  8216,  8648,  9088,  9536,  9984,
       10432, 10880, 11320, 11760, 12208, 12648, 13088, 13536, 13984, 14424, 14864, 15312,
       15752, 16200, 16656, 17104, 17552, 17992, 18432, 18872, 19312, 19760, 20200, 20648,
       21096, 21536, 21976, 22416, 22872, 23312, 23752, 24200, 24648, 25096, 25536, 25984, 
       26432, // 26880,
       32765
};

#define NUM_NOTES 12
#define NUM_SCALES 120

#define KEYBOARD_WIDTH 11
#define SCALE_BITS 12
