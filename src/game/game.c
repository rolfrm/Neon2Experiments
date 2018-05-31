#include <GL/glew.h>
#include <iron/full.h>
#include <neon.h>
#include <GLFW/glfw3.h>

#include "gl/gl_module.h"
#include "gl/gl_utils.h"
#include "gui/windows.h"
#include "gui/window_table.h"
#include "gui/u64_to_u64.h"
#include "gui/u64_to_ptr.h"
#include "gui/methods.h"
#include <iron/datastream_server.h>
#include "gui/utf8.h"
#include "gui/persist_oop.h"
#include "gui/gui2.h"
#include "gui/u64_lut.h"
#include "gui/id_vector.h"

typedef struct{

  int prog;
  int color_loc, tex_loc, mode_loc;
  int offset_loc, size_loc, window_size_loc, uv_offset_loc, uv_size_loc;
}rectangle_shader;

typedef struct{
  u64 module_id;
  bool demo_loaded;
  rectangle_shader rect;
  vec2 center_pos;
  pobject win;
  pobject game_board;
}demo_data;

typedef struct {
  id_vector * game_ids;
  u64_to_ptr * games;
  u64_to_u64 * game_windows;
  u64 * module_id;
}engine_context;

static engine_context * get_engine_context(){
  static module_data ctx_holder = {0};
  engine_context * ctx = get_context_object(&ctx_holder, sizeof(engine_context));
  if(ctx->games == NULL){
    ctx->games = u64_to_ptr_create(NULL);
    icy_mem * mem = icy_mem_create("game/demo/module_id");
    ctx->game_ids = id_vector_create("game/demo/game_ids");
    icy_mem_realloc(mem, sizeof(u64));
    ctx->module_id = mem->ptr;
    ctx->game_windows = u64_to_u64_create(NULL);
  }
  return ctx;
  
}


static void render_game(u64 thing){
  var game_ctx = get_engine_context();
  u64 game_id;
  ASSERT(u64_to_u64_try_get(game_ctx->game_windows, &thing, &game_id));
  demo_data * dd;
  ASSERT(u64_to_ptr_try_get(game_ctx->games, &game_id, (void **) &dd));
  if(!dd->demo_loaded){
    dd->demo_loaded = true;
  
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
    dd->rect = rect;
    dd->center_pos = vec2_new(5,6);
    dd->demo_loaded = true;
  }

  { 
    rectangle_shader rect = dd->rect;
    glUseProgram(rect.prog);
    
    glUniform2f(rect.window_size_loc, 10, 10);
    glUniform1i(rect.mode_loc, 0);
    glUniform4f(rect.color_loc, 1,0,0,0);

    glUniform2f(rect.size_loc, 1, 1);
    vec2 center = dd->center_pos;
    
    for(int i = 0; i < 10; i++){
      for(int j = 0; j < 10; j++){
	
	vec2 pos = vec2_new(i,j);
	glUniform4f(rect.color_loc, 1,vec2_len(vec2_sub(pos, center)) * 0.1,0.2,0.3);	
	glUniform2f(rect.offset_loc, pos.x, pos.y);	
	//glDrawArrays(GL_TRIANGLE_STRIP,0,4);	
	
      }
    }
    dd->center_pos = vec2_add(dd->center_pos, vec2_new(randf32() * 0.1 - 0.05, randf32() * 0.1 - 0.05));
  }
}


typedef struct {
  id_vector * module_ids;
  u64_to_ptr * modules;
}game_context;


static void game_loader(u64 id, const char * command){
  UNUSED(id);
  if(string_startswith(command, "load-game")){

    var ectx = get_engine_context();
    
    u64 new_game_id = id_vector_alloc(ectx->game_ids).index;
    game_context * new_game_ctx = alloc0(sizeof(game_context));

    new_game_ctx->module_ids = id_vector_create(quickfmt("game/%i/module_ids", new_game_id));
    u64_to_ptr_set(ectx->games, new_game_id, new_game_ctx);

    dmsg(ui_log, "Loading game... %i\n", new_game_id);

    demo_data * dd = alloc0(sizeof(dd[0]));
    ASSERT(sizeof(dd[0]) == sizeof(demo_data));    
    dd->win = create_window(512,512,"GUI2");
    gui_set_background(dd->win, vec4_new(0.3,0.4,0.2,1.0));
    dd->game_board = gui_new_object();
    u64_to_u64_set(ectx->game_windows, dd->game_board, new_game_id);
    set_method(dd->game_board, render_method, (method) render_game);
    gui_child_add(dd->win, dd->game_board);
    u64_to_ptr_set(ectx->games, new_game_id, dd);
  }
}

void init_game(){
  var game_ctx = get_engine_context();
  
  console_register_handler(game_ctx->module_id, game_loader);
}