#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "hi_comm_vo.h"
#include "hi_comm_vi.h"
#include "hi_comm_vpss.h"
#include "hi_type.h"

#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_vpss.h"

#define USAGE_HELP(void)\
    {\
        printf("\n\tusage : %s  group para value \n", argv[0]);    \
        printf("\n\t para: \n");    \
        printf("\t\tenNR    [0, disable; 1,enable]\n");    \
    }

#define CHECK_RET(express,name)\
    do{\
        if (HI_SUCCESS != express)\
        {\
            printf("%s failed at %s: LINE: %d ! errno:%#x \n", \
                   name, __FUNCTION__, __LINE__, express);\
            return HI_FAILURE;\
        }\
    }while(0)

#ifdef __HuaweiLite__
HI_S32 vpss_attr(int argc, char* argv[])
#else
HI_S32 main(int argc, char* argv[])
#endif
{
    HI_S32 s32Ret;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_NR_PARAM_U unNrParam = {{0}};

    char paraTemp[16];
    HI_U32 value = 0;
    VPSS_GRP VpssGrp = 0;
    const char* para = paraTemp;

    if (argc < 4)
    {
        USAGE_HELP();
        return -1;
    }

    strcpy(paraTemp, argv[2]);
    value = atoi(argv[3]);
    VpssGrp = atoi(argv[1]);

    s32Ret = HI_MPI_VPSS_GetGrpAttr(VpssGrp, &stVpssGrpAttr);
    CHECK_RET(s32Ret, "HI_MPI_VPSS_GetGrpAttr");

#if 0
    s32Ret = HI_MPI_VPSS_GetNRParam(VpssGrp, &unNrParam);
    CHECK_RET(s32Ret, "HI_MPI_VPSS_GetGrpParam");
#endif
    if (0 == strcmp(para, "enNR"))
    {
        stVpssGrpAttr.bNrEn = value;
    }
    else
    {
        printf("err para\n");
        USAGE_HELP();
    }

    s32Ret = HI_MPI_VPSS_SetGrpAttr(VpssGrp, &stVpssGrpAttr);
    CHECK_RET(s32Ret, "HI_MPI_VPSS_SetGrpAttr");

#if 0
    s32Ret = HI_MPI_VPSS_SetNRParam(VpssGrp, &unNrParam);
    CHECK_RET(s32Ret, "HI_MPI_VPSS_SetNRParam");

#endif
    printf("\t\tenNR     %d\n",  stVpssGrpAttr.bNrEn);
#if 0
    printf("\t\typk      %d\n",  unNrParam.stNRParam_V1.s32YPKStr);
    printf("\t\tysf       %d\n", unNrParam.stNRParam_V1.s32YSFStr);
    printf("\t\tytf       %d\n", unNrParam.stNRParam_V1.s32YTFStr);
    printf("\t\tytfmax    %d\n", unNrParam.stNRParam_V1.s32TFStrMax);
    printf("\t\tyss       %d\n", unNrParam.stNRParam_V1.s32YSmthStr);
    printf("\t\tysr       %d\n", unNrParam.stNRParam_V1.s32YSmthRat);
    printf("\t\tysfdlt    %d\n", unNrParam.stNRParam_V1.s32YSFStrDlt);
    printf("\t\tytfdlt    %d\n", unNrParam.stNRParam_V1.s32YTFStrDlt);
    printf("\t\tytfdl     %d\n", unNrParam.stNRParam_V1.s32YTFStrDl);
    printf("\t\tysfbr     %d\n", unNrParam.stNRParam_V1.s32YSFBriRat);
    printf("\t\tcsf       %d\n", unNrParam.stNRParam_V1.s32CSFStr);
    printf("\t\tctf       %d\n", unNrParam.stNRParam_V1.s32CTFstr);
#endif
    return 0;
}

