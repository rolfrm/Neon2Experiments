#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <iron/types.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iron/log.h>
#include <iron/mem.h>
#include <iron/fileio.h>
#include <iron/utils.h>
#include <iron/linmath.h>
#include <iron/time.h>
#include <iron/datastream.h>
#include <xxhash.h>
//#include "stb_image.h"
#include "gl_utils.h"
/*
u32 loadImage(u8 * pixels, u32 width, u32 height, u32 channels){
  
  GLuint tex = 0;
  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  u32 intype = 0;
  switch(channels){
  case 1:
    intype = GL_RED;
    break;
  case 2:
    intype = GL_RG;
    break;
  case 3:
    intype = GL_RGB;
    break;
  case 4:
    intype = GL_RGBA;
    break;
  default:
    ERROR("Invalid number of channels %i", channels);
  }
u32 pixels_channels = 0;
  switch(channels){
  case 1:
    pixels_channels = GL_RED;
    break;
  case 2:
    pixels_channels = GL_RG;
    break;
  case 3:
    pixels_channels = GL_RGB;
    break;
  case 4:
    pixels_channels = GL_RGBA;
    break;
  default:
    ERROR("Invalid number of channels %i", channels);
  }  
  
  glTexImage2D(GL_TEXTURE_2D, 0, intype, width, height, 0, pixels_channels, GL_UNSIGNED_BYTE, pixels);
  return tex;
}

u32 loadImagef(float * pixels, u32 width, u32 height, u32 channels){
  return loadImagefx(pixels, width, height, channels, GL_LINEAR);
}

u32 loadImagefx(float * pixels, u32 width, u32 height, u32 channels, int interp){
  
  GLuint tex = 0;
  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interp);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interp);
  u32 intype = 0;
  switch(channels){
  case 1:
    intype = GL_R32F;
    break;
  case 2:
    intype = GL_RG32F;
    break;
  case 3:
    intype = GL_RGB32F;
    break;
  case 4:
    intype = GL_RGBA32F;
    break;
  default:
    ERROR("Invalid number of channels %i", channels);
  }
  u32 pixels_channels = 0;
  switch(channels){
  case 1:
    pixels_channels = GL_RED;
    break;
  case 2:
    pixels_channels = GL_RG;
    break;
  case 3:
    pixels_channels = GL_RGB;
    break;
  case 4:
    pixels_channels = GL_RGBA;
    break;
  default:
    ERROR("Invalid number of channels %i", channels);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, intype, width, height, 0, pixels_channels, GL_FLOAT, pixels);
  //glGenerateMipmap(GL_TEXTURE_2D);
  return tex;
}
*/

data_stream gl_verbose = {.name = "GL Verbose"};

u32 compileShader(int program, const char * code){
  u32 ss = glCreateShader(program);
  i32 l = strlen(code);
  glShaderSource(ss, 1, (void *) &code, &l); 
  glCompileShader(ss);
  int compileStatus = 0;	
  glGetShaderiv(ss, GL_COMPILE_STATUS, &compileStatus);
  if(compileStatus == 0){
    dmsg(gl_verbose, "Error during shader compilation:");
    int loglen = 0;
    glGetShaderiv(ss, GL_INFO_LOG_LENGTH, &loglen);
    char * buffer = alloc0(loglen);
    glGetShaderInfoLog(ss, loglen, NULL, buffer);

    dmsg(gl_verbose,"%s", buffer);
    dealloc(buffer);
  } else{
    dmsg(gl_verbose, "--- Success");
  }
  return ss;
}

u32 compileShaderFromFile(u32 gl_prog_type, const char * filepath){
  dmsg(gl_verbose,"Compiling shader '%s'\n", filepath);
  char * vcode = read_file_to_string(filepath);
  u32 vs = compileShader(gl_prog_type, vcode);
  dealloc(vcode);
  return vs;
}

u32 createShaderFromFiles(const char * vs_path, const char * fs_path){
  u32 vs = compileShaderFromFile(GL_VERTEX_SHADER, vs_path);
  u32 fs = compileShaderFromFile(GL_FRAGMENT_SHADER, fs_path);
  u32 result = linkGlProgramv(2, vs, fs);
  return result;
}



u32 linkGlProgram(u32 shader_cnt, int * shaders){
  u32 prog = glCreateProgram();
  for(u32 i = 0; i < shader_cnt; i++){
    glAttachShader(prog, shaders[i]);
  }
  glLinkProgram(prog);

  int program_linked;
 
  glGetProgramiv(prog, GL_LINK_STATUS, &program_linked);

  if (program_linked != GL_TRUE)
    {
      i32 loglen;
      glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &loglen);      
      char * buffer = alloc0(loglen * sizeof(char) + 1);
      glGetProgramInfoLog(prog, 1024, &loglen, buffer);
      logd("%s", buffer);
      dealloc(buffer);
      // Write the error to a log
    }
  return prog;
}

u32 linkGlProgramv(u32 shader_cnt, ...){
  int shaders[shader_cnt];

  { // read arglist
    va_list arglist;
    va_start (arglist, shader_cnt);
    for(u32 i = 0; i < shader_cnt; i++)
      shaders[i] = va_arg(arglist, u32);
    
    va_end(arglist);
  }
  return linkGlProgram(shader_cnt, shaders);

}

int compiled_shader(shader_program * prog, ...){
  XXH64_state_t * hasher = NULL;
  u64 * hash = prog->hash;
  int * shaders = prog->shaders;
  va_list arglist;
  va_start (arglist, prog);
  var tstamp = timestampf();
  int i = 0;
  bool change = false;
  f64 delta = (tstamp - prog->timestamp);
  while(true){
    
    ASSERT(i < 5);
    u32 shader_type = va_arg(arglist, u32);
    if(shader_type == 0)
      break;//

    const char * path = va_arg(arglist, const char *);

    if(shaders[i] != 0 && delta < 1.0)
      continue;
    
    if(hasher == NULL){
      hasher = XXH64_createState();
    }
    XXH64_reset(hasher, 0);

    char * vcode = read_file_to_string(path);
    ASSERT(vcode != NULL);
    XXH64_update(hasher, vcode, strlen(vcode));
    u64 hsh = XXH64_digest(hasher);

    if(hsh != hash[i]){
      dmsg(gl_verbose, "recompiling '%s'\n", path);
      if(shaders[i] != 0)
	glDeleteShader(shaders[i]);
      
      shaders[i] = compileShader(shader_type, vcode);
      hash[i] = hsh;
      change = true;
    }
    dealloc(vcode);
    i++;
  }
  va_end(arglist);
  if(change){
    if(prog->program != 0)
      glDeleteProgram(prog->program);
    int program = linkGlProgram(i, shaders);
    prog->program = program;
  }
  if(delta > 1.0 || change)
    prog->timestamp = tstamp;
  if(hasher != NULL)
    XXH64_freeState(hasher);
  return change;
}

void debugglcalls(GLenum source,
		  GLenum type,
		  GLuint id,
		  GLenum severity,
		  GLsizei length,
		  const GLchar *message,
		  const void *userParam){
  UNUSED(length);
  UNUSED(userParam);

  switch(type){
  case GL_DEBUG_TYPE_ERROR:
    logd("%i %i %i i\n", source, type, id, severity);
    ERROR("%s\n", message);
    ASSERT(false);
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
  case GL_DEBUG_TYPE_PORTABILITY:
  case GL_DEBUG_TYPE_OTHER:
    return;
  case GL_DEBUG_TYPE_PERFORMANCE:
    break;
  default:
    break;
  }
  logd("%i %i %i i\n", source, type, id, severity);
  logd("%s\n", message);
}

void gl_init_debug_calls(){
  glDebugMessageCallback(debugglcalls, NULL);
}

void glUniformVec2(u32 uniform, vec2 v){
  glUniform2f(uniform, v.x, v.y);
}

void glUniformMat3(u32 uniform_location, mat3 mat){
  glUniformMatrix3fv(uniform_location, 1, false, &mat.data[0][0]);
}
