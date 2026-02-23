#pragma once

#include <qbytearray>
#include <QVector>
#include "filemanager\net\defines.h"
#include "filemanager\net\export.h"
#include "filemanager\net\PacketParserBase.h"

namespace fm
{
	class FM_NET_EXPORT ServerPacketParser : public PacketParserBase<fm::CLIENT_TO_SERVER_INFO>
	{
	public:
		ServerPacketParser();

	public:

	};

}
