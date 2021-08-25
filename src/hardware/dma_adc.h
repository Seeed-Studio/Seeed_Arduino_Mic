#include <Arduino.h>

/**
 * @brief DMAC descriptor structure
 */

typedef struct {
    uint16_t btctrl;
    uint16_t btcnt;
    uint32_t srcaddr;
    uint32_t dstaddr;
    uint32_t descaddr;
} dmacdescriptor;

void dma_adc_init(uint16_t *buf_0, uint16_t *buf_1);
void dma_adc_deinit();
void dma_adc_pause();
void dma_adc_resume();
void IrqHandler();