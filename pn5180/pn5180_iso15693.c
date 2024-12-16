#include "pn5180_iso15693.h"
#include <stdlib.h>
#include <stdio.h>
pn5180_iso15693_error_code_t pn5180_iso15693_get_inventory(uint8_t *uid) {
	uint8_t inventory[3] = { 0x26, 0x01, 0x00 };
	for (int i=0; i<8; i++) {
		uid[i] = 0;
	}
	uint8_t * readBuffer;
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_issue_command(inventory, sizeof(inventory), &readBuffer);
	if (PN5180_ISO15693_ERR_OK != rc) {
		return rc;
	}
	for (int i=0; i<8; i++) {
		uid[i] = readBuffer[2+i];
	}
	return PN5180_ISO15693_ERR_OK;
}
pn5180_iso15693_error_code_t pn5180_iso15693_read_single_block(uint8_t *uid, uint8_t blockNo, uint8_t *blockData, uint8_t blockSize) {
	uint8_t readSingleBlock[] = { 0x22, 0x20, 1,2,3,4,5,6,7,8, blockNo };
	for (int i=0; i<8; i++) {
		readSingleBlock[2+i] = uid[i];
	}
	uint8_t *resultPtr;
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_issue_command(readSingleBlock, sizeof(readSingleBlock), &resultPtr);
	if (PN5180_ISO15693_ERR_OK != rc) {
		return rc;
	}
	for (int i=0; i<blockSize; i++) {
		blockData[i] = resultPtr[1+i];
	}
	return PN5180_ISO15693_ERR_OK;
}

pn5180_iso15693_error_code_t pn5180_iso15693_write_single_block(uint8_t *uid, uint8_t blockNo, uint8_t *blockData, uint8_t blockSize) 
{
	uint8_t writeSingleBlock[] = { 0x22, 0x21, 1,2,3,4,5,6,7,8, blockNo }; // UID has LSB first!
	uint8_t writeCmdSize = sizeof(writeSingleBlock) + blockSize;
	uint8_t *writeCmd = (uint8_t*)malloc(writeCmdSize);
	uint8_t pos = 0;
	writeCmd[pos++] = writeSingleBlock[0];
	writeCmd[pos++] = writeSingleBlock[1];
	for (int i=0; i<8; i++) {
		writeCmd[pos++] = uid[i];
	}
	writeCmd[pos++] = blockNo;
	for (int i=0; i<blockSize; i++) {
		writeCmd[pos++] = blockData[i];
	}
	uint8_t *resultPtr = NULL;
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_issue_command(writeCmd, writeCmdSize, &resultPtr);
	free(writeCmd);
	if (PN5180_ISO15693_ERR_OK != rc) {
		return rc;
	}
	return PN5180_ISO15693_ERR_OK;
}

pn5180_iso15693_error_code_t pn5180_iso15693_get_system_info(uint8_t *uid, uint8_t *blockSize, uint8_t *numBlocks) {
	uint8_t sysInfo[] = { 0x22, 0x2b, 1,2,3,4,5,6,7,8 };  // UID has LSB first!
	for (int i=0; i<8; i++) {
		sysInfo[2+i] = uid[i];
	}
	uint8_t *readBuffer = NULL;
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_issue_command(sysInfo, sizeof(sysInfo), &readBuffer);
	if (PN5180_ISO15693_ERR_OK != rc) {
		return rc;
	}
	for (int i=0; i<8; i++) {
		uid[i] = readBuffer[2+i];
	}
	uint8_t *p = &readBuffer[10];
	uint8_t infoFlags = readBuffer[1];
	if (infoFlags & 0x01) { // DSFID flag
		uint8_t dsfid = *p++;
	}
	if (infoFlags & 0x02) { // AFI flag
		uint8_t afi = *p++;
	}
	if (infoFlags & 0x04) { // VICC Memory size
		*numBlocks = *p++;
		*blockSize = *p++;
		*blockSize = (*blockSize) & 0x1f;
		*blockSize = *blockSize + 1; // range: 1-32
		*numBlocks = *numBlocks + 1; // range: 1-256
		uint16_t viccMemSize = (*blockSize) * (*numBlocks);
	}
	if (infoFlags & 0x08) { // IC reference
		uint8_t icRef = *p++;
	}
	return PN5180_ISO15693_ERR_OK;
}

pn5180_iso15693_error_code_t pn5180_iso15693_get_random_number(uint8_t *randomData) {
	uint8_t getrandom[] = {0x02, 0xB2, 0x04};
	uint8_t *readBuffer;
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_issue_command(getrandom, sizeof(getrandom), &readBuffer);
	if (rc == PN5180_ISO15693_ERR_OK) {
		randomData[0] = readBuffer[1];
		randomData[1] = readBuffer[2];
	}
	return rc;
}


pn5180_iso15693_error_code_t pn5180_iso15693_set_password(uint8_t *password, uint8_t *random) {
	uint8_t setPassword[] = {0x02, 0xB3, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00};
	uint8_t *readBuffer = NULL;
	setPassword[4] = password[0] ^ random[0];
	setPassword[5] = password[1] ^ random[1];
	setPassword[6] = password[2] ^ random[0];
	setPassword[7] = password[3] ^ random[1];
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_issue_command(setPassword, sizeof(setPassword), &readBuffer);
	return rc;
}

pn5180_iso15693_error_code_t pn5180_iso15693_enable_privacy(uint8_t *password, uint8_t *random) {
	uint8_t setPrivacy[] = {0x02, 0xBA, 0x04, 0x00, 0x00, 0x00, 0x00};
	uint8_t *readBuffer = NULL;
	setPrivacy[3] = password[0] ^ random[0];
	setPrivacy[4] = password[1] ^ random[1];
	setPrivacy[5] = password[2] ^ random[0];
	setPrivacy[6] = password[3] ^ random[1];
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_issue_command(setPrivacy, sizeof(setPrivacy), &readBuffer);
	return rc;
}

pn5180_iso15693_error_code_t pn5180_iso15693_write_password(uint8_t *password, uint8_t *uid) {
	uint8_t writePassword[] = {0x22, 0xB4, 0x04, uid[0], uid[1], uid[2], uid[3], uid[4], uid[5], uid[6], uid[7], 0x04, 0x00, 0x00, 0x00, 0x00};
	uint8_t *readBuffer = NULL;
	writePassword[12] = password[0];
	writePassword[13] = password[1];
	writePassword[14] = password[2];
	writePassword[15] = password[3];
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_issue_command(writePassword, sizeof(writePassword), &readBuffer);
	return rc;
}

pn5180_iso15693_error_code_t pn5180_iso15693_unlock_ICODE_SLIX2(uint8_t *password) {
	uint8_t random[]= {0x00, 0x00};
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_get_random_number(random);
	if (rc != PN5180_ISO15693_ERR_OK) {
		return rc;
	}
	rc = pn5180_iso15693_set_password(password, random);
	return rc;
}

pn5180_iso15693_error_code_t pn5180_iso15693_lock_ICODE_SLIX2(uint8_t *password) {
	uint8_t random[]= {0x00, 0x00};
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_get_random_number(random);
	if (rc != PN5180_ISO15693_ERR_OK) {
		return rc;
	}
	rc = pn5180_iso15693_enable_privacy(password, random);
	return rc;
}

pn5180_iso15693_error_code_t pn5180_iso15693_new_password_ICODE_SLIX2(uint8_t *newpassword, uint8_t *oldpassword, uint8_t *uid) {
	uint8_t random[]= {0x00, 0x00};
	pn5180_iso15693_error_code_t rc = pn5180_iso15693_get_random_number(random);
	if (rc != PN5180_ISO15693_ERR_OK) {
		return rc;
	}
	rc = pn5180_iso15693_set_password(oldpassword, random);
	if (rc != PN5180_ISO15693_ERR_OK) {
		return rc;
	}
	rc = pn5180_iso15693_write_password(newpassword, uid); 
	return rc;
}

pn5180_iso15693_error_code_t pn5180_iso15693_issue_command(uint8_t *cmd, uint8_t cmdLen, uint8_t **resultPtr) 
{
	pn5180_send_data(cmd, cmdLen, 0);
	pn5180_delay(10);
	uint32_t status = pn5180_get_IRQ_status();
	if (0 == (status & PN5180_RX_SOF_DET_IRQ_STAT)) {
		return PN5180_ISO15693_ERR_NO_CARD;
	}
	while (0 == (status & PN5180_RX_IRQ_STAT)) {
		pn5180_delay(10);
		status = pn5180_get_IRQ_status();
	}
	uint32_t rxStatus;
	pn5180_read_register(PN5180_RX_STATUS, &rxStatus);
	uint16_t len = (uint16_t)(rxStatus & 0x000001ff);
	*resultPtr = pn5180_read_data(len, 0);
	if (0L == *resultPtr) {
		return PN5180_ISO15693_ERR_UNKNOWN_ERROR;
	}
	uint32_t irqStatus = pn5180_get_IRQ_status();
	if (0 == (PN5180_RX_SOF_DET_IRQ_STAT & irqStatus)) { 
		pn5180_clear_IRQ_status(PN5180_TX_IRQ_STAT | PN5180_IDLE_IRQ_STAT);
		return PN5180_ISO15693_ERR_NO_CARD;
	}
	uint8_t responseFlags = (*resultPtr)[0];
	if (responseFlags & (1<<0)) { 
		uint8_t errorCode = (*resultPtr)[1];
		if (errorCode >= 0xA0) { 
			return PN5180_ISO15693_ERR_CUSTOM_CMD_ERROR;
		}
		else 
		{
			return (pn5180_iso15693_error_code_t)errorCode;
		}
	}
	pn5180_clear_IRQ_status(PN5180_RX_SOF_DET_IRQ_STAT | PN5180_IDLE_IRQ_STAT | PN5180_TX_IRQ_STAT | PN5180_RX_IRQ_STAT);
	return PN5180_ISO15693_ERR_OK;
}

bool pn5180_iso15693_setup_RF() 
{
	if(!pn5180_load_RF_config(0x0d, 0x8d)) return false;
	if(!pn5180_set_RF_on()) return false;
	pn5180_write_register_with_AND_mask(PN5180_SYSTEM_CONFIG, 0xfffffff8); 
	pn5180_write_register_with_OR_mask(PN5180_SYSTEM_CONFIG, 0x00000003);  
	return true;
}

const char * pn5180_iso15693_strerror(pn5180_iso15693_error_code_t errno) {
	switch (errno) {
		case PN5180_ISO15693_ERR_NO_CARD: return ("No card detected!");
		case PN5180_ISO15693_ERR_OK: return ("OK!");
		case PN5180_ISO15693_ERR_NOT_SUPPORTED: return ("Command is not supported!");
		case PN5180_ISO15693_ERR_NOT_RECOGNIZED: return ("Command is not recognized!");
		case PN5180_ISO15693_ERR_OPTION_NOT_SUPPORTED: return ("Option is not supported!");
		case PN5180_ISO15693_ERR_UNKNOWN_ERROR: return ("Unknown error!");
		case PN5180_ISO15693_ERR_BLOCK_NOT_AVAILABLE: return ("Specified block is not available!");
		case PN5180_ISO15693_ERR_BLOCK_ALREADY_LOCKED: return ("Specified block is already locked!");
		case PN5180_ISO15693_ERR_BLOCK_IS_LOCKED: return ("Specified block is locked and cannot be changed!");
		case PN5180_ISO15693_ERR_BLOCK_NOT_PROGRAMMED: return ("Specified block was not successfully programmed!");
		case PN5180_ISO15693_ERR_BLOCK_NOT_LOCKED: return ("Specified block was not successfully locked!");
		default:
			if ((errno >= 0xA0) && (errno <= 0xDF)) {
			return ("Custom command error code!");
		}
		else {
			return ("Undefined error code in ISO15693!");
		}
	}
}
