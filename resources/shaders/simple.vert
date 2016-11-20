#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Texcoord;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

uniform vec3 colorVec;
uniform vec3 lightOrigin;

out vec3 pass_Normal;
out vec3 pass_color;
out vec3 pass_lightVec;
out vec3 pass_viewVec;

out vec2 pass_textureCoordinate;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;

	vec3 planet_Loc = vec3((ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0f));
	pass_lightVec = normalize(lightOrigin - planet_Loc.xyz);
	pass_viewVec = normalize(- planet_Loc.xyz);

	pass_color = colorVec;

	pass_textureCoordinate= in_Texcoord;
}
