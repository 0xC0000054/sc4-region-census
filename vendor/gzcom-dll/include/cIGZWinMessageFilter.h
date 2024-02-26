#pragma once
#include "cIGZUnknown.h"

class cGZMessage;
class cIGZWin;

/**
 * @brief Allows window messages to be captured before
 * they are sent to the target window.
 */
class cIGZWinMessageFilter : public cIGZUnknown
{
public:

	/**
	 * @brief Filters a message before it is sent to the target window.
	 * @param pWin A pointer to the target window.
	 * @param message The window message.
	 * @return True to filter the message and stop it from being sent
	 * to the target window; otherwise, false.
	 */
	virtual bool PreFilterMessage(cIGZWin* pWin, cGZMessage& message) = 0;
};