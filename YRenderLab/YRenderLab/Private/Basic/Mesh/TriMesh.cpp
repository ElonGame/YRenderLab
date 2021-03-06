#include <Public/Basic/Mesh/TriMesh.h>
#include <Public/Basic/Mesh/Cube.h>
#include <Public/Viewer/Raster.h>
#include <Public/Viewer/ShadowGen.h>

//Create static Cube
//std::shared_ptr<TriMesh> TriMesh::OriginCube = GenCube();


TriMesh::TriMesh(
	const std::vector<unsigned int>& indice,
	const std::vector<Vector3>& position,
	const std::vector<Vector3>& normal,
	const std::vector<Vector2>& texcoords,
	const std::vector<Vector3>& tangents)
	:
	indice(indice),
	position(position),
	normal(normal),
	texcoords(texcoords),
	tangents(tangents)
{
	if (indice.size() <= 0
		|| indice.size() % 3 != 0
		|| position.size() <= 0
		|| normal.size() != position.size()
		|| texcoords.size() != position.size()
		|| (tangents.size() != 0 && tangents.size() != position.size())
		)
	{
		printf("ERROR: TriMesh is invalid.\n");
		return;
	}

	//for (unsigned int i = 0; i < indice.size(); i += 3) {
	//	triangles.push_back(Triangle::New(indice[i], indice[i + 1], indice[i + 2]));
	//}

	//#TODO：这里处理镜像模型，根据w手动reverse一下tangent
	//if(tangents.empty())
	//	GenTangents();
}

TriMesh::TriMesh(
	std::vector<unsigned int>&& indice, 
	std::vector<Vector3>&& position, 
	std::vector<Vector3>&& normal, 
	std::vector<Vector2>&& texcoords, 
	std::vector<Vector3>&& tangents)
{
	if (indice.size() <= 0
		|| indice.size() % 3 != 0
		|| position.size() <= 0
		|| normal.size() != position.size()
		|| texcoords.size() != position.size()
		|| (tangents.size() != 0 && tangents.size() != position.size())
		)
	{
		printf("ERROR: TriMesh is invalid.\n");
		return;
	}
	this->indice = std::move(indice);
	this->position = std::move(position);
	this->normal = std::move(normal);
	this->texcoords = std::move(texcoords);
	this->tangents = std::move(tangents);

	//if(tangents.empty())
//	GenTangents();
}


TriMesh::TriMesh(
	const int indiceNum,
	const int vertexNum,
	const unsigned int* indice,
	const Vector3* positions,
	const Vector3* normals,
	const Vector2* texcoords,
	const Vector3* tangents)
{
	if (!indice || !positions || !normals || !texcoords) {
		printf("ERROR: TriMesh is invalid.\n");
		return;
	}
}


void TriMesh::InitAfterNew() {

}

void TriMesh::RenderPrimitive(const std::shared_ptr<Raster>& raster, const YGM::Transform& model){
	raster->RenderMesh(shared_this<TriMesh>(), model);
}

void TriMesh::RenderShadowPrimitive(const std::shared_ptr<ShadowGen>& shadowgen, const YGM::Transform& model) {
	shadowgen->RenderMesh(shared_this<TriMesh>(), model);
}