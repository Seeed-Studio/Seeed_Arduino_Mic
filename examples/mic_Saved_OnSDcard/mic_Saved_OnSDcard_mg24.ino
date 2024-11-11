
#include <mic.h>
#include <SD.h>

uint32_t singleBuffer[NUM_SAMPLES];
uint32_t singleBuffer1[NUM_SAMPLES1];
MG24_ADC adc(singleBuffer, singleBuffer1);

File audioFile;
int dataconst = 66;

void setup() {
    Serial.begin(921600);
    Serial.println("aaa");
    adc.start();
}

void loop() {
  //Serial.println("aaa");
    if (adc.dataReady()) {
      Serial.println("ok");
            if (!SD.begin(D2)) {
                Serial.println("SD卡初始化失败！");
                return;
            }

            String fileName = "audio.wav";
            int fileIndex = 1;

            // 检查文件是否存在，若存在则更改文件名
            while (SD.exists(fileName)) {
                fileName = "audio_" + String(fileIndex) + ".wav";
                fileIndex++;
            }

            audioFile = SD.open(fileName, FILE_WRITE);
            if (!audioFile) {
                Serial.println("无法创建文件！");
                return;
            }

            writeWavHeader(audioFile, 16000, 3);
            int totalSamples = NUM_SAMPLES * dataconst * 3;
            for (int i = 0; i < NUM_SAMPLES * dataconst * 3; i++) {
                int adcValue = singleBuffer1[i] ;
                audioFile.write(adcValue & 0xFF);
                audioFile.write((adcValue >> 8) & 0xFF);
                Serial.println(i);
            }

            updateWavHeader(audioFile, totalSamples);
            audioFile.close();
            Serial.println("音频文件创建成功！");

        adc.resetData();
    }
    delay(100);
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
