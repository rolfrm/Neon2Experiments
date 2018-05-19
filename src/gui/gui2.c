#include <GL/glew.h>
#include <iron/full.h>
#include <neon.h>
#include "gl/gl_module.h"
#include "gl/gl_utils.h"
#include "windows.h"
#include "windows.c"
#include "window_table.h"
#include "window_table.c"
#include "u64_to_u64.h"
#include "u64_to_u64.c"

#include <iron/datastream_server.h>

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
  u64_to_u64 * children;

  bool init_dbg;
  rectangle_shader rect;
  vec2 center_pos;
}window_data;

window_data * get_window_ctx(){
  static module_data ctx_holder2 = {0};
  window_data * window_data = get_module_data(&ctx_holder2);
  dmsg(ui_verbose,"rendering windows..\n");
  if(window_data == NULL){
    window_data = alloc0(sizeof(window_data[0]));
    window_data->window_vector = windows_create(NULL);
    window_data->window_table = window_table_create(NULL);
    window_data->children = u64_to_u64_create(NULL);
    ((bool *)&(window_data->children->is_multi_table))[0] = true;
    set_module_data(&ctx_holder2, window_data);
    dmsg(ui,"Created windows holder\n");
  }else{
  }
  return window_data;
}

static void pre_render_scene(){
  
  let wind =  get_window_ctx();
  if(!wind->init_dbg){
    gl_init_debug_calls();
    wind->init_dbg = true;

    int r_vs = compileShaderFromFile(GL_VERTEX_SHADER, "rect_shader.vs");
    int r_fs = compileShaderFromFile(GL_FRAGMENT_SHADER, "rect_shader.fs");
    logd(" %i %i\n", r_vs, r_fs);

    rectangle_shader rect;
    rect.prog = linkGlProgram(2, r_vs,r_fs);
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

    

  }
  
  
}

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
