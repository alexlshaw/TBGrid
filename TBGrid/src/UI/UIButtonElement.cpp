#include "UIButtonElement.h"

UIButtonElement::UIButtonElement(glm::vec2 position, glm::vec2 size, Texture* image, std::string label)
	: UIElement(position, size)
{
	if (image != nullptr)
	{
		std::shared_ptr<UIImageElement> buttonImage = std::make_shared<UIImageElement>(glm::vec2{}, size, image);
		addChild(buttonImage);
		hasImage = true;
	}
	if (label.length() > 0)
	{
		glm::vec2 pos = computeTextCentering(label, 1.0f, GraphicsResourceManager::getInstance().defaultFont);
		std::shared_ptr<UITextElement> buttonText = std::make_shared<UITextElement>(label, pos, 1.0f, glm::vec3{});
		addChild(buttonText);
		hasLabel = true;
	}
}

void UIButtonElement::setLabel(std::string labelText, float textScale, glm::vec3 textColor)
{
	if (hasLabel)
	{
		int idx = hasImage ? 1 : 0;
		auto element = dynamic_cast<UITextElement*>(children[idx].get());
		if (element)
		{
			element->text = labelText;
			element->scale = textScale;
			element->color = textColor;
		}
	}
}

bool UIButtonElement::consumeClick(glm::vec2 clickLocation)
{
	//Note that a button does not pass the click event down to any child elements
	//So don't nest buttons
	if (contains(clickLocation))
	{
		if (onClick)
		{
			onClick();
		}
		return true;
	}
	return false;
}

glm::vec2 UIButtonElement::computeTextCentering(std::string_view text, const float scale, const FontAtlas* font) const
{
	float totalWidth = 0.0f;
	for (const char& c : text)
	{
		Character character = font->characters[c];
		//for the width, we have to account for spaces between letters, so we use the advance
		totalWidth += static_cast<float>(character.ax);
	}
	float halfWidth = totalWidth * scale * 0.5f;
	float x = (size.x * 0.5f) - halfWidth;

	//For the height, we just use the actual bitmap height of a "normal", full size letter
	float halfHeight = static_cast<float>(font->characters['A'].bh) * scale * 0.5f;
	float y = (size.y * 0.5f) - halfHeight;
	return glm::vec2(x, y);
}