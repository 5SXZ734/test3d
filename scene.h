#ifndef __SCENE_H__
#define __SCENE_H__

#include <math.h>
#include <list>
#include <limits>
#include "matrix.h"
#include "mesh.h"
#include "util.h"
#include "clip.h"


namespace My
{

	template <typename T, int SIZE>
	class Obj_t : public MatrixT<T, SIZE>
	{
	public:
		//const int	Size = SIZE;
		typedef T	BaseType;
		typedef	Obj_t<T, SIZE>	MyObj;
		typedef	VectorT<T, SIZE>	Vector;
		typedef MatrixT<T, SIZE>	Matrix;
		typedef PointT<T, SIZE>	Vertex;
		//typedef PointT<T, SIZE>	Point;
		typedef	Mesh_t<T, SIZE>	MyMesh;
		typedef	MeshMesh_t<T, SIZE>	MyMeshMesh;
		typedef	MeshLine_t<T, SIZE>	MyMeshLine;
		typedef	MeshPoint_t<T, SIZE>	MyMeshPoint;
		typedef	MeshMesh_t<T, SIZE>		MyMeshMesh;
		typedef std::list<Obj_t *>		MyObjList;
		typedef std::list<MyMesh *>		MyMeshList;
		typedef Line<Vertex, Vector>	MyLine;
	protected:
		MyObjList	mObj;//children
		MyMeshList	mMeshes;
	public:
		Obj_t()
		{
			loadIdentity();
		}
		virtual ~Obj_t()
		{
			removeChildren();
		}
		void removeChildren()
		{
			for (MyObjList::iterator i(mObj.begin()); i != mObj.end(); i++)
			{
				Obj_t *p(*i);
				delete p;
			}
			mObj.clear();
			for (MyMeshList::iterator j(mMeshes.begin()); j != mMeshes.end(); j++)
			{
				MyMesh *p(*j);
				delete p;
			}
			mMeshes.clear();
		}
		MyObjList &objects(){ return mObj; }
		MyMeshList &meshes(){ return mMeshes; }
		Obj_t *addObject()
		{
			Obj_t *pObj(new Obj_t());
			mObj.push_back(pObj);
			return pObj;
		}
		MyMeshLine *addLine()
		{
			MyMeshLine *p(new MyMeshLine());
			mMeshes.push_back(p);
			return p;
		}
		MyMeshLine *addLine(const Vertex &a, const Vertex &b)
		{
			MyMeshLine *p(new MyMeshLine(a, b));
			mMeshes.push_back(p);
			return p;
		}
		MyMeshPoint *addPoint(float radius)
		{
			MyMeshPoint *p(new MyMeshPoint(radius));
			mMeshes.push_back(p);
			return p;
		}
		MyMeshPoint *addPoint(const Vertex &a, float radius)
		{
			MyMeshPoint *p(new MyMeshPoint(a, radius));
			mMeshes.push_back(p);
			return p;
		}
		MyMeshMesh *addMesh(int nv, T *v, int nf, int *f)
		{
			MyMeshMesh *p(new MyMeshMesh(nv, v, nf, f));
			mMeshes.push_back(p);
			return p;
		}
		MyMeshMesh *addMesh()
		{
			MyMeshMesh *p(new MyMeshMesh());
			mMeshes.push_back(p);
			return p;
		}
		const Matrix &matrix(){ return *this; }
	};

	class Scene2D : public Obj_t<float, 3>
	{
	public:
		Scene2D(){}
		virtual ~Scene2D(){}
		//void setViewport(const Rectf &v){ mv = v; }
		virtual IClip<Vertex> *clipper(){ return NULL; }

		virtual bool OnMouse(const Vertex &, int){ return false; }
		virtual bool OnKeyDown(WPARAM){ return false; }

		void addAxes()
		{
			float a(100);
			MyObj &o(*addObject());
			MyMeshLine &lx(*o.addLine(Point3f(-a, 0, 1), Point3f(a, 0, 1)));
			lx.setColor(255, 0, 0);
			MyMeshLine &ly(*o.addLine(Point3f(0, -a, 1), Point3f(0, a, 1)));
			ly.setColor(0, 255, 0);
		}


		MyObj *addHexSwirl(const Point2f &cp)
		{
			float radius(0.2f);
			float rotAngle(0);
			MyObj &o(*addObject());
			for (int i(0); i < 20; i++)
			{
				MyMeshLine &l(*o.addLine());
				l.initNGon(6, cp, radius, rotAngle);
				radius += 0.2f;
				rotAngle += 10.0f;
			}
			return &o;
		}
		bool dissectCircle(std::vector<Point2f> &a)
		{
			int n((int)a.size());
			if (n < 3) return false;
			double angle(0);
			double angleInc(2*M_PI/n);
			for (int i(0); i < n; i++)
			{
				a[i] = Point2f(float(cos(angle)), float(sin(angle)));
				angle += angleInc;
			}
			return true;
		}
		void addRosette(int n, float radius)
		{
			std::vector<Point2f> a(n);
			dissectCircle(a);
			MyObj &o(*addObject());
			for (unsigned i(0); i < a.size()-1; i++)
				for (unsigned j(i+1); j < a.size(); j++)
					o.addLine(a[i], a[j]);
		}
		void addArc(const Vertex &center, float radius, float startAngle, float sweep)
		{
			const int n(30);
			float angle(DEG2RAD(startAngle));
			float angleInc(DEG2RAD(sweep)/n);
			Vertex cp(center);
			MyObj &o(*addObject());
			MyMeshLine &l(*o.addLine());
			for (int k(0); k <= n; k++, angle += angleInc)
			{
				Vec3f b(cos(angle), sin(angle));
				l.addVertex(cp + b*radius);
			}
		}
		void addCircle(const Vertex &cp, float radius)
		{
			addArc(cp, radius, 0, 360);
		}

		void add3PointCircle(Vertex A, Vertex B, Vertex C)
		{
			Vector a(B - A);
			Vector b(C - B);
			Vector c(A - C);

			Vertex Z(0,0,1);

			Line<Vertex, Vector> L1(A + a/2, a.perp());//bisector
			Line<Vertex, Vector> L2(C + c/2, c.perp());//bisector

			Vertex X;
			if (intersection2(L1, L2, X))
				addCircle(X, (X - A).length());
		}

	};

	class ObjScene2D : public Obj_t<float, 4>
	{
		Scene2D	*mpS2D;
	public:
		ObjScene2D(Scene2D	*pS2D) : mpS2D(pS2D){}
		Scene2D *scene(){ return mpS2D; }
		Scene2D::Vertex hit(const MyLine &L)
		{
			Vertex Z(0,0,0,1);
			Plain<Vertex, Vector> P(Z+col(3), col(2));
			Vertex I;
			if (!intersection3(L, P, I))
				return Scene2D::Vertex(0,0,0);//invalid point
			return Scene2D::Vertex(I[0], I[1], 1);
		}
	};

	//template <typename T, int SIZE>
	class Scene : public Obj_t<float, 4>//<T, SIZE>
	{
//		Rectf mw;//world view

	public:
		Scene()
		{
		}
		virtual ~Scene(){}

		ObjScene2D *addScene2D(Scene2D	*pS2D)
		{
			ObjScene2D *pObj(new ObjScene2D(pS2D));
			mObj.push_back(pObj);
			return pObj;
		}

		MyObj *addCube()
		{
			MyObj *pObj(addObject());
			float vertices[] = {
				0, 0, 0,
				1, 0, 0,
				1, 1, 0,
				0, 1, 0,
				0, 0, -1,//4
				1, 0, -1,
				1, 1, -1,
				0, 1, -1
			};
			int faces[] = {
				4, 0, 1, 2, 3,//front
				4, 1, 5, 6, 2,//right
				4, 5, 4, 7, 6,//back
				4, 4, 0, 3, 7,//left
				4, 4, 5, 1, 0,//bottom
				4, 3, 2, 6, 7
			};

			MyMeshMesh &a(*pObj->addMesh(
				sizeof(vertices)/sizeof(float), vertices,
				sizeof(faces)/sizeof(int), faces
				));

			a.calcNormals();
			return pObj;
		}

		MyObj *addSphere(int Slices, int Stacks)
		{
			MyObj *pObj(addObject());

			MyMeshMesh &a(*pObj->addMesh());

			double Radius = 1;
			Vertex Center(0, 0, 0, 1);

            // Fill the vertices, normals, and textures collections.
            for (int stack = 0; stack <= Stacks; stack++)
            {
                double phi = M_PI / 2 - stack * M_PI / Stacks;
                double y = Radius * sin(phi);
                double scale = -Radius * cos(phi);

                for (int slice = 0; slice <= Slices; slice++)
                {
                    double theta = slice * 2 * M_PI / Slices;
                    double x = scale * sin(theta);
                    double z = scale * cos(theta);

                    Vector normal(float(x), float(y), float(z), 1);
                    //normals.Add(normal);
					a.addVertex(Center + normal);
                    //vertices.Add(normal + Center);
                    //textures.Add(new PointT((double)slice / Slices, (double)stack / Stacks));
                }
            }

            // Fill the indices collection.
            for (int stack = 0; stack < Stacks; stack++)
            {
                for (int slice = 0; slice < Slices; slice++)
                {
                    if (stack != 0)
                    {
						MyMeshMesh::Face &f(a.addFace());
                        f.addVertexID((stack + 0) * (Slices + 1) + slice);
                        f.addVertexID((stack + 1) * (Slices + 1) + slice);
                        f.addVertexID((stack + 0) * (Slices + 1) + slice + 1);
                    }

                    if (stack != Stacks - 1)
                    {
						MyMeshMesh::Face &f(a.addFace());
                        f.addVertexID((stack + 0) * (Slices + 1) + slice + 1);
                        f.addVertexID((stack + 1) * (Slices + 1) + slice);
                        f.addVertexID((stack + 1) * (Slices + 1) + slice + 1);
                    }
                }
            }

			a.calcNormals();
			return pObj;
        }

		void addAxes()
		{
			MyObj &o(*addObject());
			float a(100);//FLT_MAX);
			MyMeshLine &lx(*o.addLine(Vertex(-a, 0, 0, 1), Vertex(a, 0, 0, 1)));
			lx.setColor(255, 0, 0);
			MyMeshLine &ly(*o.addLine(Vertex(0, -a, 0, 1), Vertex(0, a, 0, 1)));
			ly.setColor(0, 255, 0);
			MyMeshLine &lz(*o.addLine(Vertex(0, 0, -a, 1), Vertex(0, 0, a, 1)));
			lz.setColor(0, 0, 255);
		}

		void addFloor()
		{
			MyObj &o(*addObject());
			float a(100);
			float step(2);
			for (float i(-a); i <= a; i += 2.0f)
			{
				MyMeshLine &l(*o.addLine(Vertex(i, 0, a, 1), Vertex(i, 0, -a, 1)));
				l.setColor(192, 192, 192);
			}
			for (float j(-a); j <= a; j += 2.0f)
			{
				MyMeshLine &l(*o.addLine(Vertex(a, 0, j, 1), Vertex(-a, 0, j, 1)));
				l.setColor(192, 192, 192);
			}
		}
	};

}//namespace



#endif//__SCENE_H__
