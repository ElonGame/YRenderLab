#include <Public/Viewer/DeferredRaster.h>
#include <Public/Scene/Scene.h>
#include <Public/Viewer/EnviromentGen.h>
#include <Public/OpenGLRHI/GLFBO.h>
#include <Public/OpenGLRHI/GlfwWindow.h>
#include <Public/Scene/MeshComponent.h>
#include <Public/Scene/Yobject.h>
#include <Public/Scene/TransformComponent.h>
#include <Public/Basic/BSDF_StandardPBR.h>
#include <Public/Basic/MaterialComponent.h>
#include <Public/OpenGLRHI/GLAD/glad/glad.h>
#include <Public/Scene/LightComponent.h>
#include <Public/Lights/DirectionalLight.h>
#include <Public/Lights/PointLight.h>

DeferredRaster::DeferredRaster(std::shared_ptr<Scene> scene, std::shared_ptr<GlfwWindow> pGLWindow)
	:
	Raster(scene, New<EnviromentGen>(pGLWindow), pGLWindow)
{
}

void DeferredRaster::Resize(unsigned int width, unsigned int height) {
	gbufferFBO.Resize(width, height);
	windowFBO.Resize(width, height);
}

//#TODO����deferred�иı�ֱ���ʱ�������FBO
void DeferredRaster::Draw() {
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update UBO
	{
		UpdateUBO_Camera();
		UpdateUBO_DirectionalLights();
		UpdateUBO_Environment();
	}

	//Update data that like  lut, shadow map, etc...
	{
		enviromentGen->UpdateEnvironment(scene);
	}


	{
		Pass_GBuffer();
		//GLFBO::DebugOutPutFrameBuffer(gbufferFBO);

		glDisable(GL_DEPTH_TEST);
		Pass_Lights();
		Pass_AmbientLight();
		glEnable(GL_DEPTH_TEST);

		//��Ⱦ��opqueue������ȾTransparent
		//Pass_Transparent();

		Pass_SkyBox();
		//GLFBO::DebugOutPutFrameBuffer(windowFBO);

		//Pass_TAA();

		Pass_PostProcess();
	}

}

void DeferredRaster::Initial() {
	Raster::Initial();

	InitShader_GBuffer();
	InitShader_Lights();
	InitShader_AmbientLight();
	InitShader_Skybox();
	InitShader_TAA();
	InitShader_PostProcess();

	new (&gbufferFBO)
		GLFBO(
			pGLWindow->GetViewPortW(), 
			pGLWindow->GetViewPortH(),
			{
				GLTexture::TexTureformat::TEX_GL_RGBA32F, //pixpos + roughness
				GLTexture::TexTureformat::TEX_GL_RGBA32F, //normal + metallic
				GLTexture::TexTureformat::TEX_GL_RGBA32F  //albedo + ao
			}
	);

	new (&windowFBO)
		GLFBO(
			pGLWindow->GetViewPortW(),
			pGLWindow->GetViewPortH(),
			{
				GLTexture::TexTureformat::TEX_GL_RGB32F
			}
	);

}


void DeferredRaster::InitShader_GBuffer() {
	//Default material GBuffer
	{
		new (&GBuffer_StandardPBRShader) GLShader("Data/shaders/P3N3T2T3.vs", "Data/shaders/DeferredPipline/GBuffer_StandardPBR.fs");
		GBuffer_StandardPBRShader.SetInt("Material.albedoTexture",0);
		GBuffer_StandardPBRShader.SetInt("Material.metallicTexture", 1);
		GBuffer_StandardPBRShader.SetInt("Material.roughnessTexture", 2);
		GBuffer_StandardPBRShader.SetInt("Material.aoTexture", 3);
		GBuffer_StandardPBRShader.SetInt("Material.normalTexture", 4);
		MapUBOToShader(GBuffer_StandardPBRShader);
	}
}

void DeferredRaster::InitShader_Lights() {
	new (&DirectLight_Shader) GLShader("Data/shaders/P2T2.vs", "Data/shaders/DeferredPipline/DirectLight.fs");
	//int StartIndex = 0;
	DirectLight_Shader.SetInt("GBuffer0", 0);
	DirectLight_Shader.SetInt("GBuffer1", 1);
	DirectLight_Shader.SetInt("GBuffer2", 2);

	MapUBOToShader(DirectLight_Shader);
}

void DeferredRaster::InitShader_AmbientLight() {
	new (&AmbientLight_Shader) GLShader("Data/shaders/P2T2.vs", "Data/shaders/DeferredPipline/AmbientLight.fs");
	AmbientLight_Shader.SetInt("GBuffer0", 0);
	AmbientLight_Shader.SetInt("GBuffer1", 1);
	AmbientLight_Shader.SetInt("GBuffer2", 2);

	//
	AmbientLight_Shader.SetInt("irradianceMap", 3);
	AmbientLight_Shader.SetInt("prefilterMap", 4);
	AmbientLight_Shader.SetInt("brdfLUT", 5);


	MapUBOToShader(AmbientLight_Shader);
}

void DeferredRaster::InitShader_Skybox() {
	new (&Skybox_Shader) GLShader("Data/shaders/SkyBox/skybox.vs", "Data/shaders/SkyBox/skybox.fs");
	Skybox_Shader.SetInt("skybox", 0);
	MapUBOToShader(Skybox_Shader);
}


void DeferredRaster::InitShader_TAA() {

}

void DeferredRaster::InitShader_PostProcess() {
	new (&postProcessShader) GLShader("Data/shaders/P2T2.vs", "Data/shaders/DeferredPipline/PostProcess.fs");
	postProcessShader.SetInt("SceneBuffer", 0);
	//��ʱ����uniform block ���Բ���Ҫӳ��
	//MapUBOToShader(postProcessShader)
}



void DeferredRaster::Pass_GBuffer() {
	gbufferFBO.Use();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Reset transform
	//ObjectTransformVec.clear();
	//ObjectTransformVec.emplace_back(1.f);

	this->Visit(scene->GetRoot());
}

void DeferredRaster::Pass_Lights() {
	windowFBO.Use();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	// set gbuffer
	gbufferFBO.GetColorTexture(0).Use(0);
	gbufferFBO.GetColorTexture(1).Use(1);
	gbufferFBO.GetColorTexture(2).Use(2);

	pGLWindow->GetVAO(GlfwWindow::VAOTYPE::Screen).Draw(DirectLight_Shader);
}



//����������յ�Pass�����ÿ�depth����Ϊǰ���pass�Ѿ����һ�������Ϣ��
void DeferredRaster::Pass_AmbientLight() {
	//��������Ҫ���ӽ�����
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	windowFBO.Use();

	gbufferFBO.GetColorTexture(0).Use(0);
	gbufferFBO.GetColorTexture(1).Use(1);
	gbufferFBO.GetColorTexture(2).Use(2);

	const int environmentBase = 3;

	auto environment = scene->GetEnviromentImg();
	if (environment) {
		auto irradianceMap = enviromentGen->GetIrradianceMap();
		auto prefilterMap = enviromentGen->GetPrefilterMap();	
		auto brdfLUT = enviromentGen->GetBRDF_LUT();

		irradianceMap.Use(environmentBase);
		prefilterMap.Use(environmentBase + 1);
		brdfLUT.Use(environmentBase + 2);
	}

	pGLWindow->GetVAO(GlfwWindow::VAOTYPE::Screen).Draw(AmbientLight_Shader);
	glDisable(GL_BLEND);
}


void DeferredRaster::Pass_SkyBox() {
	GLFBO::CopyFrameBuffer(windowFBO,gbufferFBO,GLFBO::RenderTargetCopyType::COPY_DEPTH_BUFFER);
	windowFBO.Use();
	if(auto environment = scene->GetEnviromentImg()){
		enviromentGen->GetSkyBox().Use(0);
	}
	glDepthFunc(GL_LEQUAL);
	pGLWindow->GetVAO(TriMesh::OriginCube).Draw(Skybox_Shader);
	glDepthFunc(GL_LESS);
}

void DeferredRaster::Pass_TAA() {

}

void DeferredRaster::Pass_PostProcess(){
	GLFBO::UseDefault();
	windowFBO.GetColorTexture(0).Use(0);
	pGLWindow->GetVAO(GlfwWindow::VAOTYPE::Screen).Draw(postProcessShader);

}



void DeferredRaster::Visit(std::shared_ptr<YObject> obj) {
	auto mesh = obj->GetComponent<MeshComponent>();
	auto material = obj->GetComponent<MaterialComponent>();

	//auto transform = obj->GetComponent<TransformComponent>();
	//if (transform != nullptr) {
	//	//����û���Ը��ڵ��transformΪ��
	//	ObjectTransformVec.push_back(/*modelVec.back() * */ transform->GetTransform());
	//}

	if (material && material->GetMaterial() && mesh && mesh->GetMesh()) {
		this->Visit(Cast<BSDF_StandardPBR>(material->GetMaterial()));
		this->Visit(mesh->GetMesh());
	}
	
	for (auto child : obj->GetChildrens()) {
		this->Visit(child);
	}

}


void DeferredRaster::Visit(std::shared_ptr<BSDF_StandardPBR> material) {

	SetCurShader(GBuffer_StandardPBRShader);
	GBuffer_StandardPBRShader.SetVec3f("Material.colorFactor", material->colorFactor);
	GBuffer_StandardPBRShader.SetFloat("Material.metallicFactor", material->metallicFactor);
	GBuffer_StandardPBRShader.SetFloat("Material.roughnessFactor", material->roughnessFactor);

	const int texNum = 5;
	std::shared_ptr<Image> imgs[texNum] = {
		material->albedoTexture,
		material->metallicTexture,
		material->roughnessTexture,
		material->aoTexture,
		material->normalTexture
	};


	//��Ϊ������Bindֻ�г�ʼ������Texture�ŵ��ã�����ֻ�е�һ֡����Texture������
	//������Useʱ�м䲻Ҫʹ����������
	auto _Texture0 = pGLWindow->GetTexture(imgs[0]);
	auto _Texture1 = pGLWindow->GetTexture(imgs[1]);
	auto _Texture2 = pGLWindow->GetTexture(imgs[2]);
	auto _Texture3 = pGLWindow->GetTexture(imgs[3]);
	auto _Texture4 = pGLWindow->GetTexture(imgs[4]);

	_Texture0.Use(0);
	_Texture1.Use(1);
	_Texture2.Use(2);
	_Texture3.Use(3);
	_Texture4.Use(4);
}


void DeferredRaster::Visit(std::shared_ptr<TriMesh> mesh) {
	pGLWindow->GetVAO(mesh).Draw(GBuffer_StandardPBRShader);
}