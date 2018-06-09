#version 410


#define type_rect 0
#define type_circ 1
#define type_polygon 2

uniform vec4 color;
uniform int type;

uniform vec2 img_pos;
uniform vec2 img_size;
uniform vec2 window_size;
out vec4 fragcolor;

void main(){
  vec2 fc = gl_FragCoord.xy / window_size;
  fc = ((fc - vec2(0.5, 0.5)) * 2.0 - img_pos) / (img_size / 2);
  
  if(type == type_rect){
    fc = abs(fc);
    if(fc.x < 1 && fc.y < 1)
      fragcolor  = color;
    else
      discard;
  }else if(type == type_circ){
    float l = length(fc);
    
    if( l < 1)
      fragcolor  = color;
    else
      discard;
  }   
}
