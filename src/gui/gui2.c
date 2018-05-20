#include <GL/glew.h>
#include <iron/full.h>
#include <neon.h>
#include <GLFW/glfw3.h>

#include "gl/gl_module.h"
#include "gl/gl_utils.h"
#include "windows.h"
#include "windows.c"
#include "window_table.h"
#include "window_table.c"
#include "u64_to_u64.h"
#include "u64_to_u64.c"

#include "u64_to_ptr.h"
#include "u64_to_ptr.c"

#include "methods.h"
#include "methods.c"

#include <iron/datastream_server.h>
#include "utf8.h"

u32 gui_render_window;

static data_stream ui = {.name = "2UI"};
static data_stream ui_verbose = {.name = "UI Verbose"};

typedef struct{

  int prog;
  int color_loc, tex_loc, mode_loc;
  int offset_loc, size_loc, window_size_loc, uv_offset_loc, uv_size_loc;
}rectangle_shader;


typedef struct {
  windows * window_vector;
  window_table * window_table;
  u64_to_ptr * window_ctx;
  u64_to_u64 * children;
  u64_to_u64 * baseclass;
  methods * methods;

  bool init_dbg;

  bool demo_loaded;
  rectangle_shader rect;
  vec2 center_pos;
  windows_index win;
}window_data;

  
method _get_method(u64 class_id, u64 method_id);
void _set_method(u64  class_id, u64 method_id, method m);
#define get_method(class, method) _get_method(class, (size_t)&method)
#define set_method(class, method, impl) _set_method(class, (size_t)&method, impl)
window_data * get_window_ctx();

method _get_method(u64 class_id, u64 method_id){
  var ctx = get_window_ctx();
  method m;
  method_key key = {.method = method_id, .class = class_id};
  if(methods_try_get(ctx->methods, &key, &m))
    return m;
  return NULL;
}

void _set_method(u64  class_id, u64 method_id, method m){
  var ctx = get_window_ctx();
  method_key key = {.method = method_id, .class = class_id};
  methods_set(ctx->methods, key, m);
}



u64 get_window_id(GLFWwindow * glwindow){
  var ctx = get_window_ctx();
  for(u64 i = 0; i < ctx->window_ctx->count; i++){
    if(ctx->window_ctx->value[i + 1] == glwindow)
      return ctx->window_ctx->key[i + 1];
  }
  return 0;
}

#define get_method(class, method) _get_method(class, (size_t)&method)
#define set_method(class, method, impl) _set_method(class, (size_t)&method, impl)

u64 get_baseclass(u64 item, u64 * index){
  var ctx = get_window_ctx();
  u64 _index;
  if(u64_to_u64_iter(ctx->baseclass, &item, 1, NULL, &_index, 1, index) == 0)
    return 0;
  return ctx->baseclass->value[_index];
}

void _set_baseclass(u64 item, u64 class){
  var ctx = get_window_ctx();
  u64_to_u64_set(ctx->baseclass, item, class);
}

#define set_baseclass(item, class) _set_baseclass(item, (size_t)&class);

#define CALL_BASE(Item, Method, ...)\
  ({\
    u64 index = 0, base = 0;\
    while(0 != (base = get_baseclass(Item, &index))){	\
      method m = get_method(base, Method);		\
      if(m != NULL) m(Item, __VA_ARGS__);		\
    }							\
  })

#define CALL_METHOD(Item, Method, ...)\
  ({\
    method m1 = get_method(Item, Method);	\
    if(m1 != NULL) m1(Item, __VA_ARGS__);		\
  })

u64 window_class;

u64 window_pos_method;
u64 window_size_method;
u64 mouse_over_method;
u64 mouse_button_method;
u64 mouse_scroll_method;
u64 key_method;
u64 char_method;
u64 window_close_method;
u64 render_method;

void window_pos_callback(GLFWwindow* glwindow, int xpos, int ypos)
{
  u64 w = get_window_id(glwindow);
  CALL_METHOD(w, window_pos_method, xpos, ypos);
}

void window_size_callback(GLFWwindow* glwindow, int width, int height)
{
  u64 w = get_window_id(glwindow);
  CALL_METHOD(w, window_size_method, width, height);
}

void cursor_pos_callback(GLFWwindow * glwindow, double x, double y){
  u64 win_id = get_window_id(glwindow);
  CALL_METHOD(win_id, mouse_over_method, x, y);
}

void mouse_button_callback(GLFWwindow * glwindow, int button, int action, int mods){
  u64 win_id = get_window_id(glwindow);
  CALL_METHOD(win_id, mouse_button_method, button, action, mods);
}

void scroll_callback(GLFWwindow * glwindow, double x, double y){
  u64 win_id = get_window_id(glwindow);
  CALL_METHOD(win_id, mouse_scroll_method, x, y);
}

void key_callback(GLFWwindow* glwindow, int key, int scancode, int action, int mods){
  u64 win_id = get_window_id(glwindow);
  CALL_METHOD(win_id, key_method, key, scancode, action, mods);
}

void char_callback(GLFWwindow * glwindow, u32 codepoint){
  if(0 == codepoint_to_utf8(codepoint,NULL, 10))
    return; // WTF! Invalid codepoint!
  u64 win_id = get_window_id(glwindow);
  CALL_METHOD(win_id, char_method, codepoint);
    
}

void window_close_callback(GLFWwindow * glwindow){
  u64 win_id = get_window_id(glwindow);
  CALL_METHOD(win_id, window_close_method, 0);
}

void on_window_class_render(u64 win_id){
  logd("Rendering class.. %i\n", win_id);
}

void on_window_render(u64 win_id){
  logd("Rendering.. %i\n", win_id);
  CALL_BASE(win_id, render_method, 0);
}

windows_index create_window(float width, float height, const char * title){
  var win_ctx = get_window_ctx();
  GLFWwindow * window = module_create_window(width, height, title);
  var idx = windows_alloc(win_ctx->window_vector);
  set_baseclass(idx.index, window_class);
  window_table_set(win_ctx->window_table, idx);
  u64_to_ptr_set(win_ctx->window_ctx, idx.index, window);

  glfwSetWindowPosCallback(window, window_pos_callback);
  glfwSetWindowSizeCallback(window, window_size_callback);
  glfwSetCursorPosCallback(window, cursor_pos_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetWindowCloseCallback(window, window_close_callback);
  glfwSetCharCallback(window, char_callback);
  glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
  set_method(idx.index, render_method, (method) on_window_render);

  return idx;
}
static void win_pos_update(u64 win_id, int x, int y){
  logd("id: %i Pos: %i %i\n", win_id, x, y);
}

static void cursor_pos_update(u64 win_id, double x, double y){
  logd("Cursor id: %i Pos: %f %f\n", win_id, x, y);
}

static void pre_render_scene(){
  
  let wind =  get_window_ctx();
  if(!wind->init_dbg){
    gl_init_debug_calls();
    wind->init_dbg = true;
  }

  for(u64 i = 0; i < wind->window_table->count; i++){
    CALL_METHOD(wind->window_table->id[i + 1].index, render_method, 0);
  }
  
  GLFWwindow * ptr;
  if(!wind->demo_loaded){
    wind->demo_loaded = true;
    wind->win = create_window(512,512,"GUI2");

    ASSERT(u64_to_ptr_try_get(wind->window_ctx, &(wind->win.index), (void *) &ptr));
    glfwMakeContextCurrent(ptr);
    int r_vs = compileShaderFromFile(GL_VERTEX_SHADER, "rect_shader.vs");
    int r_fs = compileShaderFromFile(GL_FRAGMENT_SHADER, "rect_shader.fs");

    rectangle_shader rect;
    rect.prog = linkGlProgram(2, r_vs,r_fs);
    glDeleteShader(r_vs);
    glDeleteShader(r_fs);
    int loc(const char * name){
      return glGetUniformLocation(rect.prog, name);
    }
    rect.color_loc = loc("color");
    rect.tex_loc = loc("tex");
    rect.mode_loc = loc("mode");
    rect.size_loc = loc("size");
    rect.offset_loc = loc("offset");
    rect.window_size_loc = loc("window_size");
    rect.uv_offset_loc = loc("uv_offset");
    rect.uv_size_loc = loc("uv_size");
    wind->rect = rect;
    wind->center_pos = vec2_new(5,6);
    wind->demo_loaded = true;

    set_method(wind->win.index, window_pos_method, (method)win_pos_update);
    set_method(wind->win.index, mouse_over_method, (method)cursor_pos_update);
    
  }else{
    ASSERT(u64_to_ptr_try_get(wind->window_ctx, &(wind->win.index), (void *)&ptr));
    glfwMakeContextCurrent(ptr);
  }


  
  
  {
    
    rectangle_shader rect = wind->rect;
    glUseProgram(rect.prog);
    
    glUniform2f(rect.window_size_loc, 10, 10);
    glUniform1i(rect.mode_loc, 0);
    glUniform4f(rect.color_loc, 1,0,0,0);

    glUniform2f(rect.size_loc, 1, 1);
    vec2 center = wind->center_pos;
    
    for(int i = 0; i < 10; i++){
      for(int j = 0; j < 10; j++){
	
	vec2 pos = vec2_new(i,j);
	glUniform4f(rect.color_loc, 1,vec2_len(vec2_sub(pos, center)) * 0.1,0.2,0.3);	
	glUniform2f(rect.offset_loc, pos.x, pos.y);	
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);	
	
      }
    }
    wind->center_pos = vec2_add(wind->center_pos, vec2_new(randf32() * 0.1 - 0.05, randf32() * 0.1 - 0.05));
    glfwSwapBuffers(ptr);
    

  }
}

//GLFWwindow 

//void update_activity(const data_stream * stream, const void * _data, size_t _length, void * userdata){

//}

static void _log_print(const data_stream * stream, const void * _data, size_t _length, void * userdata){
  UNUSED(stream);UNUSED(_data);UNUSED(_length);UNUSED(userdata);
  fwrite(_data, 1, _length, stdout);
}

void gui_init_module(){
  gui_render_window = intern_string("gui/render");

  register_method(gui_render_window, pre_render_scene);
  register_event(gui_render_window, gl_post_render, false);

  data_stream_listener * l1 = alloc0(sizeof(*l1));
  l1->process = _log_print;
  data_stream_listen(l1, &ui);
  
  //var ds = datastream_server_run();
  //datastream_server_wait_for_connect(ds);
}

window_data * get_window_ctx(){
  static module_data ctx_holder2 = {0};
  window_data * window_data = get_module_data(&ctx_holder2);
  dmsg(ui_verbose,"rendering windows..\n");
  if(window_data == NULL){
    window_data = alloc0(sizeof(window_data[0]));
    window_data->window_vector = windows_create(NULL);
    window_data->window_table = window_table_create(NULL);
    window_data->children = u64_to_u64_create(NULL);
    window_data->window_ctx = u64_to_ptr_create(NULL);
    ((bool *)&(window_data->children->is_multi_table))[0] = true;
    window_data->baseclass = u64_to_u64_create(NULL);
    ((bool *)&(window_data->baseclass->is_multi_table))[0] = true;
    window_data->methods = methods_create(NULL);
    set_module_data(&ctx_holder2, window_data);
    dmsg(ui,"Created windows holder\n");
    set_method((size_t)&window_class, render_method, (method) on_window_class_render);

  }
  return window_data;
}
