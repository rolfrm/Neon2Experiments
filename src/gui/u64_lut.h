// This file is auto generated by icy-table.
#include "icydb.h"
typedef struct _u64_lut{
  char ** column_names;
  char ** column_types;
  size_t count;
  const bool is_multi_table;
  const int column_count;
  int (*cmp) (const u64 * k1, const u64 * k2);
  const size_t sizes[1];

  u64 * key;
  icy_mem * key_area;
}u64_lut;

u64_lut * u64_lut_create(const char * optional_name);
void u64_lut_set(u64_lut * table, u64 key);
void u64_lut_insert(u64_lut * table, u64 * key, size_t count);
void u64_lut_lookup(u64_lut * table, u64 * keys, size_t * out_indexes, size_t count);
void u64_lut_remove(u64_lut * table, u64 * keys, size_t key_count);
void u64_lut_clear(u64_lut * table);
void u64_lut_unset(u64_lut * table, u64 key);
bool u64_lut_try_get(u64_lut * table, u64 * key);
void u64_lut_print(u64_lut * table);
size_t u64_lut_iter(u64_lut * table, u64 * keys, size_t keycnt, u64 * optional_keys_out, size_t * indexes, size_t cnt, size_t * iterator);
