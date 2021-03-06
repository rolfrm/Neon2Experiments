#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include <iron/types.h>
#include <iron/log.h>
#include <iron/mem.h>
#include <iron/utils.h>
#include <iron/datastream.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <icydb.h>

#include <neon.h>

#include "gl_module.h"

u32 gl_pre_render;
u32 gl_post_render;

typedef struct{
  GLFWwindow * window;

}gl_context_data;
static data_stream gl_debug_high = {.name = "GL DEBUG"};
static data_stream gl_info = {.name = "GL INFO"};
static data_stream glfw_debug = {.name = "GLFW Error"};

static void glfwError(int x, const char * err){
  UNUSED(x);
  dmsg(glfw_debug,"%s", err);
  ERROR("%i %s", x, err);
}


static void neon_gl_debug_callback(u32 source, u32 type, u32 id, u32 severity, int length, const char * message, const void * userdata){
  UNUSED(source);
  UNUSED(type);
  UNUSED(id);
  UNUSED(severity);
  UNUSED(userdata);
  dlog(gl_debug_high,message, length);
}

gl_context_data * get_or_init_context(){
  static module_data ctx_holder;
  static bool glfw_inited = false;
  gl_context_data * ctx = get_module_data(&ctx_holder);
  if(!ctx){
    dmsg(gl_info, "Creating a new context\n");
    glfwSwapInterval(0);
    ctx = alloc0(sizeof(ctx[0]));
    set_module_data(&ctx_holder, ctx);
    if(!glfw_inited){
      dmsg(gl_info, "Initialized glfw\n");
      if(!glfwInit())
	ERROR("Not able to initialize glfw.\n");
      
      glfwSetErrorCallback(glfwError);
    }
    glfw_inited = true;
      
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow * window = glfwCreateWindow(512, 512, "GL MODULE", NULL, NULL);
    dmsg(gl_info, "Created context : %i    %i    %i\n", ctx, ctx->window, window);
    if(window == NULL)
      ERROR("Unable to create window!\n");
    ASSERT(window != NULL);
    ctx->window = window;
    glfwMakeContextCurrent(window);
    glewInit();
    glDebugMessageCallback(neon_gl_debug_callback, NULL);
    glEnable(GL_DEBUG_OUTPUT);
  }
  return ctx;
}

GLFWwindow * module_create_window(int width, int height, const char * title){
  var ctx = get_or_init_context();
  glfwWindowHint(GLFW_SAMPLES, 16);
  glfwWindowHint(GLFW_DEPTH_BITS, 32);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
  GLFWwindow * window = glfwCreateWindow(width, height, title, NULL, ctx->window);
  glfwMakeContextCurrent(window);
  glDebugMessageCallback(neon_gl_debug_callback, NULL);
  glEnable(GL_DEBUG_OUTPUT);
  return window;
}

void pre_render_scene(){
  gl_context_data * ctx = get_or_init_context();
  glfwMakeContextCurrent(ctx->window);
}

void post_render_scene(){
  glfwPollEvents();
}
void gui_init_module();
void init_module(){
  gl_pre_render = intern_string("gl/pre_render");
  gl_post_render = intern_string("gl/post_render");

  register_event(gl_pre_render, 0, false);
  register_event(gl_post_render, gl_pre_render, true);

  register_method(gl_pre_render, pre_render_scene);
  register_method(gl_post_render, post_render_scene);

  gui_init_module();
  
}

bool isKeyDown(int key){
  var ctx = get_or_init_context();
  int keystate = glfwGetKey(ctx->window, key);
  return keystate == GLFW_PRESS;
  
}
