#include "dma_adc.h"

#if defined(WIO_TERMINAL)

volatile dmacdescriptor wrb[DMAC_CH_NUM] __attribute__ ((aligned (16)));          // Write-back DMAC descriptors
dmacdescriptor descriptor_section[DMAC_CH_NUM] __attribute__ ((aligned (16)));    // DMAC channel descriptors
dmacdescriptor descriptor __attribute__ ((aligned (16)));                         // Place holder descriptor

DMA_ADC_Class::~DMA_ADC_Class()
{
  end();
}

uint8_t DMA_ADC_Class::begin(){

  // Configure DMA to sample from ADC at a regular interval (triggered by timer/counter)
  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;                          // Specify the location of the descriptors
  DMAC->WRBADDR.reg = (uint32_t)wrb;                                          // Specify the location of the write back descriptors
  DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);                // Enable the DMAC peripheral
  DMAC->Channel[1].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(TC5_DMAC_ID_OVF) |      // Set DMAC to trigger on TC5 timer overflow
                                 DMAC_CHCTRLA_TRIGACT_BURST;                  // DMAC burst transfer
                                 
  descriptor.descaddr = (uint32_t)&descriptor_section[1];                     // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                           // Take the result from the ADC0 RESULT register
  descriptor.dstaddr = (uint32_t)buf_0 + sizeof(uint16_t) * _buf_size;  // Place it in the adc_buf_0 array
  descriptor.btcnt = _buf_size;                                             // Beat count
  descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                            // Beat size is HWORD (16-bits)
                      DMAC_BTCTRL_DSTINC |                                    // Increment the destination address
                      DMAC_BTCTRL_VALID |                                     // Descriptor is valid
                      DMAC_BTCTRL_BLOCKACT_SUSPEND;                           // Suspend DMAC channel 0 after block transfer
  memcpy(&descriptor_section[0], &descriptor, sizeof(descriptor));            // Copy the descriptor to the descriptor section
  
  descriptor.descaddr = (uint32_t)&descriptor_section[0];                     // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                           // Take the result from the ADC0 RESULT register
  descriptor.dstaddr = (uint32_t)buf_1 + sizeof(uint16_t) * _buf_size;  // Place it in the adc_buf_1 array
  descriptor.btcnt = _buf_size;                                             // Beat count
  descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                            // Beat size is HWORD (16-bits)
                      DMAC_BTCTRL_DSTINC |                                    // Increment the destination address
                      DMAC_BTCTRL_VALID |                                     // Descriptor is valid
                      DMAC_BTCTRL_BLOCKACT_SUSPEND;                           // Suspend DMAC channel 0 after block transfer
  memcpy(&descriptor_section[1], &descriptor, sizeof(descriptor));            // Copy the descriptor to the descriptor section

  // Configure NVIC
  NVIC_SetPriority(DMAC_1_IRQn, 0);    // Set the Nested Vector Interrupt Controller (NVIC) priority for DMAC1 to 0 (highest)
  NVIC_EnableIRQ(DMAC_1_IRQn);         // Connect DMAC1 to Nested Vector Interrupt Controller (NVIC)

  // Activate the suspend (SUSP) interrupt on DMAC channel 1
  DMAC->Channel[1].CHINTENSET.reg = DMAC_CHINTENSET_SUSP;

  // Configure ADC
  ADC1->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_AIN12_Val; // Set the analog input to ADC1/AIN12
  while(ADC1->SYNCBUSY.bit.INPUTCTRL);                // Wait for synchronization
  ADC1->SAMPCTRL.bit.SAMPLEN = 0x00;                  // Set max Sampling Time Length to half divided ADC clock pulse (2.66us)
  while(ADC1->SYNCBUSY.bit.SAMPCTRL);                 // Wait for synchronization 
  ADC1->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV128;       // Divide Clock ADC GCLK by 128 (48MHz/128 = 375kHz)
  ADC1->CTRLB.reg = ADC_CTRLB_RESSEL_12BIT |          // Set ADC resolution to 12 bits
                    ADC_CTRLB_FREERUN;                // Set ADC to free run mode       
  while(ADC1->SYNCBUSY.bit.CTRLB);                    // Wait for synchronization
  ADC1->CTRLA.bit.ENABLE = 1;                         // Enable the ADC
  while(ADC1->SYNCBUSY.bit.ENABLE);                   // Wait for synchronization
  ADC1->SWTRIG.bit.START = 1;                         // Initiate a software trigger to start an ADC conversion
  while(ADC1->SYNCBUSY.bit.SWTRIG);                   // Wait for synchronization

  // Enable DMA channel 1
  DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;

  // Configure Timer/Counter 5
  GCLK->PCHCTRL[TC5_GCLK_ID].reg = GCLK_PCHCTRL_CHEN |        // Enable perhipheral channel for TC5
                                   GCLK_PCHCTRL_GEN_GCLK1;    // Connect generic clock 0 at 48MHz
   
  TC5->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;               // Set TC5 to Match Frequency (MFRQ) mode

  // configure the sample rate
  switch (_sampling_rate) {
    case 16000:
      TC5->COUNT16.CC[0].reg = _16KHZ;                          // Set the trigger to 16 kHz: (48Mhz / 16000) - 1
      break;
    case 8000:
      TC5->COUNT16.CC[0].reg = _8KHZ;                          // Set the trigger to 8 kHz: (48Mhz / 8000) - 1
      break;
    default:
      return 0;   
  }

  while (TC5->COUNT16.SYNCBUSY.bit.CC0);                      // Wait for synchronization

  // Start Timer/Counter 5
  TC5->COUNT16.CTRLA.bit.ENABLE = 1;                          // Enable the TC5 timer
  while (TC5->COUNT16.SYNCBUSY.bit.ENABLE);                   // Wait for synchronization

  return -1;
}

void DMA_ADC_Class::end(){

  DMAC->CTRL.bit.DMAENABLE = 0;                                  // Disable the DMAC peripheral
  TC5->COUNT16.CTRLA.bit.ENABLE = 0;                             // Disable the TC5 timer
  NVIC_DisableIRQ(DMAC_1_IRQn);                                  // Disable DMAC1 at Nested Vector Interrupt Controller (NVIC)

}

void DMA_ADC_Class::pause(){
    NVIC_DisableIRQ(DMAC_1_IRQn);
}

void DMA_ADC_Class::resume(){
    NVIC_EnableIRQ(DMAC_1_IRQn);
}

void DMA_ADC_Class::set_callback(void(*function)(uint16_t *buf, uint32_t buf_len)) {
  _onReceive = function;
}

void DMAC_1_Handler() {
  // Check if DMAC channel 1 has been suspended (SUSP)
  if (DMAC->Channel[1].CHINTFLAG.bit.SUSP) {

     // Debug: make pin high before copying buffer
    if (*DMA_ADC_Class::_debug_pin_ptr) {
    digitalWrite(*DMA_ADC_Class::_debug_pin_ptr, HIGH);
    }

    // Restart DMAC on channel 1 and clear SUSP interrupt flag
    DMAC->Channel[1].CHCTRLB.reg = DMAC_CHCTRLB_CMD_RESUME;
    DMAC->Channel[1].CHINTFLAG.bit.SUSP = 1;

    // See which buffer has filled up, and dump results into large buffer
    if (*DMA_ADC_Class::_buf_count_ptr) {
      DMA_ADC_Class::_onReceive(DMA_ADC_Class::buf_0_ptr, *DMA_ADC_Class::_buf_size_ptr);
    } else {
      DMA_ADC_Class::_onReceive(DMA_ADC_Class::buf_1_ptr, *DMA_ADC_Class::_buf_size_ptr);
    }

    // Flip to next buffer
    *DMA_ADC_Class::_buf_count_ptr = (*DMA_ADC_Class::_buf_count_ptr + 1) % 2;

    // Debug: make pin low after copying buffer
    if (*DMA_ADC_Class::_debug_pin_ptr) {
    digitalWrite(*DMA_ADC_Class::_debug_pin_ptr, LOW);
    }
  }
}

#endif