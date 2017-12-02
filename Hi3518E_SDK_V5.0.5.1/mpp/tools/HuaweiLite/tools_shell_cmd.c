/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/

#include "los_task.h"
#include "hi_type.h"
#include "shell.h"
#include "tools_shell_cmd.h"

extern HI_S32 vou_chn_dump(int argc, char* argv[]);
extern HI_S32 vou_screen_dump(int argc, char* argv[]);
extern HI_S32 vo_video_csc_main(int argc, char* argv[]);
extern HI_S32 vpss_attr(int argc, char* argv[]);
extern HI_S32 vpss_chn_dump(int argc, char* argv[]);
extern HI_S32 vpss_src_dump(int argc, char* argv[]);
//extern HI_S32 rc_attr(int argc, char* argv[]);
//extern HI_S32 vi_fisheye_attr(int argc, char* argv[]);


extern HI_S32 aenc_dump(int argc, char* argv[]);
extern HI_S32 ai_dump(int argc, char* argv[]);
extern HI_S32 ao_dump(int argc, char *argv[]);
#if 1
extern HI_S32 isp_debug(int argc, char* argv[]);
#endif
extern HI_S32 vi_bayerdump(int argc, char* argv[]);
extern HI_S32 vi_dump(int argc, char* argv[]);

#define  ARGS_SIZE_T         20
#define  ARG_BUF_LEN_T       256
static char *ptask_args[ARGS_SIZE_T];
static char *args_buf_t = NULL;

void tools_cmd_com(unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3)
{
    int i = 0, ret;
    unsigned int argc = p0;
    char **argv = (char **)p1;

    //Set_Interupt(0);    

    dprintf("\ntools input command:\n");
    for(i=0; i<argc; i++) {
        dprintf("%s ", argv[i]);
    }
    dprintf("\n");
    if(strcmp(argv[0],"vpss_chn_dump")==0) //vpss_chn_dump
    {
        ret = vpss_chn_dump(argc,argv);
        dprintf("\vpss_chn_dump finshed\n");
    }    
    else if(strcmp(argv[0],"vpss_src_dump")==0) //vpss_src_dump
    {
        ret = vpss_src_dump(argc,argv);
        dprintf("\nvpss_src_dump finshed\n");
    }
    else if(strcmp(argv[0],"vpss_attr")==0) //vpss_attr
    {
        ret = vpss_attr(argc,argv);
        dprintf("\nvpss_attr finshed\n");
    }
    #if 0
    else if(strcmp(argv[0],"rc_attr")==0) //rc_attr
    {
        ret = rc_attr(argc,argv);
        dprintf("\nrc_attr finshed\n");
    }
    #endif
    else if(strcmp(argv[0],"vou_chn_dump")==0) //vou_chn_dump
    {
        ret = vou_chn_dump(argc,argv);
        dprintf("\nvou_chn_dump finshed\n");
    }
    else if(strcmp(argv[0],"vou_screen_dump")==0) //vou_screen_dump
    {
        ret = vou_screen_dump(argc,argv);
        dprintf("\nvou_screen_dump finshed\n");
    }
    else if(strcmp(argv[0],"vo_csc_config")==0) //vo_csc_config
    {
        ret = vo_video_csc_main(argc,argv);
        dprintf("\nvo_csc_config finshed\n");
    }
    
    else if(strcmp(argv[0],"ai_dump")==0) //ai_dump
    {
        ret = ai_dump(argc,argv);
        dprintf("\nai_dump finshed\n");
    }
    else if(strcmp(argv[0],"ao_dump")==0) //ao_dump
    {
        ret = ao_dump(argc,argv);
        dprintf("\nao_dump finshed\n");
    }
    else if(strcmp(argv[0],"aenc_dump")==0) //aenc_dump
    {
        ret = aenc_dump(argc,argv);
        dprintf("\naenc_dump finshed\n");
    }

    else if(strcmp(argv[0],"vi_bayerdump")==0) //vi_bayerdump
    {
        ret = vi_bayerdump(argc,argv);
        dprintf("\nvi_bayerdump finshed\n");
    }
    else if(strcmp(argv[0],"vi_dump")==0) //vi_dump
    {
        ret = vi_dump(argc,argv);
        dprintf("\nvi_dump finshed\n");
    }
    else if(strcmp(argv[0],"isp_debug")==0) //vi_dump
    {
        ret = isp_debug(argc,argv);
        dprintf("\nisp_debug finshed\n");
    }

    //taskid = -1;
}

void tools_cmd_app(int argc, char **argv )
{
    int i = 0, ret = 0;
    int len = 0;
    char *pch = NULL;
    TSK_INIT_PARAM_S stappTask;
    int taskid = -1;
//    printf("xxxx %d,%s,%s,%s,%s\n",argc,argv[0],argv[1],argv[2],argv[3]);

    if (taskid != -1)
    {
        dprintf("There's a app_main task existed.");
    }

    args_buf_t = zalloc(ARG_BUF_LEN_T);
    pch = args_buf_t;
    for(i=0; i<ARGS_SIZE_T; i++)
    {
        ptask_args[i] = NULL;
    }
    
    for(i = 0; i < argc; i++)
    {
        len =  strlen(argv[i]);
        memcpy(pch , argv[i], len);
        ptask_args[i] = pch;
        //keep a '\0' at the end of a string.
        pch = pch + len + 1;
        if (pch >= args_buf_t +ARG_BUF_LEN_T) {
            dprintf("args out of range!\n");
            break;
        }
    }

    memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
    stappTask.pfnTaskEntry = (TSK_ENTRY_FUNC)tools_cmd_com;
    stappTask.uwStackSize  = 0x80000;
    stappTask.pcName = "vpss_chn_dump";
    stappTask.usTaskPrio = 10;
    stappTask.uwResved   = LOS_TASK_STATUS_DETACHED;
    stappTask.auwArgs[0] = argc;
    stappTask.auwArgs[1] = (UINT32)ptask_args;
    ret = LOS_TaskCreate(&taskid, &stappTask);

    dprintf("Task %d\n", taskid);



}

void tools_cmd_register(void)
{
    osCmdReg(CMD_TYPE_STD,"vou_chn_dump",3,(CMD_CBK_FUNC)tools_cmd_app);
    osCmdReg(CMD_TYPE_STD,"vou_screen_dump",2,(CMD_CBK_FUNC)tools_cmd_app);
    osCmdReg(CMD_TYPE_STD,"vo_csc_config",6,(CMD_CBK_FUNC)tools_cmd_app);
    osCmdReg(CMD_TYPE_STD,"vpss_attr",3,(CMD_CBK_FUNC)tools_cmd_app);
    osCmdReg(CMD_TYPE_STD,"vpss_chn_dump",6,(CMD_CBK_FUNC)tools_cmd_app);
    osCmdReg(CMD_TYPE_STD,"vpss_src_dump",1,(CMD_CBK_FUNC)tools_cmd_app);
//    osCmdReg(CMD_TYPE_STD,"rc_attr",2,(CMD_CBK_FUNC)tools_cmd_app);
//	osCmdReg(CMD_TYPE_STD,"vi_fisheye_attr",3,(CMD_CBK_FUNC)tools_cmd_app);
    
    osCmdReg(CMD_TYPE_STD,"aenc_dump",4,(CMD_CBK_FUNC)tools_cmd_app);
    osCmdReg(CMD_TYPE_STD,"ai_dump",5,(CMD_CBK_FUNC)tools_cmd_app);
    osCmdReg(CMD_TYPE_STD,"ao_dump",5,(CMD_CBK_FUNC)tools_cmd_app);
	#if 1
    osCmdReg(CMD_TYPE_STD,"isp_debug",3,(CMD_CBK_FUNC)tools_cmd_app);    
    #endif
	osCmdReg(CMD_TYPE_STD,"vi_bayerdump",3,(CMD_CBK_FUNC)tools_cmd_app);
    osCmdReg(CMD_TYPE_STD,"vi_dump",2,(CMD_CBK_FUNC)tools_cmd_app);
}

