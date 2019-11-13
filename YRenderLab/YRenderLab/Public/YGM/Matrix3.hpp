#ifndef _YRENDER_YGM_MAT3x3_HPP_
#define _YRENDER_YGM_MAT3x3_HPP_

#include <Public/YGM/Vector3.hpp>
#include <iostream>

namespace YRender {
	namespace YGM {
		template<typename T>
		class Mat3x3 {
			//private:
			//	using type_value = T;
		public:
			Mat3x3(T d0, T d1, T d2) : m{
				{d0, static_cast<T>(0),static_cast<T>(0)},
				{static_cast<T>(0),d1,static_cast<T>(0)},
				{static_cast<T>(0),static_cast<T>(0),d2} }
			{}

			explicit Mat3x3(T d = static_cast<T>(1)) : Mat3x3(d, d, d) { }

			Mat3x3(const Vector<3, T> & row0, const Vector<3, T> & row1, const Vector<3, T> & row2)
				:m{ row0, row1, row2} { }

			// mat Ϊ�����򣬵���glsl���ǰ��н��������ڴ��е�ֵ��00 10 20 30����
			explicit Mat3x3(const T mat[3][3]) { memcpy(m, mat, 9 * sizeof(T)); }

			Mat3x3(
				T t00, T t01, T t02,
				T t10, T t11, T t12,
				T t20, T t21, T t22
			) : m{
			{t00, t01, t02},
			{t10, t11, t12},
			{t20, t21, t22}} { }

			Mat3x3(const Mat3x3<T>& mat) { memcpy(m, mat.m, 9 * sizeof(T)); }

		public:
			const Vector<3, T>& GetRow(int i) const { return m[i]; }
			Vector<3, T>& GetRow(int i) { return m[i]; }

			// ������
			T* Data() { return &(m[0][0]); }

			// ������
			const T* Data() const { return const_cast<T*>(&(m[0][0])); }

		public:
			bool IsIdentity() const {
				static constexpr T one = static_cast<T>(1);
				static constexpr T zero = static_cast<T>(0);

				return (m[0][0] == one && m[0][1] == zero && m[0][2] == zero
					&& m[1][0] == zero && m[1][1] == one && m[1][2] == zero
					&& m[2][0] == zero && m[2][1] == zero && m[2][2] == one);
			}

			T Tr() const { return m[0][0] + m[1][1] + m[2][2]; }

			Mat3x3 Transpose() const {
				const auto& mat = *this;
				return Mat3x3(
					mat(0, 0), mat(1, 0), mat(2, 0),
					mat(0, 1), mat(1, 1), mat(2, 1),
					mat(0, 2), mat(1, 2), mat(2, 2)
				);
			}

			const Mat3x3& selfTranspose() {
				for (int i = 0; i < 3; ++i) {
					for (int j = i; j < 3; ++j) {
						if (i == j)
							continue;
						std::swap(m[i][j], m[j][i]);
					}
				}
				return *this;
			}

			Mat3x3 Inverse() const {
				int indxc[3], indxr[3];
				int ipiv[3] = { 0, 0, 0, 0 };
				T minv[3][3];
				memcpy(minv, m, 3 * 3 * sizeof(T));
				for (int i = 0; i < 3; i++) {
					int irow = 0, icol = 0;
					T big = static_cast<T>(0);
					// Choose pivot
					//��¼���ֵ�����У��ں�������Ӧ�е����Խ�Ԫֵ�Ƿ�Ϊ0��
					for (int j = 0; j < 3; j++) {
						if (ipiv[j] != 1) {
							for (int k = 0; k < 3; k++) {
								if (ipiv[k] == 0) {
									if (std::abs(minv[j][k]) >= big) {
										big = static_cast<T>(std::abs(minv[j][k]));
										irow = j;
										icol = k;
									}
								}
								//����������²������ĳ���Խ�Ԫ��Ӧֵ����1
								//else if (ipiv[k] > 1)
								//	return Mat3x3(static_cast<T>(0));
							}
						}
					}
					//���õ�ǰ�е����Խ�ԪֵΪ1��֮����ѡȡ��Ԫʱ������Ը��и���
					++ipiv[icol];
					//�滻pivot��[icol][icol]λ�ã���λ��Ϊ��ǰ�е����Խ��ߴ������ﲢû���滻�����Ͻ�
					if (irow != icol) {
						for (int k = 0; k < 3; ++k)
							std::swap(minv[irow][k], minv[icol][k]);
					}
					//���ڼ�¼����λ�õ��������飬�������Ľ��Ϊ�����ĺ�����
					indxr[i] = irow;
					indxc[i] = icol;
					if (minv[icol][icol] == static_cast<T>(0))
						return Mat3x3(static_cast<T>(0));

					// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
					//����Ӧ���Խ�Ԫ���г���ԭֵ
					T pivinv = static_cast<T>(1) / minv[icol][icol];
					minv[icol][icol] = 1.;
					for (int j = 0; j < 3; j++)
						minv[j][icol] *= pivinv;

					//������Ĵ�������ĵط�����minv[icol][j] = 0; 
					//����Ӧ�е�ֵ����뵥λ��������ͬ��Ȼ���پ�����ͬ�ļ��������������λ����任��Ľ����ͬ
					//������Ӧ������������Ԫֵ��ͬ��������֤�˺�������ʱ������Ȼ��ȷ���ǵ�λ����ı任���Ҳ���������
					for (int j = 0; j < 3; j++) {
						if (j != icol) {
							T save = minv[icol][j];
							minv[icol][j] = 0;
							for (int k = 0; k < 3; k++)
								minv[k][j] -= minv[k][icol] * save;
						}
					}
				}

				//��󽫽���ǰ��˳��ԭ���������������Ǿ����н������������������൱�ڳ���ԭʼ�б任����ԭ˳��
				// Swap columns to reflect permutation
				for (int j = 3; j >= 0; j--) {
					if (indxr[j] != indxc[j]) {
						for (int k = 0; k < 3; k++)
							std::swap(minv[indxr[j]][k], minv[indxc[j]][k]);
					}
				}
				return Mat3x3(minv);
			}

		public:
			Mat3x3& operator=(const Mat3x3& rhs) {
				memcpy(m, rhs.m, 9 * sizeof(T));
				return *this;
			}

			bool operator ==(const Mat3x3 & rhs) const {
				return
					m[0][0] == rhs.m[0][0]
					&& m[0][1] == rhs.m[0][1]
					&& m[0][2] == rhs.m[0][2]
					&& m[1][0] == rhs.m[1][0]
					&& m[1][1] == rhs.m[1][1]
					&& m[1][2] == rhs.m[1][2]
					&& m[2][0] == rhs.m[2][0]
					&& m[2][1] == rhs.m[2][1]
					&& m[2][2] == rhs.m[2][2];
			}

			bool operator!=(const Mat3x3 & rhs) const {
				return
					m[0][0] != rhs.m[0][0]
					|| m[0][1] != rhs.m[0][1]
					|| m[0][2] != rhs.m[0][2]
					|| m[1][0] != rhs.m[1][0]
					|| m[1][1] != rhs.m[1][1]
					|| m[1][2] != rhs.m[1][2]
					|| m[2][0] != rhs.m[2][0]
					|| m[2][1] != rhs.m[2][1]
					|| m[2][2] != rhs.m[2][2];
			}

			const Mat3x3 operator*(const Mat3x3 & rhs) const {
				const auto& lhs = *this;
				T t00 = lhs(0, 0) * rhs(0, 0) + lhs(0, 1) * rhs(1, 0) + lhs(0, 2) * rhs(2, 0);
				T t01 = lhs(0, 0) * rhs(0, 1) + lhs(0, 1) * rhs(1, 1) + lhs(0, 2) * rhs(2, 1);
				T t02 = lhs(0, 0) * rhs(0, 2) + lhs(0, 1) * rhs(1, 2) + lhs(0, 2) * rhs(2, 2);
				T t10 = lhs(1, 0) * rhs(0, 0) + lhs(1, 1) * rhs(1, 0) + lhs(1, 2) * rhs(2, 0);
				T t11 = lhs(1, 0) * rhs(0, 1) + lhs(1, 1) * rhs(1, 1) + lhs(1, 2) * rhs(2, 1);
				T t12 = lhs(1, 0) * rhs(0, 2) + lhs(1, 1) * rhs(1, 2) + lhs(1, 2) * rhs(2, 2);
				T t20 = lhs(2, 0) * rhs(0, 0) + lhs(2, 1) * rhs(1, 0) + lhs(2, 2) * rhs(2, 0);
				T t21 = lhs(2, 0) * rhs(0, 1) + lhs(2, 1) * rhs(1, 1) + lhs(2, 2) * rhs(2, 1);
				T t22 = lhs(2, 0) * rhs(0, 2) + lhs(2, 1) * rhs(1, 2) + lhs(2, 2) * rhs(2, 2);
				return Mat3x3(
					t00, t01, t02,
					t10, t11, t12,
					t20, t21, t22
				);
			}

			const Vector<3, T> operator*(const Vector<3, T>& rhs) const {
				return Vector<3, T>
					(
						this->m[0][0] * rhs[0] + this->m[0][1] * rhs[1] + this->m[0][2] * rhs[2],
						this->m[1][0] * rhs[0] + this->m[1][1] * rhs[1] + this->m[1][2] * rhs[2],
						this->m[2][0] * rhs[0] + this->m[2][1] * rhs[1] + this->m[2][2] * rhs[2]
						);
			}



			T operator()(int row, int col) const {
				return m[row][col];
			}

			T& operator()(int row, int col) {
				return m[row][col];
			}




			friend std::ostream & operator<<(std::ostream& os, const Mat3x3& mat) {
				os << "[" << Math::ToZero(mat(0, 0)) << ", " << Math::ToZero(mat(0, 1)) << ", " << Math::ToZero(mat(0, 2)) << ", " <<  std::endl;
				os << Math::ToZero(mat(1, 0)) << ", " << Math::ToZero(mat(1, 1)) << ", " << Math::ToZero(mat(1, 2)) << ", " <<  std::endl;
				os << Math::ToZero(mat(2, 0)) << ", " << Math::ToZero(mat(2, 1)) << ", " << Math::ToZero(mat(2, 2)) << ", " << std::endl<< "]";
				return os;
			}

		private:
			//��Ϊ������
			Vector<3, T> m[3];
		};
	}
	using Mat3f = YGM::Mat3x3<float>;
}


#endif