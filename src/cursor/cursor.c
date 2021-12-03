#include <time.h>
#include "cursor.h"
#include <sys/stat.h>

static char *file_name;
static MetaDB *meta_db;

bool file_exists(char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int update_meta_db() {
    FILE *f;
    size_t meta_db_size = SIZE_META_DB;
    MetaDB *meta_db_target = malloc(meta_db_size);
    if (!meta_db_target) {
        perror(ERROR_MESSAGE_META_ALLOC);
        return ERROR_DATABASE_META_ALLOC;
    }

    memset(meta_db_target, 0, meta_db_size);
    f = fopen(file_name, "r+b");
    if (f == NULL) {
        perror(ERROR_MESSAGE_DATABASE_FILE_OPEN);
        return ERROR_DATABASE_FILE_OPEN;
    }

    size_t read_bytes = fread(meta_db_target, meta_db_size, 1, f);
    if (read_bytes < 1) {
        perror(ERROR_MESSAGE_DATABASE_READ);
        return ERROR_DATABASE_READ;
    }

    if (strcmp(meta_db_target->format_text, FORMAT_TEXT) != 0) {
        perror(ERROR_MESSAGE_DATABASE_FORMAT);
        return ERROR_DATABASE_FORMAT;
    }

    meta_db = meta_db_target;
    fclose(f);
    return NORMAL_END;
}

int open_database_file(FILE **file) {
//    int return_code = update_meta_db();
//    if (return_code != NORMAL_END) return return_code;
    *file = fopen(file_name, "r+b");
    if (file == NULL) {
        perror(ERROR_MESSAGE_DATABASE_FILE_OPEN);
        return ERROR_DATABASE_FILE_OPEN;
    }
    return NORMAL_END;
}

void print_info(MetaDB metaData) {
    struct tm created_tm = *localtime(&metaData.created);
    struct tm modified_tm = *localtime(&metaData.updated);
    printf("Database info:\n\tCreated at: %02d.%02d.%d %02d:%02d:%02d\n\tLast updated: %02d.%02d.%d %02d:%02d:%02d\n\tMetaTables count: %zu\n\n",
           created_tm.tm_mday, created_tm.tm_mon + 1, created_tm.tm_year + 1900, created_tm.tm_hour,
           created_tm.tm_min, created_tm.tm_sec,
           modified_tm.tm_mday, modified_tm.tm_mon + 1, modified_tm.tm_year + 1900, modified_tm.tm_hour,
           modified_tm.tm_min, modified_tm.tm_sec,
           metaData.tables_count
    );
}

void print_table_meta(MetaTable meta_table, size_t offset) {
    printf("Table info:\n\tName %s\n\tOffset: %zu\n\tAmount columns: %zu\n\tAmount rows: %zu\n\tFirst row offset: %zu\n\tFirst column offset: %zu\n\tNext table offset: %zu\n",
           meta_table.name, offset, meta_table.amount_columns, meta_table.amount_rows, meta_table.first_row_offset,
           meta_table.first_column_offset, meta_table.next_table_offset
       );
}

void print_column_meta(MetaColumn meta_column) {
    printf("Column info:\n\tName %s\n\tNext column offset: %zu\n\tType: %zu\n\tIs key: %d\n\tIs required: %d\n",
           meta_column.name, meta_column.next_column_offset, meta_column.type, meta_column.is_key, meta_column.is_required
    );
}

void print_row_meta(MetaRow meta_row) {
    printf("Row info:\n\tSize %d\n\tFirst data offset: %zu\n\tNext row offset: %zu\n",
           meta_row.row_size, meta_row.first_data_offset, meta_row.next_row_offset
    );
}

void db_init(char *f_name) {
    file_name = f_name;
    if (file_exists(file_name)) {
        int return_code = update_meta_db();
        if (return_code != NORMAL_END) exit(return_code);
        print_info(*meta_db);
        return;
    }
    FILE *file = fopen(file_name, "wb");
    MetaDB metaData = {
            .format_text = FORMAT_TEXT,
            .created = time(NULL),
            .updated = time(NULL),
            .tables_count = 0,
            .first_table_offset = 0
    };
    fwrite(&metaData, SIZE_META_DB, 1, file);
    fclose(file);
    update_meta_db();
}

void update_file_meta() {
    debugp("Update meta db\n");
    meta_db->updated = time(NULL);
    FILE *file;
    open_database_file(&file);
    fseek(file, 0, SEEK_SET);
    size_t size = fwrite(meta_db, SIZE_META_DB, 1, file);
    debugf("Write %zu bytes\n", size);
    fclose(file);
}

void set_tables_count(size_t count) {
    meta_db->tables_count = count;
    update_file_meta();
}

void set_first_table_offset(size_t offset) {
    meta_db->first_table_offset = offset;
    update_file_meta();
}

void alloc_db_free(size_t requested_size, size_t *start_sector, size_t *alloc_size) {
    FILE *f;
    open_database_file(&f);
    int fd = fileno(f);
    *start_sector = lseek(fd, 0, SEEK_END);
    *alloc_size = requested_size;
    fclose(f);
}

MetaTable *read_table_meta(FILE *f, size_t table_offset) {
    MetaTable *table_meta = malloc(SIZE_META_TABLE);
    fseek(f, table_offset, SEEK_SET);
    if (fread(table_meta, SIZE_META_TABLE, 1, f) > 0) return table_meta;
    perror(ERROR_MESSAGE_DATABASE_TABLE_READ);
    exit(ERROR_DATABASE_TABLE_READ);
}

MetaRow *read_row_meta(FILE *f, size_t row_offset) {
    MetaRow *row_meta = malloc(SIZE_META_ROW);
    memset(row_meta, 0, SIZE_META_ROW);
    fseek(f, row_offset, SEEK_SET);
    fread(row_meta, SIZE_META_ROW, 1, f);
    return row_meta;
}

MetaColumn *read_column_meta(FILE *f, size_t offset) {
    MetaColumn *column_meta = malloc(SIZE_META_COLUMN);
    memset(column_meta, 0, SIZE_META_COLUMN);
    fseek(f, offset, SEEK_SET);
    fread(column_meta, SIZE_META_COLUMN, 1, f);
    return column_meta;
}

void read_table_columns_meta(FILE *f, MetaTable table_meta, MetaColumn **columns) {
    *columns = malloc(SIZE_META_COLUMN * table_meta.amount_columns);
    size_t offset = table_meta.first_column_offset;
    for (int i = 0; i < table_meta.amount_columns; ++i) {
        MetaColumn *column_meta = malloc(SIZE_META_COLUMN);
        memset(column_meta, 0, SIZE_META_COLUMN);
        fseek(f, offset, SEEK_SET);
        fread(column_meta, SIZE_META_COLUMN, 1, f);

        (*columns)[i] = *column_meta;
        offset = column_meta->next_column_offset;
    }
}

char **read_row_data(FILE *f, MetaRow row_meta) {
    debugp("Start read row data\n");
    char **row_data = malloc(row_meta.row_size * sizeof(char *));
    for (int i = 0; i < row_meta.row_size; ++i) {
        row_data[i] = malloc(TEXT_LENGTH_MAX);
        memset(row_data[i], 0, TEXT_LENGTH_MAX);
        debugf("First data offset: %zu\n", row_meta.first_data_offset);
        fseek(f, (long) row_meta.first_data_offset + TEXT_LENGTH_MAX * i, SEEK_SET);
        fread(row_data[i], TEXT_LENGTH_MAX, 1, f);
        debugf("Read data for col %d: %s\n", i, row_data[i]);
    }
    debugp("End read row data\n");
    return row_data;
}

void write_db_part(void *value, size_t size, size_t offset) {
    FILE *file;
    open_database_file(&file);
    fseek(file, offset, SEEK_SET);
    fwrite(value, size, 1, file);
    debugf("Write table bytes to: %zu size: %zu\n", offset, size);
    fclose(file);
}

void write_column_meta(MetaColumn *column_meta, size_t offset) {
    debugp("Write column meta\n");
    if (DEBUG) print_column_meta(*column_meta);
    write_db_part(column_meta, SIZE_META_COLUMN, offset);
}

void write_table_meta(MetaTable *table_meta, size_t offset) {
    debugp("Write table meta");
    if (DEBUG) print_table_meta(*table_meta, offset);
    write_db_part(table_meta, SIZE_META_TABLE, offset);
}

void write_row_meta(MetaRow *row_meta, size_t offset) {
    debugp("Write row meta\n");
    if (DEBUG) print_row_meta(*row_meta);
    write_db_part(row_meta, SIZE_META_ROW, offset);
}

void write_zeros_db_part(FILE *file, size_t size, size_t offset) {
    open_database_file(&file);
    char *zeros = malloc(size);
    memset(zeros, 0, size);
    fseek(file, offset, SEEK_SET);
    fwrite(zeros, SIZE_META_ROW, 1, file);
    free(zeros);
}

int find_table_by_name(char *table_name, MetaTable *p_target_table_meta, size_t *p_table_offset) {
    FILE *f;
    size_t table_offset = meta_db->first_table_offset;
    debugf("Find table by name: %s. first table offset: %zu\n", table_name, table_offset);
    while (table_offset > 0) {
        open_database_file(&f);
        MetaTable table = *read_table_meta(f, table_offset);
        fclose(f);
        if (DEBUG) print_table_meta(table, table_offset);
        char name_a[TEXT_LENGTH_MAX];
        char name_b[TEXT_LENGTH_MAX];
        strncpy(name_a, table_name, TEXT_LENGTH_MAX);
        strncpy(name_b, table.name, TEXT_LENGTH_MAX);
        if (!string_equals(name_a, name_b)) {
            table_offset = table.next_table_offset;
            continue;
        }
        if (p_table_offset != NULL) *p_table_offset = table_offset;
        if (p_target_table_meta != NULL) *p_target_table_meta = table;
        debugp("__Found__\n");
        return NORMAL_END;
    }
    debugp("__Not Found__\n");
    return ERROR_TABLE_NOT_FOUND;
}

int find_last_table(MetaTable **p_target_table_meta, size_t *p_table_offset) {
    debugp("Find last table\n");
    if (meta_db->tables_count == 0) return ERROR_NO_TABLES;
    FILE *f;
    MetaTable * table_meta;
    size_t offset = meta_db->first_table_offset;
    for (int i = meta_db->tables_count; i > 0; --i) {
        open_database_file(&f);
        table_meta = read_table_meta(f, offset);
        if (DEBUG) print_table_meta(*table_meta, offset);
        fclose(f);
        if (i == 0 || table_meta->next_table_offset == 0) break;
        offset = table_meta->next_table_offset;
    }
    if (p_table_offset != NULL && table_meta != NULL) *p_table_offset = offset;
    if (p_target_table_meta != NULL && table_meta != NULL) *p_target_table_meta = table_meta;
    debugp("Find last table end\n\n");

    return NORMAL_END;
}

int find_last_table_row(MetaTable table_meta, MetaRow *p_target_row_meta, size_t *p_row_offset) {
    debugp("Start find last table row\n");
    if (meta_db->tables_count == 0) return ERROR_NO_TABLES;
    FILE *f;
    size_t offset = table_meta.first_row_offset;
    MetaRow * row_meta = malloc(sizeof(MetaRow));
    for (int i = table_meta.amount_rows; i > 0; --i) {
        debugf("Find last table row curr offset: %zu\n", offset);
        open_database_file(&f);
        *row_meta = *read_row_meta(f, offset);
        fclose(f);
        if (i == 0) break;
        if (row_meta->next_row_offset == 0) break;
        offset = row_meta->next_row_offset;
    }
    debugf("Find last table row result offset: %zu\n", offset);
    if (p_row_offset != NULL) *p_row_offset = offset;
    if (p_target_row_meta != NULL) *p_target_row_meta = *row_meta;
    debugp("End find last table row\n");
    return NORMAL_END;
}

int find_previous_table(size_t table_offset, MetaTable *p_target_table_meta, size_t *p_table_offset) {
    if (meta_db->tables_count == 0) return ERROR_NO_TABLES;
    FILE *f;
    size_t offset = meta_db->first_table_offset;
    MetaTable table_meta;
    for (int i = meta_db->tables_count; i > 0; --i) {
        open_database_file(&f);
        table_meta = *read_table_meta(f, offset);
        fclose(f);
        if (table_meta.next_table_offset == table_offset) break;
        if (i == 0) return ERROR_TABLE_NOT_FOUND;
        offset = table_meta.next_table_offset;
    }
    if (p_table_offset != NULL) *p_table_offset = offset;
    if (p_target_table_meta != NULL) *p_target_table_meta = table_meta;
    return NORMAL_END;
}


int find_column_by_name(
        MetaTable table_meta,
        char *column_name,
        MetaColumn *p_target_column_meta,
        int *p_index_column
) {
    FILE *f;
    size_t offset = table_meta.first_column_offset;
    debugf("Find column by name: %s, First column offset: %zu\n", column_name, offset);

    for (int i = 0; i < table_meta.amount_columns; ++i) {
        open_database_file(&f);
        MetaColumn * column = read_column_meta(f, offset);
        fclose(f);
        debugf("Read COLUMN %d meta. NAME: %s TYPE: %zu Next_offset: %zu\n",
               i, column->name, column->type, column->next_column_offset
        );

        if (column_name != NULL && !string_equals(column->name, column_name)) {
            offset = column->next_column_offset;
            continue;
        }
        if (p_index_column != NULL) *p_index_column = i;
        if (p_target_column_meta != NULL) *p_target_column_meta = *column;
        debugp("__Found__\n");
        return NORMAL_END;
    }
    debugp("__Not_Found__\n");
    return ERROR_COLUMN_NOT_FOUND;
}

void find_table_row(
        MetaTable *table_meta,
        char *column_name,
        char *search_value,
        MetaRow **p_row_meta,
        char ***p_row_data,
        int offset
) {
    debugf("Find table row: column name: %s search: %s\n", column_name, search_value);
    MetaColumn column_meta;
    int index_column;

    if (column_name != NULL) {
        int return_code = find_column_by_name(*table_meta, column_name, &column_meta, &index_column);
        if (return_code == ERROR_COLUMN_NOT_FOUND) {
            if (p_row_meta != NULL) *p_row_meta = NULL;
            if (p_row_data != NULL) *p_row_data = NULL;
            return;
        }
        debugf("Found table col: column name: %s col index: %d\n", column_name, index_column);
    }

    FILE *f;
    int tmp_offset = offset;
    size_t row_offset = table_meta->first_row_offset;

    for (int i = 0; i < table_meta->amount_rows; ++i) {
        open_database_file(&f);
        MetaRow *row_meta = read_row_meta(f, row_offset);
        fclose(f);
        if (tmp_offset-- > 0) {
            row_offset = row_meta->next_row_offset;
            continue;
        }
        open_database_file(&f);
        char ** row_data = read_row_data(f, *row_meta);
        fclose(f);

        debugp("Read row data finished\n");

        if (column_name != NULL) {
            char data_value[TEXT_LENGTH_MAX];
            debugp("Copy data value\n");
            strncpy(data_value, row_data[index_column], TEXT_LENGTH_MAX);

            debugp("Check string equal\n");
            if (search_value != NULL && !string_equals(data_value, search_value)) {
                row_offset = row_meta->next_row_offset;
                continue;
            }
        }

        if (p_row_meta != NULL) *p_row_meta = row_meta;
        if (p_row_data != NULL) *p_row_data = row_data;
        return;
    }

    if (p_row_meta != NULL) *p_row_meta = NULL;
}

void find_all_table_rows(
        char *table_name,
        char *column_name,
        void *search_value,
        MetaColumn **p_columns,
        MetaRow ***p_rows,
        char ****p_rows_data,
        int *rows_count,
        int *amount_columns
) {
    MetaColumn *ret_columns;
    MetaRow **ret_row = malloc(1024 * SIZE_META_ROW);

    MetaTable table_meta;
    find_table_by_name(table_name, &table_meta, NULL);
    FILE *f;
    open_database_file(&f);
    read_table_columns_meta(f, table_meta, &ret_columns);
    fclose(f);
    *amount_columns = table_meta.amount_columns;
    char ***ret_rows_data = malloc(table_meta.amount_rows * sizeof(char *));

    int i = 0;
    while (true) {
        MetaRow *find;
        char **data;
        find_table_row(&table_meta, column_name, search_value, &find, &data, i);
        if (find == NULL) break;
        ret_row[i] = find;
        ret_rows_data[i] = data;
        i++;
    }
    *rows_count = i;

    if (p_rows != NULL) *p_rows = ret_row;
    else free(ret_row);

    if (p_rows_data != NULL) *p_rows_data = ret_rows_data;
    else free(ret_rows_data);

    if (p_columns != NULL) *p_columns = ret_columns;
    else free(ret_columns);
}

void delete_table_columns(FILE *f, MetaTable *table_meta) {
    size_t offset = table_meta->first_column_offset;
    for (int i = 0; i < table_meta->amount_columns; ++i) {
        MetaColumn *column = read_column_meta(f, offset);
        write_zeros_db_part(f, SIZE_META_COLUMN, offset);
        offset = column->next_column_offset;
        free(column);
    }
}

void delete_table_rows(FILE *f, MetaTable *table_meta) {
    size_t offset = table_meta->first_row_offset;
    for (int i = 0; i < table_meta->amount_rows; ++i) {
        MetaRow *row_meta = read_row_meta(f, offset);
        write_zeros_db_part(f, row_meta->row_size, row_meta->first_data_offset);
        write_zeros_db_part(f, SIZE_META_ROW, offset);
        offset = row_meta->next_row_offset;
        free(row_meta);
    }
}

int delete_table(char *table_name) {
    FILE *f;
    open_database_file(&f);
    MetaTable table_meta;
    size_t table_offset;
    int return_code = find_table_by_name(table_name, &table_meta, &table_offset);

    if (return_code != NORMAL_END) return return_code;

    delete_table_columns(f, &table_meta);
    delete_table_rows(f, &table_meta);
    write_db_part(f, SIZE_META_TABLE, table_offset);
    fclose(f);

    if (meta_db->first_table_offset == table_offset) {
        set_first_table_offset(table_meta.next_table_offset);
    } else {
        MetaTable previous_table;
        size_t previous_table_offset;
        find_previous_table(table_offset, &previous_table, &previous_table_offset);
        previous_table.next_table_offset = table_meta.next_table_offset;
        write_table_meta(&previous_table, previous_table_offset);
    }

    set_tables_count(meta_db->tables_count - 1);
    return NORMAL_END;
}

int create_table(char *name, int amount_columns, MetaColumn columns[]) {
    debugf("\n\nCreate table start. Table name: %s Amount cols: %d\n", name, amount_columns);

    size_t offset = SIZE_META_DB;

    int return_code = find_table_by_name(name, NULL, NULL);
    if (return_code == NORMAL_END) return ERROR_TABLE_ALREADY_EXISTS;

    MetaTable * last_table_meta = malloc(SIZE_META_TABLE);
    memset(last_table_meta, 0, SIZE_META_TABLE);
    size_t last_table_offset = 0;

    return_code = find_last_table(&last_table_meta, &last_table_offset);
    debugf("Last table found. Offset: %zu\n", last_table_offset);
    if (return_code != ERROR_NO_TABLES) offset = SIZE_META_DB;
    if (DEBUG && last_table_offset != 0) print_table_meta(*last_table_meta, last_table_offset);
    debugp("LAST_____\n\n");

    size_t first_col_offset = 0;
    for (int i = 0; i < amount_columns; ++i) {
        size_t start = 0;
        size_t len = 0;
        columns[i].next_column_offset = 0;
        alloc_db_free(SIZE_META_COLUMN, &start, &len);
        debugf("Found free start: %zu  col name: %s\n", start, columns[i].name);
        write_column_meta(&columns[i], start);
        if (i != 0) columns[i - 1].next_column_offset = start;
        else first_col_offset = start;
    }

    for (int i = 0; i < amount_columns; ++i) {
        if (i == 0) {
            debugf("write col to: %zu\n", first_col_offset);
            write_column_meta(&columns[i], first_col_offset);
            continue;
        }
        debugf("write col to: %zu\n", columns[i-1].next_column_offset);
        write_column_meta(&columns[i], columns[i-1].next_column_offset);
    }

    MetaTable *table_meta = malloc(SIZE_META_TABLE);
    memset(table_meta, 0, SIZE_META_TABLE);

    strncpy(table_meta->name, name, TEXT_LENGTH_MAX);
    table_meta->first_column_offset = first_col_offset;
    table_meta->first_row_offset = 0;
    table_meta->amount_columns = amount_columns;
    table_meta->amount_rows = 0;
    table_meta->next_table_offset = 0;

    size_t start = 0;
    size_t len = 0;
    alloc_db_free(SIZE_META_TABLE, &start, &len);

    write_table_meta(table_meta, start);
    debugp("Created table:\n");
    print_table_meta(*table_meta, start);

    if (meta_db->tables_count == 0) set_first_table_offset(start);
    else {
        debugf("Last table offset: %zu\n", last_table_offset);
        last_table_meta->next_table_offset = start;
        write_table_meta(last_table_meta, last_table_offset);
    }
    set_tables_count(meta_db->tables_count + 1);
    free(table_meta);
    debugp("Table creation end\n");
    free(last_table_meta);
    print_info(*meta_db);
    return 0;
}

int insert_into_table(char *table_name, int items_count, EntryToInsert items[]) {
    debugp("Start insert into\n");
    MetaTable table_meta;
    size_t table_offset;
    int return_code = find_table_by_name(table_name, &table_meta, &table_offset);
    if (return_code != NORMAL_END) return return_code;

    size_t row_size = items_count;
    size_t row_size_bytes = TEXT_LENGTH_MAX * items_count;
    char *row_data = malloc(row_size_bytes);
    memset(row_data, 0, row_size_bytes);
    debugp("Row data allocated\n");

    MetaColumn column_meta;
    for (int k = 0; k < items_count; ++k) {
        EntryToInsert item = items[k];
        return_code = find_column_by_name(table_meta, item.column_name, &column_meta, NULL);
        if (return_code != NORMAL_END) return return_code;

        if (column_meta.is_required && strlen(item.value_data) == 0) return ERROR_EMPTY_REQUIRED_PARAMETER;
        if (column_meta.is_key) {
            MetaRow *find;
            find_table_row(&table_meta, column_meta.name, item.value_data, &find, NULL, 0);
            if (find != NULL) {
                free(find);
                free(row_data);
                return ERROR_ENTRY_WITH_ID_ALREADY_EXISTS;
            }
        }
        debugp("Start copy data \n");

        strncpy(row_data + TEXT_LENGTH_MAX * k, item.value_data, TEXT_LENGTH_MAX);
        strncpy(row_data + TEXT_LENGTH_MAX * (k + 1) - 1, "\0", 1);
        debugp("End copy data \n");
    }

    size_t start = 0;
    size_t len = 0;
    alloc_db_free(row_size_bytes, &start, &len);
    write_db_part(row_data, row_size_bytes, start);
    MetaRow row_meta = {
            .row_size = row_size,
            .first_data_offset = start,
            .next_row_offset = 0
    };
    alloc_db_free(SIZE_META_ROW, &start, &len);
    write_db_part(&row_meta, SIZE_META_ROW, start);


    if (table_meta.amount_rows > 0) {
        MetaRow last_row_meta;
        size_t last_row_offset;
        find_last_table_row(table_meta, &last_row_meta, &last_row_offset);
        last_row_meta.next_row_offset = start;
        write_row_meta(&last_row_meta, last_row_offset);
    } else {
        table_meta.first_row_offset = start;
    }
    table_meta.amount_rows++;
    write_table_meta(&table_meta, table_offset);
    return NORMAL_END;
}
