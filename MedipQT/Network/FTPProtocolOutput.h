#ifndef INC_FTPPROTOCOLOUTPUT_H
#define INC_FTPPROTOCOLOUTPUT_H

#pragma once

#include "FTPclient.h"
class QString;

namespace nsFTP
{
/// @brief Shows communication between server and client
class CFTPProtocolOutput : public nsFTP::CFTPClient::CNotification
{
public:
   CFTPProtocolOutput();
   virtual ~CFTPProtocolOutput();
   static DWORD CALLBACK ProtocolStreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

protected:
   virtual void OnInternalError(const tstring& strErrorMsg, const tstring& strFileName, DWORD dwLineNr);
   virtual void OnSendCommand(const tstring& strCommand);
   virtual void OnResponse(const CReply& Reply);

private:
   void WriteLine(const QString& cszLine, COLORREF crText);
};
};

#endif // INC_FTPPROTOCOLOUTPUT_H
