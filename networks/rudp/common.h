#ifndef __COMMON_H__
#define __COMMON_H__
typedef struct msg { 
    uint16_t proto;
    uint16_t frag_seq;		
    uint16_t msg_seq; 
    uint16_t msg_len;
    uint32_t crc_value;
    char *data;
} __attribute__((packed)) MSG;

#endif /* __COMMON_H__ */

