﻿#if !GM_USE_DX_MATH

#ifndef XMISNAN
static_assert(sizeof(unsigned int) == 4, "Wrong unsigned int size.");
#define XMISNAN(x)  ((*(unsigned int*)&(x) & 0x7F800000) == 0x7F800000 && (*(unsigned int*)&(x) & 0x7FFFFF) != 0)
#endif

namespace glm
{
	inline vec4 combine_vec4(const vec3& v3, const vec4& v4)
	{
		return vec4(v3, v4[3]);
	}

	// identity
	template <typename T>
	inline T& identity();

	template <>
	inline mat4& identity()
	{
		static mat4 m(1.f);
		return m;
	}

	template <>
	inline mat3& identity()
	{
		static mat3 m(1.f);
		return m;
	}

	template <>
	inline mat2& identity()
	{
		static mat2 m(1.f);
		return m;
	}

	template <>
	inline glm::quat& identity()
	{
		static glm::quat q(1, 0, 0, 0);
		return q;
	}

	//transform
	inline mat4 scale(const vec3& v)
	{
		return scale(identity<mat4>(), v);
	}

	inline mat4 scale(gm::GMfloat x, gm::GMfloat y, gm::GMfloat z)
	{
		return scale(identity<mat4>(), vec3(x, y, z));
	}

	inline mat4 translate(const vec3& v)
	{
		return translate(identity<mat4>(), v);
	}

	inline vec3 inhomogeneous(const vec4& v4)
	{
		return vec3(v4.x / v4.w, v4.y / v4.w, v4.z / v4.w);
	}

	inline vec3 make_vec3(const gm::GMfloat(&v)[3])
	{
		return make_vec3(static_cast<const gm::GMfloat*>(v));
	}

	template <typename T>
	inline gm::GMfloat lengthSq(const T& left)
	{
		return dot(left, left);
	}

	template <typename T>
	inline T safeNormalize(const T& vec)
	{
		gm::GMfloat l2 = glm::length2(vec);
		if (l2 >= FLT_EPSILON*FLT_EPSILON)
		{
			return vec / Sqrt(l2);
		}
		else
		{
			return T(0);
		}
	}

	// 以屏幕向下方向为y轴正方向来点来计算unproject，和glm::unproject的y轴方向相反
	template<typename T, typename U, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> unprojectScreenCoordDirectionZO(vec<3, T, Q> const& win, mat<4, 4, T, Q> const& model, mat<4, 4, T, Q> const& proj, vec<4, U, Q> const& viewport)
	{
		mat<4, 4, T, Q> Inverse = inverse(proj * model);

		vec<4, T, Q> tmp = vec<4, T, Q>(win, T(1));
		tmp.x = (tmp.x - T(viewport[0])) / T(viewport[2]);
		tmp.y = -(tmp.y - T(viewport[1])) / T(viewport[3]);
		tmp.x = tmp.x * static_cast<T>(2) - static_cast<T>(1);
		tmp.y = tmp.y * static_cast<T>(2) + static_cast<T>(1);

		vec<4, T, Q> obj = Inverse * tmp;
		obj /= obj.w;

		return vec<3, T, Q>(obj);
	}

	inline vec4 reciprocal(const vec4& V)
	{
		vec4 result;
		result[0] = 0.0f;

		for (auto i = 0; i < 4; i++)
		{
			if (XMISNAN(V[i]))
			{
				reinterpret_cast<gm::GMuint32&>(result[i]) = 0x7FC00000;
			}
			else if (V[i] == 0.0f || V[i] == -0.0f)
			{
				reinterpret_cast<gm::GMuint32&>(result[i]) = 0x7F800000 | (reinterpret_cast<const gm::GMuint32&>(V[i]) & 0x80000000);
			}
			else
			{
				result[i] = 1.f / V[i];
			}
		}
		return result;
	}

	inline vec3 minComponent(const vec3& V1, const vec3& V2)
	{
		vec4 result;
		result[0] = (V1[0] < V2[0]) ? V1[0] : V2[0];
		result[1] = (V1[1] < V2[1]) ? V1[1] : V2[1];
		result[2] = (V1[2] < V2[2]) ? V1[2] : V2[2];
		return result;
	}

	inline vec3 maxComponent(const vec3& V1, const vec3& V2)
	{
		vec4 result;
		result[0] = (V1[0] > V2[0]) ? V1[0] : V2[0];
		result[1] = (V1[1] > V2[1]) ? V1[1] : V2[1];
		result[2] = (V1[2] > V2[2]) ? V1[2] : V2[2];
		return result;
	}

	inline vec4 minComponent(const vec4& V1, const vec4& V2)
	{
		vec4 result;
		result[0] = (V1[0] < V2[0]) ? V1[0] : V2[0];
		result[1] = (V1[1] < V2[1]) ? V1[1] : V2[1];
		result[2] = (V1[2] < V2[2]) ? V1[2] : V2[2];
		result[3] = (V1[3] < V2[3]) ? V1[3] : V2[3];
		return result;
	}

	inline vec4 maxComponent(const vec4& V1, const vec4& V2)
	{
		vec4 result;
		result[0] = (V1[0] > V2[0]) ? V1[0] : V2[0];
		result[1] = (V1[1] > V2[1]) ? V1[1] : V2[1];
		result[2] = (V1[2] > V2[2]) ? V1[2] : V2[2];
		result[3] = (V1[3] > V2[3]) ? V1[3] : V2[3];
		return result;
	}
}
#endif

const GMQuat g_GMQuat_identity = Identity<GMQuat>();

inline GMVec3::GMVec3(const GMVec4& V)
{
#if GM_USE_DX_MATH
	v_ = V.v_;
#else
	v_ = glm::vec3(V.v_[0], V.v_[1], V.v_[2]);
#endif
}

inline GMQuat::GMQuat(const GMVec3& U, const GMVec3& V)
#if !GM_USE_DX_MATH
	:v_(U.v_, V.v_)
#endif
{
#if GM_USE_DX_MATH
	gm::GMfloat norm_u_norm_v = Sqrt(Dot(U, U) * Dot(V, V));
	gm::GMfloat real_part = norm_u_norm_v + Dot(U, V);
	GMVec3 t;
	if (real_part < static_cast<gm::GMfloat>(1.e-6f) * norm_u_norm_v)
	{
		// If u and v are exactly opposite, rotate 180 degrees
		// around an arbitrary orthogonal axis. Axis normalisation
		// can happen later, when we normalise the quaternion.
		real_part = 0.f;
		t = Fabs(U.getX()) > Fabs(U.getZ()) ? GMVec3(-U.getY(), U.getX(), 0.f) : GMVec3(0.f, -U.getZ(), U.getY());
	}
	else
	{
		// Otherwise, build quaternion the standard way.
		t = Cross(U, V);
	}

	GMQuat Q = Normalize(GMQuat(t.getX(), t.getY(), t.getZ(), real_part));
	v_ = Q.v_;
#endif
}

template <>
inline GMVec2 Zero()
{
	GMVec2 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::XMVectorZero();
#else
	V.v_ = glm::zero<glm::vec2>();
#endif
	return V;
}

template <>
inline GMVec3 Zero()
{
	GMVec3 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::XMVectorZero();
#else
	V.v_ = glm::zero<glm::vec3>();
#endif
	return V;
}

template <>
inline GMVec4 Zero()
{
	GMVec4 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::XMVectorZero();
#else
	V.v_ = glm::zero<glm::vec4>();
#endif
	return V;
}

template <>
inline GMMat4 Zero()
{
	GMMat4 V;
#if GM_USE_DX_MATH
	V.v_.r[0] = DirectX::g_XMZero;
	V.v_.r[1] = DirectX::g_XMZero;
	V.v_.r[2] = DirectX::g_XMZero;
	V.v_.r[3] = DirectX::g_XMZero;
#else
	V.v_ = glm::zero<glm::mat4>();
#endif
	return V;
}

inline GMMat4 __getIdentityMat4()
{
	GMMat4 v;
#if GM_USE_DX_MATH
	v.v_ = DirectX::XMMatrixIdentity();
#else
	v.v_ = glm::identity<glm::mat4>();
#endif
	return v;
}

inline GMQuat __getIdentityQuat()
{
	GMQuat v;
#if GM_USE_DX_MATH
	v.v_ = DirectX::XMQuaternionIdentity();
#else
	v.v_ = glm::identity<glm::quat>();
#endif
	return v;
}

inline GMVec2 operator-(const GMVec2& V)
{
	GMVec2 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::operator-(V.v_);
#else
	R.v_ = -V.v_;
#endif
	return R;
}

inline GMVec3 operator-(const GMVec3& V)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::operator-(V.v_);
#else
	R.v_ = -V.v_;
#endif
	return R;
}

inline GMVec4 operator-(const GMVec4& V)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::operator-(V.v_);
#else
	R.v_ = -V.v_;
#endif
	return R;
}

inline GMVec2 operator+(const GMVec2& V1, const GMVec2& V2)
{
	GMVec2 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator+(V1.v_, V2.v_);
#else
	V.v_ = V1.v_ + V2.v_;
#endif
	return V;
}

inline GMVec2 operator*(const GMVec2& V1, gm::GMfloat S)
{
	GMVec2 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator*(V1.v_, S);
#else
	V.v_ = V1.v_ * S;
#endif
	return V;
}

inline GMVec2 operator/(const GMVec2& V1, gm::GMfloat S)
{
	GMVec2 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator/(V1.v_, S);
#else
	V.v_ = V1.v_ / S;
#endif
	return V;
}

inline GMVec2 operator-(const GMVec2& V1, const GMVec2& V2)
{
	GMVec2 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator-(V1.v_, V2.v_);
#else
	V.v_ = V1.v_ - V2.v_;
#endif
	return V;
}

inline GMVec3 operator+(const GMVec3& V1, const GMVec3& V2)
{
	GMVec3 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator+(V1.v_, V2.v_);
#else
	V.v_ = V1.v_ + V2.v_;
#endif
	return V;
}

inline GMVec3& operator+=(GMVec3& V1, const GMVec3& V2)
{
#if GM_USE_DX_MATH
	DirectX::operator+=(V1.v_, V2.v_);
#else
	V1 = V1 + V2;
#endif
	return V1;
}

inline GMVec3 operator-(const GMVec3& V1, const GMVec3& V2)
{
	GMVec3 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator-(V1.v_, V2.v_);
#else
	V.v_ = V1.v_ - V2.v_;
#endif
	return V;
}

inline GMVec3& operator-=(GMVec3& V1, const GMVec3& V2)
{
#if GM_USE_DX_MATH
	DirectX::operator-=(V1.v_, V2.v_);
#else
	V1 = V1 - V2;
#endif
	return V1;
}

inline GMVec3 operator*(const GMVec3& V1, gm::GMfloat S)
{
	GMVec3 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator*(V1.v_, S);
#else
	V.v_ = V1.v_ * S;
#endif
	return V;
}

inline GMVec3& operator*=(GMVec3& V1, gm::GMfloat S)
{
#if GM_USE_DX_MATH
	DirectX::operator*=(V1.v_, S);
#else
	V1 = V1 * S;
#endif
	return V1;
}

inline GMVec3& operator*=(GMVec3& V1, const GMVec3& V2)
{
	V1 = V1 * V2;
	return V1;
}

inline GMVec3 operator/(const GMVec3& V1, gm::GMfloat S)
{
	GMVec3 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator/(V1.v_, S);
#else
	V.v_ = V1.v_ / S;
#endif
	return V;
}

inline GMVec3& operator/=(GMVec3& V1, gm::GMfloat S)
{
#if GM_USE_DX_MATH
	DirectX::operator/=(V1.v_, S);
#else
	V1 = V1 / S;
#endif
	return V1;
}

inline GMVec4 operator/(const GMVec4& V1, gm::GMfloat S)
{
	GMVec4 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator/(V1.v_, S);
#else
	V.v_ = V1.v_ / S;
#endif
	return V;
}

inline GMVec4 operator+(const GMVec4& V1, const GMVec4& V2)
{
	GMVec4 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator+(V1.v_, V2.v_);
#else
	V.v_ = V1.v_ + V2.v_;
#endif
	return V;
}

inline GMVec4 operator-(const GMVec4& V1, const GMVec4& V2)
{
	GMVec4 V;
#if GM_USE_DX_MATH
	V.v_ = DirectX::operator-(V1.v_, V2.v_);
#else
	V.v_ = V1.v_ - V2.v_;
#endif
	return V;
}

inline GMVec4 operator*(const GMVec4& V, gm::GMfloat S)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::operator*(V.v_, S);
#else
	R.v_ = V.v_ * S;
#endif
	return R;
}

inline GMMat4 operator+(const GMMat4& M1, const GMMat4& M2)
{
	GMMat4 R;
	R.v_ = M1.v_ + M2.v_;
	return R;
}

inline GMMat4 operator+=(GMMat4& M1, const GMMat4& M2)
{
	M1 = M1 + M2;
	return M1;
}

inline bool operator==(const GMVec2& V1, const GMVec2& V2)
{
#if GM_USE_DX_MATH
	return DirectX::XMVector2Equal(V1.v_, V2.v_);
#else
	return V1.v_ == V2.v_;
#endif
}

inline bool operator==(const GMVec3& V1, const GMVec3& V2)
{
#if GM_USE_DX_MATH
	return DirectX::XMVector2Equal(V1.v_, V2.v_);
#else
	return V1.v_ == V2.v_;
#endif
}

inline bool operator==(const GMVec4& V1, const GMVec4& V2)
{
#if GM_USE_DX_MATH
	return DirectX::XMVector4Equal(V1.v_, V2.v_);
#else
	return V1.v_ == V2.v_;
#endif
}

inline bool operator==(const GMQuat& Q1, const GMQuat& Q2)
{
#if GM_USE_DX_MATH
	return DirectX::XMVector4Equal(Q1.v_, Q2.v_);
#else
	return Q1.v_ == Q2.v_;
#endif
}

inline GMMat4 operator*(const GMMat4& M1, const GMMat4& M2)
{
	GMMat4 R;
#if GM_USE_DX_MATH
	R.v_ = M1.v_.operator*(M2.v_);
#else
	// opengl为列优先，为了计算先M1变换，再M2变换，应该用M2 * M1
	R.v_ = M2.v_ * M1.v_;
#endif
	return R;
}

inline bool operator!=(const GMVec2& V1, const GMVec2& V2)
{
	return !(V1 == V2);
}

inline bool operator!=(const GMVec3& V1, const GMVec3& V2)
{
	return !(V1 == V2);
}

inline bool operator!=(const GMVec4& V1, const GMVec4& V2)
{
	return !(V1 == V2);
}

inline bool operator!=(const GMQuat& Q1, const GMQuat& Q2)
{
	return !(Q1 == Q2);
}

inline GMVec3 operator*(const GMVec3& V1, const GMVec3& V2)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::operator*(V1.v_, V2.v_);
#else
	R.v_ = V1.v_ * V2.v_;
#endif
	return R;
}

inline GMVec4 operator*(const GMVec4& V1, const GMVec4& V2)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::operator*(V1.v_, V2.v_);
#else
	R.v_ = V1.v_ * V2.v_;
#endif
	return R;
}

inline GMVec4 operator*(const GMVec4& V, const GMMat4& M)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVector4Transform(V.v_, M.v_);
#else
	R.v_ = M.v_ * V.v_;
#endif
	return R;
}

inline GMVec4 operator*(const GMVec4& V, const GMQuat& Q)
{
#if GM_USE_DX_MATH
	return V * QuatToMatrix(Q);
#else
	GMVec4 R;
	R.v_ = Q.v_ * V.v_;
	return R;
#endif
}

inline GMVec3 operator*(const GMVec3& V, const GMQuat& Q)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVector3Transform(V.v_, QuatToMatrix(Q).v_);
#else
	R.v_ = Q.v_ * V.v_;
#endif
	return R;
}

inline GMQuat operator*(const GMQuat& V, const GMQuat& Q)
{
	GMQuat R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMQuaternionMultiply(V.v_, Q.v_);
#else
	R.v_ = Q.v_ * V.v_;
#endif
	return R;
}

inline GMMat4 operator*(gm::GMfloat V, const GMMat4& M)
{
	GMMat4 R;
	R.v_ = V * M.v_;
	return R;
}

inline GMMat4 QuatToMatrix(const GMQuat& quat)
{
	GMMat4 mat;
#if GM_USE_DX_MATH
	mat.v_ = DirectX::XMMatrixRotationQuaternion(quat.v_);
#else
	mat.v_ = glm::mat4_cast(quat.v_);
#endif
	return mat;
}

inline GMMat4 LookAt(const GMVec3& position, const GMVec3& center, const GMVec3& up)
{
	GMMat4 mat;
#if GM_USE_DX_MATH
	mat.v_ = DirectX::XMMatrixLookAtLH(position.v_, center.v_, up.v_);
#else
	mat.v_ = glm::lookAt(position.v_, center.v_, up.v_);
#endif
	return mat;
}

inline gm::GMfloat Dot(const GMVec2& V1, const GMVec2& V2)
{
#if GM_USE_DX_MATH
	return DirectX::XMVectorGetX(DirectX::XMVector2Dot(V1.v_, V2.v_));
#else
	return glm::dot(V1.v_, V2.v_);
#endif
}

inline gm::GMfloat Dot(const GMVec3& V1, const GMVec3& V2)
{
#if GM_USE_DX_MATH
	return DirectX::XMVectorGetX(DirectX::XMVector3Dot(V1.v_, V2.v_));
#else
	return glm::dot(V1.v_, V2.v_);
#endif
}

inline gm::GMfloat Dot(const GMVec4& V1, const GMVec4& V2)
{
#if GM_USE_DX_MATH
	return DirectX::XMVectorGetX(DirectX::XMVector4Dot(V1.v_, V2.v_));
#else
	return glm::dot(V1.v_, V2.v_);
#endif
}

inline GMVec3 Normalize(const GMVec3& V)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVector3Normalize(V.v_);
#else
	R.v_ = glm::safeNormalize(V.v_);
#endif
	return R;
}

inline GMVec4 Normalize(const GMVec4& V)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVector4Normalize(V.v_);
#else
	R.v_ = glm::safeNormalize(V.v_);
#endif
	return R;
}

inline GMVec4 PlaneNormalize(const GMVec4& V)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMPlaneNormalize(V.v_);
#else
	GMVec3 N = V;
	gm::GMfloat Len = Length(N);
	R = V / Len;
#endif
	return R;
}

inline GMQuat Normalize(const GMQuat& Q)
{
	GMQuat R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMQuaternionNormalize(Q.v_);
#else
	R.v_ = glm::normalize(Q.v_);
#endif
	return R;
}

inline GMVec3 FastNormalize(const GMVec3& V)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVector3Normalize(V.v_);
#else
	R.v_ = glm::fastNormalize(V.v_);
#endif
	return R;
}

inline GMVec3 MakeVector3(const gm::GMfloat* f)
{
	return GMVec3(f[0], f[1], f[2]);
}

inline GMVec3 MakeVector3(const GMVec4& V)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = V.v_;
#else
	R.v_ = glm::make_vec3(V.v_);
#endif
	return R;
}

inline GMVec4 CombineVector4(const GMVec3& V1, const GMVec4& V2)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = V1.v_;
	R.setW(V2.getW());
#else
	R.v_ = glm::combine_vec4(V1.v_, V2.v_);
#endif
	return R;
}

inline GMMat4 Translate(const GMVec3& V)
{
	GMMat4 M;
#if GM_USE_DX_MATH
	M.v_ = DirectX::XMMatrixTranslationFromVector(V.v_);
#else
	M.v_ = glm::translate(V.v_);
#endif
	return M;
}

inline GMMat4 Translate(const GMVec4& V)
{
	GMMat4 M;
#if GM_USE_DX_MATH
	M.v_ = DirectX::XMMatrixTranslationFromVector(V.v_);
#else
	M.v_ = glm::translate(V.v_);
#endif
	return M;
}

inline GMMat4 Scale(const GMVec3& V)
{
	GMMat4 M;
#if GM_USE_DX_MATH
	M.v_ = DirectX::XMMatrixScalingFromVector(V.v_);
#else
	M.v_ = glm::scale(V.v_);
#endif
	return M;
}

inline GMQuat Rotate(gm::GMfloat Angle, const GMVec3& Axis)
{
	return Rotate(g_GMQuat_identity, Angle, Axis);
}

inline GMQuat Rotate(const GMQuat& Start, gm::GMfloat Angle, const GMVec3& Axis)
{
	GMQuat Q;
#if GM_USE_DX_MATH
	Q.v_ = DirectX::XMQuaternionMultiply(Start.v_, DirectX::XMQuaternionRotationAxis(Axis.v_, Angle));

#else
	Q.v_ = glm::rotate(Start.v_, Angle, Axis.v_);
#endif
	return Q;
}

inline void GetTranslationFromMatrix(const GMMat4& M, GMFloat4& F)
{
	GMFloat16 f16;
	M.loadFloat16(f16);
	F[0] = f16[3][0];
	F[1] = f16[3][1];
	F[2] = f16[3][2];
	F[3] = 1.f;
}

inline void GetScalingFromMatrix(const GMMat4& M, GMFloat4& F)
{
	GMFloat16 f16;
	M.loadFloat16(f16);
	F[0] = f16[0][0];
	F[1] = f16[1][1];
	F[2] = f16[2][2];
	F[3] = 1.f;
}

inline void GetTranslationAndScalingFromMatrix(const GMMat4& M, GMFloat4& T, GMFloat4& S)
{
	GMFloat16 f16;
	M.loadFloat16(f16);
	T[0] = f16[3][0];
	T[1] = f16[3][1];
	T[2] = f16[3][2];
	T[3] = 1.f;
	S[0] = f16[0][0];
	S[1] = f16[1][1];
	S[2] = f16[2][2];
	S[3] = 1.f;
}

inline GMMat4 Ortho(gm::GMfloat left, gm::GMfloat right, gm::GMfloat bottom, gm::GMfloat top, gm::GMfloat n, gm::GMfloat f)
{
	GMMat4 M;
#if GM_USE_DX_MATH
	M.v_ = DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, n, f);
#else
	M.v_ = glm::ortho(left, right, bottom, top, n, f);
#endif
	return M;
}

inline gm::GMfloat Length(const GMVec3& V)
{
#if GM_USE_DX_MATH
	GMFloat4 f;
	DirectX::XMStoreFloat4(&f.v_, DirectX::XMVector3Length(V.v_));
	return f[0];
#else
	return glm::length(V.v_);
#endif
}

inline gm::GMfloat Length(const GMVec4& V)
{
#if GM_USE_DX_MATH
	return DirectX::XMVectorGetX(DirectX::XMVector4Length(V.v_));
#else
	return glm::length(V.v_);
#endif
}

inline gm::GMfloat LengthSq(const GMVec3& V)
{
#if GM_USE_DX_MATH
	return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(V.v_));
#else
	return glm::lengthSq(V.v_);
#endif
}

inline GMVec3 Cross(const GMVec3& V1, const GMVec3& V2)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVector3Cross(V1.v_, V2.v_);
#else
	R.v_ = glm::cross(V1.v_, V2.v_);
#endif
	return R;
}

inline GMMat4 Perspective(gm::GMfloat fovy, gm::GMfloat aspect, gm::GMfloat n, gm::GMfloat f)
{
	GMMat4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, n, f);
#else
	R.v_ = glm::perspective(fovy, aspect, n, f);
#endif
	return R;
}

inline GMMat4 Transpose(const GMMat4& M)
{
	GMMat4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMMatrixTranspose(M.v_);
#else
	R.v_ = glm::transpose(M.v_);
#endif
	return R;
}

inline GMMat4 Inverse(const GMMat4& M)
{
	GMMat4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMMatrixInverse(nullptr, M.v_);
#else
	R.v_ = glm::inverse(M.v_);
#endif
	return R;
}

inline GMMat4 InverseTranspose(const GMMat4& M)
{
#if GM_USE_DX_MATH
	return Transpose(Inverse(M));
#else
	GMMat4 R;
	R.v_ = glm::inverseTranspose(M.v_);
	return R;
#endif
}

inline GMQuat Lerp(const GMQuat& Q1, const GMQuat& Q2, gm::GMfloat T)
{
	GMQuat R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMQuaternionSlerp(Q1.v_, Q2.v_, T);
#else
	R.v_ = glm::slerp(Q1.v_, Q2.v_, T);
#endif
	return R;
}

inline GMQuat RotationTo(const GMVec3& src, const GMVec3& dest, const GMVec3& fallbackAxis)
{
	// 采用Orge的实现

	const static GMQuat s_identity = Identity<GMQuat>();
	GMQuat q;

	gm::GMfloat d = Dot(src, dest);
	// If dot == 1, vectors are the same
	if (d >= 1.0f)
	{
		return s_identity;
	}
	if (d < (1e-6f - 1.0f))
	{
		if (fallbackAxis != Zero<GMVec3>())
		{
			// rotate 180 degrees about the fallback axis
			q = Rotate(Radians(PI), fallbackAxis);
		}
		else
		{
			// Generate an axis
			GMVec3 axis = Cross(GMVec3(1, 0, 0), src);
			if (Length(axis) == 0) // pick another if colinear
				axis = Cross(GMVec3(0, 1, 0), src);
			axis = Normalize(axis);
			q = Rotate(Radians(PI), axis);
		}
	}
	else
	{
		gm::GMfloat s = Sqrt((1 + d) * 2);
		gm::GMfloat invs = 1 / s;
		GMVec3 c = Cross(src, dest);
		q.setX(c.getX() * invs);
		q.setY(c.getY() * invs);
		q.setZ(c.getZ() * invs);
		q.setW(s * 0.5f);
		q = Normalize(q);
	}
	return q;
}

inline GMVec3 Unproject(
	const GMVec3& V,
	gm::GMfloat ViewportX,
	gm::GMfloat ViewportY,
	gm::GMfloat ViewportWidth,
	gm::GMfloat ViewportHeight,
	const GMMat4& Projection,
	const GMMat4& View,
	const GMMat4& World
)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = XMVector3Unproject(
		V.v_,
		ViewportX,
		ViewportY,
		ViewportWidth,
		ViewportHeight,
		0,
		1,
		Projection.v_,
		View.v_,
		World.v_
	);
#else
	glm::vec4 viewport(ViewportX, ViewportY, ViewportWidth, ViewportHeight);
	R.v_ = glm::unprojectScreenCoordDirectionZO(
		V.v_,
		(World * View).v_,
		Projection.v_,
		viewport
	);
#endif
	return R;
}

inline void GetFrustumPlanesFromProjectionViewModelMatrix(
	gm::GMfloat FarZ,
	gm::GMfloat NearZ,
	const GMMat4& ProjectionViewModelMatrix,
	GMVec4& FarPlane,
	GMVec4& NearPlane,
	GMVec4& RightPlane,
	GMVec4& LeftPlane,
	GMVec4& TopPlane,
	GMVec4& BottomPlane
)
{
	GMFloat16 M;
	ProjectionViewModelMatrix.loadFloat16(M);

	LeftPlane = GMVec4(
		M[0][3] + M[0][0],
		M[1][3] + M[1][0],
		M[2][3] + M[2][0],
		M[3][3] + M[3][0]
	);
	LeftPlane = PlaneNormalize(LeftPlane);

	RightPlane = GMVec4(
		M[0][3] - M[0][0],
		M[1][3] - M[1][0],
		M[2][3] - M[2][0],
		M[3][3] - M[3][0]
	);
	RightPlane = PlaneNormalize(RightPlane);

	BottomPlane = GMVec4(
		M[0][3] + M[0][1],
		M[1][3] + M[1][1],
		M[2][3] + M[2][1],
		M[3][3] + M[3][1]
	);
	BottomPlane = PlaneNormalize(BottomPlane);

	TopPlane = GMVec4(
		M[0][3] - M[0][1],
		M[1][3] - M[1][1],
		M[2][3] - M[2][1],
		M[3][3] - M[3][1]
	);
	TopPlane = PlaneNormalize(TopPlane);

	NearPlane = GMVec4(
		-(NearZ * M[0][3] - M[0][2]),
		-(NearZ * M[1][3] - M[1][2]),
		-(NearZ * M[2][3] - M[2][2]),
		-(NearZ * M[3][3] - M[3][2])
	);
	NearPlane = PlaneNormalize(NearPlane);

	FarPlane = GMVec4(
		FarZ * M[0][3] - M[0][2],
		FarZ * M[1][3] - M[1][2],
		FarZ * M[2][3] - M[2][2],
		FarZ * M[3][3] - M[3][2]
	);
	FarPlane = PlaneNormalize(FarPlane);
}

template <typename T>
inline T Lerp(const T& S, const T& E, gm::GMfloat P)
{
	return (E - S) * P + S;
}

inline GMVec3 Inhomogeneous(const GMVec4& V)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = V.v_;
	DirectX::operator/=(R.v_, V.getW());
#else
	R.v_ = glm::inhomogeneous(V.v_);
#endif
	return R;
}

inline GMMat4 Inhomogeneous(const GMMat4& M)
{
	GMMat4 R(M);
	GMFloat16 f16;
	R.loadFloat16(f16);
	f16[0][3] =
	f16[1][3] =
	f16[2][3] =
	f16[3][0] =
	f16[3][1] =
	f16[3][2] = 0.f;
	f16[3][3] = 1.f;
	R.setFloat16(f16);
	return R;
}

inline void CopyToArray(const GMVec3& V, gm::GMfloat* array)
{
	GMFloat4 f4;
	V.loadFloat4(f4);
	array[0] = f4[0];
	array[1] = f4[1];
	array[2] = f4[2];
}

inline void CopyToArray(const GMVec4& V, gm::GMfloat* array)
{
	GMFloat4 f4;
	V.loadFloat4(f4);
	array[0] = f4[0];
	array[1] = f4[1];
	array[2] = f4[2];
	array[3] = f4[3];
}

#if GM_USE_DX_MATH
template <typename T>
inline gm::GMfloat* ValuePointer(const T& data)
{
	return (gm::GMfloat*)(&data.v_);
}

template <>
inline gm::GMfloat* ValuePointer(const GMMat4& data)
{
	return (gm::GMfloat*)(&(data.v_));
}
#else
template <typename T>
inline gm::GMfloat* ValuePointer(const T& data)
{
	return (gm::GMfloat*)(&data.v_.x);
}

template <>
inline gm::GMfloat* ValuePointer(const GMMat4& data)
{
	return (gm::GMfloat*)(&data.v_[0]);
}
#endif

template <>
inline gm::GMfloat* ValuePointer(const GMFloat4& data)
{
	return (gm::GMfloat*)(&(data[0]));
}

inline GMVec4 SplatX(const GMVec4& V)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVectorSplatX(V.v_);
#else
	auto v = V.getX();
	R.v_ = glm::vec4(v, v, v, v);
#endif
	return R;
}

inline GMVec4 SplatY(const GMVec4& V)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVectorSplatY(V.v_);
#else
	auto v = V.getY();
	R.v_ = glm::vec4(v, v, v, v);
#endif
	return R;
}

inline GMVec4 SplatZ(const GMVec4& V)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVectorSplatZ(V.v_);
#else
	auto v = V.getZ();
	R.v_ = glm::vec4(v, v, v, v);
#endif
	return R;
}

inline GMVec4 SplatW(const GMVec4& V)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVectorSplatW(V.v_);
#else
	auto v = V.getW();
	R.v_ = glm::vec4(v, v, v, v);
#endif
	return R;
}

inline GMVec4 Reciprocal(const GMVec4& V)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVectorReciprocal(V.v_);
#else
	R.v_ = glm::reciprocal(V.v_);
#endif
	return R;
}

inline GMVec4 MinComponent(const GMVec4& V1, const GMVec4& V2)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVectorMin(V1.v_, V2.v_);
#else
	R.v_ = glm::minComponent(V1.v_, V2.v_);
#endif
	return R;
}

inline GMVec4 MaxComponent(const GMVec4& V1, const GMVec4& V2)
{
	GMVec4 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVectorMax(V1.v_, V2.v_);
#else
	R.v_ = glm::maxComponent(V1.v_, V2.v_);
#endif
	return R;
}

inline GMVec3 MinComponent(const GMVec3& V1, const GMVec3& V2)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVectorMin(V1.v_, V2.v_);
#else
	R.v_ = glm::minComponent(V1.v_, V2.v_);
#endif
	return R;
}

inline GMVec3 MaxComponent(const GMVec3& V1, const GMVec3& V2)
{
	GMVec3 R;
#if GM_USE_DX_MATH
	R.v_ = DirectX::XMVectorMax(V1.v_, V2.v_);
#else
	R.v_ = glm::maxComponent(V1.v_, V2.v_);
#endif
	return R;
}