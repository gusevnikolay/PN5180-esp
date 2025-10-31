#ifndef PN5180ISO14443_H
#define PN5180ISO14443_H

#include "PN5180.h"
uint8_t pn5180_iso14443_activate_typeA(uint8_t *buffer, uint8_t kind, int *timeout);
bool    pn5180_iso14443_mifare_block_read(uint8_t blockno,uint8_t *buffer);
uint8_t pn5180_iso14443_mifare_block_write_16(uint8_t blockno, uint8_t *buffer);
bool    pn5180_iso14443_mifare_halt();
bool    pn5180_iso14443_setup_RF();
uint8_t pn5180_iso14443_read_card_serial(uint8_t *buffer, int *timeout);    
bool    pn5180_iso14443_is_card_present();    
#endif 
