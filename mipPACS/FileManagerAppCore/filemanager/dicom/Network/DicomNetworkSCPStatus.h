#pragma once

namespace fm
{
	enum class DicomNetworkSCPStatus
	{
		NOT_RUNNING,
		STARTED,
		NETOWRK_INITIALIZED,
		WAIT_ASSOC,
		DOWNLOADING,
		RELEASE_ASSOC,
		DROP_NETWORK,
		FINISHED,
	};
}
