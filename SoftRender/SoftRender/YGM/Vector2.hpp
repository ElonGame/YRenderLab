﻿#ifndef _YRENDER_YGM_VECTOR2_HPP_
#define _YRENDER_YGM_VECTOR2_HPP_

#include <YGM/Vector.hpp>

namespace YRender {
	namespace YGM {
		template<typename T>
		class Vector<2, T> {
		private:
			using value_type = T;
		public:
			template <typename U,typename V>
			Vector(U x, V y) :
				x(static_cast<value_type>(0)),
				y(static_cast<value_type>(0)) {}
			explicit Vector(value_type val) : Vector(val, val) {}
			Vector() : Vector(static_cast<value_type>(0)) {}
			Vector(const Vector& rhs) : Vector(rhs.x, rhs.y, rhs.z) {}
			bool operator==(const Vector& rhs) {
				return Math::Equal(x, rhs.x) && Math::Equal(y, rhs.y);
			}
			bool operator!=(const Vector& rhs) {
				return !Math::Equal(x, rhs.x) || !Math::Equal(y, rhs.y);
			}
			Vector& operator=(const Vector& rhs) 
			{
				x = rhs.x;
				y = rhs.y;
			}

		public:
			union 
			{
				value_type _data[2];
				struct {
					union {value_type x,r,s};
					union {value_type y,g,t};
				};
			};
		};
	}
}

#endif