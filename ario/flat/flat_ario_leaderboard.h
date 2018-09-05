#ifndef __FLAT__ARIO_LEADERBOARD_H__
#define __FLAT__ARIO_LEADERBOARD_H__

#include "global_define.h"

enum CollectionType { ALL_USER = 0, FRIENDS = 1 };
enum TimeFrame { DAILY = 0, WEEKLY = 1, ALL_TIME = 2 };

//************************************
// Method:    StartSetup
// FullName:  IArioLeaderboard::StartSetup
// Access:    virtual public 
// Returns:   int
// Qualifier: First function that init leaderboard interface for game
//************************************
int ARIO_CALLING_CONVERSION ArioLeaderboard_StartSetup(char* base64Key);

//************************************
// Method:    GetLeaderboards
// FullName:  IArioLeaderboard::GetLeaderboards
// Access:    virtual public 
// Returns:   void
// Qualifier: Request to get list of available leaderboards for game
//************************************
void ARIO_CALLING_CONVERSION ArioLeaderboard_GetLeaderboards(int request_code, JSON_CALLBACK callback = nullptr);

//************************************
// Method:    SubmitScore
// FullName:  IArioLeaderboard::SubmitScore
// Access:    virtual public 
// Returns:   void
// Qualifier: Set score on the leaderboard.
//************************************
void ARIO_CALLING_CONVERSION ArioLeaderboard_SubmitScore(int64_t value, int leaderboard_id, char* metadata,
	int request_code,JSON_CALLBACK callback = nullptr);


//************************************
// Method:    GetScores
// FullName:  IArioLeaderboard::GetScores
// Access:    virtual public 
// Returns:   void
// Qualifier: get list of user in the leaderboard
//************************************
void ARIO_CALLING_CONVERSION ArioLeaderboard_GetScores(int leaderboard_id, int collection_type, int time_frame,
	int offset, int limit, int request_code, JSON_CALLBACK callback = nullptr);


//************************************
// Method:    GetMyRank
// FullName:  IArioLeaderboard::GetMyRank
// Access:    virtual public 
// Returns:   void
// Qualifier: get user rank and other user around the user
//************************************
void ARIO_CALLING_CONVERSION ArioLeaderboard_GetMyRank(int leaderboard_id, int collection_type, int time_frame,
	int up_limit, int down_limit, int request_code, JSON_CALLBACK callBack = nullptr);

#endif //__FLAT__ARIO_LEADERBOARD_H__
