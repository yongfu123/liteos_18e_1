#!/bin/sh

#set key~key3
himm 0x20100800 0x00000001
himm 0x20100804 0x00000002
himm 0x20100808 0x00000003
himm 0x2010080c 0x00000004
sleep 1
@start write key to efuse of 0 [5:4]=otp_key_add, KEY在OTP的地址.
himm 0x20100810 0x00000005
sleep 1

himd 0x20100814
#should be 0x88010000