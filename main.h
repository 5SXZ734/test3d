#ifndef __MAIN_H__
#define __MAIN_H__

#include <vector>
#include <windows.h>

#include "canvas.h"
#include "scene.h"
#include "camera.h"

namespace My {

	class ITest
	{
	protected:
		typedef Scene	MyScene;
		static ITest *gpSelf;

		std::vector<MyScene * >	mSceneList;
		Camera4f	mCamera;
		Recti	mv;//screen view area
		Scene2D	*mpScene2D;
		Point2i mMouse;
	public:
		ITest()
			: mv(Recti(Point2i(0, 0), Sizei(640, 480))),
			mpScene2D(NULL)
		{
			gpSelf = this;
		}
		virtual ~ITest()
		{
			for (int i(0); i < (int)mSceneList.size(); i++)
				delete mSceneList[i];
			gpSelf = NULL;
		}
		static ITest *self(){ return gpSelf; }
	protected:
		void setViewport(const Recti &v)
		{
			mv = v;
		}
		const Recti &vewport(){ return mv; }
		MyScene &newScene()
		{
			mSceneList.push_back(new MyScene());
			return *mSceneList.back();
		}
		/*Scene2D &newScene2D(Rectf w)
		{
			Scene *pScene(new MyScene());
			mSceneList.push_back(std::make_pair(pScene, Rectf(w)));
			mpScene2D = pScene->addScene2D();
			return *mpScene2D;
		}
*/
		int scenesTotal(){ return (int)mSceneList.size(); }
		//Scene2D &scene2D(){ return *mpScene2D; }
		MyScene &scene(int i = -1)
		{
			if (i < 0)
				return *mSceneList.back();
			return *mSceneList[i];
		}
		/*Rectf worldView(int i = -1)
		{
			if (i < 0)
				return mSceneList.back().second.worldView();
			return mSceneList[i].second.worldView();
		}*/
		Camera4f &setCamera(const Camera4f &a){ mCamera = a; return mCamera; }
		Camera4f &camera(){ return mCamera; }
	public:
		virtual void OnSetup(){}
		virtual void OnDraw(HDC hdc)
		{
			Canvas canvas(hdc);
			canvas.setPerspective(camera().projectionMatrix());
			canvas.setViewport(vewport());
			canvas.drawViewportFrame();
			canvas.draw(scene(), camera());
		}

		virtual bool OnKeyDown(WPARAM wParam)
		{
			Camera4f &w(camera());
			switch (wParam)
			{
			case VK_UP: w.slide(0, 0, +0.3f); return true;
			case VK_DOWN: w.slide(0, 0, -0.3f); return true;
			case VK_LEFT: w.slide(+0.3f, 0, 0); return true;
			case VK_RIGHT: w.slide(-0.3f, 0, 0); return true;

			case VK_HOME: w.pitch(-2); return true;
			case VK_END: w.pitch(+2); return true;
			case VK_DELETE: w.yaw(-2); return true;
			case VK_NEXT: w.yaw(+2); return true;
			case VK_INSERT: w.roll(-2); return true;
			case VK_PRIOR: w.roll(+2); return true;

			case VK_F1: w.setNearDist(w.nearDist()*1.1f); return true;
			case VK_F2: w.setNearDist(w.nearDist()*0.9f); return true;
			case VK_F3: w.setViewAngle(w.viewAngle()+1); return true;
			case VK_F4: w.setViewAngle(w.viewAngle()-1); return true;
			default:
				return false;
			}
			return true;
		}

		virtual bool OnMouse(const Scene::MyLine &, int){ return false; }
		virtual bool OnMouseDown(int sx, int sy, int btn)
		{
			SetCursor (LoadCursor (NULL, IDC_CROSS)) ;
			mMouse = Point2i(sx, sy);
			Canvas canvas(0);
			canvas.setViewport(vewport());
			Scene::Vertex b;
			canvas.v2w(Point2i(sx, sy), b);
			Scene::Vertex c[2];
			c[0] = Scene::Vertex(0,0,0,1);
			camera().p2e(b, c[1]);
			c[1].restore();
			//c[1] = c[0]+Scene::Vector(0,0,-100);
			Scene::Vertex d[2];
			camera().c2w(c[0], d[0]);
			camera().c2w(c[1], d[1]);
			Scene::MyLine L(d[0], d[1]-d[0]);
#if(0)
			Scene::MyObj &o(*scene().addObject());
			Scene::MyMeshLine &l(*o.addLine(d[1], d[1]+L.B()*1000));
			l.setColor(127, 0, 127);
#endif
			return OnMouse(L, btn);
		}
		virtual bool OnMouseUp(int sx, int sy)
		{
			SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
			return false;
		}
		virtual bool OnMouseWheel(int d)
		{
			Scene::Matrix t;
			t.loadIdentity();
			Camera4f &w(camera());
			Scene::Matrix &mw(w);
			t.translate(Scene::Vector(0, 0, float(d)/120, 0));
			mw = t*mw;
			return true;
		}
		virtual bool OnMouseMove(int x, int y)
		{
			/*Point2i b(x, y);
			Vec2i c(b-mMouse);
			//camera().pitch(-2.0f*sgn(c[0]));
			camera().yaw(2.0f*sgn(c[1]));
			mMouse = b;*/
			return	true;
		}
	};

}//namespace

#endif//__MAIN_H__
