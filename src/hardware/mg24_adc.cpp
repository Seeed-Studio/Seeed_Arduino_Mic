#if defined(ARDUINO_SILABS)
#include "mg24_adc.h"

LDMA_Descriptor_t descriptor;
static MG24_ADC *adc_instance = nullptr;

MG24_ADC::MG24_ADC(uint32_t *buffer, uint32_t *buffer1)
    : buffer(buffer), buffer1(buffer1), index(0), dataCount(0) {
    adc_instance = this;
}

void MG24_ADC::initPRS() {
    CMU_ClockEnable(cmuClock_PRS, true);
    PRS_SourceAsyncSignalSet(PRS_CHANNEL, PRS_ASYNC_CH_CTRL_SOURCESEL_LETIMER0, PRS_LETIMER0_CH0);
    PRS_ConnectConsumer(PRS_CHANNEL, prsTypeAsync, prsConsumerIADC0_SINGLETRIGGER);
}

void MG24_ADC::initADC() {
    IADC_Init_t init = IADC_INIT_DEFAULT;
    IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
    IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
    IADC_SingleInput_t singleInput = IADC_SINGLEINPUT_DEFAULT;

    CMU_ClockEnable(cmuClock_IADC0, true);
    CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);
    init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);
    init.warmup = iadcWarmupKeepInStandby;
    init.iadcClkSuspend1 = true;

    initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
    initAllConfigs.configs[0].vRef = 1210;
    initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed2x;
    initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain0P5x;
    initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0, CLK_ADC_FREQ, 0, iadcCfgModeNormal, init.srcClkPrescale);

    initSingle.triggerSelect = iadcTriggerSelPrs0PosEdge;
    initSingle.dataValidLevel = iadcFifoCfgDvl2;
    initSingle.fifoDmaWakeup = true;
    initSingle.start = true;

    singleInput.posInput = IADC_INPUT_0_PORT_PIN;
    singleInput.negInput = iadcNegInputGnd;

    IADC_init(IADC0, &init, &initAllConfigs);
    IADC_initSingle(IADC0, &initSingle, &singleInput);

    GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
}

void MG24_ADC::initLETIMER() {
    CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
    LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

    CMU_LFXOInit(&lfxoInit);
    CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_LETIMER0, true);

    uint32_t topValue = CMU_ClockFreqGet(cmuClock_LETIMER0) / LETIMER_FREQ;
    letimerInit.comp0Top = true;
    letimerInit.topValue = topValue;
    letimerInit.ufoa0 = letimerUFOAPulse;
    letimerInit.repMode = letimerRepeatFree;

    LETIMER_Init(LETIMER0, &letimerInit);
}

void MG24_ADC::initLDMA(uint32_t size) {
    LDMA_Init_t init = LDMA_INIT_DEFAULT;
    LDMA_TransferCfg_t transferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_IADC0_IADC_SINGLE);
    descriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&IADC0->SINGLEFIFODATA, buffer, size, 0);

    LDMA_Init(&init);
    LDMA_StartTransfer(IADC_LDMA_CH, &transferCfg, &descriptor);
}

void MG24_ADC::start() {
    dataconst = DATACONST;
    initPRS();
    initADC();
    initLDMA(NUM_SAMPLES);
    initLETIMER();
}

void MG24_ADC::resetData() {
    dataCount = 0;
    index = 0;
    std::memset(buffer1, 0, NUM_SAMPLES1 * sizeof(uint32_t));
    std::memset(buffer, 0, NUM_SAMPLES * sizeof(uint32_t));
}

bool MG24_ADC::dataReady() {
    return dataCount >= DATACONST * 3;
}

extern "C" {
    __attribute__((__used__)) void LDMA_IRQHandler() {
        LDMA_IntClear(1 << IADC_LDMA_CH);

        if (adc_instance != nullptr) {
            if (adc_instance->index + NUM_SAMPLES <= NUM_SAMPLES1) {
                std::memcpy(&adc_instance->buffer1[adc_instance->index], adc_instance->buffer, NUM_SAMPLES * sizeof(uint32_t));
                adc_instance->index += NUM_SAMPLES;
            } else {
                LDMA_StopTransfer(IADC_LDMA_CH);
                adc_instance->index = 0;
                std::memcpy(&adc_instance->buffer[adc_instance->index], adc_instance->buffer, NUM_SAMPLES * sizeof(uint32_t));
                adc_instance->index += NUM_SAMPLES;
            }
            adc_instance->dataCount++;
        }
    }
}

#endif
