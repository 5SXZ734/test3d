#ifndef __CLIP_H__
#define __CLIP_H__

#include <algorithm>
#include "vector.h"
#include "util.h"

namespace My {

	template <typename PointT>
	struct Segment
	{
		PointT a, b;
		Segment(const PointT &_a, const PointT &_b) : a(_a), b(_b){}
	};

	template <typename VERTEX>
	class IClip
	{
	public:
		IClip(){}
		virtual bool isEmpty(){ return true; }
		virtual int getBounds(std::vector<VERTEX> &){ return 0; }
		virtual int clipSegment(const Segment<VERTEX> &, std::vector<Segment<VERTEX> > &){ return 0; }
		virtual int clipPolygon(std::vector<VERTEX> v, std::vector<VERTEX> &w){ return 0; }
	};

	////////////////////////////////////////////////
	// Cohen - Sutherland

	template <typename VERTEX>
	class ClipCohenSutherland : public Rectf,
		public IClip<VERTEX>
	{
	public:
		ClipCohenSutherland(){}
		ClipCohenSutherland(const Rectf &r)
			: Rectf(r)
		{
		}
		void set(const Rectf &v){ *this = v; }
		virtual int getBounds(std::vector<VERTEX> &w)
		{
			w.push_back(VERTEX(a[0], a[1], 1));
			w.push_back(VERTEX(b[0], a[1], 1));
			w.push_back(VERTEX(b[0], b[1], 1));
			w.push_back(VERTEX(a[0], b[1], 1));
			return 4;
		}
		virtual int clipSegment(const Segment<VERTEX> &s0, std::vector<Segment<VERTEX> > &w)//by Cohen-Sutherland
		{
			w.push_back(s0);
			Segment<VERTEX> &s(w.back());
			unsigned char u1(code(s.a)), u2(code(s.b));//code
			for (;;)
			{
				if (!(u1 | u2))
					return 1;//trivial accept
				if ((u1 & u2))
					return 0;//trivial reject
				if (u1)//p1 is outside
				{
					chop(s.a, u1, s.b);
					u1 = code(s.a);
				}
				else//p2 is outside
				{
					chop(s.b, u2, s.a);
					u2 = code(s.b);
				}
			}
			return 1;
		}
		void setBounds(const std::vector<VERTEX> &w)
		{
			a[0] = b[0] = w[0][0];
			a[1] = b[1] = w[0][1];
			for (size_t i(1); i < w.size(); i++)
			{
				a[0] = min(a[0], w[i][0]);
				b[0] = max(b[0], w[i][0]);
				a[1] = min(a[1], w[i][1]);
				b[1] = max(b[1], w[i][1]);
			}
		}
	protected:
		unsigned char code(const VERTEX &p)
		{
			unsigned char u(0);
			if (p[0] < a[0]) u |= 8;
			if (p[1] > b[1]) u |= 4;
			if (p[0] > b[0]) u |= 2;
			if (p[1] < a[1]) u |= 1;
			return u;
		}
		void chop(VERTEX &p, unsigned char u, const VERTEX &q)
		{
			float delx(q[0] - p[0]);
			float dely(q[1] - p[1]);
			if (u & 8)//to the left
			{
				p[1] += (a[0] - p[0])*dely / delx;
				p[0] = a[0];
			}
			else if (u & 2)//to the right
			{
				p[1] += (b[0] - p[0])*dely / delx;
				p[0] = b[0];
			}
			else if (u & 1)//below
			{
				p[0] += (a[1] - p[1])*delx / dely;
				p[1] = a[1];
			}
			else if (u & 4)//above
			{
				p[0] += (b[1] - p[1])*delx / dely;
				p[1] = b[1];
			}
		}
	};

	/////////////////////////////////////////////////////
	// Sutherland-Hodgman

	template <typename VERTEX>
	class ClipSutherlandHodgman : public ClipCohenSutherland<VERTEX>
	{
	public:
		ClipSutherlandHodgman(){}
		ClipSutherlandHodgman(const Rectf &r)
			: ClipCohenSutherland<VERTEX>(r)
		{
		}
		virtual int clipPolygon(std::vector<VERTEX> v, std::vector<VERTEX> &w)
		{
			for (unsigned char side(0x8); side; side >>= 1)
			{
				w.clear();
				size_t N(v.size());
				for (size_t i(0); i < N; i++)
				{
					Segment<VERTEX> s(v[i], v[(i+1)%N]);
					unsigned char ua(code(s.a)&side);
					unsigned char ub(code(s.b)&side);
					if (!ua && !ub)
					{
						w.push_back(s.b);
					}
					else if (!ua && ub)
					{
						chop(s.b, ub, s.a);
						w.push_back(s.b);
					}
					else if (ua && !ub)
					{
						chop(s.a, ua, s.b);
						w.push_back(s.a);
						w.push_back(s.b);
					}
				}
				v = w;
			}
			return 1;
		}
	};

	//////////////////////////////////////////////////
	// 

	template <typename T>
	struct BC3//boundary coordinate
	{
		T	m[6];
		unsigned	u;//outcode
		BC3(const PointT<T, 4> &P)
			: u(0)
		{
			for (int i(0); i < 3; i++)
			{
				m[i<<1] = P[3]+P[i];
				m[(i<<1)+1] = P[3]-P[i];
			}
			if (P[0]/P[3] < -1)
				u |= 0x01;
			if (P[0]/P[3] > 1)
				u |= 0x02;
			if (P[1]/P[3] < -1)
				u |= 0x04;
			if (P[1]/P[3] > 1)
				u |= 0x08;
			if (P[2]/P[3] < -1)
				u |= 0x10;
			if (P[2]/P[3] > 1)
				u |= 0x20;
		}
	};

	template <typename T>
	int clipEdge(PointT<T, 4> &A, PointT<T, 4> &C)
	{
		T tIn(T(0)), tOut(T(1)), tHit;
		BC3<T> aBC(A), cBC(C);
		if ((aBC.u & cBC.u) != 0)
			return 0;//trivial reject
		if ((aBC.u | cBC.u) == 0)
			return 1;//trivial accept
		for (int i(0); i < 6; i++)
		{
			if (cBC.m[i] < 0)//exits: C is outside
			{
				tHit = aBC.m[i]/(aBC.m[i]-cBC.m[i]);
				tOut = min(tOut, tHit);
			}
			else if(aBC.m[i] < 0)//enters: A is outside
			{
				tHit = aBC.m[i]/(aBC.m[i]-cBC.m[i]);
				tIn = max(tIn, tHit);
			}
			if (tIn > tOut)
				return 0;//early out
		}
		PointT<T, 4> tmp(A);
		if (aBC.u != 0)
			tmp = A+(C-A)*tIn;
		if (cBC.u != 0)
			C = A+(C-A)*tOut;
		A = tmp;
		return 1;
	}

	////////////////////////////////////////////////
	// Cyrus - Beck

	template <typename VERTEX>
	class CyrusBeckClip : public std::vector<VERTEX>,
		public IClip<VERTEX>
	{
	public:
		typedef typename VERTEX::ValueType T;
		typedef VectorT<T, VERTEX::Size>	VECTOR;
		CyrusBeckClip(){}
		CyrusBeckClip(const std::vector<VERTEX> &CLIP)
			: std::vector<VERTEX>(CLIP)
		{
		}

		virtual bool isEmpty(){ return empty(); }
		virtual int getBounds(std::vector<VERTEX> &w){ w = *this; return (int)size(); }
		void setBounds(const std::vector<VERTEX> &w){ assign(w.begin(), w.end()); }

		int clipSegment(const Segment<VERTEX> &s0, std::vector<Segment<VERTEX> > &w)
		{
			typedef VERTEX::ValueType T;
			Segment<VERTEX> s(s0);
			T tIn(0.0), tOut(1.0);
			VECTOR c(s.b-s.a);
			size_t N(size());
			for (size_t i(0); i < N; i++)
			{
				VECTOR t(at(i)-s.a);
				VECTOR l(at((i+1)%N)-at(i));
				T num(l.perp().dot(t));
				T denum(l.perp().dot(c));

				if (denum < 0)//ray is entering
				{
					T tHit(num/denum);
					if (tHit > tOut)
						return 0;//early out
					if (tHit > tIn)
						tIn = tHit;
				}
				else if (denum > 0)//ray is exiting
				{
					T tHit(num/denum);
					if (tHit < tIn)
						return 0;//early out
					if (tHit < tOut)
						tOut = tHit;
				}
				else//ray is parallel
				{
					if (num <= 0)
						return 0;//missed the line
				}
			}
			if (tOut < 1)
				s.b = s.a + c*tOut;
			if (tIn > 0)
				s.a = s.a + c*tIn;

			w.push_back(s);
			return 1;
		}
	};

	template <typename VERTEX>
	class CyrusBeckClipEx : public CyrusBeckClip<VERTEX>
	{
	public:
		CyrusBeckClipEx(){}
		CyrusBeckClipEx(const std::vector<VERTEX> &CLIP)
			: CyrusBeckClipr<VERTEX>(CLIP)
		{
		}

		int clipSegment(const Segment<VERTEX> &seg, std::vector<Segment<VERTEX> > &w)
		{
			std::vector<T> hitList;
			size_t N(size());
			VERTEX A(seg.a);
			VECTOR a(seg.b-seg.a);
			for (size_t i(0); i < N; i++)
			{
				VERTEX B(at(i));
				VECTOR b(at((i+1)%N)-at(i));
				VECTOR bn(b.perp());
				T denom(bn.dot(a));
				if (denom != 0)
				{
					VECTOR c(B - A);
					T t(bn.dot(c) / denom);
					//if (t >= 0 && t <= 1)
					{
						T u(a.perp().dot(c) / denom);
						if (u >= 0 && u <= 1)
							hitList.push_back(t);
					}
				}
			}
			std::sort(hitList.begin(), hitList.end());
			assert(hitList.size()%2 == 0);//even
			for (size_t j(0); j < hitList.size(); j += 2)
			{
				if (hitList[j] > 1 || hitList[j+1] < 0)
					break;
				Segment<VERTEX> s(A+a*hitList[j], A+a*hitList[j+1]);
				if (hitList[j+1] > 1)
					s.b = seg.b;
				if (hitList[j] < 0)
					s.a = seg.a;
				w.push_back(s);
			}
			return (int)w.size();
		}
	};
}

#endif//__CLIP_H__
