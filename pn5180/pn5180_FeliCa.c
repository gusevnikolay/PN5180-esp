#include "pn5180_FeliCa.h"
#include "pn5180.h"

bool pn5180_FeliCa_setup_RF() {
  if (!pn5180_load_RF_config(0x09, 0x89)) return false;
  if (!pn5180_set_RF_on()) return false;
  return true;
}

uint8_t pn5180_FeliCa_pol_req(uint8_t *buffer) {
	uint8_t cmd[6];
	uint8_t uidLength = 0;
	if (!pn5180_load_RF_config(0x09, 0x89)) return 0;
	if (!pn5180_write_register_with_AND_mask(PN5180_SYSTEM_CONFIG, 0xFFFFFFBF)) return 0;
	cmd[0] = 0x06;             //total length
	cmd[1] = 0x00;             //POL_REQ command
	cmd[2] = 0xFF;             //
	cmd[3] = 0xFF;             // any target
	cmd[4] = 0x01;             // System Code request
	cmd[5] = 0x00;             // 1 timeslot only
	if (!pn5180_send_data(cmd, 6, 0x00)) return 0;
	pn5180_delay(50);
	uint8_t *internalBuffer = pn5180_read_data(20, NULL);
	if (!internalBuffer) return 0;
	for (int i=0; i<20; i++)buffer[i] = internalBuffer[i];
	if( buffer[1] != 0x01){
		uidLength = 0;
	} else {
		uidLength = 8;
	}
	return uidLength;
}

uint8_t pn5180_FeliCa_read_card_serial(uint8_t *buffer) {
	uint8_t response[20];
	uint8_t uidLength;
	for (int i = 0; i < 20; i++)response[i] = 0;
	uidLength = pn5180_FeliCa_pol_req(response);
	if (uidLength == 0) return 0;
	for (int i = 0; i < uidLength; i++) buffer[i] = response[i+2];
	return uidLength;
}

bool pn5180_FeliCa_is_card_present() {
	uint8_t buffer[8];
	return (pn5180_FeliCa_read_card_serial(buffer) != 0);
}
