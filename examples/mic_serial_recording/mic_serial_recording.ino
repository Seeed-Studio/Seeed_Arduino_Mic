#include <mic.h>
#include "processing/filters.h"

// Settings
#define DEBUG 1                 // Enable pin pulse during ISR  
#define SAMPLES 16000*3

mic_config_t mic_config{
  .mic_type = 0,                // 0 - DMA ADC MIC
  .channel_cnt = 1,
  .sampling_rate = 16000,
  .buf_size = 1600,
  .adc_pin = ADC1,
  .debug_pin = 1                // Toggles each DAC ISR (if DEBUG is set to 1)
};


int16_t recording_buf[SAMPLES];
volatile uint8_t recording = 0;
volatile static bool record_ready = false;

FilterBuHp filter;

void setup() {

  Serial.begin(57600);
  while (!Serial) {delay(10);}
  
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  
  Mic.onReceive(audio_rec_callback);

  if (!Mic.begin(&mic_config)) {
    Serial.println("init_fail");
    while (1);
  }

  Serial.println("init_ok");

}

void loop() { 

  String resp = Serial.readString();

if (resp == "init\n" && !recording){
  Serial.println("init_ok");
}
  
if (resp == "rec\n" && !recording) {

    recording = 1;
    record_ready = false;  
}

  if (!recording && record_ready)
  {
    
  Serial.println("rec_ok");
  for (int i = 0; i < SAMPLES; i++) {
    
  Serial.println(recording_buf[i]);

  }

  Serial.println("fi");
  record_ready = false; 
  }
}

static void audio_rec_callback(uint16_t *buf, uint32_t buf_len) {

  static uint32_t idx = 0;

  if (recording) {
    for (uint32_t i = 0; i < buf_len; i++) {
  
      // Convert 12-bit unsigned ADC value to 16-bit PCM (signed) audio value
      recording_buf[idx++] = filter.step((int16_t)(buf[i] - 1024) * 16);
      //recording_buf[idx++] = (int16_t)(buf[i] - 1024) * 16;  
          
      if (idx >= SAMPLES){ 
      idx = 0;
      recording = 0;
      record_ready = true;
      break;
     } 
    }
  }
}