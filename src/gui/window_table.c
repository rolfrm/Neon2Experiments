// This file is auto generated by icy-table.
#ifndef TABLE_COMPILER_INDEX
#define TABLE_COMPILER_INDEX
#define array_element_size(array) sizeof(array[0])
#define array_count(array) (sizeof(array)/array_element_size(array))
#include "icydb.h"
#include <stdlib.h>
#endif


window_table * window_table_create(const char * optional_name){
  static const char * const column_names[] = {(char *)"id"};
  static const char * const column_types[] = {"windows_index"};
  window_table * instance = calloc(sizeof(window_table), 1);
  instance->column_names = (char **)column_names;
  instance->column_types = (char **)column_types;
  
  icy_table_init((icy_table * )instance, optional_name, 1, (unsigned int[]){sizeof(windows_index)}, (char *[]){(char *)"id"});
  
  return instance;
}

void window_table_insert(window_table * table, windows_index * id, size_t count){
  void * array[] = {(void* )id};
  icy_table_inserts((icy_table *) table, array, count);
}

void window_table_set(window_table * table, windows_index id){
  void * array[] = {(void* )&id};
  icy_table_inserts((icy_table *) table, array, 1);
}

void window_table_lookup(window_table * table, windows_index * keys, size_t * out_indexes, size_t count){
  icy_table_finds((icy_table *) table, keys, out_indexes, count);
}

void window_table_remove(window_table * table, windows_index * keys, size_t key_count){
  size_t indexes[key_count];
  size_t index = 0;
  size_t cnt = 0;
  while(0 < (cnt = icy_table_iter((icy_table *) table, keys, key_count, NULL, indexes, array_count(indexes), &index))){
    icy_table_remove_indexes((icy_table *) table, indexes, cnt);
    index = 0;
  }
}

void window_table_clear(window_table * table){
  icy_table_clear((icy_table *) table);
}

void window_table_unset(window_table * table, windows_index key){
  window_table_remove(table, &key, 1);
}

bool window_table_try_get(window_table * table, windows_index * id){
  void * array[] = {(void* )id};
  void * column_pointers[] = {(void *)table->id};
  size_t __index = 0;
  icy_table_finds((icy_table *) table, array[0], &__index, 1);
  if(__index == 0) return false;
  unsigned int sizes[] = {sizeof(windows_index)};
  for(int i = 1; i < 1; i++){
    if(array[i] != NULL)
      memcpy(array[i], column_pointers[i] + __index * sizes[i], sizes[i]); 
  }
  return true;
}

void window_table_print(window_table * table){
  icy_table_print((icy_table *) table);
}

size_t window_table_iter(window_table * table, windows_index * keys, size_t keycnt, windows_index * optional_keys_out, size_t * indexes, size_t cnt, size_t * iterator){
  return icy_table_iter((icy_table *) table, keys, keycnt, optional_keys_out, indexes, cnt, iterator);

}
