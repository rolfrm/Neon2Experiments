// This file is auto generated by table_compiler
u32_to_sequence * u32_to_sequence_create(const char * optional_name){
  static const char * const column_names[] = {(char *)"key", (char *)"sequence"};
  static const char * const column_types[] = {"u32", "index_table_sequence"};
  u32_to_sequence * instance = alloc0(sizeof(u32_to_sequence));
  abstract_sorttable_init((abstract_sorttable * )instance, optional_name, 2, (u32[]){sizeof(u32), sizeof(index_table_sequence)}, (char *[]){(char *)"key", (char *)"sequence"});
  instance->column_names = (char **)column_names;
  instance->column_types = (char **)column_types;
  return instance;
}

void u32_to_sequence_insert(u32_to_sequence * table, u32 * key, index_table_sequence * sequence, u64 count){
  void * array[] = {(void* )key, (void* )sequence};
  abstract_sorttable_inserts((abstract_sorttable *) table, array, count);
}

void u32_to_sequence_set(u32_to_sequence * table, u32 key, index_table_sequence sequence){
  void * array[] = {(void* )&key, (void* )&sequence};
  abstract_sorttable_inserts((abstract_sorttable *) table, array, 1);
}

void u32_to_sequence_lookup(u32_to_sequence * table, u32 * keys, u64 * out_indexes, u64 count){
  abstract_sorttable_finds((abstract_sorttable *) table, keys, out_indexes, count);
}

void u32_to_sequence_remove(u32_to_sequence * table, u32 * keys, u64 key_count){
  u64 indexes[key_count];
  u64 index = 0;
  u64 cnt = 0;
  while(0 < (cnt = abstract_sorttable_iter((abstract_sorttable *) table, keys, key_count, NULL, indexes, array_count(indexes), &index))){
    abstract_sorttable_remove_indexes((abstract_sorttable *) table, indexes, cnt);
    index = 0;
  }
}

void u32_to_sequence_clear(u32_to_sequence * table){
  abstract_sorttable_clear((abstract_sorttable *) table);
}

void u32_to_sequence_unset(u32_to_sequence * table, u32 key){
  u32_to_sequence_remove(table, &key, 1);
}

bool u32_to_sequence_try_get(u32_to_sequence * table, u32 * key, index_table_sequence * sequence){
  void * array[] = {(void* )key, (void* )sequence};
  void * column_pointers[] = {(void *)table->key, (void *)table->sequence};
  u64 __index = 0;
  abstract_sorttable_finds((abstract_sorttable *) table, array[0], &__index, 1);
  if(__index == 0) return false;
  u32 sizes[] = {sizeof(u32), sizeof(index_table_sequence)};
  for(int i = 1; i < 2; i++){
    if(array[i] != NULL)
      memcpy(array[i], column_pointers[i] + __index * sizes[i], sizes[i]); 
  }
  return true;
}

void u32_to_sequence_print(u32_to_sequence * table){
  abstract_sorttable_print((abstract_sorttable *) table);
}

u64 u32_to_sequence_iter(u32_to_sequence * table, u32 * keys, size_t keycnt, u32 * optional_keys_out, u64 * indexes, u64 cnt, u64 * iterator){
  return abstract_sorttable_iter((abstract_sorttable *) table, keys, keycnt, optional_keys_out, indexes, cnt, iterator);

}
