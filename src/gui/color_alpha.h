// This file is auto generated by icy-table.
#include "icydb.h"
typedef struct _color_alpha{
  char ** column_names;
  char ** column_types;
  size_t count;
  const bool is_multi_table;
  const int column_count;
  int (*cmp) (const u64 * k1, const u64 * k2);
  const size_t sizes[2];

  u64 * key;
  vec4 * color;
  icy_mem * key_area;
  icy_mem * color_area;
}color_alpha;

color_alpha * color_alpha_create(const char * optional_name);
void color_alpha_set(color_alpha * table, u64 key, vec4 color);
void color_alpha_insert(color_alpha * table, u64 * key, vec4 * color, size_t count);
void color_alpha_lookup(color_alpha * table, u64 * keys, size_t * out_indexes, size_t count);
void color_alpha_remove(color_alpha * table, u64 * keys, size_t key_count);
void color_alpha_clear(color_alpha * table);
void color_alpha_unset(color_alpha * table, u64 key);
bool color_alpha_try_get(color_alpha * table, u64 * key, vec4 * color);
void color_alpha_print(color_alpha * table);
size_t color_alpha_iter(color_alpha * table, u64 * keys, size_t keycnt, u64 * optional_keys_out, size_t * indexes, size_t cnt, size_t * iterator);
