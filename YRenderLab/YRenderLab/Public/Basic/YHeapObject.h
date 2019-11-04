#ifndef _YRENDER_BASIC_YHEAPOBJECT_H_
#define _YRENDER_BASIC_YHEAPOBJECT_H_

#include <memory>

namespace YRender {
	//���౻shared_ptr����������ԭ��ָ��ʱҲ�ܶ��ڴ���ȷ����
	//std::shared_ptr<YHeapObject> p1(new YHeapObject());
	//std::shared_ptr<YHeapObject> p2 = bp1->YHeapObject();
	//�����̳���YHeapObject�Ϸ�p1��p2�ᱻ��������
	class YHeapObject : public std::enable_shared_from_this<YHeapObject> {
		template<typename Type,typename ...Args>
		friend const std::shared_ptr<Type> New(Args&& ... args);
	};
}


#endif