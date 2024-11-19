#include "Material.h"
#include "DebuggingTools.h"
#include <format>

Material::Material(std::string name, Shader* shader, Texture* texture) 
	: name(name), 
	lit(false),
	useNormals(false),
	shader(shader), 
	texture(texture),
	textureUniform(-1),
	viewPosUniform(-1),
	lightUniformBlockIndex(-1),
	uboLight(-1),
	shininess(1.0f),
	normalMatrix(-1), 
	enableBlending(false)
{
	projectionViewMatrix = shader->getUniformLocation("projectionViewMatrix");
	modelMatrix = shader->getUniformLocation("modelMatrix");
	if (texture)
	{
		textureUniform = shader->getUniformLocation("tex");
	}
}

void Material::setLit(bool val)
{
	lit = val;
	if (lit)
	{
		setUseNormals(true);
		viewPosUniform = shader->getUniformLocation("viewPos");
		shininessUniform = shader->getUniformLocation("shininess");
		lightUniformBlockIndex = shader->getUniformBlockLocation("LightBlock");
		glUniformBlockBinding(shader->getHandle(), lightUniformBlockIndex, 0);
		//create the uniform buffer object
		glGenBuffers(1, &uboLight);
		glBindBuffer(GL_UNIFORM_BUFFER, uboLight);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlock), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboLight, 0, sizeof(LightBlock));
	}
}

void Material::setUseNormals(bool val)
{
	useNormals = val;
	if (useNormals)
	{
		normalMatrix = shader->getUniformLocation("normalMatrix");
	}
}

void Material::use(Camera* camera, const LightBlock& lights)
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
	if (texture)
	{
		shader->setUniform(textureUniform, 0);
	}

	shader->setUniform(projectionViewMatrix, camera->getProjectionMatrix() * camera->getViewMatrix());

	if (lit)
	{
		shader->setUniform(viewPosUniform, camera->transform.getPosition());
		shader->setUniform(shininessUniform, shininess);
		glBindBuffer(GL_UNIFORM_BUFFER, uboLight);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlock), &lights, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		DEBUG_PRINT_GL_ERRORS("Material.cpp: use()");
	}
	//We have an arbitrary set of extra properties, set them all with the corresponding values
	for (auto& prop : vectorPropertyValues)
	{
		int uniform = vectorPropertyUniforms[prop.first];
		shader->setUniform(uniform, prop.second);
	}
	for (auto& prop : floatPropertyValues)
	{
		int uniform = floatPropertyUniforms[prop.first];
		shader->setUniform(uniform, prop.second);
	}
	if (texture)
	{
		glActiveTexture(GL_TEXTURE0);
		texture->use();
	}
}

void Material::setTransform(Transform transform)
{
	//this function assumes that use() has already been called to activate the shader
	shader->setUniform(modelMatrix, transform.getMatrix());
	if (useNormals)
	{
		shader->setUniform(normalMatrix, transform.getNormalMatrix());
	}
}

void Material::setProperty(std::string propertyName, glm::vec4 propertyValue, bool createIfMissing)
{
	if (vectorPropertyValues.contains(propertyName))
	{
		vectorPropertyValues[propertyName] = propertyValue;
	}
	else if (createIfMissing)
	{
		//trying to set a property which we were hitherto unaware existed
		int loc = shader->getUniformLocation(propertyName.c_str());
		if (loc != -1)
		{
			//great, it exists in the shader
			vectorPropertyValues[propertyName] = propertyValue;
			vectorPropertyUniforms[propertyName] = loc;
		}
	}
	else
	{
		DEBUG_PRINTLN("Attempting to set nonexistent property: " + propertyName + " on material: " + name);
	}
}

void Material::setProperty(std::string propertyName, float propertyValue, bool createIfMissing)
{
	if (floatPropertyValues.contains(propertyName))
	{
		floatPropertyValues[propertyName] = propertyValue;
	}
	else if (createIfMissing)
	{
		//trying to set a property which we were hitherto unaware existed
		int loc = shader->getUniformLocation(propertyName.c_str());
		if (loc != -1)
		{
			//great, it exists in the shader
			floatPropertyValues[propertyName] = propertyValue;
			floatPropertyUniforms[propertyName] = loc;
		}
	}
	else
	{
		DEBUG_PRINTLN("Attempting to set nonexistent property: " + propertyName + " on material: " + name);
	}
}
