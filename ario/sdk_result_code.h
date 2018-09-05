#ifndef __SDK_RESULT_CODE_H__
#define  __SDK_RESULT_CODE_H__


// General and purchase result code
const int	RESULT_OK = 0; // success
const int	RESULT_USER_CANCELED = 1; // user pressed back or canceled a dialog
const int	RESULT_SERVICE_UNAVAILABLE = 2; // service unavailable
const int	RESULT_BILLING_UNAVAILABLE = 3;// this billing API version is not supported for the type requested
const int	RESULT_ITEM_UNAVAILABLE = 4;// requested SKU is not available for purchase
const int	RESULT_DEVELOPER_ERROR = 5;// invalid arguments provided to the API
const int	RESULT_ERROR = 6;// Fatal error during the API action
const int	RESULT_ITEM_ALREADY_OWNED = 7;// Failure to purchase since item is already owned
const int	RESULT_ITEM_NOT_OWNED = 8; // Failure to consume since item is not owned
   
const int	RESULT_VERIFICATION_FAILED = 9;// Invalid public key
const int	RESULT_INVALID_CONSUMPTION = 10; // Consume an already consumed item or no item to consume
const int	RESULT_GAME_NOT_OWNED = 11 ; // User have not owned the game
const int	RESULT_SERVICE_NOT_SUPPORTED = 12; // The service not support in this version of Ario, need to update Ario!
// for example game want to use leaderboard service but leaderboard not available in current version of ario!


// leaderboard result code
const int	RESULT_NO_LEADERBOARD = 100;
const int	RESULT_FRAUD_LIMIT_EXCEED = 101;

// achievement result code
const int	RESULT_NO_ACHIEVEMENT = 200;
const int	RESULT_INACTIVE_ACHIEVEMENT = 201;
const int	RESULT_ACHIEVEMENT_ALREADY_UNLOCKED = 202;

#endif // __SDK_RESULT_CODE_H__
