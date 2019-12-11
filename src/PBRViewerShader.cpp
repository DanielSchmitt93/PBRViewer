#include "PBRViewerShader.h"

#include <fstream>
#include <sstream>

#include <../ext/eigen/Eigen/Eigen>
#include "PBRViewerLogger.h"

/// <summary>
/// Initializes a new instance of the <see cref="LearnOpenGLShader"/> class.
/// </summary>	
PBRViewerShader::PBRViewerShader() : PBRViewerShader("", "", "") {}

/// <summary>
/// Initializes a new instance of the <see cref="LearnOpenGLShader"/> class.
/// </summary>
/// <param name="vertexPath">The filepath to the vertex shader.</param>
/// <param name="fragmentPath">The filepath to the fragment shader.</param>
/// <param name="geometryPath">The (optional) filepath to the geometry shader.</param>
PBRViewerShader::PBRViewerShader( std::string const& vertexPath,
                                  std::string const& fragmentPath,
                                  std::string const& geometryPath )
{
	myVertexCode << ReadFile(vertexPath);
	myFragmentCode << ReadFile(fragmentPath);	
	myGeometryCode << ReadFile(geometryPath);		
}

/// <summary>
/// Adds the content of a file at the end of the shader.
/// This is useful to distribute common code to shader instances and thus preserve the DRY principle. 
/// Shader code can initially define the declarations of methods and then integrate the implementations from a common source. 
/// </summary>
/// <param name="shaderType">The type of the shader where to append the file.</param>
/// <param name="filepath">The filepath to the additional code.</param>	
GLvoid PBRViewerShader::AddFileAtTheEnd( const GLenum shaderType, const std::string& filepath )
{
	switch (shaderType)
	{
		case GL_VERTEX_SHADER:
			myVertexCode << ReadFile(filepath);
			break;
		case GL_FRAGMENT_SHADER:
			myFragmentCode << ReadFile(filepath);
			break;
		case GL_GEOMETRY_SHADER:
			myGeometryCode << ReadFile(filepath);
			break;
		default:
			PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, "This shader type is not supported:", std::to_string(shaderType));
			break;
	}	
}

/// <summary>
/// Compiles the shader. IMPORTANT: Call this method before using this shader instance.
/// This method also checks for compile and link errors and reports them to the standard output stream.
/// </summary>
GLvoid PBRViewerShader::Compile()
{	
	std::string vertexCode = myVertexCode.str();
	std::string fragmentCode = myFragmentCode.str();
	std::string geometryCode = myGeometryCode.str();

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	// Compile vertex shader
	const GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vShaderCode, nullptr);
	glCompileShader(vertexShaderID);
	checkCompileErrors(vertexShaderID, "VERTEX");

	// Compile fragment Shader
	const GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fShaderCode, nullptr);
	glCompileShader(fragmentShaderID);
	checkCompileErrors(fragmentShaderID, "FRAGMENT");

	// If geometry shader is given, compile geometry shader
	GLuint geometry = 0;
	if (GL_FALSE == geometryCode.empty())
	{
		const GLchar* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}

	// Link all shaders to a program
	myID = glCreateProgram();
	glAttachShader(myID, vertexShaderID);
	glAttachShader(myID, fragmentShaderID);
	if (GL_FALSE == geometryCode.empty())
	{
		glAttachShader(myID, geometry);
	}

	glLinkProgram(myID);
	checkCompileErrors(myID, "PROGRAM");

	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	if (GL_FALSE == geometryCode.empty())
	{
		glDeleteShader(geometry);
	}
}

/// <summary>
/// Gets the identifier of the shader.
/// </summary>
/// <returns>The identifier of the shader.</returns>
GLuint PBRViewerShader::GetID() const
{
	return myID;
}

/// <summary>
/// Activates this shader. Use this method before setting any variables.
/// </summary>	
GLvoid PBRViewerShader::Use() const
{
	glUseProgram(myID);
}

/// <summary>
/// Sets the specified <see cref="GLboolean"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="value">The value of the uniform variable.</param>	
GLvoid PBRViewerShader::setBool( const std::string& name, const GLboolean value ) const
{
	glUniform1i(glGetUniformLocation(myID, name.c_str()), static_cast<GLint>(value));
}

/// <summary>
/// Sets the specified <see cref="GLint"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="value">The value of the uniform variable.</param>	
GLvoid PBRViewerShader::setInt( const std::string& name, const GLint value ) const
{
	glUniform1i(glGetUniformLocation(myID, name.c_str()), value);
}

/// <summary>
/// Sets the specified <see cref="GLfloat"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="value">The value of the uniform variable.</param>	
GLvoid PBRViewerShader::setFloat( const std::string& name, const GLfloat value ) const
{
	glUniform1f(glGetUniformLocation(myID, name.c_str()), value);
}

/// <summary>
/// Sets the specified <see cref="glm::vec2"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="value">The value of the uniform variable.</param>	
GLvoid PBRViewerShader::setVec2( const std::string& name, const glm::vec2& value ) const
{
	glUniform2fv(glGetUniformLocation(myID, name.c_str()), 1, &value[0]);
}

/// <summary>
/// Sets the specified <see cref="GLfloat"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="x">The x component of the vector.</param>
/// <param name="y">The y component of the vector.</param>
GLvoid PBRViewerShader::setVec2( const std::string& name, const GLfloat x, const GLfloat y ) const
{
	glUniform2f(glGetUniformLocation(myID, name.c_str()), x, y);
}

/// <summary>
/// Sets the specified <see cref="glm::vec3"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="value">The value of the uniform variable.</param>	
GLvoid PBRViewerShader::setVec3( const std::string& name, const glm::vec3& value ) const
{
	glUniform3fv(glGetUniformLocation(myID, name.c_str()), 1, &value[0]);
}

/// <summary>
/// Sets the specified <see cref="glm::vec3"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="x">The x component of the vector.</param>
/// <param name="y">The y component of the vector.</param>
/// <param name="z">The z component of the vector.</param>
GLvoid PBRViewerShader::setVec3( const std::string& name, const GLfloat x, const GLfloat y, const GLfloat z ) const
{
	glUniform3f(glGetUniformLocation(myID, name.c_str()), x, y, z);
}

/// <summary>
/// Sets the specified <see cref="glm::vec4"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="value">The value of the uniform variable.</param>	
GLvoid PBRViewerShader::setVec4( const std::string& name, const glm::vec4& value ) const
{
	glUniform4fv(glGetUniformLocation(myID, name.c_str()), 1, &value[0]);
}

/// <summary>
/// Sets the specified <see cref="glm::vec4"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="x">The x component of the vector.</param>
/// <param name="y">The y component of the vector.</param>
/// <param name="z">The z component of the vector.</param>
/// <param name="w">The w component of the vector.</param>
GLvoid PBRViewerShader::setVec4( const std::string& name, const GLfloat x, const GLfloat y, const GLfloat z,
                                 const GLfloat w ) const
{
	glUniform4f(glGetUniformLocation(myID, name.c_str()), x, y, z, w);
}

/// <summary>
/// Sets the specified <see cref="glm::mat2"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="mat">The value of the uniform variable.</param>	
GLvoid PBRViewerShader::setMat2( const std::string& name, const glm::mat2& mat ) const
{
	glUniformMatrix2fv(glGetUniformLocation(myID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/// <summary>
/// Sets the specified <see cref="glm::mat3"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="mat">The value of the uniform variable.</param>	
GLvoid PBRViewerShader::setMat3( const std::string& name, const glm::mat3& mat ) const
{
	glUniformMatrix3fv(glGetUniformLocation(myID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/// <summary>
/// Sets the specified <see cref="glm::mat4"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="mat">The value of the uniform variable.</param>
GLvoid PBRViewerShader::setMat4( const std::string& name, const glm::mat4& mat ) const
{
	glUniformMatrix4fv(glGetUniformLocation(myID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/// <summary>
/// Sets the specified <see cref="Eigen::Matrix4f"/> uniform variable.
/// </summary>
/// <param name="name">The name of the variable within the shader file.</param>
/// <param name="mat">The value of the uniform variable.</param>	
GLvoid PBRViewerShader::setMat4( const std::string& name, const Eigen::Matrix4f& mat ) const
{
	glUniformMatrix4fv(glGetUniformLocation(myID, name.c_str()), 1, GL_FALSE, mat.data());
}

std::string PBRViewerShader::ReadFile( const std::string& filepath) const noexcept
{
	if(filepath.empty())
	{
		return "";
	}

	try
	{
		std::ifstream shaderFile;
		std::stringstream result;

		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		shaderFile.open(filepath);
		result << shaderFile.rdbuf();
		shaderFile.close();

		return result.str();
	}
	catch (...)
	{
		PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, "Could not read the shader file.", "Filepath: " + filepath);
		return "";
	}	
}

/// <summary>
/// Checks for compile errors.
/// </summary>
/// <param name="shader">The shader to check.</param>
/// <param name="type">The type of the shader.</param>	
GLvoid PBRViewerShader::checkCompileErrors( const GLuint shader, const std::string& type ) const
{
	GLint success;
	GLchar infoLog[1024];

	if (type == "PROGRAM")
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (GL_FALSE == success)
		{
			GLint bufferLength;
			glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &bufferLength);
			if(bufferLength <= 0)
			{
				PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, "Program compilation failed but was not able to get a log message.");
				return;
			}

			glGetProgramInfoLog(shader, bufferLength, nullptr, infoLog);

			std::stringstream message("Could not compile a shader of type: ");
			message << type;
			message << "Info log: " << infoLog << std::endl;

			PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, message.str());
		}

		return;
	}

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (GL_FALSE == success)
	{
		GLint bufferLength;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &bufferLength);
		if (bufferLength <= 0)
		{
			PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, "Shader compilation failed but was not able to get a log message.");
			return;
		}

		glGetProgramInfoLog(shader, bufferLength, nullptr, infoLog);

		std::stringstream message("Could not link a shader of type: ");
		message << type;
		message << "Info log: " << infoLog << std::endl;

		PBRViewerLogger::PrintErrorMessage(__FILE__, __LINE__, message.str());
	}
}

/// <summary>
/// Creates a shader object.
/// </summary>
/// <param name="type">The type of the shader to create.</param>
/// <param name="src">The filepath to the shader file.</param>
/// <returns>The ID of the shader.</returns>
GLuint PBRViewerShader::CreateShader( const GLenum type, const GLchar* src ) const
{
	const GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	return shader;
}