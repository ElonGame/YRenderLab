#ifndef _RENDERCLASS_H
#define _RENDERCLASS_H

#include <iostream>

//����������Ⱦ���㣬�����ݴ��䵽Device�У�����Device��ʲô�ӿڣ�ֻ��ҪDrawPix

namespace YRender {
	class RenderClass {
	public:
		RenderClass(const RenderClass& rhs) = delete;
		RenderClass& operator=(const RenderClass& rhs) = delete;
		static RenderClass* GetRender();

	private:
		~RenderClass();
		RenderClass();
	
	};


}

#endif
