#include <algorithm>
#include <cmath>
#include <random>

#include "cinder/app/AppNative.h"
#ifdef CINDER_WINRT
#include "cinder/app/RendererDx.h"
#include "cinder/dx/dx.h"
#include "cinder/dx/DxTexture.h"
namespace gl = cinder::dx;
#else
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#endif
#include "cinder/Text.h"
#include "cinder/Font.h"

#include "Circle.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CircleTouchApp : public AppNative {
 public:
	CircleTouchApp();
	void prepareSettings(Settings *settings);
	void setup();
	void mouseDown(MouseEvent event);
	void keyDown(KeyEvent event);
	void update();
	void reset();
	void draw();
	void drawCircles();
	void drawText(std::string text, Vec2f pos, Color color);
	void drawScore();
	void drawLevel();
	void drawNumCircles();
	void drawLives();
	void drawLostLife();
	void drawPause();
	void drawPaused();
	void drawGameOver();

private:
	random_device mDev;
	mt19937 mEng;
	double mLastUpdateTime;
	double mCirclesResidue;
	double mCirclesPerSec;
	double mRadiusPerSec;
	vector<Circle> mCircles;

	bool mRunOnce;
	bool mGameOver;
	bool mPaused;
	unsigned int mLevel;
	double mScore;
	unsigned int mNumCirclesPopped;
	unsigned int mNumLives;
	bool mShowLostLife;
	unsigned int mRestartTaps;

	static const double MAX_CIRCLE_AREA;
	static const unsigned int CIRCLES_PER_LEVEL;
	static const float PAUSE_Y_POS;
	static const float PAUSE_WIDTH;
	static const float PAUSE_HEIGHT;
};

const double CircleTouchApp::MAX_CIRCLE_AREA = 0.5;
const unsigned int CircleTouchApp::CIRCLES_PER_LEVEL = 20;
const float CircleTouchApp::PAUSE_Y_POS = 20.0f;
const float CircleTouchApp::PAUSE_WIDTH = 60.0f;
const float CircleTouchApp::PAUSE_HEIGHT = 60.0f;


CircleTouchApp::CircleTouchApp()
: mDev(), mEng(mDev()), mLastUpdateTime(0.0),
mCirclesResidue(0.0), mCirclesPerSec(1.0),
mRadiusPerSec(0.1), mCircles(), mRunOnce(false)
{
}

void CircleTouchApp::prepareSettings(Settings *settings)
{
	settings->setTitle("CircleTouch");
	settings->setResizable(true);
}

void CircleTouchApp::setup()
{
	reset();
}

void CircleTouchApp::mouseDown(MouseEvent event)
{
	auto pos(event.getPos());
	auto w(getWindowWidth());
	auto h(getWindowHeight());
	Vec2f sp(static_cast<float>(pos.x) / w, static_cast<float>(pos.y) / h);

	/* Check for pause/unpause. */
	if ((((w - PAUSE_WIDTH) / 2.0f) <= pos.x) && (pos.x <= ((w + PAUSE_WIDTH) / 2.0f))
		&& ((PAUSE_Y_POS <= pos.y) && (pos.y <= (PAUSE_Y_POS + PAUSE_HEIGHT))))
	{
		if (mGameOver || !mRunOnce)
		{
			mRunOnce = true;
			reset();
		}
		else
		{
			mPaused = !mPaused;
			mLastUpdateTime = getElapsedSeconds();
		}
	}
	else if (mGameOver || mPaused)
	{
		return;
	}

	for(auto it = begin(mCircles); it != end(mCircles); )
	{
		auto c(*it);
		auto p(c.getPos());
		if( (fabs(p.x - sp.x) < c.getRadius())
			&& (fabs(p.y - sp.y) < c.getRadius() * w / h) )
		{
			auto circle = *it;
			mScore += std::max(0.0, (MAX_CIRCLE_AREA - circle.getArea()) * 10.0 * mLevel);
			++mNumCirclesPopped;
			if (mNumCirclesPopped >= CIRCLES_PER_LEVEL)
			{
				++mLevel;
				mNumCirclesPopped = 0;
			}
			it = mCircles.erase(it);

			// Only allow one circle to be popped for each tap.
			return;
		}
		else
		{
			++it;
		}
	}
}

void CircleTouchApp::keyDown(KeyEvent event)
{
	auto key = event.getCode();
	switch (key)
	{
	case KeyEvent::KEY_F11:
		setFullScreen(!isFullScreen());
		break;
	case KeyEvent::KEY_SPACE:
		if (mGameOver || !mRunOnce)
		{
			mRunOnce = true;
			reset();
		}
		else
		{
			mPaused = !mPaused;
			mLastUpdateTime = getElapsedSeconds();
		}
		break;
	}
}

void CircleTouchApp::update()
{
	if (mGameOver || mPaused || !mRunOnce)
	{
		return;
	}
	auto now = getElapsedSeconds();
	auto diff = now - mLastUpdateTime;

	uniform_real_distribution<float> distW(0.0f, 1.0f);
	uniform_real_distribution<float> distH(0.0f, 1.0f);
	uniform_real_distribution<float> distCol(0.0f, 1.0f);
	mCirclesResidue += diff * mCirclesPerSec * (1.0 + mLevel / 5.0);
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
	auto radiusInc = static_cast<float>(diff * mRadiusPerSec * (1.0 + mLevel / 5.0));
	for (auto it = begin(mCircles); it != end(mCircles); )
	{
		auto &c(*it);
		c.setRadius(c.getRadius() + radiusInc);
		if (c.getArea() > MAX_CIRCLE_AREA)
		{
			mShowLostLife = true;
			--mNumLives;
			if (mNumLives == 0)
			{
				mGameOver = true;
				mRunOnce = true;
			}
			it = mCircles.erase(it);
		}
		else
		{
			++it;
		}
	}

	mLastUpdateTime = now;
}

void CircleTouchApp::reset()
{
	mCircles.clear();
	mLastUpdateTime = getElapsedSeconds();
	mLevel = 1;
	mNumCirclesPopped = 0;
	mScore = 0;
	mNumLives = 5;
	mShowLostLife = false;
	mRestartTaps = 0;
	mGameOver = false;
	mPaused = false;
}

void CircleTouchApp::draw()
{
	gl::clear(Color(0.1f, 0.1f, 0.1f));
	gl::setMatricesWindow(getWindowSize());
	gl::enableAlphaBlending(false);

	drawCircles();
	drawLevel();
	drawScore();
	drawNumCircles();
	drawLives();
	drawLostLife();
	drawPause();
	drawPaused();
	drawGameOver();
}

void CircleTouchApp::drawCircles()
{
	auto w = getWindowWidth();
	auto h = getWindowHeight();
	for (const auto& c : mCircles)
	{
		c.draw(w, h);
	}
}

void CircleTouchApp::drawText(std::string text, Vec2f pos, Color color)
{
	TextLayout layout;
	layout.setFont(Font("Arial", 24));
	layout.setColor(color);
	layout.addLine(text);
	layout.addLine(""); // Fix while single line winrt text is rendered incorrect vertically.
	gl::color(color);
	auto tex = gl::Texture::create(layout.render(true, false));
	if (pos.x < 0.0)
	{
		pos.x = getWindowWidth() - tex->getWidth() + pos.x;
	}
	if (pos.y < 0.0)
	{
		pos.y = getWindowHeight() - tex->getHeight() + pos.y;
	}
	gl::draw(tex, pos);
}

void CircleTouchApp::drawLevel()
{
	drawText("Level: " + std::to_string(mLevel), Vec2f(10, 10), Color::white());
}

void CircleTouchApp::drawScore()
{
	drawText("Score: " + std::to_string(static_cast<int>(mScore)), Vec2f(-10, 10), Color::white());
}

void CircleTouchApp::drawNumCircles()
{
	drawText("Remaining Circles: " + std::to_string(CIRCLES_PER_LEVEL - mNumCirclesPopped), Vec2f(10, -10), Color::white());
}

void CircleTouchApp::drawLives()
{
	auto color = (mNumLives <= 1) ? Color("red") : Color::white();
	drawText("Lives: " + std::to_string(mNumLives), Vec2f(-10, -10), color);
}

void CircleTouchApp::drawLostLife()
{
	if (mShowLostLife)
	{
		gl::color(Color(1.0f, 0.5f, 0.5f));
		gl::drawSolidRect(Rectf(Vec2f(0.0f, 0.0f),
			Vec2f(static_cast<float>(getWindowWidth()),
			static_cast<float>(getWindowHeight()))));
	}
	mShowLostLife = false;
}

void CircleTouchApp::drawPause()
{
	const auto winW = static_cast<float>(getWindowWidth());
	const auto xPos = winW / 2.0f;
	const auto wOver2 = PAUSE_WIDTH / 2.0f;
	const auto wOver6 = PAUSE_WIDTH / 6.0f;

	gl::color(Color(0.5f, 0.5f, 0.5f));
	if (mPaused || mGameOver || !mRunOnce)
	{
		gl::drawSolidTriangle(Vec2f(xPos - wOver2, PAUSE_Y_POS),
			Vec2f(xPos + wOver2, PAUSE_Y_POS + PAUSE_HEIGHT / 2.0f),
			Vec2f(xPos - wOver2, PAUSE_Y_POS + PAUSE_HEIGHT));
	}
	else
	{
		gl::drawSolidRect(Rectf(Vec2f(xPos - wOver2, PAUSE_Y_POS), Vec2f(xPos - wOver6, PAUSE_Y_POS + PAUSE_HEIGHT)));
		gl::drawSolidRect(Rectf(Vec2f(xPos + wOver2, PAUSE_Y_POS), Vec2f(xPos + wOver6, PAUSE_Y_POS + PAUSE_HEIGHT)));
	}
}

void CircleTouchApp::drawPaused()
{
	if (!mPaused)
	{
		return;
	}
	TextLayout layout;
	layout.setFont(Font("Arial", 60));
	layout.setColor(Color::white());
	layout.addCenteredLine("Paused");
	layout.setFont(Font("Arial", 24));
	layout.addCenteredLine("Spacebar to continue");
	layout.addCenteredLine(""); // Workaround for winrt.
	gl::color(Color::white());
	auto tex = gl::Texture::create(layout.render(true, false));
	gl::draw(tex, Vec2f((getWindowWidth() - tex->getWidth()) / 2.0F, (getWindowHeight() - tex->getHeight()) / 2.0F));
}

void CircleTouchApp::drawGameOver()
{
	if (!mGameOver && mRunOnce)
	{
		return;
	}
	TextLayout layout;
	if (mGameOver)
	{
		layout.setFont(Font("Arial", 60));
		layout.setColor(Color("red"));
		layout.addCenteredLine("Game Over");
		layout.setFont(Font("Arial", 48));
		layout.addCenteredLine("Score: " + std::to_string(static_cast<int>(mScore)));
		layout.setFont(Font("Arial", 24));
		layout.addCenteredLine("Spacebar to restart");
		layout.addCenteredLine(""); // Workaround for winrt.
	}
	else
	{
		layout.setFont(Font("Arial", 60));
		layout.setColor(Color("white"));
		layout.addCenteredLine("Circle Touch");
		layout.setFont(Font("Arial", 24));
		layout.addCenteredLine("Spacebar to start");
		layout.addCenteredLine(""); // Workaround for winrt.
	}
	gl::color(Color("red"));
	auto tex = gl::Texture::create(layout.render(true, false));
	gl::draw(tex, Vec2f((getWindowWidth() - tex->getWidth()) / 2.0F, (getWindowHeight() - tex->getHeight()) / 2.0F));
}

#ifdef CINDER_WINRT
CINDER_APP_NATIVE(CircleTouchApp, RendererDx(0))
#else
CINDER_APP_NATIVE(CircleTouchApp, RendererGl(0))
#endif
