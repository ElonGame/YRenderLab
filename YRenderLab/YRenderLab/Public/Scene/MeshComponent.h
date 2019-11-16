#ifndef _YRENDER_SCENE_MESHCOMPONENT_H_
#define _YRENDER_SCENE_MESHCOMPONENT_H_

#include <Public/Basic/Mesh/TriMesh.h>
#include <Public/Scene/Component.h>


namespace YRender {
	//class TriMesh;
	class MeshComponent : public Component {
	public:
		MeshComponent(
			const std::shared_ptr<YObject>& obj,
			const std::shared_ptr<TriMesh>& mesh
		):
			Component(obj),
			mesh(mesh)
		{

		}

	protected:
		MeshComponent() = delete;
		virtual ~MeshComponent() = default; //������ʱ֪ͨȫ���������VAO

	//public:
	//	virtual void Accept() override;

	public:
		const std::shared_ptr<TriMesh> GetMesh() const { return mesh; };

	private:
		std::shared_ptr<TriMesh> mesh;
	};
}



#endif