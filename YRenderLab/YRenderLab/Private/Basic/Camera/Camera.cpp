#include <Public/Basic/Camera/Camera.h>

#include <Public/OpenGLRHI/GLAD/glad/glad.h>

namespace YRender {
	const float Camera::ASPECT_WH = 1.333f;
	const float Camera::NEAR_PLANE = 0.1f;
	const float Camera::FAR_PLANE = 100.0f;
	const float Camera::YAW = 0.f;  //Clockwise rotation is the nagative direction
	const float Camera::PITCH = 0.f;
	const float Camera::FOV = 50.0f;

	Camera::Camera(
		const Vector3& pos,
		float yaw,
		float pitch,
		float aspect,
		float nearPlane,
		float farPlane,
		const Vector3& up,
		ENUM_ProjectType projectionMode
	)
		: 
		position(pos),
		yaw(yaw),
		pitch(pitch),
		aspect_wh(aspect),
		nearPlane(nearPlane),
		farPlane(farPlane),
		worldup(up),
		project_mode(projectionMode),
		fov(FOV)
	{

		Camera::updateCameraVectors();	
	}




	void Camera::Initial(int w, int h){
		glGenBuffers(1, &cameraUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
		glBufferData(GL_UNIFORM_BUFFER, 160, NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		aspect_wh = static_cast<float>(w) / static_cast<float>(h);
		UpdateCamera();
	}

	void Camera::SetWH(int w, int h) {
		aspect_wh = static_cast<float>(w) / static_cast<float>(h);
		UpdateCamera();
	}


	void Camera::SetCameraPose(const Vector3& pos, float yaw,float pitch) {
		this->position = pos;
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
		UpdateCamera();
	}


	void Camera::updateCameraVectors() {
		// Calculate the new Front vector
		//opengl����ת˳��������תpitch -> yaw -> roll
		//�ڴ�ע��camera��front�Ǹ���Eular�Ǹ��µģ���LookAt��targetʵ���Ͼ���pos + front
		//						  [cos(yaw),  sin(yaw)sin(pitch),  sin(yaw)cos(pitch)]
		// R(yaw) * R(pitch) == > [0,         cos(pitch),          -sin(pitch)]
		//					      [-sin(yaw), cos(yaw)sin(pitch),  cos(yaw)cos(pitch)]
		//���ű�ʾfront����
		//								  [-sin(yaw)cos(pitch)]
		// R(yaw) * R(pitch) * front == > [sin(pitch)]
		//								  [-cos(yaw)cos(pitch)]
		//������ʾ������yaw��pitch��ʼֵ��0��ʼ���Ƶ�

		//u������㷽ʽ��opengl���������ͬ,yaw����ʼֵΪ-PI/2,ʵ�ʱ�Ϊ��3PI/2 - x, ��- x - PI/2
		//								 [-sin(yaw), -cos(yaw)sin(pitch),  -cos(yaw)cos(pitch)]
		// R(yaw - PI/2) * R(pitch) == > [0,         cos(pitch),          -sin(pitch)]
		//								 [cos(yaw), -sin(yaw)sin(pitch),  -sin(yaw)cos(pitch)]
		//����ͬ���ʾfront����
		//								  [cos(yaw)cos(pitch)]
		// R(yaw) * R(pitch) * front == > [sin(pitch)]
		//								  [sin(yaw)cos(pitch)]
		//������ʾ������yaw��-PI/2��ʼ���Ƶ���

		front.x = -sin(YGM::Math::Radians(yaw)) * cos(YGM::Math::Radians(pitch));
		front.y = sin(YGM::Math::Radians(pitch));
		front.z = -cos(YGM::Math::Radians(yaw)) * cos(YGM::Math::Radians(pitch));

		//front.x = cos(YGM::Math::Radians(yaw)) * cos(YGM::Math::Radians(pitch));
		//front.y = sin(YGM::Math::Radians(pitch));
		//front.z = sin(YGM::Math::Radians(yaw)) * cos(YGM::Math::Radians(pitch));

		front.SelfNormalize();
		right = front.Cross(worldup).Normalize();  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		up = right.Cross(front).Normalize();
	}


	void Camera::ProcessKeyboard(ENUM_Movement direction,float deltaTime){
		float velocity = 200.f * deltaTime;  //ready change for deltaTime
		switch (direction) {
		case ENUM_Movement::MOVE_FORWARD:
			position += front * velocity;
			break;
		case ENUM_Movement::MOVE_BACKWARD:
			position -= front * velocity;
			break;
		case ENUM_Movement::MOVE_LEFT:
			position -= right * velocity;
			break;
		case ENUM_Movement::MOVE_RIGHT:
			position += right * velocity;
			break;
		case ENUM_Movement::MOVE_UP:
			position += up * velocity;
			break;
		case ENUM_Movement::MOVE_DOWN:
			position -= up * velocity;
			break;
		}
		UpdateCamera();
	}



	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void Camera::ProcessMouseMovement(float xoffset, float yoffset)
	{
		xoffset *= 0.2f;
		yoffset *= 0.2f;

		yaw += xoffset;
		pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		//if (constrainPitch)
		//{
			if (pitch > 89.f)
				pitch = 89.f;
			else if (pitch < -89.f)
				pitch = -89.f;
		//}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
		UpdateCamera();
	}


	void Camera::UpdateCamera() {
		glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
		auto viewMatrix = GetViewMatrix().Transpose();
		auto projectM = GetProjectMatrix().Transpose();

		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, viewMatrix.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, projectM.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, 128, 12, position.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, 140, 4, &nearPlane);
		glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &farPlane);
		glBufferSubData(GL_UNIFORM_BUFFER, 148, 4, &fov);
		glBufferSubData(GL_UNIFORM_BUFFER, 152, 4, &aspect_wh);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
