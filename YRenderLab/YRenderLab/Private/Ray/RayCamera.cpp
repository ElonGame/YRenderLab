#include <Public/Ray/RayCamera.h>
#include <Public/Basic/Image/Image.h>
#include <Public/YGM/RGB.hpp>
#include <Public/Ray/HitinfoManager.h>
#include <Public/Ray/Sphere.h>

#include <random>

namespace YRender {
	RayCamera::RayCamera(
		const Vector3& lowLeftCorner,
		const Vector3& horizontal,
		const Vector3& vertical,
		const Vector3& position
	) :
		LowLeft(lowLeftCorner),
		horizontal(horizontal),
		vertical(vertical),
		position(position)
	{
		ray.SetOrigin(position);
	}

	void RayCamera::TestRayTracer() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0, 1);
		int width = 400;
		int height = 200;
		int samples = 100;
		RayCamera camera;
		uint8_t* OutPutData = new uint8_t[width * height * 3];
		HitInfo hitrecord;
		auto HitManager = HitInfoManager::GetInstance();
		HitManager->GeometricList.push_back(new Sphere(Vector3(0.f, 0.f, -1.f), 0.5f));
		HitManager->GeometricList.push_back(new Sphere(Vector3(0.f, -100.5f, -1.f), 100.f));
		for (int j = 0; j < height; ++j) 
		{
			for (int i = 0; i < width * 3; i += 3) 
			{
				RGBf Color;
				for (int s = 0; s < samples; ++s) 
				{
					float u = (i + dis(gen)) / static_cast<float>(width * 3);
					float v = (j + dis(gen)) / static_cast<float>(height);
					Ray& ray = camera.GetRay(u, v);
					float Consume = 1.f;
					while (HitManager->CheckHit(ray, 0.001f, YGM::Math::MaxFloat, hitrecord)) {
						Consume *= 0.5f;
						Vector3 ScatterVec = hitrecord.Normal + Sphere::GeneratorRandPoint();
						ray.SetDirection(ScatterVec);
						ray.SetOrigin(hitrecord.HitPoint);
					}
					float lerpVal = (ray.GetDirection().y + 1.0f) * 0.5f;
					Color += (RGBf::White * (1.f - lerpVal) + RGBf(0.5f, 0.7f, 1.f) * lerpVal) * Consume;
				}
				Color /= static_cast<float>(samples);
				Color = Vector3(pow(Color.x,0.45f), pow(Color.y,0.45f), pow(Color.z,0.45f));
				//���½���Ϊ���-��stb������
				OutPutData[i + (height - 1 - j) * width * 3] = static_cast<uint8_t>(Color.r * 255.f);
				OutPutData[i + (height - 1 - j) * width * 3 + 1] = static_cast<uint8_t>(Color.g * 255.f);
				OutPutData[i + (height - 1 - j) * width * 3 + 2] = static_cast<uint8_t>(Color.b * 255.f);
			}
		}
		Image::OutPng("C:\\Users\\Administrator\\Desktop\\ray.png", width, height, 3, OutPutData);
		delete OutPutData;
	}
}

