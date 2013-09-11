#pragma once

#include <utility>

#include "cinder/app/AppBasic.h"

using namespace ci;

class Circle
{
public:
	Circle(Vec2f pos = Vec2f(0.0f, 0.0f), float radius = 1.0f, Color color = Color(1.0f, 0.0f, 0.0f))
		: mPos(pos), mRadius(radius), mColor(color) {};
	~Circle() {};

	void setPos(Vec2f pos) { mPos = pos; }
	Vec2f getPos() const { return mPos; }
	void setRadius(float radius) { mRadius = radius; }
	float getRadius() const { return mRadius; }
	void setColor(Color color) { mColor = color; }
	Color getColor() const { return mColor; }

	void draw(int w, int h) const
	{
		gl::color(mColor);
		gl::drawSolidCircle(Vec2f(mPos.x * w, mPos.y * h), mRadius * w);
	}

private:
	Vec2f mPos;
	float mRadius;
	Color mColor;
};