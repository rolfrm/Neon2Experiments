#version 410

uniform mat3 tform;
layout(location=0) in vec2 vertex;
//asd

out vec2 uv;
void main(){

vec3 p = tform * vec3(vertex, 1);
  gl_Position = vec4(p, 1);
}
