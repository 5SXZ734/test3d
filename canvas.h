#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <windows.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <list>
#include <stack>
#include <assert.h>

#include "scene.h"
#include "camera.h"
#include "clip.h"

#pragma warning (disable:4996)

namespace My
{
	class Painter
	{
	public:
		virtual void moveTo(const Scene2D::Vertex &) = 0;
		virtual void lineTo(const Scene2D::Vertex &) = 0;
		virtual void drawPoint(const Scene2D::Vertex &, float) = 0;
		virtual void drawLine(const Scene2D::Vertex &, const Scene2D::Vertex &) = 0;
	};

	class Canvas2D
	{
		typedef Scene2D::Vertex Vertex;
		typedef Scene2D::Vector Vector;
		HDC    mhDC;
		std::stack<Scene2D::Matrix>	mCT;
		IClip<Vertex> *CLIP;

	public:
		
		Canvas2D(HDC hDC, IClip<Vertex> *clip)
			: mhDC(hDC),
			CLIP(clip)
		{
		}

		//void setClipper(const Rectf &v){ CLIP.set(v); }

		void draw(Scene2D *pScene, Painter &rP)
		{
			if (CLIP)
			{
				std::vector<Vertex> w;
				if (CLIP->getBounds(w))
				{
					HPEN hPen(CreatePen (PS_SOLID, 1, RGB(255, 255, 0)) );
					SelectObject(mhDC, hPen);
					rP.moveTo(w[0]);
					for (size_t i(0); i < w.size(); i++)
						rP.lineTo(w[(i+1)%w.size()]);
					DeleteObject(hPen);
				}
			}

			Scene2D::Matrix m;
			m.loadIdentity();
			mCT.push(m);
			drawObject(pScene, rP);
			mCT.pop();
		}

		void drawObject(Scene2D::MyObj *pObj, Painter &rP)
		{
			mCT.push(mCT.top()*pObj->matrix());

			Scene2D::MyObjList &objects(pObj->objects());
			for (Scene2D::MyObjList::iterator ito(objects.begin()); ito != objects.end(); ito++)
			{
				drawObject(*ito, rP);
			}

			Scene2D::MyMeshList &meshes(pObj->meshes());
			for (Scene2D::MyMeshList::iterator itm(meshes.begin()); itm != meshes.end(); itm++)
			{
				if (!drawLine(dynamic_cast<Scene2D::MyMeshLine *>(*itm), rP))
					if (!drawPoint(dynamic_cast<Scene2D::MyMeshPoint *>(*itm), rP))
					{
					}
			}

			mCT.pop();
		}


		bool drawLine(Scene2D::MyMeshLine *pLine, Painter &rP)
		{
			if (!pLine)
				return false;
			int i2(pLine->size());
			int i3(i2);
			if (pLine->isClosed())
				i3++;
			Color c(pLine->color());

			HPEN hPen(CreatePen (PS_SOLID, 1, RGB(c[0], c[1], c[2])) );
			HPEN hPen2(CreatePen (PS_SOLID, 1, RGB(196, 196, 196)) );
			SelectObject(mhDC, hPen);

			if (pLine->isClosed())
			{
				std::vector<Vertex> w(pLine->vertices());
				for (size_t i(0); i < w.size(); i++)
					w[i] = mCT.top()*w[i];

				CLIP->clipPolygon(w, w);
				size_t N(w.size());
				if (N > 0)
				{
					for (size_t i(0); i < N+1; i++)
					{
						Vertex c(w[i%N]);
						//Vertex c(mCT.top()*b);
						if (i == 0)
							rP.moveTo(c);
						else
							rP.lineTo(c);
					}
				}
			}
			else
			{

				Vertex a;
				for (int i(0); i < i3; i++)
				{
					Vertex &b(pLine->at(i%i2));
					Vertex c(mCT.top()*b);

					if (!CLIP)
					{
						if (i == 0)
							rP.moveTo(c);
						else
							rP.lineTo(c);
					}
					else
					{
						if (i > 0)
						{
							Segment<Vertex> s0(a, c);
							std::vector<Segment<Vertex> > w;
							if (CLIP->clipSegment(s0, w))
							{
								if (s0.a != w[0].a)
								{
									SelectObject(mhDC, hPen2);
									rP.drawLine(s0.a, w[0].a);
								}
								SelectObject(mhDC, hPen);
								rP.drawLine(w[0].a, w[0].b);
								if (w[0].b != s0.b)
								{
									SelectObject(mhDC, hPen2);
									rP.drawLine(w[0].b, s0.b);
								}
							}
							else
							{
								SelectObject(mhDC, hPen2);
								rP.drawLine(s0.a, s0.b);
							}
						}
					}
					a = c;
				}

			}
			if (pLine->endPoints() > 0)
			{
				for (int i(0); i < i2; i++)
				{
					Vertex a(pLine->at(i));
					Vertex b(mCT.top()*a);
					rP.drawPoint(b, (float)pLine->endPoints());
				}
			}
			DeleteObject(hPen);
			DeleteObject(hPen2);
			return true;
		}

		bool drawPoint(Scene2D::MyMeshPoint *pPoint, Painter &rP)
		{
			if (!pPoint)
				return false;

			for (int i(0); i < pPoint->size(); i++)
			{
				Scene2D::Vertex a(pPoint->at(i));
				Scene2D::Vertex b(mCT.top()*a);
				rP.drawPoint(b, pPoint->radius());
			}
			return true;
		}

	};


	class Canvas : public Painter
	{
		HDC    mhDC;
		Recti   mv;//viewport
		Scene::Matrix	mVT[2];//world view 2 screen vew mapping
		Scene::Vertex	mCP;//current position in world coords
		Scene::Matrix mPT;//perspective transformation
		std::stack<Scene::Matrix>	mCT;
		std::vector<HPEN> mPens;

	private:
		size_t addPen(int a, int w, COLORREF c)
		{
			return mPens.size()-1;
		}

	protected://Painter

		virtual void moveTo(const Scene2D::Vertex &b)
		{
			Scene::Vertex c(b[0], b[1], 0, 1);
			Scene::Vertex d(mCT.top()*c);
			moveTo(d);
		}
		virtual void lineTo(const Scene2D::Vertex &b)
		{
			Scene::Vertex c(b[0], b[1], 0, 1);
			Scene::Vertex d(mCT.top()*c);
			lineTo(d);
		}
		virtual void drawPoint(const Scene2D::Vertex &b, float ep)
		{
			Scene::Vertex c(b[0], b[1], 0, 1);
			Scene::Vertex d(mCT.top()*c);
			drawPoint(d, ep);
		}
		virtual void drawLine(const Scene2D::Vertex &a, const Scene2D::Vertex &b)
		{
			moveTo(a);
			lineTo(b);
		}

	public:
		Canvas(HDC hDC) : mhDC(hDC){}
		~Canvas()
		{
			for (size_t i(0); i < mPens.size(); i++)
				DeleteObject(mPens[i]);
		}
		void setPerspective(const Scene::Matrix &m){ mPT = m; }
		void setViewport(const Recti &v)//, float aspect = 0.0f)
		{
			mv = v;
			/*if (aspect != 0)
			{
				Sizei vp(mv.size());
				//Rectf w(world());
				float R(aspect);
				if (R > vp.w / float(vp.h))
					vp.h = int(vp.w / R);
				else
					vp.w = int(vp.h*R);
				mv.setSize(vp);
			}*/
			Scene::Matrix &vt(mVT[0]);
			vt.loadIdentity();
			vt[0][0] = mv.width() / 2.0f;// mw.width();
			vt[0][vt.size() - 1] = mv.a[0] - vt[0][0] * -1.0f;// mw.a[0];
			vt[1][1] = -mv.height() / 2.0f;// mw.height();
			vt[1][vt.size() - 1] = mv.b[1] - vt[1][1] * -1.0f;// mw.a[1];
			//inverted
			mVT[1] = inverse(vt);
		}

		void moveTo(const Scene::Vertex &a)
		{
			Scene::Vertex b0(mPT*a);
			b0.restore();
			Scene::Vertex b(mVT[0]*b0);
			MoveToEx(mhDC, int(b[0]), int(b[1]), NULL);
			mCP = a;
		}
		void lineTo(const Scene::Vertex &a)
		{
			Scene::Vertex A0(mCP);
			Scene::Vertex C0(a);

			float N(-1);

			if (A0[2] <= N || C0[2] <= N)
			{

				if (A0[2] > N)
					std::swap(A0, C0);

				if (C0[2] > N)
				{
					Scene::Vertex B0(0,0,N,1);
					Scene::Vector n(0,0,-1,0);

					Line<Scene::Vertex, Scene::Vector> L(A0, C0-A0);
					Plain<Scene::Vertex, Scene::Vector> P(B0, n);
					intersection3(L, P, C0);
				}

				Scene::Vertex A(mPT*A0);
				Scene::Vertex C(mPT*C0);
				if (clipEdge(A, C))
				{
					A.restore();
					C.restore();
					Scene::Vertex A2(mVT[0]*A);
					Scene::Vertex C2(mVT[0]*C);
					MoveToEx(mhDC, int(A2[0]), int(A2[1]), NULL);
					LineTo(mhDC, int(C2[0]), int(C2[1]));
				}
			}
			mCP = a;
		}

		bool drawMesh(Scene::MyMeshMesh *p)
		{
			if (!p)
				return false;

			Color c(p->color());
			HPEN hPen(CreatePen (PS_SOLID, 1, RGB(c[0], c[1], c[2])) );
			SelectObject(mhDC, hPen);
			for (size_t i(0); i < p->faces().size(); i++)
			{
				const Scene::MyMeshMesh::Face &f(p->face(i));
				const Scene::Vector &n0(f.normal());
				Scene::Vector n(mCT.top()*n0);
				Scene::Vertex cp0(f.center(p->vertices()));
				Scene::Vertex cp(mCT.top()*cp0);
				Scene::Vertex Z(0,0,0,1);
				Scene::Vector z(cp-Z);
				if (n.dot(z) <= 0)//remove back faces
				{
					for (size_t j(0); j < f.vIds.size()+1; j++)
					{
						int n(f.vIds[j%f.vIds.size()].index);
						const Scene::Vertex &v(p->vertex(n));
						Scene::Vertex w(mCT.top()*v);
						if (j == 0)
							moveTo(w);
						else
							lineTo(w);
					}
				}
			}
			DeleteObject(hPen);
#if(0)//draw normals
			{
			HPEN hPen(CreatePen (PS_SOLID, 1, RGB(127, 127, 0)) );
			SelectObject(mhDC, hPen);
			for (size_t i(0); i < p->faces().size(); i++)
			{
				const Scene::MyMeshMesh::Face &f(p->face(i));
				Scene::Vertex na(f.center(p->vertices()));
				Scene::Vertex nb(na+f.normal());
				Scene::Vertex a(mCT.top()*na);
				Scene::Vertex b(mCT.top()*nb);
				moveTo(a);
				lineTo(b);
			}
			DeleteObject(hPen);
			}
#endif
			return true;
		}

		bool drawLine(Scene::MyMeshLine *pLine)
		{
			if (!pLine)
				return false;

			Color c(pLine->color());
			HPEN hPen(CreatePen (PS_SOLID, 1, RGB(c[0], c[1], c[2])) );
			SelectObject(mhDC, hPen);
			for (int i(0); i < pLine->size(); i++)
			{
				Scene::Vertex &a(pLine->at(i));
				Scene::Vertex b(mCT.top()*a);
				if (i == 0)
					moveTo(b);
				else
					lineTo(b);
			}
			DeleteObject(hPen);
			return true;
		}

		bool drawPoint(Scene::MyMeshPoint *pPoint)
		{
			if (!pPoint)
				return false;

			for (int i(0); i < pPoint->size(); i++)
			{
				Scene::Vertex a(pPoint->at(i));
				Scene::Vertex b(mCT.top()*a);
				drawPoint(b, pPoint->radius());
			}
			return true;
		}

		void drawObject(Scene::MyObj *pObj)
		{
			mCT.push(mCT.top()*pObj->matrix());

			ObjScene2D *pScene2D(dynamic_cast<ObjScene2D *>(pObj));
			if (pScene2D)
			{
				Canvas2D C2D(mhDC, pScene2D->scene()->clipper());
				C2D.draw(pScene2D->scene(), *this);
			}

			Scene::MyObjList &objects(pObj->objects());
			for (Scene::MyObjList::iterator ito(objects.begin()); ito != objects.end(); ito++)
			{
				drawObject(*ito);
			}

			Scene::MyMeshList &meshes(pObj->meshes());
			for (Scene::MyMeshList::iterator itm(meshes.begin()); itm != meshes.end(); itm++)
			{
				if (!drawMesh(dynamic_cast<Scene::MyMeshMesh *>(*itm)))
				if (!drawLine(dynamic_cast<Scene::MyMeshLine *>(*itm)))
				if (!drawPoint(dynamic_cast<Scene::MyMeshPoint *>(*itm)))
				{
				}
			}

			mCT.pop();
		}
		void draw(Scene &sc, const Camera4f &camera)
		{
			mCT.push(camera);
			drawObject(&sc);
			mCT.pop();
		}
		void drawPoint(const Scene::Vertex &a, float radius) const
		{
			Scene::Vertex b0(mPT*a);
			b0.restore();
			Scene::Vertex b(mVT[0]*b0);
			COLORREF color = RGB(0,0,0);
			if (radius == 0)
				SetPixel(mhDC, int(b[0]), int(b[1]), color);
			else
				Ellipse(mhDC, int(b[0]-radius), int(b[1]-radius), int(b[0]+radius), int(b[1]+radius));
		}

		/*void drawRect(const Rectf &r)
		{
			Pointf a(r.a), b(r.b);
			moveTo(a);
			lineTo(Pointf(b[0], a[1]));
			lineTo(b);
			lineTo(Pointf(a[0], b[1]));
			lineTo(a);
		}*/

		Recti viewport(){ return mv; }

		void drawViewportFrame()
		{
			Recti v(viewport());
			MoveToEx(mhDC, mv.a[0], mv.a[1], NULL);
			LineTo(mhDC, mv.b[0], mv.a[1]);
			LineTo(mhDC, mv.b[0], mv.b[1]);
			LineTo(mhDC, mv.a[0], mv.b[1]);
			LineTo(mhDC, mv.a[0], mv.a[1]);
		}
	

	public:
		/*void w2v(const Pointf &a, Pointi &b)
		{
			Vec3f v;
			v[0] = a[0];
			v[1] = a[1];
			v[2] = 1;
			Vec3f w(mVT*v);
			b[0] = int(w[0]);
			b[1] = int(w[1]);
		}*/
		void v2w(const Point2i &a, Scene::Vertex &b)
		{
			Scene::Vertex v(float(a[0]), float(a[1]), 0, 1);
			b = mVT[1]*v;
		}

	};



}//namespace My


#endif//__CANVAS_H__
