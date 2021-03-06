// This file is auto generated by icy-table.
#include "icydb.h"

typedef void (* method)(u64 object, ...);
typedef struct{
  u64 method;
  u64 class;
}method_key;

typedef struct _methods{
  char ** column_names;
  char ** column_types;
  size_t count;
  const bool is_multi_table;
  const int column_count;
  int (*cmp) (const method_key * k1, const method_key * k2);
  const size_t sizes[2];

  method_key * key;
  method * value;
  icy_mem * key_area;
  icy_mem * value_area;
}methods;

methods * methods_create(const char * optional_name);
void methods_set(methods * table, method_key key, method value);
void methods_insert(methods * table, method_key * key, method * value, size_t count);
void methods_lookup(methods * table, method_key * keys, size_t * out_indexes, size_t count);
void methods_remove(methods * table, method_key * keys, size_t key_count);
void methods_clear(methods * table);
void methods_unset(methods * table, method_key key);
bool methods_try_get(methods * table, method_key * key, method * value);
void methods_print(methods * table);
size_t methods_iter(methods * table, method_key * keys, size_t keycnt, method_key * optional_keys_out, size_t * indexes, size_t cnt, size_t * iterator);
