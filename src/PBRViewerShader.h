#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

#include <../ext/eigen/Eigen/Eigen>

/// <summary>
/// This class represents a shader object. It offers convenience methods to set uniform variables of the shader.
/// </summary>
class PBRViewerShader
{
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="LearnOpenGLShader"/> class.
	/// </summary>	
	PBRViewerShader();

	/// <summary>
	/// Initializes a new instance of the <see cref="LearnOpenGLShader"/> class.
	/// </summary>
	/// <param name="vertexPath">The filepath to the vertex shader.</param>
	/// <param name="fragmentPath">The filepath to the fragment shader.</param>
	/// <param name="geometryPath">The (optional) filepath to the geometry shader.</param>
	PBRViewerShader( std::string const& vertexPath,
	                 std::string const& fragmentPath,
	                 std::string const& geometryPath = "" );

	/// <summary>
	/// Adds the content of a file at the end of the shader.
	/// This is useful to distribute common code to shader instances and thus preserve the DRY principle. 
	/// Shader code can initially define the declarations of methods and then integrate the implementations from a common source. 
	/// </summary>
	/// <param name="shaderType">The type of the shader where to append the file.</param>
	/// <param name="filepath">The filepath to the additional code.</param>	
	GLvoid AddFileAtTheEnd( GLenum shaderType, const std::string& filepath );

	/// <summary>
	/// Compiles the shader. IMPORTANT: Call this method before using this shader instance.
	/// This method also checks for compile and link errors and reports them to the standard output stream.
	/// </summary>
	GLvoid Compile();

	/// <summary>
	/// Gets the identifier of the shader.
	/// </summary>
	/// <returns>The identifier of the shader.</returns>
	GLuint GetID() const;

	/// <summary>
	/// Activates this shader. Use this method before setting any variables.
	/// </summary>	
	GLvoid Use() const;

	/// <summary>
	/// Sets the specified <see cref="GLboolean"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="value">The value of the uniform variable.</param>	
	GLvoid setBool( const std::string& name, GLboolean value ) const;

	/// <summary>
	/// Sets the specified <see cref="GLint"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="value">The value of the uniform variable.</param>	
	GLvoid setInt( const std::string& name, GLint value ) const;

	/// <summary>
	/// Sets the specified <see cref="GLfloat"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="value">The value of the uniform variable.</param>	
	GLvoid setFloat( const std::string& name, GLfloat value ) const;

	/// <summary>
	/// Sets the specified <see cref="glm::vec2"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="value">The value of the uniform variable.</param>	
	GLvoid setVec2( const std::string& name, const glm::vec2& value ) const;

	/// <summary>
	/// Sets the specified <see cref="GLfloat"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="x">The x component of the vector.</param>
	/// <param name="y">The y component of the vector.</param>
	GLvoid setVec2( const std::string& name, GLfloat x, GLfloat y ) const;

	/// <summary>
	/// Sets the specified <see cref="glm::vec3"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="value">The value of the uniform variable.</param>	
	GLvoid setVec3( const std::string& name, const glm::vec3& value ) const;

	/// <summary>
	/// Sets the specified <see cref="glm::vec3"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="x">The x component of the vector.</param>
	/// <param name="y">The y component of the vector.</param>
	/// <param name="z">The z component of the vector.</param>
	GLvoid setVec3( const std::string& name, GLfloat x, GLfloat y, GLfloat z ) const;

	/// <summary>
	/// Sets the specified <see cref="glm::vec4"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="value">The value of the uniform variable.</param>	
	GLvoid setVec4( const std::string& name, const glm::vec4& value ) const;

	/// <summary>
	/// Sets the specified <see cref="glm::vec4"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="x">The x component of the vector.</param>
	/// <param name="y">The y component of the vector.</param>
	/// <param name="z">The z component of the vector.</param>
	/// <param name="w">The w component of the vector.</param>
	GLvoid setVec4( const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w ) const;

	/// <summary>
	/// Sets the specified <see cref="glm::mat2"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="mat">The value of the uniform variable.</param>	
	GLvoid setMat2( const std::string& name, const glm::mat2& mat ) const;

	/// <summary>
	/// Sets the specified <see cref="glm::mat3"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="mat">The value of the uniform variable.</param>	
	GLvoid setMat3( const std::string& name, const glm::mat3& mat ) const;

	/// <summary>
	/// Sets the specified <see cref="glm::mat4"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="mat">The value of the uniform variable.</param>	
	GLvoid setMat4( const std::string& name, const glm::mat4& mat ) const;

	/// <summary>
	/// Sets the specified <see cref="Eigen::Matrix4f"/> uniform variable.
	/// </summary>
	/// <param name="name">The name of the variable within the shader file.</param>
	/// <param name="mat">The value of the uniform variable.</param>	
	GLvoid setMat4( const std::string& name, const Eigen::Matrix4f& mat ) const;

private:
	GLuint myID = 0u;

	std::stringstream myVertexCode;
	std::stringstream myFragmentCode;
	std::stringstream myGeometryCode;

	std::string ReadFile( const std::string& filepath ) const noexcept;

	/// <summary>
	/// Checks for compile errors.
	/// </summary>
	/// <param name="shader">The shader to check.</param>
	/// <param name="type">The type of the shader.</param>	
	GLvoid checkCompileErrors( GLuint shader, const std::string& type ) const;

	/// <summary>
	/// Creates a shader object.
	/// </summary>
	/// <param name="type">The type of the shader to create.</param>
	/// <param name="src">The filepath to the shader file.</param>
	/// <returns>The ID of the shader.</returns>
	GLuint CreateShader( GLenum type, const GLchar* src ) const;
};
