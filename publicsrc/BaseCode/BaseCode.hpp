#pragma once
#ifdef _WINDOWS_
#include "stdafx.h"
#endif

//#include "CallLolInterfaceCommon.h"
//extern  _CallLolInterface LolInterfaceEncAddr=0;


#include "hook/inline/HookInternal.h"
#include "File/CFileIO.hpp"
#include "Pairs/Pairs.h"
#ifdef _HOOK_INTERNAL
	#include "hook/inline/HookInternal.cpp"
#endif

#ifdef _FILEIO
	#include "File/CFileIO.cpp"
#endif

#ifdef _STRING_EX
	#include "String/STRING_EX.cpp"
#endif

#ifdef _SYSTEM_INTERFACE
	#include "System/System.cpp"
#endif

#ifdef _CRndNum
	#include "stdlib/cRandom.cpp"
#endif

#ifdef _PAIRS
	#include "Pairs/Pairs.cpp"
#endif
#ifdef _ROTATE
	#include "crypt/rotate.cpp
#endif