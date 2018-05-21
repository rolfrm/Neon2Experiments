#include <GL/glew.h>
#include <iron/full.h>
#include <neon.h>
#include <GLFW/glfw3.h>

#include "gl/gl_module.h"
#include "gl/gl_utils.h"
#include "windows.h"
#include "window_table.h"
#include "u64_to_u64.h"
#include "u64_to_ptr.h"
#include "methods.h"
#include <iron/datastream_server.h>
#include "utf8.h"
#include "persist_oop.h"
#include "gui2.h"
#include "gui/pid_to_vec4.h"

u32 gui_render_window;

data_stream ui_log = {.name = "2UI"};
data_stream ui_verbose_log = {.name = "UI Verbose"};

typedef struct {
  windows * window_vector;
  window_table * window_table;
  u64_to_ptr * window_ctx;
  u64_to_u64 * children;

  pid_to_vec4 * background_color;

  bool init_dbg;
}window_data;

window_data * get_window_ctx();

u64 get_window_id(GLFWwindow * glwindow){
  var ctx = get_window_ctx();
  for(u64 i = 0; i < ctx->window_ctx->count; i++){
    if(ctx->window_ctx->value[i + 1] == glwindow)
      return ctx->window_ctx->key[i + 1];
  }
  return 0;
}

GLFWwindow * get_glfw_window(u64 window_id){
  var ctx = get_window_ctx();
  void * ptr = NULL;
  u64_to_ptr_try_get(ctx->window_ctx, &window_id, &ptr);
  return ptr;
}

vec4 gui_get_background(pid object){
  var ctx = get_window_ctx();
  vec4 color = vec4_zero;
  pid_to_vec4_try_get(ctx->background_color, &object, &color);
  return color;
}

void gui_set_background(pid object, vec4 color){
  var ctx = get_window_ctx();
  pid_to_vec4_set(ctx->background_color, object, color);  
}


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
  var win = get_glfw_window(win_id);
  dmsg(ui_verbose_log, "Rendering.. %i\n", win_id);
  
  var bg = gui_get_background(win_id);
  
  glfwMakeContextCurrent(win);
  glClearColor(bg.x,bg.y,bg.z,bg.w);
  glClear(GL_COLOR_BUFFER_BIT);
  
  CALL_BASE(win_id, render_method, 0);
  glfwSwapBuffers(win);
  
}

pobject create_window(float width, float height, const char * title){
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

  return idx.index;
}

/*static void win_pos_update(u64 win_id, int x, int y){
  logd("id: %i Pos: %i %i\n", win_id, x, y);
}

static void cursor_pos_update(u64 win_id, double x, double y){
  logd("Cursor id: %i Pos: %f %f\n", win_id, x, y);
  }*/

static void pre_render_scene(){
  
  let wind =  get_window_ctx();
  if(!wind->init_dbg){
    gl_init_debug_calls();
    wind->init_dbg = true;
  }

  for(u64 i = 0; i < wind->window_table->count; i++){
    CALL_METHOD(wind->window_table->id[i + 1].index, render_method, 0);
  }
  
}

//GLFWwindow 

//void update_activity(const data_stream * stream, const void * _data, size_t _length, void * userdata){

//}

static void _log_print(const data_stream * stream, const void * _data, size_t _length, void * userdata){
  UNUSED(stream);UNUSED(_data);UNUSED(_length);UNUSED(userdata);
  fwrite(_data, 1, _length, stdout);
}
void init_game();
void gui_init_module(){
  gui_render_window = intern_string("gui/render");

  register_method(gui_render_window, pre_render_scene);
  register_event(gui_render_window, gl_post_render, false);

  data_stream_listener * l1 = alloc0(sizeof(*l1));
  l1->process = _log_print;
  data_stream_listen(l1, &ui_log);
  
  init_game();
  
  //var ds = datastream_server_run();
  //datastream_server_wait_for_connect(ds);
}

window_data * get_window_ctx(){
  static module_data ctx_holder2 = {0};
  window_data * window_data = get_module_data(&ctx_holder2);
  dmsg(ui_verbose_log,"rendering windows..\n");
  if(window_data == NULL){
    window_data = alloc0(sizeof(window_data[0]));
    window_data->window_vector = windows_create(NULL);
    window_data->window_table = window_table_create(NULL);
    window_data->children = u64_to_u64_create(NULL);
    window_data->window_ctx = u64_to_ptr_create(NULL);
    window_data->background_color = pid_to_vec4_create(NULL);
    ((bool *)&(window_data->children->is_multi_table))[0] = true;
    set_module_data(&ctx_holder2, window_data);
    dmsg(ui_log,"Created windows holder\n");
    set_method((size_t)&window_class, render_method, (method) on_window_class_render);
  }
  return window_data;
}
