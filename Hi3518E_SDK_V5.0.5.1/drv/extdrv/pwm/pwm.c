/*  extdrv/interface/pwm.c
 *
 * Copyright (c) 2006 Hisilicon Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 * History:
 *      23-march-2011 create this file
 */

#include <linux/module.h>
#include <linux/errno.h>
#ifndef CONFIG_HISI_SNAPSHOT_BOOT
#include <linux/miscdevice.h>
#endif
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <asm/uaccess.h>
#include <linux/version.h>
//#include <asm/system.h>
#include <asm/io.h>
#include "pwm.h"


#ifndef __HuaweiLite__
//#include <mach/io.h>/* for IO_ADDRESS */
#ifdef CONFIG_HISI_SNAPSHOT_BOOT
#include "himedia.h"
#endif

#else
#ifndef __iomem
#define __iomem
#endif
#ifndef ENOIOCTLCMD
#define ENOIOCTLCMD	515	/* No ioctl command */
#endif
#endif

#include "pwm_arch.h"

void __iomem *reg_pwmI_base_va = 0;

#define  PWM_WRITE_REG(Addr, Value) ((*(volatile unsigned int *)(Addr)) = (Value))
#define  PWM_READ_REG(Addr)         (*(volatile unsigned int *)(Addr))

//PWM
#define PWM_ENABLE          0x01
#define PWM_DISABLE         0x00

#define PWM_DEVNAME  "pwm"

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
static struct himedia_device s_stPwmDevice;
#endif

static int PWM_DRV_Disable(unsigned char pwm_num)
{
    if (pwm_num >= PWM_NUM_MAX)
    {
		printk("The pwm number is big than the max value!\n");
        return -1;
    }
    switch (pwm_num)
    {
        case 0:
            PWM_WRITE_REG(PWM0_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
            break;
        case 1:
            PWM_WRITE_REG(PWM1_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
            break;
        case 2:
            PWM_WRITE_REG(PWM2_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
            break;
        case 3:
            PWM_WRITE_REG(PWM3_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
            break;
        case 4:
            PWM_WRITE_REG(PWM4_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
            break;
        case 5:
            PWM_WRITE_REG(PWM5_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
            break;
        case 6:
            PWM_WRITE_REG(PWM6_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
            break;
        case 7:
            PWM_WRITE_REG(PWM7_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
            break;
        default:
            break;
    }

    return 0;
}


int PWM_DRV_Write(unsigned char pwm_num, unsigned int duty, unsigned int period, unsigned char enable)
{
    if (pwm_num >= PWM_NUM_MAX)
    {
		printk("The pwm number is big than the max value!\n");
        return -1;
    }
    if (enable)
    {

        switch (pwm_num)
        {
            case 0:
                PWM_WRITE_REG(PWM0_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);

                PWM_WRITE_REG(PWM0_CFG_REG0(reg_pwmI_base_va), period);
                PWM_WRITE_REG(PWM0_CFG_REG1(reg_pwmI_base_va), duty);
                PWM_WRITE_REG(PWM0_CFG_REG2(reg_pwmI_base_va), 10); //pwm output number

                PWM_WRITE_REG(PWM0_CTRL_REG(reg_pwmI_base_va), (1 << 2 | PWM_ENABLE)); // keep the pwm always output;
                //printk("The PWMI0 state %x\n",PWM_READ_REG(PWM0_STATE_REG));
                break;

            case 1:
                PWM_WRITE_REG(PWM1_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);

                PWM_WRITE_REG(PWM1_CFG_REG0(reg_pwmI_base_va), period);
                PWM_WRITE_REG(PWM1_CFG_REG1(reg_pwmI_base_va), duty);
                PWM_WRITE_REG(PWM1_CFG_REG2(reg_pwmI_base_va), 10); //pwm output number

                PWM_WRITE_REG(PWM1_CTRL_REG(reg_pwmI_base_va), (1 << 2 | PWM_ENABLE)); // keep the pwm always output;
                //printk("The PWMI1 state %x\n",PWM_READ_REG(PWM1_STATE_REG));
                break;

            case 2:
                PWM_WRITE_REG(PWM2_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);

                PWM_WRITE_REG(PWM2_CFG_REG0(reg_pwmI_base_va), period);
                PWM_WRITE_REG(PWM2_CFG_REG1(reg_pwmI_base_va), duty);
                PWM_WRITE_REG(PWM2_CFG_REG2(reg_pwmI_base_va), 10); //pwm output number

                PWM_WRITE_REG(PWM2_CTRL_REG(reg_pwmI_base_va), (1 << 2 | PWM_ENABLE)); // keep the pwm always output;
                //printk("The PWMI2 state %x\n",PWM_READ_REG(PWM2_STATE_REG));
                break;

            case 3:
                PWM_WRITE_REG(PWM3_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);

                PWM_WRITE_REG(PWM3_CFG_REG0(reg_pwmI_base_va), period);
                PWM_WRITE_REG(PWM3_CFG_REG1(reg_pwmI_base_va), duty);
                PWM_WRITE_REG(PWM3_CFG_REG2(reg_pwmI_base_va), 10); //pwm output number

                PWM_WRITE_REG(PWM3_CTRL_REG(reg_pwmI_base_va), (1 << 2 | PWM_ENABLE)); // keep the pwm always output;
                //printk("The PWMI3 state %x\n",PWM_READ_REG(PWM3_STATE_REG));
                break;

            case 4:
                PWM_WRITE_REG(PWM4_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
                PWM_WRITE_REG(PWM4_CFG_REG0(reg_pwmI_base_va), period);
                PWM_WRITE_REG(PWM4_CFG_REG1(reg_pwmI_base_va), duty);
                PWM_WRITE_REG(PWM4_CFG_REG2(reg_pwmI_base_va), 10); //pwm output number
                PWM_WRITE_REG(PWM4_CTRL_REG(reg_pwmI_base_va), (1 << 2 | PWM_ENABLE)); // keep the pwm always output;
                break;
            case 5:
                PWM_WRITE_REG(PWM5_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
                PWM_WRITE_REG(PWM5_CFG_REG0(reg_pwmI_base_va), period);
                PWM_WRITE_REG(PWM5_CFG_REG1(reg_pwmI_base_va), duty);
                PWM_WRITE_REG(PWM5_CFG_REG2(reg_pwmI_base_va), 10); //pwm output number
                PWM_WRITE_REG(PWM5_CTRL_REG(reg_pwmI_base_va), (1 << 2 | PWM_ENABLE)); // keep the pwm always output;
                break;
            case 6:
                PWM_WRITE_REG(PWM6_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
                PWM_WRITE_REG(PWM6_CFG_REG0(reg_pwmI_base_va), period);
                PWM_WRITE_REG(PWM6_CFG_REG1(reg_pwmI_base_va), duty);
                PWM_WRITE_REG(PWM6_CFG_REG2(reg_pwmI_base_va), 10); //pwm output number
                PWM_WRITE_REG(PWM6_CTRL_REG(reg_pwmI_base_va), (1 << 2 | PWM_ENABLE)); // keep the pwm always output;
                break;
            case 7:
                PWM_WRITE_REG(PWM7_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);
                PWM_WRITE_REG(PWM7_CFG_REG0(reg_pwmI_base_va), period);
                PWM_WRITE_REG(PWM7_CFG_REG1(reg_pwmI_base_va), duty);
                PWM_WRITE_REG(PWM7_CFG_REG2(reg_pwmI_base_va), 10); //pwm output number
                PWM_WRITE_REG(PWM7_CTRL_REG(reg_pwmI_base_va), (1 << 2 | PWM_ENABLE)); // keep the pwm always output;
                break;
            default:
                PWM_WRITE_REG(PWM0_CTRL_REG(reg_pwmI_base_va), PWM_DISABLE);

                PWM_WRITE_REG(PWM0_CFG_REG0(reg_pwmI_base_va), period);
                PWM_WRITE_REG(PWM0_CFG_REG1(reg_pwmI_base_va), duty);
                PWM_WRITE_REG(PWM0_CFG_REG2(reg_pwmI_base_va), 10); //pwm output number

                PWM_WRITE_REG(PWM0_CTRL_REG(reg_pwmI_base_va), (1 << 2 | PWM_ENABLE)); // keep the pwm always output;
                //printk("The PWMII0 state %x\n",PWM_READ_REG(PWM0_STATE_REG));
                break;
        }
    }
    else
    {
        PWM_DRV_Disable(pwm_num);
    }


    return 0;
}

/* file operation                                                           */
#ifdef __HuaweiLite__
int PWM_Open(struct file* file)
{
    return 0 ;

}

int  PWM_Close(struct file* file)
{
    return 0;
}
#else
int PWM_Open(struct inode * inode, struct file * file)
{
   return 0 ;

}

int  PWM_Close(struct inode * inode, struct file * file)
{
    return 0;
}
#endif

static long PWM_Ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    PWM_DATA_S __user *argp = (PWM_DATA_S __user*)arg;

    unsigned char  PwmNum;
    unsigned int Duty;
    unsigned int Period;
    unsigned char  enable;

    switch (cmd)
    {
        case PWM_CMD_WRITE:
        {
            PwmNum  = argp->pwm_num;
            Duty    = argp->duty;
            Period  = argp->period;
            enable  = argp->enable;

            PWM_DRV_Write(PwmNum, Duty, Period, enable);
            break;
        }

        case PWM_CMD_READ:
        {
            break;
        }

        default:
        {
            printk("invalid ioctl command!\n");
            return -ENOIOCTLCMD;
        }
    }

    return 0 ;
}

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
static int PWM_freeze(struct himedia_device *pdev)
{
    printk(KERN_DEBUG "%s  %d\n", __FUNCTION__, __LINE__);
    return 0;
}

static int PWM_restore(struct himedia_device *pdev)
{
    printk(KERN_DEBUG "%s  %d\n", __FUNCTION__, __LINE__);
    return 0;
}
#endif
#ifdef __HuaweiLite__
const static struct file_operations_vfs pwm_fops = 
{
    .open = PWM_Open,
    .close = PWM_Close,
    .ioctl = PWM_Ioctl,
};
#else
static struct file_operations pwm_fops =
{
    .owner      = THIS_MODULE,
    .unlocked_ioctl = PWM_Ioctl,
    .open       = PWM_Open   ,
    .release    = PWM_Close  ,
};
#endif

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
struct himedia_ops stPwmDrvOps =
{
    .pm_freeze = PWM_freeze,
    .pm_restore  = PWM_restore
};
#else
static struct miscdevice pwm_dev =
{
    .minor   = MISC_DYNAMIC_MINOR,
    .name    = "pwm"    ,
    .fops    = &pwm_fops,
};
#endif




/* module init and exit                                                     */
#ifdef __HuaweiLite__
int pwm_init(void)
{
    int     ret;
    reg_pwmI_base_va = (void __iomem*)IO_ADDRESS(PWMI_ADRESS_BASE);

    ret = register_driver("/dev/pwm", &pwm_fops, 0666, 0);
    if (ret)
    {
        printk("register pwd device failed with %#x!\n", ret);
        return -1;
    }

    return 0;
}

void pwm_exit(void)
{
    int i;

    for (i = 0; i < PWM_NUM_MAX; i++)
    {
        PWM_DRV_Disable(i);
    }

    reg_pwmI_base_va = NULL;
    unregister_driver("/dev/pwm");
}
#else

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
static int __init pwm_init(void)
#else
static int pwm_init(void)
#endif
{
    int     ret;

	if(!reg_pwmI_base_va)
	{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
    	reg_pwmI_base_va = (void __iomem*)ioremap_nocache(PWMI_ADRESS_BASE, 0x1000);
#else
    	reg_pwmI_base_va = NULL;
#endif
	}
	
#ifdef CONFIG_HISI_SNAPSHOT_BOOT

    snprintf(s_stPwmDevice.devfs_name, sizeof(s_stPwmDevice.devfs_name), PWM_DEVNAME);
    s_stPwmDevice.minor  = HIMEDIA_DYNAMIC_MINOR;
    s_stPwmDevice.fops   = &pwm_fops;
    s_stPwmDevice.drvops = &stPwmDrvOps;
    s_stPwmDevice.owner  = THIS_MODULE;
    ret = himedia_register(&s_stPwmDevice);
    if (ret)
    {
        printk("register i2c device failed with %#x!\n", ret);
        return -1;
    }
	
#else
    ret = misc_register(&pwm_dev);
    if (ret != 0)
    {
        printk("register i2c device failed with %#x!\n", ret);
        return -1;
    }
#endif
    return 0;
}
static void __exit pwm_exit(void)
{
    int i;
    for (i = 0; i < PWM_NUM_MAX; i++)
    {
        PWM_DRV_Disable(i);
    }
    reg_pwmI_base_va = NULL;
#ifdef CONFIG_HISI_SNAPSHOT_BOOT
    himedia_unregister(&s_stPwmDevice);
#else
    misc_deregister(&pwm_dev);
#endif
}
#endif

#ifndef __HuaweiLite__
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
module_init(pwm_init);
module_exit(pwm_exit);

MODULE_DESCRIPTION("PWM Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon");
#else
#include <linux/of_platform.h>
static int pwm_probe(struct platform_device *pdev)
{
	struct resource *mem;
    
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    reg_pwmI_base_va = devm_ioremap_resource(&pdev->dev, mem);
    if (IS_ERR(reg_pwmI_base_va))
            return PTR_ERR(reg_pwmI_base_va);

    printk("++++++++++ reg_pwmI_base_va = %p\n",reg_pwmI_base_va);

	
    pwm_init();
    return 0;
}
static int pwm_remove(struct platform_device *pdev)
{
    printk("<%s> is called\n",__FUNCTION__);
    pwm_exit();
    return 0;
}
static const struct of_device_id pwm_match[] = {
        { .compatible = "hisilicon,pwm" },
        {},
};
MODULE_DEVICE_TABLE(of, pwm_match);
static struct platform_driver pwm_driver = {
        .probe          = pwm_probe,
        .remove         = pwm_remove,
        .driver         = {
                .name   = "pwm",
                .of_match_table = pwm_match,
        }
};
module_platform_driver(pwm_driver);
MODULE_LICENSE("GPL");
#endif
#endif
