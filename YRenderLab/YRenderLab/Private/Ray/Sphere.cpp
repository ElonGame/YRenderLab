#include <Public/Ray/Sphere.h>


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
}