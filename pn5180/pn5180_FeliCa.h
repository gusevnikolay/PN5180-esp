#ifndef PN5180_FeliCa_H_
#define PN5180_FeliCa_H_
#include "PN5180.h"
uint8_t pn5180_FeliCa_pol_req(uint8_t *buffer);
bool    pn5180_FeliCa_setup_RF();
uint8_t pn5180_FeliCa_read_card_serial(uint8_t *buffer);    
bool    pn5180_FeliCa_is_card_present();    
#endif 
