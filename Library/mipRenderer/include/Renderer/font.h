#pragma once
#include "mip/core.h"

namespace mip
{
	class COLOR;
	class RECTi;

	class TEXT_PRINT
	{
	public:
		TEXT_PRINT();

		bool init(int width, int height, const char* font = nullptr);
		void release();
		void draw(COLOR * color);

		void drawText(const char* str, int x, int y);
		void print(const char* str);
		void scroll(int lines);
		void moveCursor(int x, int y);
		void reset();

		static muint32 ktx_load(const char * filename, muint32 tex = 0);

	private:
		muint32      text_buffer;
		muint32      font_texture;
		muint32      vao;

		muint32      text_program;
		char *      screen_buffer;
		int         buffer_width;
		int         buffer_height;
		bool        dirty;
		int         cursor_x;
		int         cursor_y;
	};
	
	class FONT
	{
	public:
		FONT();
		~FONT();
		void release();

		bool createFont(const std::wstring& strTypeface,
			int iFontHeight,
			int iFontWeight);

		bool drawText(const std::string& strText, int XPos,
			int YPos, float iRed = 1.0, float iGreen = 1.0, float iBlue = 1.0);

		bool getTextSize(const std::wstring& strText, RECTi * outRect);

		static void setDeviceContext(HDC hDevContext)
		{
			m_hDeviceContext = hDevContext;
		}

	private:
		static HDC m_hDeviceContext;
		muint32 m_uiListBase;
		HFONT m_hFont;
	};
};