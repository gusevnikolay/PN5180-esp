#ifndef PN5180ISO15693_H
#define PN5180ISO15693_H

#include "PN5180.h"

typedef enum {
  PN5180_ISO15693_ERR_NO_CARD               = -1,
  PN5180_ISO15693_ERR_OK                    = 0,
  PN5180_ISO15693_ERR_NOT_SUPPORTED         = 0x01,
  PN5180_ISO15693_ERR_NOT_RECOGNIZED        = 0x02,
  PN5180_ISO15693_ERR_OPTION_NOT_SUPPORTED  = 0x03,
  PN5180_ISO15693_ERR_UNKNOWN_ERROR         = 0x0f,
  PN5180_ISO15693_ERR_BLOCK_NOT_AVAILABLE   = 0x10,
  PN5180_ISO15693_ERR_BLOCK_ALREADY_LOCKED  = 0x11,
  PN5180_ISO15693_ERR_BLOCK_IS_LOCKED       = 0x12,
  PN5180_ISO15693_ERR_BLOCK_NOT_PROGRAMMED  = 0x13,
  PN5180_ISO15693_ERR_BLOCK_NOT_LOCKED      = 0x14,
  PN5180_ISO15693_ERR_CUSTOM_CMD_ERROR      = 0xA0
}pn5180_iso15693_error_code_t;

pn5180_iso15693_error_code_t pn5180_iso15693_issue_command(uint8_t *cmd, uint8_t cmdLen, uint8_t **resultPtr) ;
pn5180_iso15693_error_code_t pn5180_iso15693_write_single_block(uint8_t *uid, uint8_t blockNo, uint8_t *blockData, uint8_t blockSize);
pn5180_iso15693_error_code_t pn5180_iso15693_get_inventory(uint8_t *uid);
pn5180_iso15693_error_code_t pn5180_iso15693_read_single_block(uint8_t *uid, uint8_t blockNo, uint8_t *blockData, uint8_t blockSize);
pn5180_iso15693_error_code_t pn5180_iso15693_write_single_block(uint8_t *uid, uint8_t blockNo, uint8_t *blockData, uint8_t blockSize);
pn5180_iso15693_error_code_t pn5180_iso15693_get_system_info(uint8_t *uid, uint8_t *blockSize, uint8_t *numBlocks);
pn5180_iso15693_error_code_t pn5180_iso15693_get_random_number(uint8_t *randomData);
pn5180_iso15693_error_code_t pn5180_iso15693_set_password(uint8_t *password, uint8_t *random);
pn5180_iso15693_error_code_t pn5180_iso15693_write_password(uint8_t *password, uint8_t *uid);
pn5180_iso15693_error_code_t pn5180_iso15693_enable_privacy(uint8_t *password, uint8_t *random);
pn5180_iso15693_error_code_t pn5180_iso15693_unlock_ICODE_SLIX2(uint8_t *password);
pn5180_iso15693_error_code_t pn5180_iso15693_lock_ICODE_SLIX2(uint8_t *password);
pn5180_iso15693_error_code_t pn5180_iso15693_new_password_ICODE_SLIX2(uint8_t *newpassword, uint8_t *oldpassword, uint8_t *uid);
bool                         pn5180_iso15693_setup_RF();  

#endif 
