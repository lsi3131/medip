#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager\net\ServerPacketParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace fm;


namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestServerPacketParser)
	{
	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			m_parser.Clear();
			m_resultInfos.clear();
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
		}

		TEST_METHOD(EmptyPacket_ReturnFalse)
		{
			QByteArray emptyPacket;
			Assert::IsFalse(m_parser.Parse(emptyPacket, m_resultInfos));
		}

		TEST_METHOD(ShortPacket_ReturnFalse)
		{
			QByteArray shortPacket("abc");
			Assert::IsFalse(m_parser.Parse(shortPacket, m_resultInfos));
		}

		TEST_METHOD(TargetPacket_ReturnTrue)
		{
			fm::CLIENT_TO_SERVER_INFO info1 = CreateInfo(fm::EClientCommand::file_open, "abc", true);
			QByteArray packetInfo1 = CreatePacket(info1);
			Assert::IsTrue(m_parser.Parse(packetInfo1, m_resultInfos));
			Assert::AreEqual(1, m_resultInfos.size());
			Assert::AreEqual((int)fm::EClientCommand::file_open, (int)m_resultInfos[0].Command);
		}

		TEST_METHOD(OneHalfSizePacket_ReturnTrue_OneInfo)
		{
			fm::CLIENT_TO_SERVER_INFO info1 = CreateInfo(fm::EClientCommand::file_open, "abc", true);
			QByteArray packetInfo1 = CreatePacket(info1);
			QByteArray packetInfo2 = CreateHalfPacket(info1);
			QByteArray packet = packetInfo1 + packetInfo2;
			Assert::IsTrue(m_parser.Parse(packet, m_resultInfos));
			Assert::AreEqual(1, m_resultInfos.size());
		}

		TEST_METHOD(TwoTargetPacket_ReturnTrue_TwoInfo)
		{
			fm::CLIENT_TO_SERVER_INFO info1 = CreateInfo(fm::EClientCommand::file_open, "abc", true);
			fm::CLIENT_TO_SERVER_INFO info2 = CreateInfo(fm::EClientCommand::activate, "def", false);
			QByteArray packetInfo1 = CreatePacket(info1);
			QByteArray packetInfo2 = CreatePacket(info2);
			QByteArray packet = packetInfo1 + packetInfo2;
			Assert::IsTrue(m_parser.Parse(packet, m_resultInfos));
			Assert::AreEqual(2, m_resultInfos.size());
			Assert::AreEqual((int)fm::EClientCommand::file_open, (int)m_resultInfos[0].Command);
			Assert::AreEqual((int)fm::EClientCommand::activate, (int)m_resultInfos[1].Command);
		}

		TEST_METHOD(Integration)
		{
			fm::CLIENT_TO_SERVER_INFO info1 = CreateInfo(fm::EClientCommand::activate, "abc", true);
			fm::CLIENT_TO_SERVER_INFO info2 = CreateInfo(fm::EClientCommand::file_open, "def", false);
			fm::CLIENT_TO_SERVER_INFO info3 = CreateInfo(fm::EClientCommand::update, "ghi", false);
			fm::CLIENT_TO_SERVER_INFO info4 = CreateInfo(fm::EClientCommand::update, "ttt", false);
			QByteArray packetInfo1 = CreatePacket(info1);
			QByteArray packetInfo2 = CreatePacket(info2);
			QByteArray packetInfo3 = CreatePacket(info3);
			QByteArray packetHalf = CreateHalfPacket(info4);
			QByteArray packet = packetInfo1 + packetInfo2 + packetInfo3 + packetHalf;
			Assert::IsTrue(m_parser.Parse(packet, m_resultInfos));
			Assert::AreEqual(3, m_resultInfos.size());
			Assert::AreEqual((int)fm::EClientCommand::activate, (int)m_resultInfos[0].Command);
			Assert::AreEqual((int)fm::EClientCommand::file_open, (int)m_resultInfos[1].Command);
			Assert::AreEqual((int)fm::EClientCommand::update, (int)m_resultInfos[2].Command);
			Assert::AreEqual("abc", m_resultInfos[0].FilePath);
			Assert::AreEqual("def", m_resultInfos[1].FilePath);
			Assert::AreEqual("ghi", m_resultInfos[2].FilePath);
			Assert::AreEqual((char)true, m_resultInfos[0].IsActivated);
			Assert::AreEqual((char)false, m_resultInfos[1].IsActivated);
			Assert::AreEqual((char)false, m_resultInfos[2].IsActivated);
		}

		TEST_METHOD(PlusTwoByteArray)
		{
			QByteArray aaa("aaa");
			QByteArray bbb("bbb");
			QByteArray aaabbb = aaa + bbb;
			Assert::AreEqual("aaabbb", aaabbb.data());
			
		}

	private:
		CLIENT_TO_SERVER_INFO CreateInfo(EClientCommand cmd, char* filepath, bool isActivate)
		{
			CLIENT_TO_SERVER_INFO info;
			info.Command = cmd;
			strcpy(info.FilePath, filepath);
			info.IsActivated = isActivate;
			return info;
		}

		QByteArray CreatePacket(fm::CLIENT_TO_SERVER_INFO info)
		{
			const char* data = (const char*)&info;
			int size = sizeof(fm::CLIENT_TO_SERVER_INFO);
			return QByteArray(data, size);
		}

		QByteArray CreateHalfPacket(fm::CLIENT_TO_SERVER_INFO info)
		{
			const char* data = (const char*)&info;
			int size = sizeof(fm::CLIENT_TO_SERVER_INFO) / 2;
			return QByteArray(data, size);
		}

	private:
		ServerPacketParser m_parser;
		QVector<fm::CLIENT_TO_SERVER_INFO> m_resultInfos;
	};
}