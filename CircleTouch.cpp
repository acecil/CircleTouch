#include <cmath>
#include <random>

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "Circle.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CircleTouchApp : public AppNative {
 public:
	CircleTouchApp()
		: mDev(), mEng(mDev()), mLastUpdateTime(0.0),
		mCirclesResidue(0.0), mCirclesPerSec(1.0), 
		mRadiusPerSec(0.1), mCircles() {}
	void prepareSettings(Settings *settings);
	void setup();
	void mouseDown(MouseEvent event);
	void keyDown(KeyEvent event);
	void update();
	void draw();

private:
	random_device mDev;
	mt19937 mEng;
	double mLastUpdateTime;
	double mCirclesResidue;
	double mCirclesPerSec;
	double mRadiusPerSec;
	vector<Circle> mCircles;
};

void CircleTouchApp::prepareSettings(Settings *settings)
{
	settings->setResizable(true);
}

void CircleTouchApp::setup()
{
	mLastUpdateTime = getElapsedSeconds();
}

void CircleTouchApp::mouseDown(MouseEvent event)
{		
	auto pos(event.getPos());
	auto w(getWindowWidth());
	auto h(getWindowHeight());
	Vec2f sp(static_cast<float>(pos.x) / w, static_cast<float>(pos.y) / h);

	for(auto it = begin(mCircles); it != end(mCircles); )
	{
		auto c(*it);
		auto p(c.getPos());
		if( (fabs(p.x - sp.x) < c.getRadius())
			&& (fabs(p.y - sp.y) < c.getRadius() * w / h) )
		{
			it = mCircles.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CircleTouchApp::keyDown(KeyEvent event)
{
	if(event.getCode() == KeyEvent::KEY_F11)
	{
		setFullScreen(!isFullScreen());
	}
	
}

void CircleTouchApp::update()
{
	auto now = getElapsedSeconds();
	auto diff = now - mLastUpdateTime;

	uniform_real_distribution<float> distW(0.0f, 1.0f);
	uniform_real_distribution<float> distH(0.0f, 1.0f);
	uniform_real_distribution<float> distCol(0.0f, 1.0f);
	mCirclesResidue += diff * mCirclesPerSec;
	auto numCircles = static_cast<int>(mCirclesResidue);
	mCirclesResidue -= numCircles;
	for(auto i = 0; i < numCircles; ++i)
	{
		auto w = distW(mEng);
		auto h = distH(mEng);
		auto r = distCol(mEng);
		auto g = distCol(mEng);
		auto b = distCol(mEng);
		mCircles.push_back(Circle(Vec2f(w, h), 0.0f, Color(r, g, b)));
	}
	auto radiusInc = static_cast<float>(diff * mRadiusPerSec);
	for(auto &c : mCircles)
	{
		c.setRadius(c.getRadius() + radiusInc);
	}

	mLastUpdateTime = now;
}

void CircleTouchApp::draw()
{
	auto w = getWindowWidth();
	auto h = getWindowHeight();
	gl::clear();
	for(const auto& c: mCircles)
	{
		c.draw(w, h);
	}
}

CINDER_APP_NATIVE(CircleTouchApp, RendererGl)