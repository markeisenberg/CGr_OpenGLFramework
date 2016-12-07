#version 150

in  vec3 pass_Normal;
in vec3 pass_color;
in vec3 pass_lightVec;
in vec3 pass_viewVec;

uniform  sampler2D colTex;

in vec2 pass_textureCoordinate;

out vec4 out_Color;

//Values taken from: http://www.cs.toronto.edu/~jepson/csc2503/tutorials/phong.pdf

//uniform vec3 planetColor;

void main() {
  vec4 surfaceCol = texture(colTex, pass_textureCoordinate);
	//Code taken from: http://learnopengl.com/#!Advanced-Lighting/Advanced-Lighting
  vec3 middleDirection = normalize(pass_lightVec.xyz + pass_viewVec);

  //Normalize 3 component vecs
  vec3 normal_comp = normalize(pass_Normal);

  //Maybe
  //vec3 light_comp = normalize(pass_lightVec.xyz - pass_viewVec.xyz);
  //vec3 vec_comp = normalize(-pass_viewVec);

  float ka = 0.3; //0.65; //0.7; 0.1;
  float kd = 0.3; //0.6; //0.45; 0.6;
  float ks = 0.7; //0.9; //1.0; 0.7;

  vec3 ia = vec3(0.3f, 0.3f, 0.3f);
  vec3 id = vec3(0.3f, 0.3f, 0.3f);
  vec3 is = vec3(0.7f, 0.7f, 0.7f);

  //How reflective the planets become
  float refl_val = 2.0f; //0.5f;

  // The formula for the ambient light
  vec3 ambient = ka * ia + surfaceCol.rgb;

  //vec3 diffuse = kd * max(dot(normal_comp, light_comp), 0.0);
  //diffuse = clamp(diffuse, 0.0, 1.0);

  //Diffuse light: https://www.youtube.com/watch?v=K2lj2Rzs7hQ
  vec3 diffuse = kd * id + surfaceCol.rgb;

  //old
  //float spec_light = ks * pow(max(dot(normal_comp, middleDirection), 0.0), 9.0);

  vec3 spec_light = ks * is;

 //Equation taken for Phong from: http://web.cse.ohio-state.edu/~whmin/courses/cse5542-2013-spring/11-illumination.pdf
 vec3 result = vec3(ambient+ diffuse * dot(pass_lightVec.rgb, normal_comp)+ 
    				spec_light* pow(dot(normal_comp,middleDirection),refl_val));

  out_Color = vec4(result,1.0f);

  //old out - Do not change
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}
