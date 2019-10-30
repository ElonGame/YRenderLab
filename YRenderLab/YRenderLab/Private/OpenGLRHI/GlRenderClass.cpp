#include <Public/OpenGLRHI/GlRenderClass.h>
#include <Public/OpenGLRHI/GLShader.h>
#include <Public/OpenGLRHI/GLVAO.h>

namespace YRender {
	namespace Utils {
		template<typename T, int size>
		constexpr uint32_t GetArrLength(T(&)[size]) { return size; }
	}


	float vertices[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
	};

	GlRenderClass::GlRenderClass(GLFWwindow * window){

	}

	GlRenderClass::~GlRenderClass(){

	}

	bool GlRenderClass::Initial(const int width, const int height)
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}
		glEnable(GL_DEPTH_TEST);

		GlslShader = std::make_unique<GLShader>("data/shaders/Test.vs", "data/shaders/Test.fs");
		std::vector<uint32_t> attrVec = { 3,3 };
		GlVAO = std::make_unique<VAO>(vertices, Utils::GetArrLength(vertices) * static_cast<uint32_t>(sizeof(float)), attrVec);
		return true;
	}

	void GlRenderClass::Tick(){

	}

	void GlRenderClass::Render(){
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GlslShader->SetMat4f("view", MainCamera.GetViewMatrix().selfTranspose());
		GlslShader->SetMat4f("proj", MainCamera.GetProjectMatrix().selfTranspose());
		GlVAO->Draw(*GlslShader);											 	
	}
}


