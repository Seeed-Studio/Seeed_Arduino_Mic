#include <mic.h>

// Settings
#define DEBUG 1                 // Enable pin pulse during ISR  
#define SAMPLES 16000*3

static const int debug_pin = 1; // Toggles each DAC ISR (if DEBUG is set to 1)
File sFile;
int16_t recording_buf[SAMPLES];

// Globals - DMA and ADC
volatile uint8_t recording = 0;
volatile boolean results0Ready = false;
volatile boolean results1Ready = false;

void setup() {

#if DEBUG
  pinMode(debug_pin, OUTPUT);
#endif
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  

  Mic.onReceive(audio_rec_callback);

  if (!Mic.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }

  Serial.println("Mic initialization done.");

}

void loop() { 
  
if (digitalRead(WIO_KEY_A) == LOW && !recording) {

    Serial.println("Starting sampling");
    recording = 1;
    record_ready = false;  
}

  if (!recording && record_ready)
  {
  Serial.println("Finished sampling");
  
  for (int i = 0; i < SAMPLES; i++) {
    
  //int16_t sample = filter.step(recording_buf[i]);
  int16_t sample = recording_buf[i];
  Serial.println(sample);
  }
  
  record_ready = false; 
  }
}

static void audio_rec_callback(uint16_t *buf, uint32_t buf_len) {

  static uint32_t idx = 0;

  // Copy samples from DMA buffer to inference buffer
  if (recording) {
    for (uint32_t i = 0; i < buf_len; i++) {
  
      // Convert 12-bit unsigned ADC value to 16-bit PCM (signed) audio value
      recording_buf[idx++] = filter.step((int16_t)(buf[i] - 1024) * 16);
      
      if (idx >= SAMPLES){ 
      idx = 0;
      recording = 0;
      record_ready = true;
      break;
     }
     
    }
  }
}