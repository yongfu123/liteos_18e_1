#ifndef KFIFO_H
#define KFIFO_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "boardconfig.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifndef min
      #define min(x,y) ((x) < (y) ? x : y)
#endif

struct kfifo {
    unsigned char *buffer; /* the buffer holding the data */
    unsigned int size; /* the size of the allocated buffer */
    unsigned int in; /* data is added at offset (in % size) */
    unsigned int out; /* data is extracted from off. (out % size) */
};

static inline unsigned int __kfifo_len(struct kfifo *fifo)
{
    return fifo->in - fifo->out;
}

extern struct kfifo recvfifo;
extern u8 USART_Receive_Buf[UF_MAX_LEN];
extern void kfifo_init(struct kfifo *fifo);
extern unsigned int __kfifo_put(struct kfifo *fifo, unsigned char *buffer, unsigned int len);
extern void __kfifo_put_singlebyte(struct kfifo *fifo, unsigned char data);
extern unsigned int __kfifo_get(struct kfifo *fifo, unsigned char *buffer, unsigned int len);

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
