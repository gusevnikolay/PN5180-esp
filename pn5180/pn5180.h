#ifndef PN5180_H
#define PN5180_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PN5180_SYSTEM_CONFIG       (0x00)
#define PN5180_IRQ_ENABLE          (0x01)
#define PN5180_IRQ_STATUS          (0x02)
#define PN5180_IRQ_CLEAR           (0x03)
#define PN5180_TRANSCEIVE_CONTROL  (0x04)
#define PN5180_TIMER1_RELOAD       (0x0c)
#define PN5180_TIMER1_CONFIG       (0x0f)
#define PN5180_RX_WAIT_CONFIG      (0x11)
#define PN5180_CRC_RX_CONFIG       (0x12)
#define PN5180_RX_STATUS           (0x13)
#define PN5180_CRC_TX_CONFIG       (0x19)
#define PN5180_RF_STATUS           (0x1d)
#define PN5180_SYSTEM_STATUS       (0x24)
#define PN5180_TEMP_CONTROL        (0x25)
#define PN5180_DIE_IDENTIFIER      (0x00)
#define PN5180_PRODUCT_VERSION     (0x10)
#define PN5180_FIRMWARE_VERSION    (0x12)
#define PN5180_EEPROM_VERSION      (0x14)
#define IRQ_PIN_CONFIG      (0x1A)

typedef enum{
  PN5180_TXPN5180_RX_STATUS_IDLE       = 0,
  PN5180_TXPN5180_RX_STATUS_WAIT_TX    = 1,
  PN5180_TXPN5180_RX_STATUS_TX         = 2,
  PN5180_TXPN5180_RX_STATUS_WAIT_RX    = 3,
  PN5180_TXPN5180_RX_STATUS_WAIT_DATA  = 4,
  PN5180_TXPN5180_RX_STATUS_RX         = 5,
  PN5180_TXPN5180_RX_STATUS_LOOPBACK   = 6,
  PN5180_TXPN5180_RX_STATUS_RESERVED   = 7
}pn5180_txrx_status_t;

#define PN5180_RX_IRQ_STAT         (1<<0)  // End of RF rececption IRQ
#define PN5180_TX_IRQ_STAT         (1<<1)  // End of RF transmission IRQ
#define PN5180_IDLE_IRQ_STAT       (1<<2)  // IDLE IRQ
#define PN5180_RFOFF_DET_IRQ_STAT  (1<<6)  // RF Field OFF detection IRQ
#define PN5180_RFON_DET_IRQ_STAT   (1<<7)  // RF Field ON detection IRQ
#define PN5180_TX_RFOFF_IRQ_STAT   (1<<8)  // RF Field OFF in PCD IRQ
#define PN5180_TX_RFON_IRQ_STAT    (1<<9)  // RF Field ON in PCD IRQ
#define PN5180_RX_SOF_DET_IRQ_STAT (1<<14) // RF SOF Detection IRQ

extern void pn5180_set_reset(char level);
extern void pn5180_set_cs(char level);
extern char pn5180_get_busy();
extern void pn5180_delay(int time);
extern uint8_t pn5180_spi_send(uint8_t data);
void        pn5180_begin();
void        pn5180_reset(); 
bool        pn5180_read_eeprom(uint8_t addr, uint8_t *buffer, int len);
uint32_t    pn5180_get_IRQ_status(); 
bool        pn5180_write_register(uint8_t reg, uint32_t value); 
uint8_t *   pn5180_read_data(int len, uint8_t *buffer); 
bool        pn5180_load_RF_config(uint8_t tx, uint8_t rx); 
bool        pn5180_write_register_with_AND_mask(uint8_t reg, uint32_t mask);
bool        pn5180_write_register_with_OR_mask(uint8_t reg, uint32_t mask);
bool        pn5180_set_RF_on(); 
bool        pn5180_read_register(uint8_t reg, uint32_t *value);
bool        pn5180_clear_IRQ_status(uint32_t mask); 
bool        pn5180_send_data(uint8_t *data, int len, uint8_t validBits);
pn5180_txrx_status_t pn5180_get_transceive_state(); 
#endif 
