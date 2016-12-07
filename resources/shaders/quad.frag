#version 150

uniform sampler2D colTex;
uniform vec2 res_screen;


uniform bool setHor;

uniform bool setVert;
uniform bool setGrey;
uniform bool setGaussian;

in vec2 pass_TexCoord;
out vec4 out_Color;



const vec3 LUMINANCE_SCALING_FACTORS = vec3(0.2126f, 0.7152f, 0.0722f);
vec4 greyLum (vec4 cl)
{
    return vec4(vec3(dot(LUMINANCE_SCALING_FACTORS, cl.rgb)), cl.a);
}

vec2 horizFlip (vec2 coords)
{
    return vec2(1.0f-coords.x, coords.y);
}

vec2 vertFlip (vec2 coords)
{
    return vec2(coords.x, 1.0f-coords.y);
}

vec4 gaussianSmooth (vec2 texCoords)
{
    vec2 swap_res = vec2(1.0) / res_screen;
    vec4 total = vec4(0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float d = abs(x) + abs(y);
            float factor = 0.25f * pow(2.0f, -d);

            vec2 offset = vec2(float(x), float(y));
            vec2 texCoord = texCoords + offset * swap_res;
            vec4 cl = texture(colTex, texCoord);

            total += factor * cl;
        }
    }

    return total;
}

void main(void)
{
    vec2 texCoords = pass_TexCoord;

    if (setVert)
    {
        texCoords = vertFlip(texCoords);
    }

    if (setHor)
    {
        texCoords = horizFlip(texCoords);
    }

    vec4 color = vec4(0.0f);
    if (setGaussian)
    {
        color = gaussianSmooth(texCoords);
    } 
    else 
    {
        color = texture(colTex, texCoords);
    }

    if (setGrey)
    {
        color = greyLum(color);
    }

    out_Color = color;
}