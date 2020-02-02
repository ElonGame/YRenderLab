#ifndef _YRENDER_BASIC_BSDF_H_
#define _YRENDER_BASIC_BSDF_H_

#include <Public/Basic/Material.h>


class Image;
class BSDF : public Material {
protected:
	BSDF() = default;
	virtual ~BSDF() = default;

public:
	//BSDF������(������̳�ֻʵ��BRDF�������䲿��)
	/*
	@Param1 ���䷽��
	@Param2 ���䷽��
	@Param3 ����UV
	*/
	virtual const RGBf F(const Vector3& Wo, const Vector3& Wi, const Vector2& texcoord) = 0;

	// �����ܶȺ������������߼���
	virtual float PDF(const Vector3& Wo, const Vector3& Wi, const Vector2& texcoord) = 0;

	virtual const RGBf GetEmission() const { return RGBf(0.f); }

public:
	//��������ϵy�ᳯ��
	static Vector3 TangentSpaceToWorld(const Vector3& worldTangenet, const Vector3& worldNormal, const Vector3& TangentSpaceNormal);
};




#endif