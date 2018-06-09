#include <GL/glew.h>
#include <iron/full.h>
#include <neon.h>
#include <GLFW/glfw3.h>

#include "gl/gl_module.h"
#include "gl/gl_utils.h"
#include "windows.h"
#include "u64_to_u64.h"
#include "u64_to_ptr.h"
#include "u64_lut.h"
#include "methods.h"
#include <iron/datastream_server.h>
#include "utf8.h"
#include "persist_oop.h"
#include "gui2.h"
#include "pid_to_vec4.h"
#include "pid_lookup.h"
#include "id_vector.h"
#include "u32_vector.h"
#include "window_state.h"

u32 gui_render_window;

data_stream ui_log = {.name = "2UI"};
data_stream ui_verbose_log = {.name = "UI Verbose"};

typedef struct {
  windows * window_vector;
  pid_lookup * window_table;
  u64_to_ptr * window_ctx;
  u64_to_u64 * children;

  pid_to_vec4 * background_color;
  window_state * win_state;
  u32_vector * window_title_vector;
  bool init_dbg;
}window_data;

window_data * get_window_ctx();

u64 get_window_id(GLFWwindow * glwindow){
  var ctx = get_window_ctx();
  for(u64 i = 0; i < ctx->window_ctx->count; i++){
    if(ctx->window_ctx->value[i + 1] == glwindow)
      return ctx->window_ctx->key[i + 1];
  }
  ERROR("Unable to find window");
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

void gui_child_add(pid object, u64 child){
  ASSERT(object != child);
  var win_ctx = get_window_ctx();
  u64_to_u64_set(win_ctx->children, object, child);
}

void gui_child_remove(pid object, u64 child){
  UNUSED(object);UNUSED(child);
  ASSERT(false);
}

pid gui_next_child(pid object, u64 * index){
  var win_ctx = get_window_ctx();
  u64 idx = 0;
  u64_to_u64_iter(win_ctx->children, &object, 1, NULL, &idx, 1, index);
  return win_ctx->children->value[idx];
}

void on_window_class_render(u64 win_id){

  var win = get_glfw_window(win_id);
  
  if(win == NULL){
    var win_ctx = get_window_ctx();
    var idx = win_id;
    u32 width = 512;
    u32 height = 512;
    u32_vector_indexes indexes;
    const char * title = "Fix me";
    
    if(window_state_try_get(win_ctx->win_state, &idx, &width, &height, &indexes)){
      title = (char * ) (win_ctx->window_title_vector->values + indexes.index);
    }
    

    GLFWwindow * window = module_create_window(width, height, title);
    u64_to_ptr_set(win_ctx->window_ctx, idx, window);
    
    glfwSetWindowPosCallback(window, window_pos_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetCharCallback(window, char_callback);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
    win = get_glfw_window(win_id);
  }

  dmsg(ui_verbose_log, "Rendering.. %i\n", win_id);
  
  var bg = gui_get_background(win_id);
  
  glfwMakeContextCurrent(win);

  int width, height;
  glfwGetWindowSize(win, &width, &height);
  glViewport(0, 0, width, height);
  glClearColor(bg.x,bg.y,bg.z,bg.w);
  glClear(GL_COLOR_BUFFER_BIT);
  
  //CALL_BASE(win_id, render_method, 0);
  u64 index = 0;
  pid child = 0;
  while((child = gui_next_child(win_id, &index)) != 0){
    CALL_METHOD(child, render_method, 0);
  }
  glfwSwapBuffers(win);
  
}

pobject gui_new_object(){
  var win_ctx = get_window_ctx();
  var idx = windows_alloc(win_ctx->window_vector);
  return idx.index;
}

pobject create_window(float width, float height, const char * title){
  UNUSED(width);UNUSED(height);UNUSED(title);
  var win_ctx = get_window_ctx();

  var idx = gui_new_object();
  set_baseclass(idx, window_class);
  u32_vector_indexes indexes = u32_vector_alloc_sequence(win_ctx->window_title_vector, strlen(title) + 1);
  char * data = (char *) (win_ctx->window_title_vector->values + indexes.index);
  memcpy(data, title, strlen(title) +1);
  window_state_set(win_ctx->win_state, idx, (u32)width, (u32) height, indexes);
  pid_lookup_set(win_ctx->window_table, idx);
  
  return idx;
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
    pid obj = wind->window_table->key[i + 1];
    
    CALL_BASE(obj, render_method, 0);
  }
  
}

//GLFWwindow 

//void update_activity(const data_stream * stream, const void * _data, size_t _length, void * userdata){

//}


void init_game();

void * get_context_object(module_data * constptr, size_t size){

  void * data = get_module_data(constptr);
  if(data == NULL)
    set_module_data(constptr, (data = alloc0(size)));
  
  return data;
}

static data_stream console_log = {.name = "Console"};

typedef struct{
  u64 console_id;
}console_ctx_data;

typedef void (* command_executor)(u64 id, const char * command);

typedef struct{
  u64_lut * console_handlers;
  id_vector * ids;
  console_ctx_data * ctx_data;
  u64_to_ptr * command_executors;
}console_ctx;
static void console_base_handler(u64 id, const char * command);



console_ctx * get_console_ctx(){
static module_data _ctx = {0};
  console_ctx * ctx = get_context_object(&_ctx, sizeof(console_ctx));
  if(ctx->console_handlers == NULL){
    ctx->console_handlers = u64_lut_create(NULL);
    ctx->ids = id_vector_create("console_ids");
    icy_mem * mem = icy_mem_create("console_ctx_data");
    icy_mem_realloc(mem, sizeof(console_ctx_data));
    ctx->ctx_data = mem->ptr;
    ctx->command_executors = u64_to_ptr_create(NULL);
    if(ctx->ctx_data->console_id == 0){
      id_vector_alloc(ctx->ids);
      var newid = id_vector_alloc(ctx->ids).index;
      dmsg(ui_log,"Loading new console id: %i\n", newid);
      ctx->ctx_data->console_id = newid;
    }else{
      dmsg(ui_log,"Reusing old console id: %i\n", ctx->ctx_data->console_id);
    }
    u64_to_ptr_set(ctx->command_executors, ctx->ctx_data->console_id, (void* ) console_base_handler);
    u64_lut_set(ctx->console_handlers, ctx->ctx_data->console_id);
  }
  return ctx;
}

void console_register_handler(u64 * id, command_executor exec){
  var ctx = get_console_ctx();
  if(*id == 0){
    *id = id_vector_alloc(ctx->ids).index;
  }
  u64_to_ptr_set(ctx->command_executors, *id, (void* ) exec);
  u64_lut_set(ctx->console_handlers, *id);
}

static void console_base_handler(u64 id, const char * command){
  UNUSED(id);
  dmsg(console_log, "%s", command);
  if(string_startswith(command, "intern ")){
    dmsg(console_log, "Interning: %s -> %i\n", command + 7, intern_string(command + 7));
  }
}

static void console_handler_fcn(datastream_server * srv, const char * message, void * userdata){
  
  UNUSED(srv);
  UNUSED(userdata);
  var ctx = get_console_ctx();
  for(u64 i = 0; i < ctx->console_handlers->count; i++){
    u64 id = ctx->console_handlers->key[i + 1];
    command_executor ce = NULL;
    u64_to_ptr_try_get(ctx->command_executors, &id, (void *)&ce);
    if(ce != NULL){
      ce(id, message);
    }
  }
}

void gui_init_module(){
  var ds = datastream_server_run();
  datastream_server_set_console_handler(ds, console_handler_fcn, NULL);
  datastream_server_wait_for_connect(ds);
  
  gui_render_window = intern_string("gui/render");

  register_method(gui_render_window, pre_render_scene);
  register_event(gui_render_window, gl_post_render, false);

  if(false){
    void _log_print(const data_stream * stream, const void * _data, size_t _length, void * userdata){
      UNUSED(stream);UNUSED(_data);UNUSED(_length);UNUSED(userdata);
      fwrite(_data, 1, _length, stdout);
    }
    data_stream_listener * l1 = alloc0(sizeof(*l1));
    l1->process = _log_print;
    data_stream_listen(l1, &ui_log);
  }
  
  init_game();
  


}

window_data * get_window_ctx(){
static module_data ctx_holder2 = {0};
window_data * window_data = get_context_object(&ctx_holder2, sizeof(window_data[0]));
  dmsg(ui_verbose_log,"rendering windows..\n");
  if(window_data->window_vector == NULL){
    window_data->window_vector = windows_create(NULL);
    window_data->window_table = pid_lookup_create(NULL);
    window_data->children = u64_to_u64_create(NULL);
    window_data->window_ctx = u64_to_ptr_create(NULL);
    window_data->background_color = pid_to_vec4_create(NULL);
    ((bool *)&(window_data->children->is_multi_table))[0] = true;
    window_data->win_state = window_state_create(NULL);
    window_data->window_title_vector = u32_vector_create(NULL);

    
    dmsg(ui_log,"Created windows holder\n");
    set_method((size_t)&window_class, render_method, (method) on_window_class_render);
    gui_new_object();
  }
  return window_data;
}
