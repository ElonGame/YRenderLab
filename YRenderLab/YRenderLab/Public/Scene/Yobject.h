#ifndef _YRENDER_SCENE_YOBJECT_H_
#define _YRENDER_SCENE_YOBJECT_H_

#include <Public/Basic/YHeapObject.h>

#include <string>
#include <unordered_set>

namespace YRender {
	class Component;
	class YObject : public YHeapObject {	
	public:
		YObject(const std::string& objname) : name(objname) {}	

	protected:
		virtual ~YObject() = default;

	public:

		//�߼��̣߳�shared_ptr��ȫ����ʹ��ֵ����
		void AttachComponent(const std::shared_ptr<Component>& component);

		void DetachComponent(const std::shared_ptr<Component>& component);

		const std::unordered_set<std::shared_ptr<Component>>& Getcomponents() const { return components; };

	//public:
	//	static std::shared_ptr<YObject> New(const std::string& objname = "DefaultObj") {
	//		return YRender::New<YObject>(objname);
	//	}
	private:
		friend YHeapObject;
		//shared_ptr��hash�Ƕ�ԭ��ֵ���㣬���Բ�ͬ��shared����Ҳ�ܲ���
		std::unordered_set<std::shared_ptr<Component>> components;
		std::string name;
	};

	//template<typename ComponentType>
	//inline void Yobject::AttachComponent(const std::shared_ptr<ComponentType>& component){

	//}
}




#endif