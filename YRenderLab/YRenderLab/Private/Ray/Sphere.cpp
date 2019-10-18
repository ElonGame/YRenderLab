#include <Public/Ray/Sphere.h>
#include <random>

namespace YRender {
	bool Sphere::hit(const Ray& r, float Mint, float Maxt, HitInfo& hitinfo) {
		//�ڴ����������b = r.GetDirection().Dot(TempVal)�����2Լ��
		Vector3 TempVal = r.GetOrigin() - center;
		float a = r.GetDirection().Dot(r.GetDirection());
		float b = r.GetDirection().Dot(TempVal);
		float c = TempVal.Dot(TempVal) - radius * radius;
		float discriminant = b * b - a * c;
		if (discriminant > 0.f) {
			//��ֻ�ܱ�֤���������closest,���������������ԭ�������¿���ֻ����������
			float solution = (-b - sqrt(discriminant)) / a;
			if (solution < Maxt && solution > Mint) {
				hitinfo.t = solution;
				hitinfo.HitPoint = r.PointAtParameter(solution);
				hitinfo.Normal = (hitinfo.HitPoint - center) / radius;
				return true;
			}
			solution = (-b + sqrt(discriminant)) / a;
			if (solution < Maxt && solution > Mint) {
				hitinfo.t = solution;
				hitinfo.HitPoint = r.PointAtParameter(solution);
				hitinfo.Normal = (hitinfo.HitPoint - center) / radius;
				return true;
			}
		}
		return false;
	}

	//r^2*sin��*dr*d��*d��  ���΢�������ʾ

	//�����������������һ�㣬����Ϊr^2dr ~ r^3, sin��d�� ~ -cos��, d�� ~ ��

	//��������ֵ�ֱ�Ϊ [ r��1/3�η���-cos������]���Ӿ��ȷֲ������ȷֲ�����[-1,1]��
	
	//��Cpp�б�����ĸ�������ʹ�� dis2(0, std::nextafter(1.f, DBL_MAX))��

	//��ֵ����1.f���¸������ʾ

	Vector3 Sphere::GeneratorRandPoint() {
		std::random_device rd;
		std::mt19937 gen(rd());

		//-costheta,[-1,1]֮����ȷֲ�
		std::uniform_real_distribution<float> disTheta(-1.f, std::nextafter(1.f, FLT_MAX));

		//gama, [0,2PI]֮����ȷֲ�
		std::uniform_real_distribution<float> disGamma(0.f, std::nextafter(2.f * YGM::Math::PI, FLT_MAX));

		//radius^3, [0,r]֮����ȷֲ� 
		std::uniform_real_distribution<float> Radius(0.f, std::nextafter(1.f, FLT_MAX));

		auto r = pow(Radius(gen),0.333333f);
		auto theta = acos(disTheta(gen));
		auto garma = disGamma(gen);

		//auto test = pow(r * cos(garma) * sin(theta), 2) + pow(r * sin(garma) * sin(theta), 2) + pow(r * cos(theta), 2);
		//std::cout << test << std::endl;

		return Vector3
		(
			r * cos(garma) * sin(theta),
			r * sin(garma) * sin(theta),
			r * cos(theta)
		);

	}
}