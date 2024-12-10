#if defined(ARDUINO_SILABS)
#include "mg24_adc.h"

LDMA_Descriptor_t descriptor;
static MG24_ADC_Class *adc_instance = nullptr;

MG24_ADC_Class::~MG24_ADC_Class() {
    delete[] buf_0;
    delete[] buf_1;
}

void MG24_ADC_Class::initPRS() {
    CMU_ClockEnable(cmuClock_PRS, true);
    PRS_SourceAsyncSignalSet(PRS_CHANNEL, PRS_ASYNC_CH_CTRL_SOURCESEL_LETIMER0, PRS_LETIMER0_CH0);
    PRS_ConnectConsumer(PRS_CHANNEL, prsTypeAsync, prsConsumerIADC0_SINGLETRIGGER);
}

void MG24_ADC_Class::initADC() {
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

    initSingle.alignment = iadcAlignRight12;

    singleInput.posInput = IADC_INPUT_0_PORT_PIN;
    singleInput.negInput = iadcNegInputGnd;
    IADC_init(IADC0, &init, &initAllConfigs);
    IADC_initSingle(IADC0, &initSingle, &singleInput);
    GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;    
}

void MG24_ADC_Class::initLETIMER() {
    uint32_t freq = 20000;
    if (_sampling_rate == 16000)
    {
        freq = 20000;
    }
    if (_sampling_rate == 8000)
    {
        freq = 9000;
    }
    CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
    LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

    CMU_LFXOInit(&lfxoInit);
    CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_LETIMER0, true);
    uint32_t topValue = CMU_ClockFreqGet(cmuClock_LETIMER0) / freq;
    letimerInit.comp0Top = true;
    letimerInit.topValue = topValue;
    letimerInit.ufoa0 = letimerUFOAPulse;
    letimerInit.repMode = letimerRepeatFree;
    LETIMER_Init(LETIMER0, &letimerInit);    
}


#define LDMA_DESCRIPTOR_LINKREL_P2M_HALF_WORD(src, dest, count, linkjmp)    \
  {                                                                 \
    .xfer =                                                            \
    {                                                                  \
      .structType   = ldmaCtrlStructTypeXfer,                          \
      .structReq    = 0,                                               \
      .xferCnt      = (count) - 1,                                     \
      .byteSwap     = 0,                                               \
      .blockSize    = ldmaCtrlBlockSizeUnit1,                          \
      .doneIfs      = 1,                                               \
      .reqMode      = ldmaCtrlReqModeBlock,                            \
      .decLoopCnt   = 0,                                               \
      .ignoreSrec   = 0,                                               \
      .srcInc       = ldmaCtrlSrcIncNone,                              \
      .size         = ldmaCtrlSizeHalf,                                \
      .dstInc       = ldmaCtrlDstIncOne,                               \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,                          \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,                          \
      .srcAddr      = (uint32_t)(src),                                 \
      .dstAddr      = (uint32_t)(dest),                                \
      .linkMode     = ldmaLinkModeRel,                                 \
      .link         = 1,                                               \
      .linkAddr     = (linkjmp) * LDMA_DESCRIPTOR_NON_EXTEND_SIZE_WORD \
    }                                                                  \
  }

void MG24_ADC_Class::initLDMA(uint16_t *buffer, uint32_t size) {
    LDMA_Init_t init = LDMA_INIT_DEFAULT;
    LDMA_TransferCfg_t transferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_IADC0_IADC_SINGLE);
    descriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_HALF_WORD(&IADC0->SINGLEFIFODATA, buffer, size, 0);
    LDMA_StartTransfer(IADC_LDMA_CH, &transferCfg, &descriptor);

}

uint8_t MG24_ADC_Class::begin() {
    adc_instance = this; 
    initPRS();
    initADC();
    initLETIMER();
    initLDMA(buf_0,  _buf_size);
    return 1;
}

void MG24_ADC_Class::end() {
   LDMA_TransferDone(IADC_LDMA_CH);
    LETIMER_Enable(LETIMER0, false);
    IADC_init(IADC0, NULL, NULL);
    delete[] buf_0;
    delete[] buf_1;
    buf_0 = nullptr;
    buf_1 = nullptr;
}


void MG24_ADC_Class::set_callback(void(*function)(uint16_t *buf, uint32_t buf_len)) {
  _onReceive = function;
}


extern "C" {
    __attribute__((__used__)) void LDMA_IRQHandler_use() {
        static bool use_buf1 = false;

        if (adc_instance ) {
            uint16_t *current_buffer = use_buf1 ? adc_instance->buf_1 : adc_instance->buf_0;

            if (adc_instance->_onReceive) {
                adc_instance->_onReceive(current_buffer, adc_instance->getBufferSize());
            }

            use_buf1 = !use_buf1; 
            adc_instance->initLDMA(current_buffer, adc_instance->getBufferSize());
        }
    
       LDMA_IntClear(1 << IADC_LDMA_CH); 

    }
}

#endif