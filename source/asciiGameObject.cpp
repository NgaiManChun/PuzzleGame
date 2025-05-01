#include "asciiGameObject.h"

AsciiGameObject::AsciiGameObject(F3 size, F3 position, Texture* texture,
	F3 rotate,
	int row, int col, std::string value,
	float spacing,
	F4 color) :
	GameObjectQuad(size, position, texture, rotate, color), row(row), col(col), value(value.data()), spacing(spacing)
{}

void AsciiGameObject::SetValue(std::string value) {
	this->value = value.data();
}

void AsciiGameObject::Draw() {
	const char* chars = value.c_str();
	F2 uvRange = F2{ 1.0f / row, 1.0f / col };
	float width = value.size() * size.x + spacing * (value.size() - 1);
	float currX = position.x;
	float currY = position.y;
	if (originH == AsciiGameObjectOriginH::HCENTER) {
		currX = position.x - width * 0.5f + size.x * 0.5f;
	}
	else if (originH == AsciiGameObjectOriginH::LEFT) {
		currX = position.x + size.x * 0.5f;
	}
	else if (originH == AsciiGameObjectOriginH::RIGHT) {
		currX = position.x - width + size.x * 0.5f;
	}
	if (originV == AsciiGameObjectOriginV::VCENTER) {
		currY = position.y;
	}
	else if (originV == AsciiGameObjectOriginV::TOP) {
		currY = position.y + size.x * 0.5f;
	}
	else if (originV == AsciiGameObjectOriginV::BOTTOM) {
		currY = position.y - size.x * 0.5f;
	}

	int length = value.length();
	for (int i = 0; i < length; i++) {
		int index = chars[i] - 32;
		F2 uvOffset = F2{ uvRange.x * (index % col), uvRange.y * (index / col) };
		DrawQuad(texture, F3{ currX, currY, position.z }, size, rotate, color, uvOffset, uvRange);
		currX += size.x + spacing;
	}
}