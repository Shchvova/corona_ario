#pragma once

#include "CommonErrorCodes.h"
#include <functional>

#define STORE_ANALYTICS_RUN_GAME				1
#define STORE_ANALYTICS_EXIT_GAME				1
#define STORE_ANALYTICS_ENTER_IN_GAME			1
#define STORE_ANALYTICS_LEAVE_IN_GAME			1
#define STORE_ANALYTICS_ENTER_IN_GAME_STORE 	1
#define STORE_ANALYTICS_LEAVE_IN_GAME_STORE 	1
#define STORE_ANALYTICS_PURCHASED_ITEM 			1

#define STORE_SDK_MAJOR_VERSION(p) (p>>16)
#define STORE_SDK_MINOR_VERSION(p) (p&65535)
#define STORE_SDK_MAKE_VERSION(a,b) ((a<<16)+b)

//typedef void (WINAPI *INC_CALLBACK)(int result);
//typedef void WINAPI* pfnCallback ( int iReqId, int iResult )

DWORD WINAPI GetStoreSDKVersion() ;

int WINAPI ValidatePurchase ( char* pszPackageName, char** ppszOrderId ) ;
void WINAPI SetTestValidationResult ( int iResult ) ;

int WINAPI SendAnalyticsData ( char* pszProductId, int iStoreAnalyticsType, int iParam1, int iParam2 ) ;

int WINAPI ValidatePurchaseAsync ( char* pszPackageName, int iReqId, void WINAPI pfnCallback ( int iReqId, char* ppszOrderId, int iResult ) ) ;
int WINAPI ValidatePurchaseAsyncStd ( char* pszPackageName, int iReqId, std::function<void(int iReqId, char* pszOrderId, int iResult)> _callBack ) ;

