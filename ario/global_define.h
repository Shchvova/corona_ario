//#pragma once
//
//#if defined(ARIO_SDK_LIBRARY)
//#  define ARIO_SDK_EXPORT __declspec(dllexport)
//#else
//#  define ARIO_SDK_EXPORT __declspec(dllimport)
//#endif
#include <functional>

#define ARIO_CALLING_CONVERSION WINAPI
//     return type							 type name     arguments
typedef void		(ARIO_CALLING_CONVERSION *JSON_CALLBACK)(int requestCode, int resutl, int response_size);


#ifdef ARIO_SDK_LIBRARY
typedef std::function<void ARIO_CALLING_CONVERSION(int request_code, int result, int response_size)> AsyncCallBack;
#endif //ARIO_SDK_LIBRARY
