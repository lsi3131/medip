#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager\net\ClientPacketParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace fm;


namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestClientPacketParser)
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
			fm::SERVER_TO_CLIENT_INFO info1 = CreateInfo(fm::EServerCommand::open, "abc", true);
			QByteArray packetInfo1 = CreatePacket(info1);
			Assert::IsTrue(m_parser.Parse(packetInfo1, m_resultInfos));
			Assert::AreEqual(1, m_resultInfos.size());
			Assert::AreEqual((int)fm::EServerCommand::open, (int)m_resultInfos[0].Command);
		}

		TEST_METHOD(OneHalfSizePacket_ReturnTrue_OneInfo)
		{
			fm::SERVER_TO_CLIENT_INFO info1 = CreateInfo(fm::EServerCommand::open, "abc", true);
			QByteArray packetInfo1 = CreatePacket(info1);
			QByteArray packetInfo2 = CreateHalfPacket(info1);
			QByteArray packet = packetInfo1 + packetInfo2;
			Assert::IsTrue(m_parser.Parse(packet, m_resultInfos));
			Assert::AreEqual(1, m_resultInfos.size());
		}

		TEST_METHOD(TwoTargetPacket_ReturnTrue_TwoInfo)
		{
			fm::SERVER_TO_CLIENT_INFO info1 = CreateInfo(fm::EServerCommand::open, "abc", true);
			fm::SERVER_TO_CLIENT_INFO info2 = CreateInfo(fm::EServerCommand::update, "def", false);
			QByteArray packetInfo1 = CreatePacket(info1);
			QByteArray packetInfo2 = CreatePacket(info2);
			QByteArray packet = packetInfo1 + packetInfo2;
			Assert::IsTrue(m_parser.Parse(packet, m_resultInfos));
			Assert::AreEqual(2, m_resultInfos.size());
			Assert::AreEqual((int)fm::EServerCommand::open, (int)m_resultInfos[0].Command);
			Assert::AreEqual((int)fm::EServerCommand::update, (int)m_resultInfos[1].Command);
		}

		TEST_METHOD(Integration)
		{
			fm::SERVER_TO_CLIENT_INFO info1 = CreateInfo(fm::EServerCommand::update, "abc", true);
			fm::SERVER_TO_CLIENT_INFO info2 = CreateInfo(fm::EServerCommand::open, "def", false);
			fm::SERVER_TO_CLIENT_INFO info3 = CreateInfo(fm::EServerCommand::update, "ghi", false);
			fm::SERVER_TO_CLIENT_INFO info4 = CreateInfo(fm::EServerCommand::update, "ttt", false);
			QByteArray packetInfo1 = CreatePacket(info1);
			QByteArray packetInfo2 = CreatePacket(info2);
			QByteArray packetInfo3 = CreatePacket(info3);
			QByteArray packetHalf = CreateHalfPacket(info4);
			QByteArray packet = packetInfo1 + packetInfo2 + packetInfo3 + packetHalf;
			Assert::IsTrue(m_parser.Parse(packet, m_resultInfos));
			Assert::AreEqual(3, m_resultInfos.size());
			Assert::AreEqual((int)fm::EServerCommand::update, (int)m_resultInfos[0].Command);
			Assert::AreEqual((int)fm::EServerCommand::open, (int)m_resultInfos[1].Command);
			Assert::AreEqual((int)fm::EServerCommand::update, (int)m_resultInfos[2].Command);
			Assert::AreEqual("abc", m_resultInfos[0].FileExtensionFilter);
			Assert::AreEqual("def", m_resultInfos[1].FileExtensionFilter);
			Assert::AreEqual("ghi", m_resultInfos[2].FileExtensionFilter);
		}

	private:
		SERVER_TO_CLIENT_INFO CreateInfo(EServerCommand cmd, char* FileExtensionFilter, bool isActivate)
		{
			SERVER_TO_CLIENT_INFO info;
			info.Command = cmd;
			strcpy(info.FileExtensionFilter, FileExtensionFilter);
			return info;
		}

		QByteArray CreatePacket(fm::SERVER_TO_CLIENT_INFO info)
		{
			const char* data = (const char*)&info;
			int size = sizeof(fm::SERVER_TO_CLIENT_INFO);
			return QByteArray(data, size);
		}

		QByteArray CreateHalfPacket(fm::SERVER_TO_CLIENT_INFO info)
		{
			const char* data = (const char*)&info;
			int size = sizeof(fm::SERVER_TO_CLIENT_INFO) / 2;
			return QByteArray(data, size);
		}

	private:
		ClientPacketParser m_parser;
		QVector<fm::SERVER_TO_CLIENT_INFO> m_resultInfos;
	};
}