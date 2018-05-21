typedef u64 pid;
typedef u64 pobject;
typedef u64 pclass;

method _get_method(u64 class_id, u64 method_id);
void _set_method(u64  class_id, u64 method_id, method m);
#define get_method(class, method) _get_method(class, (size_t)&method)
#define set_method(class, method, impl) _set_method(class, (size_t)&method, impl)
u64 get_baseclass(u64 item, u64 * index);
void _set_baseclass(u64 item, u64 class);
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
    else CALL_BASE(Item, Method, __VA_ARGS__);			\
  })

