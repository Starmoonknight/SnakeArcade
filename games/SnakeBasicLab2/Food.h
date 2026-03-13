// Food.h
#pragma once
#include "Vec2.h"

struct Food
{
public:

	bool HasActiveFruit() const { return m_active; }
	Vec2 GetFruitCoord() const { return m_position; }

	void SetFruitStatus(bool active) { m_active = active; }
	void SetFruitStatus(bool active, Vec2 pos) { m_active = active; m_position = pos; }

private:
	Vec2 m_position; 
	bool m_active{ false };
};