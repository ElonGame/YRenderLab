#include <Public/Viewer/ShadowGen.h>
#include <Public/OpenGLRHI/GlfwWindow.h>
#include <Public/OpenGLRHI/GLAD/glad/glad.h>
#include <Public/Scene/Scene.h>
#include <Public/Scene/Yobject.h>
#include <Public/Lights/DirectionalLight.h>
#include <Public/Lights/PointLight.h>
#include <Public/Scene/LightComponent.h>
#include <Public/Scene/MeshComponent.h>
#include <Public/Scene/TransformComponent.h>
#include <Public/Basic/MaterialComponent.h>

ShadowGen::ShadowGen(std::shared_ptr<GlfwWindow> pGLWindow)
	:
	pGLWindow(pGLWindow), DepthMapSize(1024)
{

}

void ShadowGen::Init() {
	new (&DirectionalLightShadow) GLShader("Data/shaders/MVP_P3.vs", "Data/shaders/Common/Empty.fs");
}


void ShadowGen::UpdateShadowMap(std::shared_ptr<Scene> scene) {
	if (!scene || !scene->GetRoot() || !scene->GetCamera()) {
		printf("Error! ShadowGen::Visit\n scene or root or camera is nullptr!\n");
		return;
	}

	GLint origFBO;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFBO);
	GLint origViewport[4];
	glGetIntegerv(GL_VIEWPORT, origViewport);
	for (auto& lightComponent : scene->GetLightComponents()) {
		auto light = lightComponent->GetLight();
		if (auto directionalLight = Cast<DirectionalLight>(light) && DirectionalDepthFBOMap.find(lightComponent) == DirectionalDepthFBOMap.end()) {
			GLFBO DirectionLight_DepthMap(DepthMapSize, DepthMapSize, GLFBO::FrameBufferType::ENUM_TYPE_DEPTH);
			DirectionalDepthFBOMap.emplace(lightComponent, DirectionLight_DepthMap);
			continue;
		}
	}

	for (auto iter = DirectionalDepthFBOMap.cbegin(); iter != DirectionalDepthFBOMap.cend();) {
		auto CurIter = iter;
		++iter;
		if (CurIter->first.expired()) {
			DirectionalDepthFBOMap.erase(CurIter);
			continue;
		}
		GenDirectionalDepthMap(scene, CurIter->second, CurIter->first.lock());

		//GLFBO::DebugOutPutFrameBuffer(CurIter->second, GLFBO::DebugType::DebugType_Depth);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, origFBO);
	glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]);
}


void ShadowGen::GenDirectionalDepthMap(const std::shared_ptr<Scene>& Scene, const GLFBO& DepthFBO, const std::shared_ptr<LightComponent>& lightComponent) {
	const auto corners = Scene->GetCamera()->Corners();
	Vector3 Center;
	for (auto& Point : corners) {
		Center += Point;
	}
	Center /= static_cast<float>(corners.size());

	DepthFBO.Use();
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, DepthMapSize, DepthMapSize);
	glClear(GL_DEPTH_BUFFER_BIT);

	//scale��Ϊ1����Ҫnormalize�������Ȳ�normalize
	Vector3 LightVec = lightComponent->GetOwner()->GetObjectWorldForward();

	//������ĵ�8��������LightVec�����ͶӰȷ���ڸ÷����ϵ��������Զֵ
	float minD = FLT_MAX;
	float maxD = -FLT_MAX;
	for (auto& Point : corners) {
		float CurDis = (Point - Center).Dot(LightVec);
		if (CurDis < minD)
			minD = CurDis;
		if (CurDis > maxD)
			maxD = CurDis;
	}

	const float backRatio = 0.05f;
	float extent = maxD - minD;
	auto pos = Center + LightVec * (minD - extent * backRatio);
	auto WorldToViewTransform = YGM::Transform::LookAt(pos, Center);

	//ȷ��ͶӰ���x��y�����ߣ�
	float maxX = 0.f;
	float maxY = 0.f;
	for (auto corner : corners) {
		auto cornerInCam = WorldToViewTransform.TrasformPoint(corner).AbsVal();
		if (cornerInCam.x > maxX)
			maxX = cornerInCam.x;
		if (cornerInCam.y > maxY)
			maxY = cornerInCam.y;
	}
	auto proj = YGM::Transform::Orthographic(2 * maxX, 2 * maxY, 0, extent * (1 + backRatio));

	DirectionalLightShadow.SetMat4f("view", WorldToViewTransform.GetMatrix().Transpose());
	DirectionalLightShadow.SetMat4f("projection", proj.GetMatrix().Transpose());

	//light2pv[lightComponent] = proj * view;
	this->RenderDirectionalShadowMap(Scene->GetRoot());
}

void ShadowGen::RenderDirectionalShadowMap(std::shared_ptr<YObject> root) {
	auto mesh = root->GetComponent<MeshComponent>();
	auto transform = root->GetComponent<TransformComponent>();

	if (mesh && transform &&  mesh->GetMesh()) {
		DirectionalLightShadow.SetMat4f("model", transform->GetWorldTransform().GetMatrix().Transpose());
		pGLWindow->GetVAO(mesh->GetMesh()).Draw(DirectionalLightShadow);
	}

	for (auto child : root->GetChildrens()) {
		this->RenderDirectionalShadowMap(child);
	}
}
