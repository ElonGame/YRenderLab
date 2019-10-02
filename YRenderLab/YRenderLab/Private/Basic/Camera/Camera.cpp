#include <Public/Basic/Camera/Camera.h>



namespace YRender {
	const float Camera::ASPECT_WH = 1.333f;
	const float Camera::NEAR_PLANE = 0.01f;
	const float Camera::FAR_PLANE = 15.0f;
	const float Camera::YAW = 0.f;  //Clockwise rotation is the nagative direction
	const float Camera::PITCH = 0.0f;
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




	void Camera::SetCameraPose(const Vector3& pos, float yaw,float pitch) {
		this->position = pos;
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
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

		//u��ʦ�����㷽ʽ��opengl���������ͬ,yaw����ʼֵΪ-PI/2,ʵ�ʱ�Ϊ��3PI/2 - x, ��- x - PI/2
		//								 [-sin(yaw), -cos(yaw)sin(pitch),  -cos(yaw)cos(pitch)]
		// R(yaw - PI/2) * R(pitch) == > [0,         cos(pitch),          -sin(pitch)]
		//								 [cos(yaw), -sin(yaw)sin(pitch),  -sin(yaw)cos(pitch)]
		//����ͬ����ʾfront����
		//								  [cos(yaw)cos(pitch)]
		// R(yaw) * R(pitch) * front == > [sin(pitch)]
		//								  [sin(yaw)cos(pitch)]
		//������ʾ������yaw��-PI/2��ʼ���Ƶ���

		front.x = -sin(YGM::Math::Radians(yaw)) * cos(YGM::Math::Radians(-pitch));
		front.y = sin(YGM::Math::Radians(pitch));
		front.z = -cos(YGM::Math::Radians(yaw)) * cos(YGM::Math::Radians(-pitch));

		//front.x = cos(YGM::Math::Radians(yaw)) * cos(YGM::Math::Radians(pitch));
		//front.y = sin(YGM::Math::Radians(pitch));
		//front.z = sin(YGM::Math::Radians(yaw)) * cos(YGM::Math::Radians(pitch));

		front.SelfNormalize();
		right = front.Cross(worldup).Normalize();  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		up = right.Cross(front).Normalize();
	}


	void Camera::ProcessKeyboard(ENUM_Movement direction){
		float velocity = 0.02f;  //ready change for deltaTime
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
	}
}