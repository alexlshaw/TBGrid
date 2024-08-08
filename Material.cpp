#include "Material.h"

Material::Material(Shader* shader, Texture* texture) 
	: lit(false), shader(shader), texture(texture), ambientUniform(-1), diffuseUniform(-1), lightPositionUniform(-1), normalMatrix(-1), enableBlending(false)
{
	//viewMatrix = shader->getUniformLocation("viewMatrix");
	projectionViewMatrix = shader->getUniformLocation("projectionViewMatrix");
	modelMatrix = shader->getUniformLocation("modelMatrix");
	textureUniform = shader->getUniformLocation("tex");
}

void Material::setLit(bool val)
{
	lit = val;
	if (lit)
	{
		ambientUniform = shader->getUniformLocation("ambient");
		diffuseUniform = shader->getUniformLocation("diffuse");
		lightPositionUniform = shader->getUniformLocation("lightPosition");
		normalMatrix = shader->getUniformLocation("normalMatrix");
	}
}

void Material::use(Camera* camera, Light light)
{
	if (enableBlending)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	shader->use();
	shader->setUniform(textureUniform, 0);
	//shader->setUniform(viewMatrix, camera->getViewMatrix());

	shader->setUniform(projectionViewMatrix, camera->getProjectionMatrix() * camera->getViewMatrix());

	if (lit)
	{
		shader->setUniform(ambientUniform, light.ambient);
		shader->setUniform(diffuseUniform, light.diffuse);
		shader->setUniform(lightPositionUniform, glm::normalize(light.position));
	}


	glActiveTexture(GL_TEXTURE0);
	texture->use();
}

void Material::setTransform(Transform* transform)
{
	//this function assumes that use() has already been called to activate the shader
	shader->setUniform(modelMatrix, transform->getMatrix());
	if (lit)
	{
		shader->setUniform(normalMatrix, transform->getNormalMatrix());
	}
}