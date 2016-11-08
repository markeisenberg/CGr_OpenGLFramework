#version 150

in  vec3 pass_Normal;
in vec3 pass_color;
in vec3 pass_lightVec;
in vec3 pass_viewVec;

out vec4 out_Color;

//Values taken from: http://www.cs.toronto.edu/~jepson/csc2503/tutorials/phong.pdf
float ka = 0.1;
float kd = 0.6;
float ks = 0.7;

void main() {
	//Code taken from: http://learnopengl.com/#!Advanced-Lighting/Advanced-Lighting
  vec3 middleDirection = normalize(pass_lightVec.xyz + pass_viewVec);

  //Normalize 3 component vecs
  vec3 normal_comp = normalize(pass_Normal);

  //Maybe
  //vec3 light_comp = normalize(pass_lightVec.xyz - pass_viewVec.xyz);
  //vec3 vec_comp = normalize(-pass_viewVec);

  //Maybe needed
  //vec3 ia = vec3(0.6f, 0.6f, 0.6f);
  //vec3 id = vec3(0.3f, 0.3f, 0.3f);
  //vec3 is = vec3(1.0f, 1.0f, 1.0f);

  

  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}
