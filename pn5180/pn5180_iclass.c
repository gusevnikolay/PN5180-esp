#include "PN5180_iclass.h"
#include <string.h>

pn5180_iclass_error_t pn5180_iclass_activate_all() 
{
	pn5180_write_register(PN5180_CRC_TX_CONFIG, 0x00000000);
	pn5180_write_register(PN5180_CRC_RX_CONFIG, 0x00000000);
	uint8_t actall = (uint8_t)PN5180_ICLASS_CMD_ACTALL;
	uint8_t *rx_buffer = NULL;
	pn5180_iclass_error_t rc = pn5180_iclass_issue_command(&actall, 1, rx_buffer);
	if (rc != PN5180_ICLASS_ERROR_OK)return rc;
	return PN5180_ICLASS_ERROR_OK;
}

pn5180_iclass_error_t pn5180_iclass_identify(uint8_t *csn) 
{
	pn5180_write_register(PN5180_CRC_TX_CONFIG, 0x00000000);
	pn5180_write_register(PN5180_CRC_RX_CONFIG, 0x00000029);
	uint8_t identify = (uint8_t)PN5180_ICLASS_CMD_IDENTIFY;
	memset(csn, 0, 8);
	uint8_t *rx_buffer = NULL;
	pn5180_iclass_error_t rc = pn5180_iclass_issue_command(&identify, 1, rx_buffer);
	if (rc != PN5180_ICLASS_ERROR_OK) return rc;
	for (int i=0; i<8; i++) {
		csn[i] = rx_buffer[i];
	}
	return PN5180_ICLASS_ERROR_OK;
}


pn5180_iclass_error_t pn5180_iclass_select(uint8_t *csn) 
{
	pn5180_write_register(PN5180_CRC_TX_CONFIG, 0x00000000);
	pn5180_write_register(PN5180_CRC_RX_CONFIG, 0x00000029);
	uint8_t select[9] = {PN5180_ICLASS_CMD_SELECT, 1, 2, 3, 4, 5, 6, 7, 8};
	for (int i=0; i<8; i++) {
		select[i+1] = csn[i];
	}
	uint8_t *rx_buffer = NULL;
	pn5180_iclass_error_t rc = pn5180_iclass_issue_command(select, sizeof(select), rx_buffer);
	if (PN5180_ICLASS_ERROR_OK != rc) return rc;
	for (int i=0; i<8; i++) {
		csn[i] = rx_buffer[i];
	}
	return PN5180_ICLASS_ERROR_OK;
}

pn5180_iclass_error_t pn5180_iclass_read_check(uint8_t *ccnr) 
{
	pn5180_write_register(PN5180_CRC_TX_CONFIG, 0x00000000);
	pn5180_write_register(PN5180_CRC_RX_CONFIG, 0x00000000);
	uint8_t read_check[2] = {PN5180_ICLASS_CMD_READ_CHECK, 0x02};
	uint8_t *rx_buffer = NULL;
	pn5180_iclass_error_t rc = pn5180_iclass_issue_command(read_check, sizeof(read_check), rx_buffer);
	if (rc != PN5180_ICLASS_ERROR_OK) return rc;
	for (int i=0; i<8; i++) {
		ccnr[i] = rx_buffer[i];
	}
	return PN5180_ICLASS_ERROR_OK;
}

pn5180_iclass_error_t pn5180_iclass_check(uint8_t *mac) 
{
	pn5180_write_register(PN5180_CRC_TX_CONFIG, 0x00000000);
	pn5180_write_register(PN5180_CRC_RX_CONFIG, 0x00000000);
	uint8_t check[] = {PN5180_ICLASS_CMD_CHECK, 0, 0, 0, 0, 1, 2, 3, 4};
	for (int i=0; i<4; i++) {
		check[i+5] = mac[i];
	}
	uint8_t *rx_buffer = NULL;
	pn5180_iclass_error_t rc = pn5180_iclass_issue_command(check, sizeof(check), rx_buffer);
	if (rc != PN5180_ICLASS_ERROR_OK) return rc;
	return PN5180_ICLASS_ERROR_OK;
}

pn5180_iclass_error_t pn5180_iclass_read(uint8_t blockNum, uint8_t *block_data) 
{
	pn5180_write_register(PN5180_CRC_TX_CONFIG, 0x00000069);
	pn5180_write_register(PN5180_CRC_RX_CONFIG, 0x00000029);
	uint8_t read[] = {PN5180_ICLASS_CMD_READ, blockNum};
	uint8_t *rx_buffer = NULL;
	pn5180_iclass_error_t rc = pn5180_iclass_issue_command(read, sizeof(read), rx_buffer);
	if (rc != PN5180_ICLASS_ERROR_OK)return rc;
	for (int i=0; i<8; i++) {
		block_data[i] = rx_buffer[i];
	}
	return PN5180_ICLASS_ERROR_OK;
}

pn5180_iclass_error_t pn5180_iclass_halt() 
{
	pn5180_write_register(PN5180_CRC_TX_CONFIG, 0x00000000);
	pn5180_write_register(PN5180_CRC_RX_CONFIG, 0x00000000);
	uint8_t halt[] = {PN5180_ICLASS_CMD_HALT};
	uint8_t *rx_buffer = NULL;
	pn5180_iclass_error_t rc = pn5180_iclass_issue_command(halt, sizeof(halt), rx_buffer);
	if (rc != PN5180_ICLASS_ERROR_OK) return rc;
	return PN5180_ICLASS_ERROR_OK;
}

pn5180_iclass_error_t pn5180_iclass_issue_command(uint8_t *cmd, uint8_t cmdLen, uint8_t *result) 
{
	pn5180_send_data(cmd, cmdLen, 0);
	pn5180_delay(10);
	if ((pn5180_get_IRQ_status() & PN5180_RX_SOF_DET_IRQ_STAT) == 0) {
		return PN5180_ICLASS_ERROR_NO_CARD;
	}
	uint32_t status;
	pn5180_read_register(PN5180_RX_STATUS, &status);
	uint16_t len = (uint16_t)(status & 0x000001ff);
	uint8_t * ptr = pn5180_read_data(len, 0);
	if (*ptr == 0) {
		return PN5180_ICLASS_ERROR_UNKNOWN;
	}
	uint32_t irqStatus = pn5180_get_IRQ_status();
	if ((PN5180_RX_SOF_DET_IRQ_STAT & irqStatus) == 0) {
		pn5180_clear_IRQ_status(PN5180_TX_IRQ_STAT | PN5180_IDLE_IRQ_STAT);
		return PN5180_ICLASS_ERROR_NO_CARD;
	}
	if (PN5180_RX_SOF_DET_IRQ_STAT == (PN5180_RX_SOF_DET_IRQ_STAT & irqStatus)) {
		pn5180_clear_IRQ_status(PN5180_RX_SOF_DET_IRQ_STAT);
		return PN5180_ICLASS_ERROR_OK;
	}
	uint8_t responseFlags = (ptr)[0];
	if (responseFlags & (1<<0)) { 
		uint8_t errorCode = (ptr)[1];
		return (pn5180_iclass_error_t)errorCode;
	}
	pn5180_clear_IRQ_status(PN5180_RX_SOF_DET_IRQ_STAT | PN5180_IDLE_IRQ_STAT | PN5180_TX_IRQ_STAT | PN5180_RX_IRQ_STAT);
	return PN5180_ICLASS_ERROR_OK;
}

bool pn5180_iclass_setup_RF() 
{
	if(!pn5180_load_RF_config(0x0d, 0x8d)) return false;
	if(!pn5180_set_RF_on())return false;
	pn5180_write_register_with_AND_mask(PN5180_SYSTEM_CONFIG, 0xfffffff8);  // Idle/StopCom Command
	pn5180_write_register_with_OR_mask(PN5180_SYSTEM_CONFIG, 0x00000003);   // Transceive Command
	return true;
}

