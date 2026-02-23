#pragma once

#include <qbytearray>
#include <QVector>
#include "filemanager\net\defines.h"
#include "filemanager\net\export.h"
#include "filemanager\net\PacketParserBase.h"

namespace fm
{
	class FM_NET_EXPORT ClientPacketParser : public PacketParserBase<fm::SERVER_TO_CLIENT_INFO>
	{
	public:
		ClientPacketParser();

	public:

	private:

	};

}
