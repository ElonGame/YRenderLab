#ifndef _RENDERDEVICE_H
#define _RENDERDEVICE_H

#include <windows.h>
#include <iostream>


#include "ErrorCode.hpp"


//�����ṩʵ�ʻ��ƹ��ܣ����������㣬��ʵ�ʴ��ڳ�ʼ�����Լ�����ʼ��
namespace YRender {
	class RenderDevice {
	public:
		RenderDevice& operator=(const RenderDevice& rhs) = delete;
		RenderDevice(const RenderDevice& rhs) = delete;
		static RenderDevice* GetDevice();

	public:
		ErrorCode Initial(HWND hwnd, const int width, const int height);
		void DrawPixel(const int x,const int y);
		void Draw();

	private:
		RenderDevice();
		~RenderDevice();

		


	private:
		uint32_t* FrameBuffer;
		int width;
		int height;
		HDC Hdc;
		HBITMAP hBitmap;
		HBITMAP hOldBitmap;
		HWND hwnd;
	};
}



#endif 