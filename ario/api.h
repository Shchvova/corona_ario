#ifndef __API_H__
#define __API_H__


// interface model description can be found here!
// http://www.cplusplus.com/forum/beginner/157568/#msg807489
// https://www.codeproject.com/Articles/28969/HowTo-Export-C-classes-from-a-DLL
// in C#, you cannot call methods of an returned object from dll, because garbage collector release it's memory,
// based on the following link.
// http://www.mono-project.com/docs/advanced/pinvoke/#classes-and-structures-as-return-values

#include <string>
#include <functional> //  std::function<>
#include <windows.h>

#include "global_define.h" // ARIO_CALLING_CONVERSION, JSON_CALLBACK


namespace Ario {

	int ARIO_CALLING_CONVERSION Api_Init(char* package_name_buffer);
	void ARIO_CALLING_CONVERSION GetJsonResult(int request_code, char* result_buffer, int buffer_size);
	bool ARIO_CALLING_CONVERSION IsResultReady(int request_code,int &result, int &response_size);

}
#ifdef ARIO_SDK_LIBRARY // this define is required in order to export flat functions
#include "flat/flat_ario_user.h"
#include "flat/flat_ario_iab_helper.h"
#include "flat/flat_ario_leaderboard.h"
#include "flat/flat_ario_achievement.h"
#endif

#endif //__API_H__
