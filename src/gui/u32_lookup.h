// This file is auto generated by table_compiler
typedef struct _u32_lookup{
  char ** column_names;
  char ** column_types;
  u64 count;
  const bool is_multi_table;
  const int column_count;
  int (*cmp) (const u32 * k1, const u32 * k2);
  const u64 sizes[1];

  u32 * key;
  mem_area * key_area;
}u32_lookup;

u32_lookup * u32_lookup_create(const char * optional_name);
void u32_lookup_set(u32_lookup * table, u32 key);
void u32_lookup_insert(u32_lookup * table, u32 * key, u64 count);
void u32_lookup_lookup(u32_lookup * table, u32 * keys, u64 * out_indexes, u64 count);
void u32_lookup_remove(u32_lookup * table, u32 * keys, u64 key_count);
void u32_lookup_clear(u32_lookup * table);
void u32_lookup_unset(u32_lookup * table, u32 key);
bool u32_lookup_try_get(u32_lookup * table, u32 * key);
void u32_lookup_print(u32_lookup * table);
u64 u32_lookup_iter(u32_lookup * table, u32 * keys, size_t keycnt, u32 * optional_keys_out, u64 * indexes, u64 cnt, u64 * iterator);