/* 
 * File:   DHT11.h
 * Author: Javier
 *
 * Created on 8 de julio de 2013, 19:57
 */

#ifndef DHT11_H
#define	DHT11_H

#include <xc.h>
#include <GenericTypeDefs.h>



#define DATA      RA3
#define DATA_DIR  TRISA3

void initDHT11();
void StartSignal();
BYTE CheckResponse();
BYTE readByte();

#endif	/* DHT11_H */

