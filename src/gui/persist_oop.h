
typedef struct{
  u64 id;
}class;

typedef void (* method)(u64 control, ...);

void define_subclass(u64 class, u64 base_class);
u64 get_baseclass(u64 class, u64 * index);
void define_method(u64 class_id, u64 method_id, method handler);
method get_method(u64 class_id, u64 method_id);

class * new_class(u64 id);
method get_command_handler(u64 control_id, u64 command_id);

method * get_command_handler2(u64 control_id, u64 command_id, bool create);
void attach_handler(u64 control_id, u64 command_id, void * handler);

void * find_item(const char * table, u64 itemid, u64 size, bool create);

#define CALL_BASE_METHOD(Item, Method, ...)\
  ({\
    u64 index = 0, base = 0;\
    while(0 != (base = get_baseclass(Item, &index))){	\
      method m = get_method(base, Method);		\
      if(m != NULL) m(Item, __VA_ARGS__);		\
    }							\
  })
