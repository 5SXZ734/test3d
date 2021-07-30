#include "main.h"

#include "stdafx.h"

#include <iostream>
#include "canvas.h"
#include "vector.h"


using namespace My;

class Test1 : public ITest
{
	class My2D : public Scene2D
	{
		MyObj *mpObj;
		std::vector<MyMeshLine *>	mpl;
		ClipCohenSutherland<Vertex> CLIP;
	public:
		My2D(Rectf &v)
			: CLIP(v),
			mpObj(NULL)
		{
			addAxes();
			MyObj &o(*addObject());
			MyMeshLine &l(*o.addLine());
			for (float x(-4.0); x <= 4.0; x += 0.1f)
			{
				float y(float(sin(3.14159*x) / (3.14159*x)));
				l.addVertex(Point2f(x, y));
			}
		}
	protected:
		virtual IClip<Vertex> *clipper(){ return &CLIP; }
		virtual bool OnMouse(const Vertex &b, int)
		{
			if (!mpObj)
				mpObj = addObject();
			if (mpl.empty() || mpl.back()->size() > 1)
			{
				mpl.push_back(mpObj->addLine());
				mpl.back()->setEndPoints(3);
			}
			mpl.back()->addVertex(b);
			if (mpl.size() == 2 && mpl.back()->size() == 2)
			{
				Vertex c;
				if (mpl[0]->intersectsAt(mpl[1], c))
					mpObj->addPoint(c, 5);
				mpl.clear();
				mpObj = NULL;
			}
			return true;
		}
	};

	ObjScene2D *mpS2DObj;
public:
	Test1()
		: mpS2DObj(NULL)
	{
	}

	virtual void OnSetup()
	{
		setViewport(Recti(Point2i(40, 20), Sizei(640, 480)));
		Scene &S0(newScene());
		//setCamera(Camera4f(53.0f, vewport().ratio<float>(), 1, 100))
		setCamera(Camera4f(26.0f, 10, 1, 100))
			.set(Point3f(0,0,5), Point3f(0,0,0), Vec3f(0,1,0));

		mpS2DObj = S0.addScene2D(new My2D(Rectf(Point2f(-5, -0.4f), Sizef(10, 2))));
	}

	virtual bool OnMouse(const Scene::MyLine &L, int btn)
	{
		return mpS2DObj->scene()->OnMouse(mpS2DObj->hit(L), btn);
	}
};


class Test2 : public ITest
{
	class My2D : public Scene2D
	{
		ClipCohenSutherland<Vertex>	CLIP;
	public:
		Scene2D::MyObj *mpObj;
		My2D(Rectf &v)
			: CLIP(v)
		{
			addAxes();
			mpObj = addHexSwirl(Point2f(0, 0));
			//mpObj = addObject();
			//mpObj->addLine(Vertex(-5, -5, 1), Vertex(5,5,1));
		}
		virtual IClip<Vertex> *clipper(){ return &CLIP; }
		virtual bool OnKeyDown(WPARAM wParam)
		{
			switch (wParam)
			{
			case 0x41: mpObj->translate(-0.1f, 0); return true;//A
			case 0x44: mpObj->translate(+0.1f, 0); return true;//D
			case 0x57: mpObj->translate(0, +0.1f); return true;//W
			case 0x53: mpObj->translate(0, -0.1f); return true;//S
			//case 0x5A: mpObj->translate(0, 0, -0.1f); return true;//Z
			//case 0x58: mpObj->translate(0, 0, +0.1f); return true;//X
			case 0x45: mpObj->rotate(-2.0f); return true;//Q
			case 0x51: mpObj->rotate(+2.0f); return true;//E
			default:
				//return ITest::OnKeyDown(wParam);
				break;
			}
			return false;
		}
	};
	My2D *mpScene2D;
public:
	Test2()
	{
	}

	virtual void OnSetup()
	{
		setViewport(Recti(Point2i(100, 100), Sizei(640, 480)));
		Scene &S0(newScene());
		setCamera(Camera4f(53.0f, vewport().ratio<float>(), 1, 100))
			.set(Point3f(0,0,15), Point3f(0,0,0), Vec3f(0,1,0));

		mpScene2D = (My2D *)S0.addScene2D(new My2D(Rectf(Point2f(-1, -1), Point2f(1, 1))))->scene();
	}

	virtual bool OnKeyDown(WPARAM wParam)
	{
		if (!mpScene2D->OnKeyDown(wParam))
			return ITest::OnKeyDown(wParam);
		return true;
	}
};

class Test3 : public ITest
{
	class My2D : public Scene2D
	{
	public:
		My2D()
		{
			addAxes();
			addHexSwirl(Point2f(0, 0));
		}
	};
public:
	Test3()
	{
	}

	virtual void OnSetup()
	{
		setViewport(Recti(Point2i(100, 100), Sizei(200, 200)));
		setCamera(Camera4f(53.0f, vewport().ratio<float>(), 1, 100))
			.set(Point3f(0,0,5), Point3f(0,0,0), Vec3f(0,1,0));
		for (int i(0); i < 12; i++)
		{
			Scene &S0(newScene());
			S0.addScene2D(new My2D);
		}
	}

	virtual void OnDraw(HDC hdc)
	{
		Canvas canvas(hdc);
		for (int i(0); i < scenesTotal(); i++)
		{
			canvas.setPerspective(camera().projectionMatrix());
			Recti vp(vewport());
			vp.translate((i % 4)*vp.width(), (i / 4)*vp.height());
			canvas.setViewport(vp);
			canvas.draw(scene(i), camera());
		}
	}
};

class Test4 : public ITest
{
public:
	Test4()
	{
	}

	virtual void OnSetup()
	{
		setViewport(Recti(Point2i(100, 100), Sizei(200, 200)));
		setCamera(Camera4f(53.0f, vewport().ratio<float>(), 1, 100))
			.set(Point3f(0,0,3), Point3f(0,0,0), Vec3f(0,1,0));

		Scene &S01(newScene());
		Scene2D &S1(*S01.addScene2D(new Scene2D)->scene());
		S1.addRosette(5, 1.0f);

		Scene &S02(newScene());
		Scene2D &S2(*S02.addScene2D(new Scene2D)->scene());
		S2.addRosette(11, 1.0f);

		Scene &S03(newScene());
		Scene2D &S3(*S03.addScene2D(new Scene2D)->scene());
		S3.addRosette(17, 1.0f);
		S3.scale(0.5, 2, 0);

		Scene &S04(newScene());
		Scene2D &S4(*S04.addScene2D(new Scene2D)->scene());
		S4.addArc(Point2f(0, 0), 1, 0, 360);
	}

	virtual void OnDraw(HDC hdc)
	{
		Canvas canvas(hdc);

		for (int i(0); i < scenesTotal(); i++)
		{
			canvas.setPerspective(camera().projectionMatrix());
			Recti vp(vewport());
			vp.translate((i % 3)*vp.width(), (i / 3)*vp.height());
			canvas.setViewport(vp);
			canvas.draw(scene(i), camera());
		}
	}
};

class Test5 : public ITest
{
	class My2D : public Scene2D
	{
		MyObj *mpObj;
		std::vector<MyMeshPoint *>	mpt;
		std::vector<MyMeshLine *>	mpl;
	public:
		My2D()
			: mpObj(0)
		{
			addAxes();
			///
			mpObj = addObject();
			mpObj->addPoint(Vertex(1,0,1), 3);
			mpObj->addPoint(Vertex(0,1,1), 3);
			mpObj->addPoint(Vertex(-1,0,1), 3);
			add3PointCircle(
				Vertex(1,0,1),
				Vertex(0,1,1),
				Vertex(-1,0,1));
			mpObj = NULL;
		}
	protected:
		virtual bool OnMouse(const Vertex &b, int)
		{
			if (!mpObj)
				mpObj = addObject();
			mpt.push_back(mpObj->addPoint(b, 3));
			if (mpt.size() == 3)
			{
				add3PointCircle(
					mpt[mpt.size() - 3]->at(0),
					mpt[mpt.size() - 2]->at(0),
					mpt[mpt.size() - 1]->at(0));

				mpt.clear();
				mpObj = NULL;
			}
			return true;
		}
	};

	ObjScene2D *mpS2DObj;
public:
	Test5() : mpS2DObj(0){}

	virtual void OnSetup()
	{
		Scene &S0(newScene());
		setCamera(Camera4f(53.0f, vewport().ratio<float>(), 1, 100))
			.set(Point3f(0,0,6), Point3f(0,0,0), Vec3f(0,1,0));

		setViewport(Recti(Point2i(40, 20), Sizei(640, 480)));

		mpS2DObj = S0.addScene2D(new My2D);
	}

	virtual bool OnMouse(const Scene::MyLine &L, int btn)
	{
		return mpS2DObj->scene()->OnMouse(mpS2DObj->hit(L), btn);
	}
};

class Test6 : public ITest
{
	class My2D : public Scene2D
	{
		MyObj *mpObj, *mpObj2;
		MyMeshLine *mpl;
		//CyrusBeckClip<Vertex> CLIP;
		//CyrusBeckClipEx<Vertex> CLIP;
		ClipSutherlandHodgman<Vertex> CLIP;
	public:
		My2D(const Rectf &v)
			: //CLIP(v),
			mpObj(0), mpObj2(0),
			mpl(NULL)
		{
			addAxes();
		}
		virtual IClip<Vertex> *clipper(){ return &CLIP; }
		virtual bool OnMouse(const Vertex &b, int btn)
		{
			if (!mpObj)
			{
				removeChildren();
				addAxes();
				mpObj = addObject();
			}
			if (!mpl)
			{
				mpl = mpObj->addLine();
				mpl->setColor(0, 0, 255);
				mpl->setEndPoints(3);
			}
			mpl->addVertex(b);
			if (btn == 1)//right click
			{
				mpl->setClosed(true);

				MyObj *o(addObject());
				if (CLIP.isEmpty())
					CLIP.setBounds(mpl->vertices());
				for (int i(0); i < 100; i++)
				{
					Segment<Vertex> seg(
						Vertex(float(rand()%10-5), float(rand()%10-5), 1),
						Vertex(float(rand()%10-5), float(rand()%10-5), 1)
						);
					o->addLine(seg.a, seg.b);
	
				}

				mpl = NULL;
				mpObj2 = mpObj;
				mpObj = NULL;
			}
			return true;
		}
		virtual bool OnKeyDown(WPARAM wParam)
		{
			switch (wParam)
			{
			case 0x41: mpObj2->translate(-0.1f, 0); return true;//A
			case 0x44: mpObj2->translate(+0.1f, 0); return true;//D
			case 0x57: mpObj2->translate(0, +0.1f); return true;//W
			case 0x53: mpObj2->translate(0, -0.1f); return true;//S
			case 0x45: mpObj2->rotate(-2.0f); return true;//Q
			case 0x51: mpObj2->rotate(+2.0f); return true;//E
			default:
				break;
			}
			return false;
		}
	};

	ObjScene2D *mpS2DObj;

public:
	Test6()
		: mpS2DObj(NULL)
	{
	}

	virtual void OnSetup()
	{
		setCamera(Camera4f(53.0f, vewport().ratio<float>(), 1, 100))
			.set(Point3f(0,0,6), Point3f(0,0,0), Vec3f(0,1,0));

		setViewport(Recti(Point2i(100, 100), Sizei(640, 480)));

		Scene &S0(newScene());

		mpS2DObj = S0.addScene2D(new My2D(Rectf(Point2f(-2, -2), Point2f(2, 2))));
	}

	virtual bool OnMouse(const Scene::MyLine &L, int btn)
	{
		return mpS2DObj->scene()->OnMouse(mpS2DObj->hit(L), btn);
	}

	virtual bool OnKeyDown(WPARAM wParam)
	{
		if (!mpS2DObj->scene()->OnKeyDown(wParam))
			return ITest::OnKeyDown(wParam);
		return true;
	}
};

class Test7 : public ITest
{
	Scene::MyObj *mpObj;
public:
	Test7()
		: mpObj(NULL)
	{
	}

	virtual void OnSetup()
	{
		setViewport(Recti(Point2i(100, 100), Sizei(640, 480)));
		Scene &S0(newScene());
		setCamera(Camera4f(53.0f, vewport().ratio<float>(), 1, 100))
			//.set(Point3f(0,1,5), Point3f(0,1,0), Vec3f(0,1,0));
			.set(Point3f(5,5,5), Point3f(0,0,0), Vec3f(0,1,0));

		S0.addFloor();
		S0.addAxes();
#if(1)
		mpObj = S0.addSphere(24, 16);
		mpObj->scale(4,4,4);
		mpObj->translate(0,0,-6);

//		mpObj = S0.addCube();
		//mpObj->translate(-0.5, 0, 0);
		//mpObj->translate(0, -0.5, 0);
		//mpObj->translate(0, 0, 0.5);
//		mpObj->translate(-0.5, -0.5, 0.5);
		//mpObj->scale(2,2,2);

#else
		mpObj = S0.addObject();
		mpObj->addPoint(Scene::Vertex(1, 1, -1, 1), 5);
#endif
	}

	virtual bool OnKeyDown(WPARAM wParam)
	{
		switch (wParam)
		{
		case 0x41: mpObj->translate(-0.1f, 0, 0); return true;//A
		case 0x44: mpObj->translate(+0.1f, 0, 0); return true;//D
		case 0x57: mpObj->translate(0, +0.1f, 0); return true;//W
		case 0x53: mpObj->translate(0, -0.1f, 0); return true;//S
		case 0x5A: mpObj->translate(0, 0, -0.1f); return true;//Z
		case 0x58: mpObj->translate(0, 0, +0.1f); return true;//X
		case 0x45: mpObj->rotate(-2.0f); return true;//Q
		case 0x51: mpObj->rotate(+2.0f); return true;//E
		default:
			return ITest::OnKeyDown(wParam);
		}
		return false;
	}

	/*virtual bool OnMouseWheel(int d)
	{
		Camera4f &w(camera());
		//w.setViewAngle(w.viewAngle()+float(d)/120);
		w.slide(0, 0, float(d)/120);
		return true;
	}*/

};

Test7 test;

