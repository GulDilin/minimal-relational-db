
#ifndef SPO_LAB_1_5_CURSOR_H
#define SPO_LAB_1_5_CURSOR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../common/utils.h"

#define max_value(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

#define PAGE_SIZE 1024

#define STRING_END '\0'
#define string_equals(a, b) ({ strcmp(a, b) == 0; })

#define TEXT_LENGTH_MAX   255
#define FORMAT_TEXT_LENGTH 255
#define FORMAT_TEXT "spo-lab-db-format"

#define COLUMN_TYPE_TEXT   0
#define COLUMN_TYPE_INT    1

#define NORMAL_END 0
#define ERROR_END 1
#define ERROR_DATABASE_META_ALLOC 2
#define ERROR_DATABASE_FILE_OPEN 3
#define ERROR_DATABASE_FORMAT 4
#define ERROR_DATABASE_READ 5
#define ERROR_DATABASE_TABLE_READ 6
#define ERROR_TABLE_NOT_FOUND 7
#define ERROR_NO_TABLES 8
#define ERROR_COLUMN_NOT_FOUND 9
#define ERROR_TABLE_ALREADY_EXISTS 10
#define ERROR_EMPTY_REQUIRED_PARAMETER 11
#define ERROR_ENTRY_WITH_ID_ALREADY_EXISTS 12

#define ERROR_MESSAGE_NO_COLUMNS "Cannot execute create without columns"
#define ERROR_MESSAGE_END "Error. Operation failed"
#define ERROR_MESSAGE_META_ALLOC "Error. Cant alloc meta info"
#define ERROR_MESSAGE_DATABASE_META_ALLOC "Error. Database meta alloc failed"
#define ERROR_MESSAGE_DATABASE_FILE_OPEN "Error. Database file open failed"
#define ERROR_MESSAGE_DATABASE_FORMAT "Error. Database file has incorrect format"
#define ERROR_MESSAGE_DATABASE_READ "Error. Cannot read database"
#define ERROR_MESSAGE_DATABASE_TABLE_READ "Error. Cannot read table"
#define ERROR_MESSAGE_TABLE_NOT_FOUND "Error. Table not found"
#define ERROR_MESSAGE_NO_TABLES "Error. There are no tables"
#define ERROR_MESSAGE_COLUMN_NOT_FOUND "Error. Column not found"
#define ERROR_MESSAGE_TABLE_ALREADY_EXISTS "Error. Table already exists"
#define ERROR_MESSAGE_EMPTY_REQUIRED_PARAMETER "Error. Required parameter missing"
#define ERROR_MESSAGE_ENTRY_WITH_ID_ALREADY_EXISTS "Error. Entry with such id already exists"

#define MESSAGE_CREATED_SUCCESS "Successfully created"
#define MESSAGE_DELETED_SUCCESS "Successfully deleted"
#define MESSAGE_UPDATED_SUCCESS "Successfully updated"
#define MESSAGE_DROPPED_SUCCESS "Successfully dropped"

typedef struct MetaDB {
    char format_text[FORMAT_TEXT_LENGTH];
    time_t created;
    time_t updated;
    size_t tables_count;
    size_t first_table_offset;
} MetaDB;

typedef struct MetaColumn {
    char name[TEXT_LENGTH_MAX];
    size_t type;
    bool is_key;
    bool is_required;
    size_t next_column_offset;
} MetaColumn;

typedef struct MetaTable {
    char name[TEXT_LENGTH_MAX];
    size_t amount_columns;
    size_t amount_rows;
    size_t first_column_offset;
    size_t first_row_offset;
    size_t next_table_offset;
} MetaTable;

typedef struct MetaRow {
    int row_size;
    size_t first_data_offset;
    size_t next_row_offset;
} MetaRow;

typedef struct EntryToInsert {
    char column_name[TEXT_LENGTH_MAX];
    void *value_data;
} EntryToInsert;

#define SIZE_META_DB sizeof(MetaDB)
#define SIZE_META_TABLE sizeof(MetaTable)
#define SIZE_META_COLUMN sizeof(MetaColumn)
#define SIZE_META_ROW sizeof(MetaRow)
#define sizeof_max(a) ({ max_value(max_value(max_value(max_value(a, SIZE_META_DB), SIZE_META_TABLE), SIZE_META_COLUMN), SIZE_META_ROW); })


void db_init(char *f_name);

int create_table(char *name, int columns_count, MetaColumn columns[]);

int insert_into_table(char *table_name, int items_count, EntryToInsert items[]);

void find_table_row(
        MetaTable *table_meta,
        char *column_name,
        char *search_value,
        MetaRow **p_row_meta,
        char ***p_row_data,
        int offset
);

void find_all_table_rows(
        char *table_name,
        char *column_name,
        void *search_value,
        MetaColumn **p_columns,
        MetaRow ***p_rows,
        char ****p_rows_data,
        int *rows_count,
        int *amount_columns
);

int find_last_table_row(MetaTable table_meta, MetaRow *p_target_row_meta, size_t *p_row_offset);

int find_previous_table(size_t table_offset, MetaTable *p_target_table_meta, size_t *p_table_offset);

int find_column_by_name(MetaTable table_meta, char *column_name, MetaColumn *p_target_column_meta, int *p_index_column);

int delete_table(char *table_name);

int delete_row(char *table_name, char *field, void *key);

int update_table_rows(char *table_name, char *target_field, void *target_key, char *source_field, void *source_key);

#endif //SPO_LAB_1_5_CURSOR_H
