#include "pn5180_iso14443.h"
#include "pn5180.h"

bool pn5180_iso14443_setup_RF() 
{
  if (!pn5180_load_RF_config(0x00, 0x80)) return false;
  if (!pn5180_set_RF_on()) return false;
  return true;
}

uint16_t pn5180_iso14443_rx_bytes_received() {
	uint32_t rxStatus;
	uint16_t len = 0;
	pn5180_read_register(PN5180_RX_STATUS, &rxStatus);
	len = (uint16_t)(rxStatus & 0x000001ff);
	return len;
}
uint8_t pn5180_iso14443_activate_typeA(uint8_t *buffer, uint8_t kind) {
	uint8_t cmd[7];
	uint8_t uidLength = 0;
	if (!pn5180_load_RF_config(0x0, 0x80)) return 0;
	if (!pn5180_write_register_with_AND_mask(PN5180_SYSTEM_CONFIG, 0xFFFFFFBF))return 0;
	if (!pn5180_write_register_with_AND_mask(PN5180_CRC_RX_CONFIG, 0xFFFFFFFE))return 0;
	if (!pn5180_write_register_with_AND_mask(PN5180_CRC_TX_CONFIG, 0xFFFFFFFE))return 0;
	cmd[0] = (kind == 0) ? 0x26 : 0x52;
	if (!pn5180_send_data(cmd, 1, 0x07)) return 0;
	if (!pn5180_read_data(2, buffer)) return 0;
	cmd[0] = 0x93;
	cmd[1] = 0x20;
	if (!pn5180_send_data(cmd, 2, 0x00)) return 0;
	if (!pn5180_read_data(5, cmd+2)) return 0;
	if (!pn5180_write_register_with_OR_mask(PN5180_CRC_RX_CONFIG, 0x01)) return 0;
	if (!pn5180_write_register_with_OR_mask(PN5180_CRC_TX_CONFIG, 0x01)) return 0;
	cmd[0] = 0x93;
	cmd[1] = 0x70;
	if (!pn5180_send_data(cmd, 7, 0x00)) return 0;
	if (!pn5180_read_data(1, buffer+2)) return 0;
	if ((buffer[2] & 0x04) == 0) {
		for (int i = 0; i < 4; i++) buffer[3+i] = cmd[2 + i];
		uidLength = 4;
	} else {
		if (cmd[2] != 0x88) return 0;
		for (int i = 0; i < 3; i++) buffer[3+i] = cmd[3 + i];
		if (!pn5180_write_register_with_AND_mask(PN5180_CRC_RX_CONFIG, 0xFFFFFFFE)) return 0;
		if (!pn5180_write_register_with_AND_mask(PN5180_CRC_TX_CONFIG, 0xFFFFFFFE)) return 0;
		cmd[0] = 0x95;
		cmd[1] = 0x20;
		if (!pn5180_send_data(cmd, 2, 0x00)) return 0;
		if (!pn5180_read_data(5, cmd+2)) return 0;
		for (int i = 0; i < 4; i++) {
			buffer[6 + i] = cmd[2+i];
		}
		if (!pn5180_write_register_with_OR_mask(PN5180_CRC_RX_CONFIG, 0x01))  return 0;
		if (!pn5180_write_register_with_OR_mask(PN5180_CRC_TX_CONFIG, 0x01))  return 0;
		cmd[0] = 0x95;
		cmd[1] = 0x70;
		if (!pn5180_send_data(cmd, 7, 0x00)) return 0;
		if (!pn5180_read_data(1, buffer + 2))return 0;	
		uidLength = 7;
	}
    return uidLength;
}

bool pn5180_iso14443_mifare_block_read(uint8_t blockno, uint8_t *buffer) {
	bool success = false;
	uint16_t len;
	uint8_t cmd[2];
	cmd[0] = 0x30;
	cmd[1] = blockno;
	if (!pn5180_send_data(cmd, 2, 0x00)) return false;
	pn5180_delay(5);
	len = pn5180_iso14443_rx_bytes_received();
	if (len == 16) {
		if (pn5180_read_data(16, buffer))success = true;
	}
	return success;
}


uint8_t pn5180_iso14443_mifare_block_write_16(uint8_t blockno, uint8_t *buffer) {
	uint8_t cmd[2];
	pn5180_write_register_with_AND_mask(PN5180_CRC_RX_CONFIG, 0xFFFFFFFE);
	cmd[0] = 0xA0;
	cmd[1] = blockno;
	pn5180_send_data(cmd, 2, 0x00);
	pn5180_read_data(1, cmd);
	pn5180_send_data(buffer,16, 0x00);
	pn5180_delay(10);
	pn5180_read_data(1, cmd);
	pn5180_write_register_with_OR_mask(PN5180_CRC_RX_CONFIG, 0x1);
	return cmd[0];
}

bool pn5180_iso14443_mifare_halt() {
	uint8_t cmd[2];
	cmd[0] = 0x50;
	cmd[1] = 0x00;
	pn5180_send_data(cmd, 2, 0x00);	
	return true;
}

uint8_t pn5180_iso14443_read_card_serial(uint8_t *buffer) {
    uint8_t response[10];
	uint8_t uidLength;
    for (int i = 0; i < 10; i++) response[i] = 0;
    uidLength = pn5180_iso14443_activate_typeA(response, 1);
	if ((response[0] == 0xFF) && (response[1] == 0xFF)) return 0;
	if ((response[3] == 0x00) && (response[4] == 0x00) && (response[5] == 0x00) && (response[6] == 0x00)) return 0;
	if ((response[3] == 0xFF) && (response[4] == 0xFF) && (response[5] == 0xFF) && (response[6] == 0xFF)) return 0;
    for (int i = 0; i < 7; i++) buffer[i] = response[i+3];
	pn5180_iso14443_mifare_halt();
	return uidLength;  
}

bool pn5180_iso14443_is_card_present() {
    uint8_t buffer[10];
	return (pn5180_iso14443_read_card_serial(buffer) >=4);
}

