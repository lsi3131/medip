#pragma once

namespace fm
{
	struct WindowingInfo
	{
		int Level = 0;
		int Width = 0;

		WindowingInfo()
		{
			Level = 0;
			Width = 0;
		}

		WindowingInfo(int level, int width)
		{
			Level = level;
			Width = width;
		}

		void InitByHU(mint16 HuMin, mint16 HuMax)
		{
			Level = (HuMin + HuMax) / 2;
			Width = abs(HuMax - HuMin);
		}
	};
}
