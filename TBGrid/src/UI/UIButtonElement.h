#pragma once

#include <functional>
#include <string>
#include <string_view>
#include "FontAtlas.h"
#include "Texture.h"
#include "UIElement.h"
#include "UIImageElement.h"
#include "UITextElement.h"

class UIButtonElement : public UIElement
{
private:
	bool hasImage = false;
	bool hasLabel = false;
	glm::vec2 computeTextCentering(std::string_view text, const float scale, const FontAtlas* font) const;
public:
	std::function<void(void)> onClick;
	UIButtonElement(glm::vec2 position, glm::vec2 size, Texture* image, std::string label);
	~UIButtonElement() {}
	void setLabel(std::string labelText, float textScale = 1.0f, glm::vec3 textColor = glm::vec3{});
	bool consumeClick(glm::vec2 clickLocation) override;
};