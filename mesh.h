#ifndef __MESH_H__
#define __MESH_H__

#include "vector.h"


namespace My {

	class Color
	{
	public:
		unsigned char m[3];
		Color()
		{
			for (int i(0); i < 3; i++)
				m[i] = 0;
		}
		Color(unsigned char r, unsigned char g, unsigned char b)
		{
			m[0] = r;
			m[1] = g;
			m[2] = b;
		}
		unsigned char &operator [](const int i){ return m[i]; }
		const unsigned char &operator [](const int i) const { return m[i]; }
	};

	
	template <typename T, int SIZE>
	class Mesh_t
	{
	protected:
		typedef PointT<T, SIZE>	Vertex;
		typedef VectorT<T, SIZE>	Vector;
		std::vector<Vertex> mv;//vertex list
		Color	mColor;

	public:
		Mesh_t()
		{
		}
		virtual ~Mesh_t(){}
		virtual int size(){ return (int)mv.size(); }
		virtual Vertex &at(int i){ return mv[i]; }
		virtual Vertex &vertex(int i){ return mv[i]; }
		void addVertex(const Vertex &a){ mv.push_back(a); }
		void clear(){ mv.clear(); }
		void setColor(unsigned char r, unsigned char g, unsigned char b){ mColor = Color(r, g, b); }
		const Color &color(){ return mColor; }
		const std::vector<Vertex> &vertices() const { return mv; }
	};

	template <typename T, int SIZE>
	class MeshMesh_t : public Mesh_t<T, SIZE>
	{
	public:
		struct VertexID
		{
			int	index;
			//int normIndex;
			VertexID(int i) : index(i){}
		};
		struct Face
		{
			std::vector<VertexID>	vIds;
			Vector	vNorm;
			Face(){}
			Face(int *f)
			{
				for (int k(1); k <= f[0]; k++)
					vIds.push_back(VertexID(f[k]));
			}
			void addVertexID(size_t i){ vIds.push_back(VertexID(int(i))); }
			const Vector &normal() const { return vNorm; }
			Vertex center(const std::vector<Vertex> &vList) const
			{
				Vector t(0);
				size_t N(vIds.size());
				Vertex Z(0,0,0,1);
				for (size_t i(0); i < N; i++)
					t += (vList[vIds[i].index]-Z);
				return (Z+t/float(N));
			}
			void calcNormal(const std::vector<Vertex> &vList)//Newell method
			{
				vNorm.clear();
				size_t N(vIds.size());
				for (size_t i(0); i < N; i++)
				{
					const Vertex &v(vList[vIds[i].index]);
					const Vertex &vx(vList[vIds[(i+1)%N].index]);
					Vector w(
						(v[1]-vx[1])*(v[2]+vx[2]),
						(v[2]-vx[2])*(v[0]+vx[0]),
						(v[0]-vx[0])*(v[1]+vx[1]),
						0
						);
					vNorm += w;
				}
				vNorm.normalize();
			}
		};

		std::vector<Face> vFaces;
	public:
		MeshMesh_t(){}
		MeshMesh_t(int nv, T *v, int nf, int *f)
		{
			for (int i(0); i < nv; i += 3)
				mv.push_back(Vertex(v[i], v[i+1], v[i+2], 1));
			for (int j(0); j < nf; j += f[j]+1)
			{
				vFaces.push_back(Face(&f[j]));
				//for (int k(j+1); k < j+1+f[j]; k++)
				//	vFaces.back().vIds.push_back(f[k]);
			}
		}
		Face &addFace(){ vFaces.push_back(Face()); return vFaces.back(); }
		const Face &face(size_t i){ return vFaces[i]; }
		const std::vector<Face> &faces() const { return vFaces; }
		typedef typename std::vector<Face>::const_iterator FacesCIterator;
		void calcNormals()
		{
			for (size_t i(0); i < vFaces.size(); i++)
				vFaces[i].calcNormal(vertices());
		}
	private:

	};

	template <typename T, int SIZE>
	class MeshPoint_t : public Mesh_t<T, SIZE>
	{
		T	mfRadius;
	public:
		MeshPoint_t(float _radius = 0)
			: mfRadius(_radius)
		{
		}
		MeshPoint_t(const Vertex &_a, float _radius = 0)
			: mfRadius(_radius)
		{
			addVertex(_a);
		}
		T radius(){ return mfRadius; }
	};

	template <typename T, int SIZE>
	class MeshLine_t : public Mesh_t<T, SIZE>
	{
		bool mbClosed;
		int	mEndPoints;//radius
	public:
		MeshLine_t() : mbClosed(false), mEndPoints(0){}
		MeshLine_t(const Vertex &a, const Vertex &b)
			: mbClosed(false), mEndPoints(0)
		{
			addVertex(a);
			addVertex(b);
		}
		void initNGon(int n, const Point2f &center, float radius, float rotAngle)
		{
			assert(n > 2);
			mv.clear();
			double angle(DEG2RAD(rotAngle));
			double angleInc(2*M_PI/n);
			for (int k(0); k <= n; k++, angle += angleInc)
			{
				Vec2f b(float(cos(angle)), float(sin(angle)));
				Point2f c(center + b*radius);
				Vertex d(c);
				d[d.size() - 1] = 1;//point
				addVertex(d);
			}
		}
		bool intersectsAt(MeshLine_t *p, Vertex &I)
		{
			Line<Vertex, Vector> L1(mv[0], mv[1] - mv[0]);
			Line<Vertex, Vector> L2(p->at(0), p->at(1) - p->at(0));
			return intersection2(L1, L2, I, 1);
		}
		void setClosed(bool b){ mbClosed = b; }
		bool isClosed(){ return mbClosed; }
		void setEndPoints(int n){ mEndPoints = n; }
		int endPoints(){ return mEndPoints; }
	};
}//namespace

#endif//__MESH_H__
