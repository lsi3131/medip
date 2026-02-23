#pragma once

#include <qbytearray>
#include <QVector>
#include "filemanager\net\defines.h"
#include "filemanager\net\export.h"

namespace fm
{
	template<typename T>
	class PacketParserBase
	{
	public:
		PacketParserBase()
		{

		}

	public:
		void Clear()
		{
			m_currentReadPacket.clear();
		}

		bool Parse(QByteArray packet, QVector<T>& infos)
		{
			infos.clear();

			bool isParsed = false;
			int TotalReceivePacketSize = sizeof(T);
			while (packet.isEmpty() == false)
			{
				int ramainToReadPacketSize = TotalReceivePacketSize - m_currentReadPacket.size();
				int readPacketSize = packet.size();

				int appendPacketSize = std::min(ramainToReadPacketSize, readPacketSize);

				m_currentReadPacket.append(packet.data(), appendPacketSize);

				if (m_currentReadPacket.size() == TotalReceivePacketSize)
				{
					T* pInfo = (T*)m_currentReadPacket.data();
					if (IsValid(pInfo))
					{
						infos.push_back(*pInfo);
						isParsed = true;
					}

					m_currentReadPacket.clear();
				}
				packet.remove(0, appendPacketSize);
			}

			return isParsed;
		}
	protected:
		virtual bool IsValid(T* pData) { return true; }

	protected:
		QByteArray m_currentReadPacket;

	};

}
