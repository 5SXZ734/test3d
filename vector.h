#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <algorithm>
#include <iostream>
#include <vector>
#include <assert.h>

#define DEG2RAD(a)	float(a*M_PI/180)
#define RAD2DEG(a)	float(a*180/M_PI)

#pragma warning (disable:4200)


namespace My
{
	template <typename T, int SIZE> struct PointT;

	template <typename T, int SIZE>
	struct VectorT
	{
		T m[SIZE];
		VectorT(){}
/*		VectorT(const VectorT<T,2> &p)
		{
			m[0] = p[0];
			m[1] = p[1];
			m[2] = 1;
		}
*/		VectorT(T x)
		{
			for (int i(0); i < SIZE; i++)
				m[i] = x;
		}
		VectorT(T x, T y)
		{
			m[0] = x;
			for (int i(1); i < SIZE; i++)
				m[i] = y;
		}
		VectorT(T x, T y, T z)
		{
			m[0] = x;
			m[1] = y;
			for (int i(2); i < SIZE; i++)
				m[i] = z;
		}
		VectorT(T x, T y, T z, T w)
		{
			m[0] = x;
			m[1] = y;
			m[2] = z;
			m[3] = w;
		}
		VectorT(const VectorT<T, SIZE - 1> &o)
		{
			clear();
			for (int i(0); i < SIZE - 1; i++)
				m[i] = o[i];
		}
		void clear()
		{
			for (int i(0); i < SIZE; i++)
				m[i] = 0;
		}
		int size() const { return SIZE; }
		T &operator [](const int i){ return m[i]; }
		const T &operator [](const int i) const { return m[i]; }
		void print(std::ostream &os)
		{
			os << "[";
			for (int i(0); i < SIZE; i++)
			{
				if (i > 0)
					os << ",";
				os << m[i];
			}
			os << "]";
		}
		T length()
		{
			T s(0);
			for (int i(0); i < SIZE; i++)
				s += m[i]*m[i];
			return (T)sqrt(s);
		}
		VectorT perp() const
		{
			VectorT t(*this);
			t[0] = -m[1];
			t[1] = m[0];
			return t;
		}
		T dot(const VectorT &o) const
		{
			VectorT t;
			for (int i(0); i < SIZE; i++)
				t[i] = m[i]*o[i];
			return t.sum();
		}
		VectorT<T, 3> cross(const VectorT<T, 3> &o) const
		{
			VectorT<T, 3> t;
			t[0] = m[1]*o[2]-m[2]*o[1];
			t[1] = m[2]*o[0]-m[0]*o[2];
			t[2] = m[0]*o[1]-m[1]*o[0];
			return t;
		}
		void normalize()
		{
			T d(length());
			for (int i(0); i < SIZE; i++)
				m[i] /= d;
		}
		T sum()
		{
			T s(0);
			for (int i(0); i < SIZE; i++)
				s += m[i];
			return s;
		}
		VectorT operator *(T a) const
		{
			VectorT t;
			for (int i(0); i < SIZE; i++)
				t[i] = m[i]*a;
			return t;
		}
		VectorT operator /(T a) const
		{
			VectorT t;
			for (int i(0); i < SIZE; i++)
				t[i] = m[i]/a;
			return t;
		}
		VectorT operator +(const VectorT &o) const
		{ 
			VectorT t;
			for (int i(0); i < SIZE; i++)
				t[i] = m[i] + o[i];
			return t;
		}
		VectorT operator -(const VectorT &o) const
		{
			VectorT t;
			for (int i(0); i < SIZE; i++)
				t[i] = m[i]-o[i];
			return t;
		}
		VectorT operator-() const
		{
			VectorT t;
			for (int i(0); i < SIZE; i++)
				t[i] = -m[i];
			return t;
		}
		void operator+=(const VectorT &o)
		{
			for (int i(0); i < SIZE; i++)
				m[i] += o[i];
		}
		void restore()
		{
			for (int i(0); i < SIZE; i++)
				m[i] = m[i] / m[SIZE - 1];
		}
	};

	////////////////////////////////PointT


	template <typename T, int SIZE>
	struct PointT
	{
		typedef	T	ValueType;
		static const int Size = SIZE;
		T m[SIZE];
		PointT(){}
		PointT(T x)
		{
			for (int i(0); i < SIZE; i++)
				m[i] = x;
		}
		PointT(T x, T y){ m[0] = x; m[1] = y; }
		PointT(T x, T y, T z){ m[0] = x; m[1] = y; m[2] = z; }
		PointT(T x, T y, T z, T w){ m[0] = x; m[1] = y; m[2] = z; m[3] = w; }
		PointT(const PointT<T, SIZE - 1> &o)
		{
			clear();
			for (int i(0); i < SIZE - 1; i++)
				m[i] = o[i];
		}
		void clear()
		{
			for (int i(0); i < SIZE; i++)
				m[i] = 0;
		}
		bool isValid(){ return (m[SIZE-1] != 0); }
		int size() const { return SIZE; }
		T &operator [](const int i){ return m[i]; }
		const T &operator [](const int i) const { return m[i]; }
		bool operator==(const PointT &o)
		{
			for (int i(0); i < SIZE; i++)
				if (m[i] != o[i])
					return false;
			return true;
		}
		bool operator!=(const PointT &o){ return !(operator==(o)); }
		void print(std::ostream &os)
		{
			os << "[";
			for (int i(0); i < SIZE; i++)
			{
				if (i > 0)
					os << ",";
				os << m[i];
			}
			os << "]";
		}
		void restore()
		{
			for (int i(0); i < SIZE; i++)
				m[i] = m[i] / m[SIZE - 1];
		}
		PointT operator +(const VectorT<T, SIZE> &o) const
		{ 
			PointT t;
			for (int i(0); i < SIZE; i++)
				t[i] = m[i] + o[i];
			return t;
		}
		PointT operator -(const VectorT<T, SIZE> &o) const
		{
			PointT t;
			for (int i(0); i < SIZE; i++)
				t[i] = m[i]-o[i];
			return t;
		}
		VectorT<T, SIZE> operator -(const PointT &o) const
		{
			VectorT<T, SIZE> t;
			for (int i(0); i < SIZE; i++)
				t[i] = m[i]-o[i];
			return t;
		}
		void operator+=(const VectorT<T, SIZE> &o)
		{
			for (int i(0); i < SIZE; i++)
				m[i] += o[i];
		}
		void operator-=(const VectorT<T, SIZE> &o)
		{
			for (int i(0); i < SIZE; i++)
				m[i] -= o[i];
		}
		void translate(T dx, T dy){ m[0] += dx; m[1] += dy; }
	};



	typedef VectorT<float, 3>	Vec3f;
	typedef VectorT<int,2>		Vec2i;
	typedef VectorT<float,2>		Vec2f;
	typedef VectorT<float,3>		Vec3f;
	typedef PointT<int,2>		Point2i;
	//typedef PointT<float,2>		Pointf;
	typedef PointT<float,2>		Point2f;
	typedef PointT<float,3>		Point3f;
	template <typename T> struct Rect;






	template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }
}//namespace



#endif//__VECTOR_H__
