#include "PBRViewerSkybox.h"

#include "PBRViewerObjectCreator.h"
#include "PBRViewerOpenGLUtilities.h"
#include "PBRViewerLogger.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <stb_image.h>

PBRViewerSkybox::PBRViewerSkybox( const std::string& filepathEnvironmentTexture )
{
	myFilepathEnvironmentTexture = filepathEnvironmentTexture;
	myEquirectangularToCubemapShader = std::make_unique<PBRViewerShader>("EquirectangularToCubemap.vert",
	                                                                     "EquirectangularToCubemap.frag");
	myEquirectangularToCubemapShader->Compile();
}

GLvoid PBRViewerSkybox::Cleanup() const
{
	glDeleteTextures(1, &myEnvironmentTexture.ID);
	glDeleteTextures(1, &myIrradianceTexture.ID);
	glDeleteTextures(1, &myBRDFLookupTexture.ID);
	glDeleteTextures(1, &myPreFilteredEnvironmentMap.ID);

	glDeleteVertexArrays(1, &myVAO);
}

GLboolean PBRViewerSkybox::Init()
{
	GLboolean resultTextures = GL_TRUE;

	resultTextures &= LoadEnvironmentTexture();
	resultTextures &= CreateIrradianceTexture();
	resultTextures &= CreatePreFilteredEnvironmentMap();
	resultTextures &= CreateBRDFLookupTexture();

	myTextureToDisplay = myEnvironmentTexture.ID;
	return resultTextures;
}

GLboolean PBRViewerSkybox::LoadEnvironmentTexture()
{
	PBRViewerTexture equirectangularTexture;
	if (GL_FALSE == LoadEquirectangularTexture(myFilepathEnvironmentTexture, equirectangularTexture))
	{
		return GL_FALSE;
	}

	myEnvironmentTexture.Filepath = myFilepathEnvironmentTexture;
	myEnvironmentTexture.Type = "textureEnvironment";
	ConvertEquirectangularTextureToCubemap(equirectangularTexture, myEnvironmentTexture);

	// Create mipmap sampling for the environment map. This is needed for the specular reflections based on the roughness level of the surface.
	// Each Mipmap contains more "blurred" reflections of the environment. The rougher the surface of a material, 
	// the more blurred the specular reflections appear on the surface.
	glBindTexture(GL_TEXTURE_CUBE_MAP, myEnvironmentTexture.ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// enable pre-filter mipmap sampling (combatting visible dots artifact)
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glDeleteTextures(1, &equirectangularTexture.ID);

	return GL_TRUE;
}

GLvoid PBRViewerSkybox::SetTextureToDisplay( const PBRViewerEnumerations::SkyboxTexture currentSkyboxTexture )
{
	switch (currentSkyboxTexture)
	{
		case PBRViewerEnumerations::Environment:
			myTextureToDisplay = myEnvironmentTexture.ID;
			break;
		case PBRViewerEnumerations::Irradiance:
			myTextureToDisplay = myIrradianceTexture.ID;
			break;
		case PBRViewerEnumerations::PreFilteredEnvironment:
			myTextureToDisplay = myPreFilteredEnvironmentMap.ID;
			break;
		default:
			myTextureToDisplay = myEnvironmentTexture.ID;
			break;
	}
}

GLvoid PBRViewerSkybox::SetTextureToDisplayMipMapLevel( const GLuint currentMipMapLevel )
{
	myTextureMipMapLevelToDisplay = currentMipMapLevel;
}

GLboolean PBRViewerSkybox::LoadEquirectangularTexture( std::string& filepath, PBRViewerTexture& textureResult ) const
{
	PBRViewerTexture texture;

	GLuint hdrTexture;
	GLint width, height, nrChannels;

	stbi_set_flip_vertically_on_load(GL_TRUE);
	GLfloat* data = stbi_loadf(filepath.c_str(), &width, &height, &nrChannels, 0);
	stbi_set_flip_vertically_on_load(GL_FALSE);

	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		texture.ID = hdrTexture;
		texture.Filepath = filepath;
		texture.Type = "textureEquirectangular";

		textureResult = texture;

		stbi_image_free(data);
		return GL_TRUE;
	}

	std::string message = "Failed to image at path: ";
	message += filepath;
	PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, message);

	return GL_FALSE;
}

GLboolean PBRViewerSkybox::CreateIrradianceTexture()
{
	GLuint irradianceMap;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	PBRViewerShader irradianceShader("IrradianceConvolution.vert", "IrradianceConvolution.frag");
	irradianceShader.Compile();

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	irradianceShader.Use();
	irradianceShader.setInt("textureEnvironment", 0);
	irradianceShader.setMat4("projection", GetCaptureProjection());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, myEnvironmentTexture.ID);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	std::vector<glm::mat4> captureViews = PBRViewerOpenGLUtilities::GetCaptureViewsForCubeMap();
	for (GLuint i = 0; i < 6; ++i)
	{
		irradianceShader.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
	}

	// Reset variables
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);

	myIrradianceTexture.ID = irradianceMap;
	myIrradianceTexture.Type = "textureIrradiance";

	return GL_TRUE;
}

GLboolean PBRViewerSkybox::CreateBRDFLookupTexture()
{
	GLuint brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);

	PBRViewerShader brdfLookupShader("BRDFLookup.vert", "BRDFLookup.frag");
	brdfLookupShader.AddFileAtTheEnd(GL_FRAGMENT_SHADER, "ImportanceSampleGGX.gl");
	brdfLookupShader.AddFileAtTheEnd(GL_FRAGMENT_SHADER, "VectorTransformation.gl");
	brdfLookupShader.AddFileAtTheEnd(GL_FRAGMENT_SHADER, "NormalDistributionFunctions.gl");
	brdfLookupShader.AddFileAtTheEnd(GL_FRAGMENT_SHADER, "GeometryFunctions.gl");
	brdfLookupShader.Compile();
	
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

	// Footnote 2 from "Real Shading in Unreal Engine 4" - Precision is important while using the BRDF lookup texture, so we use GL_RG32F
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 512, 512, 0, GL_RG, GL_FLOAT, nullptr);

	// Be sure to set wrapping mode to GL_CLAMP_TO_EDGE to prevent edge sampling artifacts.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	brdfLookupShader.Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	PBRViewerOpenGLUtilities::RenderFullScreenQuad();

	// Reset variables
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);

	myBRDFLookupTexture.ID = brdfLUTTexture;
	myBRDFLookupTexture.Type = "textureBRDFLookup";

	return GL_TRUE;
}

GLboolean PBRViewerSkybox::CreatePreFilteredEnvironmentMap()
{
	// Enable seamless cubemap sampling for lower mip levels in the pre-filter map.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	const GLuint textureWidth = 512;
	const GLuint textureHeight = textureWidth;

	GLuint prefilterMap;
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.	
	PBRViewerShader prefilterShader("PreFilterEnvironmentMap.vert", "PreFilterEnvironmentMap.frag");	
	prefilterShader.AddFileAtTheEnd(GL_FRAGMENT_SHADER, "NormalDistributionFunctions.gl");	
	prefilterShader.AddFileAtTheEnd(GL_FRAGMENT_SHADER, "ImportanceSampleGGX.gl");
	prefilterShader.Compile();

	prefilterShader.Use();
	prefilterShader.setInt("textureEnvironmentMap", 0);
	prefilterShader.setInt("cubemapFaceResolution", textureWidth);
	prefilterShader.setMat4("projection", GetCaptureProjection());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, myEnvironmentTexture.ID);

	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	const GLuint maxMipLevels = 5;
	for (GLuint mip = 0u; mip < maxMipLevels; ++mip)
	{
		// Resize framebuffer according to mip-level size.
		const auto mipWidth = static_cast<GLuint>(textureWidth * std::pow(0.5, mip));
		const auto mipHeight = static_cast<GLuint>(textureHeight * std::pow(0.5, mip));
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		const GLfloat roughness = static_cast<GLfloat>(mip) / static_cast<GLfloat>(maxMipLevels - 1);
		prefilterShader.setFloat("roughness", roughness);

		std::vector<glm::mat4> captureViews = PBRViewerOpenGLUtilities::GetCaptureViewsForCubeMap();
		for (GLuint i = 0u; i < 6; ++i)
		{
			prefilterShader.setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderCube();
		}
	}

	// Reset variables
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);
	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	myPreFilteredEnvironmentMap.ID = prefilterMap;
	myPreFilteredEnvironmentMap.Type = "texturePreFilterEnvironment";

	return GL_TRUE;
}

GLvoid PBRViewerSkybox::ConvertEquirectangularTextureToCubemap( PBRViewerTexture& textureToConvert,
                                                                PBRViewerTexture& textureResult )
{
	PBRViewerTexture cubemapTexture;
	std::vector<GLfloat> vertices = PBRViewerObjectCreator::GetSkyboxVertexData();

	// 0. Setup
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// Setup cube VAO
	GLuint skyboxVBO;
	glGenVertexArrays(1, &myVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(myVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices[0] * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

	// 2. Convert equirectangular HDR image to cubemap	
	glGenTextures(1, &cubemapTexture.ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture.ID);

	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 2048, 2048, 0, GL_RGBA, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	std::vector<glm::mat4> captureViews = PBRViewerOpenGLUtilities::GetCaptureViewsForCubeMap();

	myEquirectangularToCubemapShader->Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureToConvert.ID);

	myEquirectangularToCubemapShader->setInt("textureEquirectangular", 0);
	myEquirectangularToCubemapShader->setMat4("projection", captureProjection);

	glViewport(0, 0, 2048, 2048);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (GLuint i = 0; i < 6; ++i)
	{
		myEquirectangularToCubemapShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		                       cubemapTexture.ID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(myVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Reset states
	GLint currentWindowWidth, currentWindowHeight;
	glfwGetWindowSize(glfwGetCurrentContext(), &currentWindowWidth, &currentWindowHeight);
	glViewport(0, 0, currentWindowWidth, currentWindowHeight);

	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);
	glDeleteVertexArrays(1, &skyboxVBO);	

	textureResult.ID = cubemapTexture.ID;
}

// Draw the environment texture.
GLvoid PBRViewerSkybox::Draw( std::shared_ptr<PBRViewerShader> const& shader ) const
{
	// change depth function so depth test passes when values are equal to depth buffer's content	
	glDepthFunc(GL_LEQUAL);	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, myTextureToDisplay);
	shader->setInt("textureEnvironment", 0);

	// Currently, only the prefilted environment texture has mip map levels.
	// To avoid errors within the shader, we only set the value for this texture.
	if (myTextureToDisplay == myPreFilteredEnvironmentMap.ID)
	{
		shader->setInt("mipMapLevel", myTextureMipMapLevelToDisplay);
	}
	else
	{
		shader->setInt("mipMapLevel", 0);
	}

	RenderCube();

	glDepthFunc(GL_LESS); // set depth function back to default	
}

GLvoid PBRViewerSkybox::RenderCube() const
{
	glBindVertexArray(myVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

glm::mat4 PBRViewerSkybox::GetCaptureProjection() const
{
	return glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
}
