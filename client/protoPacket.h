#include <stdint.h>

#define MAGIC_NUM 0x55
#define HEAD_LEN 8

enum {
  QUERY_TIME     = 0x01,
  QUERY_NAME     = 0x02,
  QUERY_ACTIVE   = 0x03,
  QUERY_SEND_MSG = 0x04,
  REPLY_TIME     = 0x05,
  REPLY_NAME     = 0x06,
  REPLY_ACTIVE   = 0x07,
  REPLY_SEND_MSG = 0x08,
};

typedef struct _protoHead {
  uint8_t  version;
  uint8_t  magic;
  uint16_t type;
  uint32_t len;
} ProtoHead;

typedef struct _protoPacket {
  ProtoHead head;
  char*     msg;
} ProtoPacket;

ProtoPacket* createPacket(uint16_t type, uint32_t len, uint8_t* msg);
void         serialization(uint8_t* pdata, ProtoPacket* pPacket);
void         deserialization(uint8_t* pdata, ProtoPacket* pPacket);
