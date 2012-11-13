#ifndef __CRC32_H__
#define __CRC32_H__

void gen_crc_table(void);
unsigned long update_crc(unsigned long, char *data_blk_ptr, int data_blk_size);

#endif  /* __CRC32_H__ */
