extern u32 gui_render_window;
extern data_stream ui_log;
extern data_stream ui_verbose_log;


extern pclass window_class;

extern pobject window_pos_method;
extern pobject window_size_method;
extern pobject mouse_over_method;
extern pobject mouse_button_method;
extern pobject mouse_scroll_method;
extern pobject key_method;
extern pobject char_method;
extern pobject window_close_method;
extern pobject render_method;

pobject create_window(float width, float height, const char * title);
vec4 gui_get_background(pid object);
void gui_set_background(pid object, vec4 color);
pid gui_get_child(pid object, u64 * index);
pobject gui_new_object();
void gui_child_add(pid object, u64 child);
void gui_child_remove(pid object, u64 child);

typedef void (* command_executor)(u64 id, const char * command);
void console_register_handler(u64 * id, command_executor exec);
void * get_context_object(module_data * constptr, size_t size);
