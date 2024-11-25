#if !defined(ARDUINO_SILABS)
    #error "This demo targets XIAO MG24 (Sense) only at the moment"
#endif

#include <mic.h>
#include <SD.h>

#define SD_USE                  0 // 1: Save to SD card, 2: Do not use SD card
#define UART_OUTPUT             1
uint32_t singleBuffer[NUM_SAMPLES];
uint32_t singleBuffer1[NUM_SAMPLES1];
MG24_ADC adc(singleBuffer, singleBuffer1);
File audioFile;

void setup() {
    Serial.begin(921600);
    Serial.println("begin");
    delay(100);
    adc.start();
}

void loop() {
    if (adc.dataReady()) {
        Serial.println("ok");
        if (SD_USE) {
            if (!SD.begin(D2)) {
                Serial.println("SD card initialization failed!");
                return;
            }

            String fileName = "audio.wav";
            int fileIndex = 1;

            // Check if the file exists, and if it does, rename it.
            while (SD.exists(fileName)) {
                fileName = "audio_" + String(fileIndex) + ".wav";
                fileIndex++;
            }

            audioFile = SD.open(fileName, FILE_WRITE);
            if (!audioFile) {
                Serial.println("Unable to create the file!");
                return;
            }

            writeWavHeader(audioFile, ADC_FREQ, 3);
            int totalSamples = NUM_SAMPLES * DATACONST * 3;

            for (int i = 0; i < NUM_SAMPLES * DATACONST * 3; i++) {
                int adcValue = singleBuffer1[i];
                audioFile.write(adcValue & 0xFF);
                audioFile.write((adcValue >> 8) & 0xFF);
                Serial.println(adcValue);
            }

            updateWavHeader(audioFile, totalSamples);
            audioFile.close();
            Serial.println("Audio file created successfully!");

            adc.resetData();
        } else {
            for (int i = 0; i < NUM_SAMPLES * DATACONST * 3; i++) {
                Serial.println(singleBuffer1[i]);
            }
            adc.resetData();
        }
        delay(100);
    }
}

void writeWavHeader(File &file, int sampleRate, int duration) {
    file.write("RIFF", 4);
    file.write((char*)&duration, 4);
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    uint32_t fmtSize = 16;
    file.write((char*)&fmtSize, 4);
    uint16_t audioFormat = 1;
    file.write((char*)&audioFormat, 2);
    uint16_t numChannels = 1;
    file.write((char*)&numChannels, 2);
    file.write((char*)&sampleRate, 4);
    uint32_t byteRate = sampleRate * 2;
    file.write((char*)&byteRate, 4);
    uint16_t blockAlign = 2;
    file.write((char*)&blockAlign, 2);
    uint16_t bitsPerSample = 16;
    file.write((char*)&bitsPerSample, 2);
    file.write("data", 4);
    uint32_t dataSize = sampleRate * duration * 2;
    file.write((char*)&dataSize, 4);
}

void updateWavHeader(File &file, int totalSamples) {
    file.seek(4);
    uint32_t fileSize = totalSamples * 2 + 36;
    file.write((char*)&fileSize, 4);
    file.seek(40);
    uint32_t dataSize = totalSamples * 2;
    file.write((char*)&dataSize, 4);
}
