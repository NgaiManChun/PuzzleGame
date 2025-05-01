#ifndef _ASCII_GAME_OBJECT_H
#define _ASCII_GAME_OBJECT_H

#include "gameObject.h"
using namespace MG;

enum AsciiGameObjectOriginH {
	HCENTER,
	LEFT,
	RIGHT
};

enum AsciiGameObjectOriginV {
	VCENTER,
	TOP,
	BOTTOM
};

class AsciiGameObject : public GameObjectQuad {
protected:
	int row;
	int col;
public:
	std::string value;
	float spacing;
	AsciiGameObjectOriginH originH;
	AsciiGameObjectOriginV originV;

	AsciiGameObject(F3 size, F3 position, Texture* texture,
		F3 rotate,
		int row, int col, std::string value,
		float spacing = 0.0f,
		F4 color = F4{ 1.0f, 1.0f, 1.0f, 1.0f });
	void SetValue(std::string value);
	void Draw() override;
};

#endif