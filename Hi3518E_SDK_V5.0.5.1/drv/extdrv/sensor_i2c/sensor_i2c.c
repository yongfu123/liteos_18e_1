#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/i2c.h>

#include <linux/delay.h>
#include "isp_ext.h"

#ifdef __HuaweiLite__
#include <stdio.h>
#include <i2c.h>
#endif

static struct i2c_board_info hi_info =
{
    I2C_BOARD_INFO("sensor_i2c", (0x6c>>1)),
};

static struct i2c_client* sensor_client;


int hi_sensor_i2c_write(unsigned char dev_addr,
                        unsigned int reg_addr, unsigned int reg_addr_num,
                        unsigned int data, unsigned int data_byte_num)
{
    unsigned char tmp_buf[8];
    int ret = 0;
    int idx = 0;
    struct i2c_client* client = sensor_client;

    sensor_client->addr = dev_addr;

    /* reg_addr config */
    tmp_buf[idx++] = reg_addr;
    if (reg_addr_num == 2)
    {
        client->flags  |= I2C_M_16BIT_REG;
        tmp_buf[idx++]  = (reg_addr >> 8);
    }
    else
    {
        client->flags &= ~I2C_M_16BIT_REG;
    }

    /* data config */
    tmp_buf[idx++] = data;
    if (data_byte_num == 2)
    {
        client->flags  |= I2C_M_16BIT_DATA;
        tmp_buf[idx++] = data >> 8;
    }
    else
    {
        client->flags &= ~I2C_M_16BIT_DATA;
    }

    ret = i2c_master_send(client, tmp_buf, idx);

    return ret;
}

static int hi_dev_isp_register(void)
{
    ISP_BUS_CALLBACK_S stBusCb = {0};

    stBusCb.pfnISPWriteI2CData = hi_sensor_i2c_write;

    if ((NULL != FUNC_ENTRY(ISP_EXPORT_FUNC_S, HI_ID_ISP)) && (CKFN_ISP_RegisterBusCallBack()))
    //if (CKFN_ISP_RegisterBusCallBack())
    {
        CALL_ISP_RegisterBusCallBack(0, ISP_BUS_TYPE_I2C, &stBusCb);
    }
    else
    {
        printk("register i2c_write_callback to isp failed, hi_i2c init is failed!\n");
        return -1;
    }

    return 0;
}

#ifdef __HuaweiLite__
struct i2c_client sensor_client_obj;
struct i2c_client * hi_sensor_i2c_client_init(void)
{
    #if 0
	sensor_client_obj.addr = 0x6c;
	sensor_client_obj.flags = 0x00;
	sensor_client_obj.pinfo = &hi_i2c_data[0];
    hi_i2c_hw_init(sensor_client_obj.pinfo);
    
	return &sensor_client_obj;	
    #else
    struct i2c_client *client;
    int ret;

    client = malloc(sizeof(struct i2c_client));
    if (NULL == client){
        printk("malloc i2c_client err. \n");
        return NULL;
    }

    memset(client, 0 ,sizeof(struct i2c_client));
    snprintf(client->name, sizeof(client->name), "dev/i2c_client-i2c%d", 0);
    client->addr = (0x6c>>1);

    ret = client_attach(client, 0);
    if (ret){
        printk("client_attach err. \n");
        free(client);
        return NULL;
    }
    return client;
    #endif

}
#endif


#ifndef __HuaweiLite__
static
#endif
int hi_dev_init(void)
{
    struct i2c_adapter* i2c_adap;

    // use i2c0
    #ifdef __HuaweiLite__
    sensor_client = hi_sensor_i2c_client_init();
    #else
    i2c_adap = i2c_get_adapter(0);
    sensor_client = i2c_new_device(i2c_adap, &hi_info);

    i2c_put_adapter(i2c_adap);
    #endif
    
    hi_dev_isp_register();

    return 0;
}

static void hi_dev_exit(void)
{
#ifndef __HuaweiLite__
    i2c_unregister_device(sensor_client);
#endif	
}

module_init(hi_dev_init);
module_exit(hi_dev_exit);
MODULE_LICENSE("GPL");
