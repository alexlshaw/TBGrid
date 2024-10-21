#include "UIImageElement.h"

UIImageElement::UIImageElement(glm::vec2 position, glm::vec2 size, Texture* image)
	:	UIElement(position, size),
		image(image)
{
}

void UIImageElement::draw(Shader* shader, const glm::mat4& projection) const
{
	if (image != nullptr)
	{
		image->use();
		shader->use();
		shader->setUniform("text", 0);
		shader->setUniform("projection", projection);
		glm::mat4 model{ 1.0f };
		glm::vec2 effectivePosition = computeEffectivePosition();
		model = glm::translate(model, { effectivePosition.x, effectivePosition.y, 0.0 });
		model = glm::scale(model, { size.x, size.y, 0.0f });
		shader->setUniform("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}
