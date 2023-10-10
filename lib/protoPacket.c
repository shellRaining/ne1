#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "protoPacket.h"

ProtoPacket* createPacket(uint16_t type, uint32_t len, char* msg) {
  ProtoPacket* pPacket = malloc(sizeof(ProtoPacket));
  memset(pPacket, 0, sizeof(ProtoPacket));

  pPacket->head.type = type;
  pPacket->head.len  = len;
  pPacket->msg       = malloc(len);
  memset(pPacket->msg, 0, len);
  if (msg != NULL)
    memcpy(pPacket->msg, msg, len);
  return pPacket;
}

void serialization(uint8_t* pdata, ProtoPacket* pPacket) {
  // version number
  *pdata = 1;
  pdata++;

  // magic number
  *pdata = MAGIC_NUM;
  pdata++;

  // packet type
  *(uint16_t*)pdata = htons(pPacket->head.type);
  pdata += 2;

  // packet length
  *(uint32_t*)pdata = htons(pPacket->head.len);
  pdata += 4;

  // packet data
  memcpy(pdata, pPacket->msg, pPacket->head.len);

  printf("\n");
}
void deserialization(uint8_t* pdata, ProtoPacket* pPacket) {
  // version number
  pdata++;

  // magic number
  pdata++;

  // packet type
  pPacket->head.type = ntohs(*(uint16_t*)pdata);
  pdata += 2;

  // packet length
  pPacket->head.len = ntohs(*(uint16_t*)pdata);
  pdata += 4;

  // packet data
  memcpy(pPacket->msg, pdata, pPacket->head.len);
}
