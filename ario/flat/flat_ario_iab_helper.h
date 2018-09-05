#ifndef __FLAT__ARIO_IAB_HELPER_H__
#define __FLAT__ARIO_IAB_HELPER_H__

#include "global_define.h"

int ARIO_CALLING_CONVERSION ArioIabHelper_StartSetup(char* base64PublicKey);

void ARIO_CALLING_CONVERSION ArioIabHelper_LaunchPurchaseFlow(char* sku, char* sku_type, char* developer_payload,
	int request_code, JSON_CALLBACK callback = nullptr);

void ARIO_CALLING_CONVERSION ArioIabHelper_Cunsume(char* token, int request_code, JSON_CALLBACK callback = nullptr);

void ARIO_CALLING_CONVERSION ArioIabHelper_GetInventory(char* sku_type, int request_code, JSON_CALLBACK callback = nullptr);
#endif // __FLAT__ARIO_IAB_HELPER_H__