/* OLD

#version 150

//in  vec4 pass_Normal;
out vec4 out_Color;

void main(void)
{
    out_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
} */

#version 150

//
in  vec3 pass_Color;
out vec4 out_Color;

void main(void)
{
    out_Color = vec4(pass_Color.xyz,1.0f);
}