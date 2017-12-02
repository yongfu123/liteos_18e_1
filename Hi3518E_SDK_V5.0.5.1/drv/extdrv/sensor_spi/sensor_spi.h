#ifndef __SENSOR_SPI_H__
#define	__SENSOR_SPI_H__
typedef struct hiSPI_MODULE_PARAMS_S
{
    HI_U32 u32BusNum;
    HI_U32 u32csn;
    HI_CHAR cSensor[64];
    
} SPI_MODULE_PARAMS_S;

int ssp_write_alt(unsigned int u32DevAddr, unsigned int u32DevAddrByteNum,
                  unsigned int u32RegAddr, unsigned int u32RegAddrByteNum,
                  unsigned int u32Data , unsigned int u32DataByteNum);
int ssp_read_alt(unsigned char devaddr, unsigned char addr, unsigned char *data);
#endif
