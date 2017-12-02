This file get the method to use the inter drv in HuaweLite.

1.build the drv and then create the object lib (libxxx.a) in mpp/lib/;

2.add the link of the object lib to Make.HuaweiLite
   a. add the library path: SDK_LIB_PATH := -Lxxx ...
   b. add the library link: SDK_LIB :=  $(SDK_LIB_PATH)  --start-group -llibxxx ... --end-group

3.update the file: mpp/init/sdk_init.c
   a. add the head file "hi_interdrv_param.h"(path:drv/interdrv/init/HuaweiLite) if the drv has module param
   b. add the drv init declare which in xxx_init.c  
    export int xxx_mod_init(...);
	static unsigned int xxx_init()
    {
         XXX_MODULE_PARAMS_S stXxxParam;
	     ...
	     return xxx_mod_init( &stXxxParam );
    }
   c. add the drv_init to sdk init
    HI_VOID SDK_init(void)
	{
        ...

		xxx_init();
		...
	}

4. rebuild the app and get it.
