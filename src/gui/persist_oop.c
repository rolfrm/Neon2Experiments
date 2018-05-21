#include <iron/full.h>
#include <neon.h>

#include "u64_to_u64.h"
#include "u64_to_ptr.h"
#include "methods.h"

typedef struct{
  u64_to_u64 * baseclass;
  methods * methods;
}oop_data;


oop_data * get_oop_ctx(){
  static module_data ctx_holder = {0};
  oop_data * oop_data = get_module_data(&ctx_holder);
  if(oop_data == NULL){
    oop_data = alloc0(sizeof(oop_data[0]));
    oop_data->baseclass = u64_to_u64_create(NULL);
    ((bool *)&(oop_data->baseclass->is_multi_table))[0] = true;
    oop_data->methods = methods_create(NULL);
    set_module_data(&ctx_holder, oop_data);
  }
  return oop_data;
}


method _get_method(u64 class_id, u64 method_id){
  var ctx = get_oop_ctx();
  method m;
  method_key key = {.method = method_id, .class = class_id};
  if(methods_try_get(ctx->methods, &key, &m))
    return m;
  return NULL;
}

void _set_method(u64  class_id, u64 method_id, method m){
  var ctx = get_oop_ctx();
  method_key key = {.method = method_id, .class = class_id};
  methods_set(ctx->methods, key, m);
}

u64 get_baseclass(u64 item, u64 * index){
  var ctx = get_oop_ctx();
  u64 _index;
  if(u64_to_u64_iter(ctx->baseclass, &item, 1, NULL, &_index, 1, index) == 0)
    return 0;
  return ctx->baseclass->value[_index];
}

void _set_baseclass(u64 item, u64 class){
  var ctx = get_oop_ctx();
  u64_to_u64_set(ctx->baseclass, item, class);
}
