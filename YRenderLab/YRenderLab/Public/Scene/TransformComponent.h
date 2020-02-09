#ifndef _YRENDER_SCENE_TRANSFORM_COMPONENT_H_
#define _YRENDER_SCENE_TRANSFORM_COMPONENT_H_

#include <Public/Scene/Component.h>
#include <Public/YGM/Transform.h>

//#TODO Transformֻ��Ҫ����һ�����󣬲���Ҫ������position��scale
class TransformComponent : public Component {
public:
	const static Vector3 Forward ;
	const static Vector3 Up;
	const static Vector3 Right;
public:
	TransformComponent(
		const std::shared_ptr<YObject> obj,
		const YGM::Transform& transform =  YGM::Transform(1.f)
	) :
		Component(obj),
		transform(transform)
	{

	}
protected:
	TransformComponent() = delete;
	virtual ~TransformComponent() = default;

public:
	const YGM::Transform& GetTransform() const { return transform; }

private:
	//Ӧ��Ҳ��componentToWorld Ȼ�����relative
	mutable YGM::Transform transform;

};





#endif