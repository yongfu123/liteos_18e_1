
/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : hilink_demo.c
  版 本 号   : 初稿
  作    者   : 
  生成日期   : 2016年12月30日
  最近修改   :
  功能描述   : hilink demo程序
  函数列表   :
  修改历史   :
  1.日    期   : 2016年12月30日
    作    者   : 
    修改内容   : 创建文件

******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif
#include "driver_hisi_lib_api.h"
#include "hilink_adapt.h"
#include "hostapd/hostapd_if.h"
#include "wpa_supplicant/wpa_supplicant.h"
#include "los_task.h"
#include "sys/prctl.h"
#include <pthread.h>
#include "driver_hisi_lib_api.h"
#include "hisilink_ext.h"
#include <linux/completion.h>

#ifndef CONFIG_NO_CONFIG_WRITE
#define WPA_SUPPLICANT_CONFIG_PATH  "/jffs0/etc/hisi_wifi/wifi/wpa_supplicant.conf"
#endif

#define HILINK_PERIOD_CHANNEL 50     //切信道周期
#define HILINK_PERIOD_TIMEOUT 120000 //hisilink超时时间
typedef enum
{
    HILINK_STATUS_UNCREATE,
    HILINK_STATUS_RECEIVE, //hilink处于接收组播阶段
    HILINK_STATUS_CONNECT, //hilink处于关联阶段
    HILINK_STATUS_BUTT
}hilink_status_enum;
typedef unsigned char hilink_status_enum_type;

unsigned int                gul_hilink_taskid;          /* hilink_demo_task_channel_change的taskid */
hilink_s_context            g_st_hilink_context; /* hilink内部使用结构体变量 */
hilink_s_result             gst_hilink_result;   /* 存放hilink结果的结构体变量 */
hilink_status_enum_type     guc_hilink_status = 0;

extern unsigned char        hilink_receive_flag;   /* 是否需要接收数据的标识 */
/*****************************************************************************
                                函数声明
******************************************************************************/
int           hilink_demo_connect_prepare(void);
int           hilink_demo_set_status(hilink_status_enum_type uc_type);
unsigned char hilink_demo_get_status(void);
unsigned int  hilink_get_time();
/*****************************************************************************
 函 数 名  : hilink_demo_prepare
 功能描述  : hilink启动hostapd
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回HISI_SUCC,异常返回-HISI_EFAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
int hilink_demo_prepare(void)
{
    hilink_s_pkt0len      st_pkt_len;
    int                   l_ret;
    char                  auc_ssid[33];
    unsigned int          ul_ssid_len = 0;
    struct hostapd_conf   hapd_conf;

    /* 设备信息 */
    unsigned char         auc_ssid_type[]                  = "01";
    unsigned char         auc_device_id[]                  = "9001";
    unsigned char         auc_device_sn[]                  = "X";
    unsigned char         auc_sec_type[]                   = "B";
    unsigned char         auc_sec_key[]                    = "0000000000000000000000";

    /* 尝试删除wpa_supplicant和hostapd */
    l_ret = wpa_supplicant_stop();
    if (HISI_SUCC != l_ret)
    {
        HISI_PRINT_WARNING("%s[%d]:wpa_supplicant stop fail",__func__,__LINE__);
    }
    l_ret = hostapd_stop();
    if (HISI_SUCC != l_ret)
    {
        HISI_PRINT_WARNING("%s[%d]:hostapd stop fail",__func__,__LINE__);
    }
    memset(&g_st_hilink_context, 0, sizeof(hilink_s_context));

    /* 初始化hilink库函数 */
    l_ret = hilink_link_init(&g_st_hilink_context);
    if (HISI_SUCC == l_ret)
    {
        hilink_link_reset();
        st_pkt_len.len_open = OPEN_BASE_LEN;
        st_pkt_len.len_wep  = WEP_BASE_LEN;
        st_pkt_len.len_tkip = TKIP_BASE_LEN;
        st_pkt_len.len_aes  = AES_BASE_LEN;
        /* 设置基准长度 */
        hilink_link_set_pkt0len(&st_pkt_len);
    }
    memset(auc_ssid, 0, sizeof(unsigned char)*33);
    /* 启动hostapd */
    hilink_link_get_devicessid(auc_ssid_type,auc_device_id,auc_device_sn,auc_sec_type,auc_sec_key,auc_ssid,&ul_ssid_len);
    /* 设置AP参数 */
    memset(&hapd_conf, 0, sizeof(struct hostapd_conf));
    if (ul_ssid_len > MAX_ESSID_LEN + 1)
    {
        HISI_PRINT_ERROR("%s[%d]:ul_ssid_len = %d more than 33",__func__,__LINE__,ul_ssid_len);
        return -HISI_EFAIL;
    }
    memcpy(hapd_conf.ssid, auc_ssid, ul_ssid_len);
    memcpy(hapd_conf.ht_capab, "[HT40+]", strlen("[HT40+]"));
    hapd_conf.channel_num           = 6;
    hapd_conf.wpa_key_mgmt          = WPA_KEY_MGMT_PSK;
    hapd_conf.authmode              = HOSTAPD_SECURITY_WPAPSK;
    hapd_conf.wpa                   = 2;

    hapd_conf.wpa_pairwise          = WPA_CIPHER_TKIP | WPA_CIPHER_CCMP;
    memcpy(hapd_conf.driver, "hisi", 5);
    memcpy((char *)hapd_conf.key,"hilink123",strlen("hilink123"));
    l_ret = hostapd_start(NULL, &hapd_conf);
    if (0 != l_ret)
    {
       HISI_PRINT_ERROR("%s[%d]:hostapd start failed[%d]",__func__,__LINE__,l_ret);
       return -HISI_EFAIL;
    }
    return HISI_SUCC;
}
/*****************************************************************************
 函 数 名  : hilink_demo_init
 功能描述  : hilink demo的初始化wifi驱动与hilink相关的配置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回HISI_SUCC,异常返回-HISI_EFAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
int hilink_demo_init(void)
{
#if 0
    /* 由于hilink团队尚未提供适配B051的SO库，故hilink暂时选择不编译，
    相应的无法用编译宏控制的调用位置暂时用if 0注掉 */
    int l_ret;

    memset(&gst_hilink_result, 0, sizeof(hilink_s_result));
    l_ret = hisi_hilink_adapt_init();
    if (HISI_SUCC != l_ret)
    {
        HISI_PRINT_ERROR("%s[%d]:hilink_adapt_init fail",__func__,__LINE__);
        return -HISI_EFAIL;
    }
    return HISI_SUCC;
#endif
}
/*****************************************************************************
 函 数 名  : hilink_demo_task_channel_change
 功能描述  : hilink demo的切信道线程处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
void hilink_demo_task_channel_change(void)
{
    int             l_ret;
    unsigned int    ul_ret;
    unsigned int    ul_time      = 0;
    unsigned int    ul_time_temp = 0;
    unsigned int    ul_wait_time = 0;
    unsigned char   uc_status;
    struct netif   *pst_netif;

    ul_time_temp = hilink_get_time();
    ul_time      = ul_time_temp;
    uc_status = hilink_demo_get_status();
    while((HILINK_STATUS_RECEIVE == uc_status) && (HILINK_PERIOD_TIMEOUT >= ul_wait_time))
    {
        if (RECEIVE_FLAG_ON == hilink_receive_flag)
        {
            ul_time_temp = hilink_get_time();
            if (HILINK_PERIOD_CHANNEL < (ul_time_temp - ul_time))
            {
                ul_wait_time += (ul_time_temp - ul_time);
                ul_time = ul_time_temp;
                l_ret = hilink_link_get_lock_ready();
                if (0 == l_ret)
                {
                    l_ret = hisi_hilink_change_channel();
                    if (HISI_SUCC != l_ret)
                    {
                        HISI_PRINT_WARNING("%s[%d]:change channel fail\n",__func__,__LINE__);
                    }
                    /* 复位一下hilink参数 */
                    hilink_link_reset();
                }
            }
        }
        l_ret = hisi_hilink_parse_data();
        if (HI_WIFI_STATUS_FINISH == l_ret)
        {
            LOS_TaskLock();
            hilink_receive_flag = RECEIVE_FLAG_OFF;
            LOS_TaskUnlock();
            hilink_demo_set_status(HILINK_STATUS_CONNECT);
            break;
        }
        msleep(1);
        uc_status = hilink_demo_get_status();
    }
    hisi_hilink_adapt_deinit();

    if (HILINK_PERIOD_TIMEOUT >= ul_wait_time)
    {
        hsl_memset(&gst_hilink_result, 0, sizeof(hilink_s_result));
        l_ret = hilink_link_get_result(&gst_hilink_result);
        if (HISI_SUCC == l_ret)
        {
            l_ret = hilink_demo_connect_prepare();
            if (HISI_SUCC != l_ret)
            {
                HISI_PRINT_ERROR("hilink_demo_task_channel_change:connect prepare fail[%d]\n",l_ret);
            }
        }
    }
    else
    {
        HISI_PRINT_INFO("hilink timeout\n");

        /* 删除AP模式 */
        pst_netif = netif_find("wlan0");
        if (HISI_NULL != pst_netif)
        {
            dhcps_stop(pst_netif);
        }
        l_ret = hostapd_stop();
        if (0 != l_ret)
        {
            HISI_PRINT_ERROR("%s[%d]:stop hostapd fail",__func__,__LINE__);
        }
    }
    hilink_demo_set_status(HILINK_STATUS_UNCREATE);
    ul_ret = LOS_TaskDelete(gul_hilink_taskid);
    if (HISI_SUCC != ul_ret)
    {
        HISI_PRINT_WARNING("%s[%d]:delete task fail[%d]",__func__,__LINE__,ul_ret);
    }
}
/*****************************************************************************
 函 数 名  : hilink_demo_get_status
 功能描述  : 获取hilink所处的状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
unsigned char hilink_demo_get_status(void)
{
    unsigned char uc_type;
    LOS_TaskLock();
    uc_type = guc_hilink_status;
    LOS_TaskUnlock();
    return uc_type;
}
/*****************************************************************************
 函 数 名  : hilink_demo_set_status
 功能描述  : 设置hilink所处的状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
int hilink_demo_set_status(hilink_status_enum_type uc_type)
{
    LOS_TaskLock();
    guc_hilink_status = uc_type;
    LOS_TaskUnlock();
    return HISI_SUCC;
}
/*****************************************************************************
 函 数 名  : hilink_demo_get_result
 功能描述  : 获取hilink接收到的AP信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
hilink_s_result* hilink_demo_get_result(void)
{
    hilink_s_result *pst_result;
    pst_result = &gst_hilink_result;
    return pst_result;
}
/*****************************************************************************
 函 数 名  : hilink_get_time
 功能描述  : 获取系统时间
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
unsigned int hilink_get_time(void)
{
    unsigned int ul_time = 0;
    struct timeval tv;

    gettimeofday(&tv, HISI_NULL);
    ul_time = tv.tv_usec / 1000 + tv.tv_sec * 1000;
    return ul_time;

}
/*****************************************************************************
 函 数 名  : hilink_demo_online
 功能描述  : 发送上线包
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
int hilink_demo_online(hilink_s_result* pst_result)
{
    if (HISI_NULL == pst_result)
    {
        HISI_PRINT_ERROR("%s[%d]:pst_params is null",__func__,__LINE__);
        return -HISI_EFAIL;
    }
    hisi_hilink_online_notice(pst_result);
    return HISI_SUCC;
}
extern struct completion  dhcp_complet;

/*****************************************************************************
 函 数 名  : hilink_demo_connect_prepare
 功能描述  : 获取AP信息后，切换到STA模式为关联做准备
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回HISI_SUCC,异常返回-HISI_EFAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
int hilink_demo_connect_prepare(void)
{
    int                      l_ret;
    unsigned int             ul_ret;
    hilink_s_result         *pst_result;
    /* 删除AP模式 */
    l_ret = hostapd_stop();
    if (HISI_SUCC!= l_ret)
    {
        HISI_PRINT_ERROR("%s[%d]:stop hostapd fail\n",__func__,__LINE__);
        return -HISI_EFAIL;
    }
    init_completion(&dhcp_complet);
#ifdef CONFIG_NO_CONFIG_WRITE
    l_ret = wpa_supplicant_start("wlan0", "hisi", NULL);
#else
    l_ret = wpa_supplicant_start("wlan0", "hisi", WPA_SUPPLICANT_CONFIG_PATH);
#endif
    if (HISI_SUCC != l_ret)
    {
        HISI_PRINT_ERROR("%s[%d]:start wpa_supplicant fail\n",__func__,__LINE__);
        return -HISI_EFAIL;
    }
    ul_ret = wait_for_completion_timeout(&dhcp_complet, LOS_MS2Tick(40000));//40s超时
    if (0 == ul_ret)
    {
        HISI_PRINT_ERROR("hilink_demo_connect_prepare:cannot get ip\n");
        return -HISI_EFAIL;
    }
    pst_result = hilink_demo_get_result();
    if (HISI_NULL != pst_result)
    {
        hilink_demo_online(pst_result);
    }
    return HISI_SUCC;

}
/*****************************************************************************
 函 数 名  : hilink_demo_connect
 功能描述  : 根据获取的AP信息发起关联
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
int hilink_demo_connect(hilink_s_result* pst_result)
{
    struct wpa_assoc_request wpa_assoc_req;

    memset(&wpa_assoc_req, 0, sizeof(struct wpa_assoc_request));
    wpa_assoc_req.hidden_ssid = 1;
    if (33 < pst_result->ssid_len)
    {
        HISI_PRINT_ERROR("%s[%d]:ssd_len = %d more than 33",__func__,__LINE__,pst_result->ssid_len);
        wpa_supplicant_stop();
        return -HISI_EFAIL;
    }
    strncpy(wpa_assoc_req.ssid, pst_result->ssid, pst_result->ssid_len);
    HISI_PRINT_ERROR("SSID:%s\n",pst_result->ssid);
    HISI_PRINT_ERROR("en_type:%d\n",pst_result->enc_type);
    HISI_PRINT_ERROR("sendtype:%d\n",pst_result->sendtype);
    wpa_assoc_req.auth = pst_result->enc_type;
    if (HI_WIFI_ENC_OPEN != wpa_assoc_req.auth)
    {
        if (128 < pst_result->pwd_len)
        {
            HISI_PRINT_ERROR("%s[%d]:pwd_len = %d is more than 128",__func__,__LINE__,pst_result->pwd_len);
            wpa_supplicant_stop();
            return -HISI_EFAIL;
        }
        strncpy(wpa_assoc_req.key, pst_result->pwd, pst_result->pwd_len);
    }
    wpa_connect_interface(&wpa_assoc_req);
    return HISI_SUCC;
}
extern int hsl_demo_get_status(void);
/*****************************************************************************
 函 数 名  : hilink_demo_main
 功能描述  : hilink demo的入口函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回HISI_SUCC,异常返回-HISI_EFAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年6月25日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
int hilink_demo_main(void)
{
   int              l_ret;
   unsigned int     ul_ret;
   unsigned char    uc_status;
   TSK_INIT_PARAM_S stappTask;
   uc_status = hsl_demo_get_status();
   if (0 != uc_status)
   {
       HISI_PRINT_ERROR("hisilink already start,cannot start hilink\n");
       return -HISI_EFAIL;
   }
   uc_status = hilink_demo_get_status();
   if (HILINK_STATUS_UNCREATE != uc_status)
   {
       HISI_PRINT_ERROR("hilink already start,cannot start again");
       return -HISI_EFAIL;
   }
   hilink_demo_set_status(HILINK_STATUS_RECEIVE);
   l_ret = hilink_demo_prepare();
   if (HISI_SUCC != l_ret)
   {
       HISI_PRINT_ERROR("%s[%d]:demo init fail\n",__func__,__LINE__);
       hilink_demo_set_status(HILINK_STATUS_UNCREATE);
       return -HISI_EFAIL;
   }

   /* 创建切信道线程,线程结束后自动释放 */
   memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
   stappTask.pfnTaskEntry = (TSK_ENTRY_FUNC)hilink_demo_task_channel_change;
   stappTask.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
   stappTask.pcName = "hilink_thread";
   stappTask.usTaskPrio = 8;
   stappTask.uwResved   = LOS_TASK_STATUS_DETACHED;
   ul_ret = LOS_TaskCreate(&gul_hilink_taskid, &stappTask);
   if(0 != ul_ret)
   {
       HISI_PRINT_ERROR("%s[%d]:create task fail[%d]\n",__func__,__LINE__,ul_ret);
       hilink_demo_set_status(HILINK_STATUS_UNCREATE);
       return -HISI_EFAIL;
   }
   return HISI_SUCC;
}
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

