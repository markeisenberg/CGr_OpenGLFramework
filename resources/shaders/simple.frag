#version 150

in  vec3 pass_Normal;
in vec3 pass_color;
in vec3 pass_lightVec;
in vec3 pass_viewVec;

in vec2 pass_textureCoordinate;

uniform sampler2D colorTexture;
vec3 tex_color = vec3(texture(colorTexture, pass_textureCoordinate));

out vec4 out_Color;

//Values taken from: http://www.cs.toronto.edu/~jepson/csc2503/tutorials/phong.pdf
float ka = 0.1;
float kd = 0.6;
float ks = 0.7;

uniform vec3 planetColor;

void main() {
	//Code taken from: http://learnopengl.com/#!Advanced-Lighting/Advanced-Lighting
  vec3 middleDirection = normalize(pass_lightVec.xyz + pass_viewVec);

  //Normalize 3 component vecs
  vec3 normal_comp = normalize(pass_Normal);

  //Maybe Fe-NL
  vec3 light_comp = normalize(pass_lightVec.xyz - pass_viewVec.xyz);
  vec3 vec_comp = normalize(-pass_viewVec);

  //Maybe needed
  //vec3 ia = vec3(0.6f, 0.6f, 0.6f);
  //vec3 id = vec3(0.3f, 0.3f, 0.3f);
  //vec3 is = vec3(1.0f, 1.0f, 1.0f);

  //Reflectiveness
  float refl_val = 2.0f;

  // Maybe ambient
  //vec3 ambient = ka * ia* pass_Color;

  vec3 diffuse = kd * max(dot(normal_comp, light_comp), 0.0);
  diffuse = clamp(diffuse, 0.0, 1.0);

  // Maybe diffuse
  //vec3 diffuse = kd * id* pass_Color;

  //Diffuse light: https://www.youtube.com/watch?v=K2lj2Rzs7hQ
  float spec_light = ks * pow(max(dot(normal_comp, middleDirection), 0.0), 9.0);

  // Maybe Spec
 // vec3 specular = ks * is;

//maybe result
 //vec3 result = vec3(ambient+ diffuse * dot(pass_LightVector.rgb, normal)+ 
    				//specular* pow(dot(normal,h),b));

  out_Color = vec4(ka * tex_color + diffuse + tex_color + planetColor * spec_light, 1.0);

  //Maybe out
  //out_Color = vec4(result,1.0f);

  //old out
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}
