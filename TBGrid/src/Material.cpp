#include "Material.h"
#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include <filesystem>
#include <format>
#include <fstream>

Material::Material(std::string name, Shader* shader, Texture* diffuse, Texture* specular)
	: name(name),
	lit(false),
	useNormals(false),
	shader(shader),
	diffuseMap(diffuse),
	specularMap(specular),
	shininess(1.0f),
	enableBlending(false)
{
	projectionViewMatrix = shader->getUniformLocation("projectionViewMatrix");
	modelMatrix = shader->getUniformLocation("modelMatrix");
	if (diffuse)
	{
		diffuseTextureUniform = shader->getUniformLocation("diffuse");
	}
	if (specular)
	{
		specularTextureUniform = shader->getUniformLocation("specular");
	}
}

Material::Material(std::string name)
	: name(name)
{
	std::string fullFileName = "./Data/Materials/" + name + ".mat";
	//Somewhat a redundant check since the GRM also checks that it exists, but we check just in case we're loading it from outside the GRM
	if (std::filesystem::exists(fullFileName))
	{
		GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
		std::string line;
		std::ifstream fs(fullFileName.c_str());
		while (std::getline(fs, line))
		{
			if (line.at(0) != '#') //# denotes a comment in the material file
			{
				std::string varName = line.substr(0, line.find('='));
				std::string varValue = line.substr(line.find('=') + 1, line.length());
				//check for all of the variables in which we are interested
				if (varName == "shader")
				{
					shader = resourceManager.loadShader(varValue);
					projectionViewMatrix = shader->getUniformLocation("projectionViewMatrix");
					modelMatrix = shader->getUniformLocation("modelMatrix");
				}
				else if (varName == "diffuseMap")
				{
					Texture* tex = resourceManager.loadTexture(varValue);
					diffuseMap = tex != nullptr ? tex : resourceManager.defaultWhite;
					diffuseTextureUniform = shader->getUniformLocation("diffuseMap");
				}
				else if (varName == "specularMap")
				{
					Texture* tex = resourceManager.loadTexture(varValue);
					specularMap = tex != nullptr ? tex : resourceManager.defaultWhite;
					specularTextureUniform = shader->getUniformLocation("specularMap");
				}
				else if (varName == "ambientColour")
				{
					ambientColour = parseVector(varValue);
				}
				else if (varName == "diffuseColour")
				{
					diffuseColour = parseVector(varValue);
				}
				else if (varName == "specularColour")
				{
					specularColour = parseVector(varValue);
				}
				else if (varName == "shininess")
				{
					shininess = static_cast<float>(atof(varValue.c_str()));
					shininessUniform = shader->getUniformLocation("shininess");
				}
				else if (varName == "lit")
				{
					std::transform(varValue.begin(), varValue.end(), varValue.begin(), ::tolower);
					setLit(varValue == "true");
				}
				else if (varName == "use_normals")
				{
					std::transform(varValue.begin(), varValue.end(), varValue.begin(), ::tolower);
					setUseNormals(varValue == "true");
				}
				else if (varName == "transparent")
				{
					std::transform(varValue.begin(), varValue.end(), varValue.begin(), ::tolower);
					enableBlending = varValue == "true";
				}
				else
				{
					//we're into the arbitrary properties now
					if (varValue[0] == 'f')
					{
						float val = static_cast<float>(atof(varValue.substr(2, varValue.length() - 3).c_str()));
						setProperty(varName, val, true);
					}
					else if (varValue[0] == 'v')
					{
						glm::vec4 val = parseVector(varValue.substr(2, varValue.length() - 3));
						setProperty(varName, val, true);
					}
					else
					{
						DEBUG_PRINTLN(std::format("Material: {} failed to parse property: {}", name, line));
					}
				}
				//Attempt to extract colour uniforms from the shader (since we might be setting them regardless of whether the material file defined them)
				ambientColourUniform = shader->getUniformLocation("ambientColour");
				diffuseColourUniform = shader->getUniformLocation("diffuseColour");
				specularColourUniform = shader->getUniformLocation("specularColour");
			}
		}
		fs.close();
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
		shadowMapUniform = shader->getUniformLocation("shadowMap");
		lightSpaceMatrixUniform = shader->getUniformLocation("lightSpaceMatrix");
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

void Material::use(Camera* camera, const LightBlock& lights, const glm::mat4& lightSpaceMatrix)
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
	if (diffuseMap)
	{
		shader->setUniform(diffuseTextureUniform, TEXTURE_DIFFUSE);
		glActiveTexture(GL_TEXTURE_DIFFUSE);
		diffuseMap->use();
	}
	if (ambientColourUniform != -1)
	{
		shader->setUniform(ambientColourUniform, ambientColour);
	}
	if (diffuseColourUniform != -1)
	{
		shader->setUniform(diffuseColourUniform, diffuseColour);
	}
	if (specularColourUniform != -1)
	{
		shader->setUniform(specularColourUniform, specularColour);
	}
	if (specularMap)
	{
		shader->setUniform(specularTextureUniform, TEXTURE_SPECULAR);
		glActiveTexture(GL_TEXTURE_SPECULAR);
		specularMap->use();
	}

	shader->setUniform(projectionViewMatrix, camera->getProjectionMatrix() * camera->getViewMatrix());

	if (lit)
	{
		shader->setUniform(lightSpaceMatrixUniform, lightSpaceMatrix);
		shader->setUniform(shadowMapUniform, TEXTURE_SHADOW_MAP);
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
