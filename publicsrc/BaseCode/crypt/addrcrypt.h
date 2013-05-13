#pragma once
#include <windows.h>
#define ENC_XCODE 0x789E1F0E
#define EZ_ENCRYPT_ADDR(dest,src,xcode) __asm MOV EAX,src \
	__asm XOR EAX,xcode \
	__asm BSWAP EAX \
	__asm MOV dest,EAX

#define EZ_DECRYPT_ADDR(dest,src,xcode) __asm MOV EAX,src \
	__asm BSWAP EAX \
	__asm XOR EAX,xcode \
	__asm MOV dest,EAX