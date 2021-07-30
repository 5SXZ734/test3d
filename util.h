#ifndef __UTIL_H__
#define __UTIL_H__

#include "vector.h"

namespace My
{
	template <typename T>
	struct Size
	{
		T w, h;
		Size() : w(0), h(0){}
		Size(T _w, T _h) : w(_w), h(_h){}
		T width() const { return w; }
		T height() const { return h; }
	};

	typedef	Size<int> Sizei;

	template <typename T>
	struct Rect
	{
		typedef PointT<T,2>	PointT;
		PointT    a, b;
		Rect() : a(0, 0), b(0, 0){}
		Rect(const PointT &_a, const PointT &_b) : a(_a), b(_b){}
		Rect(const PointT &p, const Size<T> &s) : a(p),
			b(p[0] + s.w, p[1] + s.h){}
		T width() const { return b[0] - a[0]; }
		T height() const { return b[1] - a[1]; }
		Size<T> size() const { return Size<T>(width(), height()); }
		void setSize(const Size<T> &s){ b[0] = a[0] + s.w; b[1] = a[1] + s.h; }
		template <typename U>
		U ratio() const { return U(width()) / U(height()); }
		void translate(T dx, T dy){ a.translate(dx, dy); b.translate(dx, dy); }
		void shrink(T dx, T dy){ a[0] += dx; b[0] -= dx; a[1] += dy; b[1] -= dy; }
		void flip(){ PointT<T> t(a); a = b; b = t; }
		bool empty(){ return width() == 0 || height() == 0; }
	};


	typedef    Rect<int>		Recti;
	typedef    Rect<float>		Rectf;
	typedef    Size<int>		Sizei;
	typedef    Size<float>		Sizef;

	template<typename PointT, typename Dir>
	struct Line//parametric
	{
		typedef	PointT	Point;
		typedef	Dir	Vector;
		PointT	a;//point
		Dir	b;//vector
		Line(const PointT &_a, const Dir &_b) : a(_a), b(_b){}
		const PointT &A() const { return a; }
		const Dir &B() const { return b; }
		Dir norm() const { return b.perp(); }
	};

	template <typename PointT, typename Normal>
	struct Plain
	{
		PointT a;
		Normal n;
		Plain(const PointT &_a, const Normal &_n) : a(_a), n(_n){}
		const PointT &A() const { return a; }
		const Normal &N() const { return n; }
	};

	template <typename LINE, typename PLAIN>
	inline bool intersection3(const LINE &L, const PLAIN &P, typename LINE::Point &I, int flag = 0)//0:check lines, 1:check segments
	{
		typedef typename LINE::Point	Point;
		typedef typename LINE::Vector	Vector;
		Point A(L.A());
		Vector b(L.B());
		Point C(P.A());
		Vector n(P.N());
		float denom(n.dot(b));
		if (denom != 0)
		{
			Vector c(C - A);
			float t(n.dot(c)/denom);
			if (flag > 0)
			{
				if (t > 0 && t < 1)
				{
					I = L.A() + b*t;
					return true;
				}
			}
			else
			{
				I = L.A() + b*t;
				return true;
			}
		}
		return false;
	}

	template <typename LINE1, typename LINE2>
	inline bool intersection2(const LINE1 &L1, const LINE2 &L2, typename LINE1::Point &I, int flag = 0)//0:check lines, 1:check segments
	{
		typedef typename LINE1::Point	Point;
		typedef typename LINE1::Vector	Vector;
		Point A(L1.A());
		Vector b(L1.B());
		Point C(L2.A());
		Vector n(L2.norm());
		float denom(n.dot(b));
		if (denom != 0)
		{
			Vector c(C - A);
			float t(n.dot(c) / denom);
			if (flag > 0)
			{
				if (t >= 0 && t <= 1)
				{
					float u(L1.norm().dot(c) / denom);
					if (u >= 0 && u <= 1)
					{
						I = A + b*t;
						return true;
					}
				}
			}
			else
			{
				I = A + b*t;
				return true;
			}
		}
		return false;
	}
};

#endif//__UTIL_H__
