#include "pn5180.h"
#include <stdio.h>
#define PN5180_WRITE_REGISTER           (0x00)
#define PN5180_WRITE_REGISTER_OR_MASK   (0x01)
#define PN5180_WRITE_REGISTER_AND_MASK  (0x02)
#define PN5180_READ_REGISTER            (0x04)
#define PN5180_WRITE_EEPROM             (0x06)
#define PN5180_READ_EEPROM              (0x07)
#define PN5180_SEND_DATA                (0x09)
#define PN5180_READ_DATA                (0x0A)
#define PN5180_SWITCH_MODE              (0x0B)
#define PN5180_LOAD_RF_CONFIG           (0x11)
#define PN5180_RF_ON                    (0x16)
#define PN5180_RF_OFF                   (0x17)

uint8_t pn5180_read_buffer[508];
bool pn5180_transceive_command(uint8_t *prepay, uint8_t *payload, int payload_szie, uint8_t *receiver, int receive_size, int *timeout);

void pn5180_begin() 
{
    pn5180_set_cs(1);
    pn5180_set_reset(1);
}

void pn5180_end()
{
    pn5180_set_cs(1);
}

bool pn5180_write_register(uint8_t reg, uint32_t value, int *timeout) 
{
	uint8_t *ptr = (uint8_t*)&value;
	uint8_t buffer[6] = {PN5180_WRITE_REGISTER, reg, ptr[0], ptr[1], ptr[2], ptr[3]};
	pn5180_transceive_command(0, buffer, 6, 0, 0, timeout);
	return true;
}

bool pn5180_write_register_with_OR_mask(uint8_t reg, uint32_t mask, int *timeout) 
{
	uint8_t *ptr = (uint8_t*)&mask;
	uint8_t buffer[6] = {PN5180_WRITE_REGISTER_OR_MASK, reg, ptr[0], ptr[1], ptr[2], ptr[3]};
	pn5180_transceive_command(0, buffer, 6, 0, 0, timeout);
	return true;
}

bool pn5180_write_register_with_AND_mask(uint8_t reg, uint32_t mask, int *timeout) 
{
	uint8_t *ptr = (uint8_t*)&mask;
	uint8_t buffer[6] = {PN5180_WRITE_REGISTER_AND_MASK, reg, ptr[0], ptr[1], ptr[2], ptr[3]};
	pn5180_transceive_command(0, buffer, 6, 0, 0, timeout);
	return true;
}

bool pn5180_read_register(uint8_t reg, uint32_t *value, int *timeout)
{
	uint8_t cmd[2] = {PN5180_READ_REGISTER, reg};
	pn5180_transceive_command(0, cmd, 2, (uint8_t*)value, 4, timeout);
	return true;
}

bool pn5180_write_eeprom(uint8_t addr, uint8_t *data, int len, int *timeout) 
{
	if ((addr > 254) || ((addr+len) > 254))return false;
	uint8_t precmd[2];
	precmd[0] = PN5180_WRITE_EEPROM;
	precmd[1] = addr;
	pn5180_transceive_command(precmd, data, len, 0, 0, timeout);
	return true;
}

bool pn5180_read_eeprom(uint8_t addr, uint8_t *buffer, int len, int *timeout) 
{
	if ((addr > 254) || ((addr+len) > 254)) return false;
	uint8_t cmd[3] = { PN5180_READ_EEPROM, addr, len };
	pn5180_transceive_command(0, cmd, 3, buffer, len, timeout);
	return true;
}

bool pn5180_send_data(uint8_t *data, int len, uint8_t validBits, int *timeout) 
{
	if (len > 260) return false;
	uint8_t precmd[2];
	precmd[0] = PN5180_SEND_DATA;
	precmd[1] = validBits;
	pn5180_write_register_with_AND_mask(PN5180_SYSTEM_CONFIG, 0xfffffff8, timeout);  
	pn5180_write_register_with_OR_mask(PN5180_SYSTEM_CONFIG, 0x00000003, timeout);   
	pn5180_txrx_status_t status = pn5180_get_transceive_state(timeout);
	if (PN5180_TXPN5180_RX_STATUS_WAIT_TX != status) {
		return false;
	}
	pn5180_transceive_command(precmd, data, len, 0, 0, timeout);
	return true;
}

uint8_t * pn5180_read_data(int len, uint8_t *buffer, int *timeout) 
{
	if (len > 508) return 0L;
	if (buffer == 0) buffer = pn5180_read_buffer;
	uint8_t cmd[2] = { PN5180_READ_DATA, 0x00 };
	pn5180_transceive_command(0, cmd, 2, buffer, len, timeout);
	return pn5180_read_buffer;
}

bool pn5180_load_RF_config(uint8_t tx, uint8_t rx, int *timeout) 
{
	uint8_t cmd[3] = { PN5180_LOAD_RF_CONFIG, tx, rx };
	pn5180_transceive_command(0, cmd, 3, 0, 0, timeout);
	return true;
}
  
bool pn5180_set_RF_on(int *timeout) 
{
	uint8_t cmd[2] = { PN5180_RF_ON, 0x00 };
	pn5180_transceive_command(0, cmd, 2, 0, 0, timeout);
	while (0 == (PN5180_TX_RFON_IRQ_STAT & pn5180_get_IRQ_status(timeout)))pn5180_delay(10); 
	pn5180_clear_IRQ_status(PN5180_TX_RFON_IRQ_STAT, timeout);
	return true;
}

bool pn5180_setRF_off(int *timeout) {
	uint8_t cmd[2] = { PN5180_RF_OFF, 0x00 };
	pn5180_transceive_command(0, cmd, 2, 0, 0, timeout);
	while (0 == (PN5180_TX_RFOFF_IRQ_STAT & pn5180_get_IRQ_status(timeout)));
	pn5180_clear_IRQ_status(PN5180_TX_RFOFF_IRQ_STAT, timeout);
	return true;
}

bool pn5180_transceive_command(uint8_t *prepay, uint8_t *payload, int payload_szie, uint8_t *receiver, int receive_size, int *timeout) {
	while (pn5180_get_busy() && (*timeout > 0)){pn5180_delay(5); *timeout = *timeout - 5;} 
	if(*timeout <= 0) return false;
	pn5180_set_cs(0);
	if(prepay != 0){
		pn5180_spi_send(prepay[0]);
		pn5180_spi_send(prepay[1]);
	}
	for(uint8_t i=0; i<payload_szie; i++) {
		pn5180_spi_send(payload[i]);
	}
	while (!pn5180_get_busy() && (*timeout > 0)){pn5180_delay(5); *timeout = *timeout - 5;} 
	if(*timeout <= 0) return false;
	pn5180_set_cs(1); pn5180_delay(10);
	while (pn5180_get_busy() && (*timeout > 0)){pn5180_delay(5); *timeout = *timeout - 5;} 
	if(*timeout <= 0) return false;
	if ((0 == receiver) || (0 == receive_size)) return true;
	pn5180_set_cs(0); pn5180_delay(20);
	for (uint8_t i=0; i<receive_size; i++) {
		receiver[i] = pn5180_spi_send(0xff);
	}
	while (!pn5180_get_busy() && (*timeout > 0)){pn5180_delay(5); *timeout = *timeout - 5;} 
	if(*timeout <= 0) return false;
	pn5180_set_cs(1); pn5180_delay(10);
	while (pn5180_get_busy() && (*timeout > 0)){pn5180_delay(5); *timeout = *timeout - 5;} 
	if(*timeout <= 0) return false;
	return true;
}

void pn5180_reset(int *timeout) 
{
	pn5180_set_reset(0);   
	pn5180_delay(10);
	pn5180_set_reset(1);  
	pn5180_delay(10);
	while (0 == (PN5180_IDLE_IRQ_STAT & pn5180_get_IRQ_status(timeout)))pn5180_delay(10);  
	pn5180_clear_IRQ_status(0xffffffff, timeout);  
}

uint32_t pn5180_get_IRQ_status(int *timeout) 
{
	uint32_t irq_status;
	pn5180_read_register(PN5180_IRQ_STATUS, &irq_status, timeout);
	return irq_status;
}

bool pn5180_clear_IRQ_status(uint32_t mask, int *timeout) 
{
	return pn5180_write_register(PN5180_IRQ_CLEAR, mask, timeout);
}

pn5180_txrx_status_t pn5180_get_transceive_state(int *timeout) 
{
	uint32_t status;
	if (!pn5180_read_register(PN5180_RF_STATUS, &status, timeout)) {
		return PN5180_TXPN5180_RX_STATUS_IDLE;
	}
	uint8_t state = ((status >> 24) & 0x07);
	return (pn5180_txrx_status_t)state;
}
