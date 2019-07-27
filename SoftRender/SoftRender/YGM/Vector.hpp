﻿#ifndef _YRENDER_YGM_VAL_HPP_
#define _YRENDER_YGM_VAL_HPP_

#include <YGM/Math.hpp>

namespace YRender {
	namespace YGM {
		template <unsigned int N, typename T>
		class Vector;
	}

	template <unsigned int N, typename T>
	using Vector = YGM::Vector<N, T>;


	using Vector2i = Vector<2, int>;
	using Vector2f = Vector<2, float>;
	using Vector2 = Vector2f;


	using Vector3i = Vector<3, int>;
	using Vector3f = Vector<3, float>;
	using Vector3 = Vector3f;
}




#endif