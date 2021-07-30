#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "solver.h"

namespace My
{
#define CLAMP(a, b, c)	((a<b)?b:((a>c)?c:a))

	template <typename T, int SIZE>
	class CameraT : public MatrixT<T, SIZE>
	{
		typedef	MatrixT<T, SIZE>	Matrix;
		typedef	MatrixT<T, 4>	Matrix4;
	protected:
		Matrix4		mPT[2];//projection matrix + it's inverse
		T mViewAngle, mAspect, mNearDist, mFarDist;
	public:
		CameraT()
			: mViewAngle(T(53)), mAspect(T(64) / T(48)), mNearDist(T(1)), mFarDist(T(2))
		{
			loadIdentity();
			updatePerspective();
		}
		CameraT(T viewAngle, T aspect, T nearDist, T farDist)
			: mViewAngle(viewAngle), mAspect(aspect), mNearDist(nearDist), mFarDist(farDist)
		{
			loadIdentity();
			updatePerspective();
		}
		T aspectRatio(){ return mAspect; }
		T nearDist(){ return mNearDist; }
		void setNearDist(T a)
		{
			mNearDist = CLAMP(a, 0.1f, 100);
			updatePerspective();
		}
		T viewAngle(){ return mViewAngle; }
		void setViewAngle(T a)
		{
			mViewAngle = CLAMP(a, 0, 180);
			updatePerspective();
		}
		void set(Point3f eye, Point3f lookAt, Vec3f up)
		{
			Vec3f n(eye - lookAt);
			Vec3f u(up.cross(n));
			n.normalize();
			u.normalize();
			Vec3f v(n.cross(u));
			Vec3f eVec(eye-Point3f(0));
			m[0][0] = u[0], m[0][1] = u[1], m[0][2] = u[2], m[0][3] = -eVec.dot(u);
			m[1][0] = v[0], m[1][1] = v[1], m[1][2] = v[2], m[1][3] = -eVec.dot(v);
			m[2][0] = n[0], m[2][1] = n[1], m[2][2] = n[2], m[2][3] = -eVec.dot(n);
			m[3][0] = 0, m[3][1] = 0, m[3][2] = 0, m[3][3] = 1;
		}

		void slide(T delU, T delV, T delN)
		{
			Matrix t;
			t.loadIdentity();
			Matrix &mw(*this);
			t.translate(Scene::Vector(delU, delV, delN, 0));
			mw = t*mw;
		}
		void roll(T angle)
		{
			Matrix t;
			t.loadIdentity();
			Matrix &mw(*this);
			t.rotate(angle, 2);
			mw = t*mw;
		}
		void yaw(T angle)
		{
			Matrix t;
			t.loadIdentity();
			Matrix &mw(*this);
			t.rotate(angle, 1);
			mw = t*mw;
		}
		void pitch(T angle)
		{
			Matrix t;
			t.loadIdentity();
			Matrix &mw(*this);
			t.rotate(angle, 0);
			mw = t*mw;
		}
		void e2p(const Scene::Vertex &a, Scene::Vertex &b)//eye -> canonical
		{
			b = mPT[0]*a;
		}
		void p2e(const Scene::Vertex &a, Scene::Vertex &b)//canonical -> eye
		{
			b = mPT[1]*a;
		}
		void c2w(const Scene::Vertex &a, Scene::Vertex &b)//camera -> world
		{
			Matrix &m(*this);
			Matrix r(inverse(m));
			b = r*a;
		}
	private:
		void updatePerspective()
		{
			float N(mNearDist);
			float F(mFarDist);
			float top(float(N*tan(DEG2RAD(mViewAngle/2))));
			float bott(-top);
			float right(top*mAspect);
			float left(-right);
			Matrix4 &a(mPT[0]);
			a.clear();
			a[0][0] = (2 * N) / (right - left);
			a[0][2] = (right + left) / (right - left);
			a[1][1] = (2 * N) / (top - bott);
			a[1][2] = (top + bott) / (top - bott);
			a[2][2] = -(F + N) / (F - N);
			a[2][3] = -(2 * F*N) / (F - N);
			a[3][2] = -1;

			mPT[1] = inverse(a);
		}
		void updateOrhto()
		{
			mPT.clear();
		}
	};

	class Camera4f : public CameraT<float, 4>
	{
	public:
		Camera4f(){}
		Camera4f(const Rectf &w)
		{
			mAspect = w.ratio<float>();
			mViewAngle = RAD2DEG(atan((w.height()/2)/(-mNearDist)));//radians
		}
		Camera4f(float viewAngle, float aspect, float nearDist, float farDist)
			: CameraT<float, 4>(viewAngle, aspect, nearDist, farDist)
		{
			loadIdentity();
		}
		/*Rectf worldView()
		{
			float h2(tan(DEG2RAD(viewAngle/2))*(m[2][3]+(-nearDist)));
			float w2(h2*aspect);
			Rectf w(Pointf(-w2, -h2), Pointf(w2, h2));
			w.translate(m[0][size()-1], m[1][size()-1]);
			return w;
		}*/
		const MatrixT<float, 4> &projectionMatrix()
		{
			return mPT[0];
		}
	};

}//namespace

#endif//__CAMERA_H__
