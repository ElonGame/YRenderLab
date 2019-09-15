#ifndef _YRENDER_SOFTRHI_WIN32WINDOW_H
#define _YRENDER_SOFTRHI_WIN32WINDOW_H


#include <Public/RenderWindow.h>
#include <Public/SoftRHI/SoftRHI.h>

namespace YRender {
	class Win32Window : public RenderWindow
	{
	public:
		Win32Window(const Win32Window& rhs) = delete;
		Win32Window& operator=(const Win32Window& rhs) = delete;
		Win32Window();
		virtual ~Win32Window();

	public:
		virtual bool Initial(const int width, const int height) override;
		virtual void Run() override;
	private:
		static LRESULT WINAPI WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		//������Ⱦ�̺߳���Էֿ���������ʱ���ֿܷ�����Ⱦ�ڴ���ѭ������
		HWND hwnd;
	};
}



#endif