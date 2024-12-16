#ifndef PN5180_ICLASS_H
#define PN5180_ICLASS_H

#include "PN5180.h"

typedef enum {
  PN5180_ICLASS_CMD_HALT = 0x00,
  PN5180_ICLASS_CMD_ACTALL = 0x0A,
  PN5180_ICLASS_CMD_IDENTIFY = 0x0C,
  PN5180_ICLASS_CMD_SELECT = 0x81,
  PN5180_ICLASS_CMD_READ_CHECK = 0x88,
  PN5180_ICLASS_CMD_CHECK = 0x05,
  PN5180_ICLASS_CMD_READ = 0x0C,
} pn5180_iclass_cmd_t;

typedef enum {
  PN5180_ICLASS_ERROR_NO_CARD = -1,
  PN5180_ICLASS_ERROR_OK      = 0,
  PN5180_ICLASS_ERROR_UNKNOWN = 0xFE,
}pn5180_iclass_error_t;
pn5180_iclass_error_t pn5180_iclass_issue_command(uint8_t *cmd, uint8_t cmdLen, uint8_t *result); 
pn5180_iclass_error_t pn5180_iclass_activate_all();
pn5180_iclass_error_t pn5180_iclass_identify(uint8_t *csn);
pn5180_iclass_error_t pn5180_iclass_select(uint8_t *csn);
pn5180_iclass_error_t pn5180_iclass_read_check(uint8_t *ccnr);
pn5180_iclass_error_t pn5180_iclass_check(uint8_t *mac);
pn5180_iclass_error_t pn5180_iclass_read(uint8_t blockNum, uint8_t *blockData);
pn5180_iclass_error_t pn5180_iclass_halt();
bool                  pn5180_iclass_setup_RF();

#endif 
