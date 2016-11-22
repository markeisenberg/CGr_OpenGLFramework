#version 150

in  vec3 pass_Normal;
in vec3 pass_color;
in vec3 pass_lightVec;
in vec3 pass_viewVec;

in vec2 pass_textureCoord;

out vec4 out_Color;

//Values taken from: http://www.cs.toronto.edu/~jepson/csc2503/tutorials/phong.pdf

uniform vec3 planetColor;
uniform sampler2D texSampler;

void main() {

  vec4 texColor = texture(texSampler, pass_textureCoord);
	//Code taken from: http://learnopengl.com/#!Advanced-Lighting/Advanced-Lighting
  vec3 middleDirection = normalize(pass_lightVec.xyz + pass_viewVec);

  //Normalize 3 component vecs
  vec3 normal_comp = normalize(pass_Normal);

  //Maybe
  //vec3 light_comp = normalize(pass_lightVec.xyz - pass_viewVec.xyz);
  //vec3 vec_comp = normalize(-pass_viewVec);

  float ka = 0.65; //0.7; 0.1;
  float kd = 0.6; //0.45; 0.6;
  float ks = 0.9; //1.0; 0.7;

  vec3 ia = vec3(0.6f, 0.6f, 0.6f);
  vec3 id = vec3(0.3f, 0.3f, 0.3f);
  vec3 is = vec3(1.0f, 1.0f, 1.0f);

  //How reflective the planets become
  float refl_val = 0.5f;

  // The formula for the ambient light
  vec3 ambient = ka * ia* pass_color;

  //vec3 diffuse = kd * max(dot(normal_comp, light_comp), 0.0);
  //diffuse = clamp(diffuse, 0.0, 1.0);

  //Diffuse light: https://www.youtube.com/watch?v=K2lj2Rzs7hQ
  vec3 diffuse = kd * id* pass_color;

  //old
  //float spec_light = ks * pow(max(dot(normal_comp, middleDirection), 0.0), 9.0);

  vec3 spec_light = ks * is;

 //Equation taken for Phong from: http://web.cse.ohio-state.edu/~whmin/courses/cse5542-2013-spring/11-illumination.pdf
 vec3 result = vec3(ambient *texColor.rgb+ diffuse * dot(pass_lightVec.rgb, normal_comp)+ //change here to texColor.rgb?
    				spec_light* pow(dot(normal_comp,middleDirection),refl_val));

  out_Color = vec4(result,1.0f);

  //old out - Do not change
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}
