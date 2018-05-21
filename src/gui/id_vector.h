// This file is auto generated by icy-vector
#include "icydb.h"
typedef struct _id_vector{
  char ** column_names;
  char ** column_types;
  size_t * count;
  size_t * capacity;
  size_t * free_index_count;
  const size_t column_count;
  icy_mem * free_indexes;
  icy_mem * header;
  const size_t column_sizes[1];
  
  u8 * key;
  icy_mem * key_area;
}id_vector;

// a vector index.
typedef struct{
  size_t index;
}id_vector_index;

typedef struct{
  size_t index;
  size_t count;
}id_vector_indexes;

id_vector * id_vector_create(const char * optional_name);
id_vector_index id_vector_alloc(id_vector * table);
id_vector_indexes id_vector_alloc_sequence(id_vector * table, size_t count);
void id_vector_remove(id_vector * table, id_vector_index index);
void id_vector_remove_sequence(id_vector * table, id_vector_indexes * indexes);
void id_vector_clear(id_vector * table);
void id_vector_optimize(id_vector * table);
void id_vector_destroy(id_vector ** table);