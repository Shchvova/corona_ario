#ifndef __FLAT__ARIO_ACHIEVEMENT_H__
#define __FLAT__ARIO_ACHIEVEMENT_H__

#include "global_define.h"

int ARIO_CALLING_CONVERSION ArioAchievement_StartSetup(char* base64Key);
void ARIO_CALLING_CONVERSION ArioAchievement_GetGameAchievements(int request_code, JSON_CALLBACK callback = nullptr);
void ARIO_CALLING_CONVERSION ArioAchievement_GetStatus(int achievement_id, int request_code, JSON_CALLBACK callback = nullptr);
void ARIO_CALLING_CONVERSION ArioAchievement_Increment(int achievementId, int num_steps, int request_code, JSON_CALLBACK callBack = nullptr);
void ARIO_CALLING_CONVERSION ArioAchievement_SetSteps(int achievementId, int num_steps, int request_code, JSON_CALLBACK callBack = nullptr);
void ARIO_CALLING_CONVERSION ArioAchievement_Unlock(int achievementId, int request_code, JSON_CALLBACK callBack = nullptr);
void ARIO_CALLING_CONVERSION ArioAchievement_ResetStatus(int achievementId, int request_code, JSON_CALLBACK callBack = nullptr);
void ARIO_CALLING_CONVERSION ArioAchievement_ShowAchievements(); 



#endif // __FLAT__ARIO_ACHIEVEMENT_H__