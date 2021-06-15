#include <time.h>
#include "cursor.h"
#include <sys/stat.h>

static char *file_name;
static int tables_count;
static MetaDB meta_db;

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

    tables_count = meta_db_target->tables_count;
    meta_db = *meta_db_target;
    fclose(f);
    return NORMAL_END;
}

int open_database_file(FILE **file) {
    int return_code = update_meta_db();
    if (return_code != NORMAL_END) return return_code;
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

void db_init(char *f_name) {
    file_name = f_name;
    if (file_exists(file_name)) {
        int return_code = update_meta_db();
        if (return_code != NORMAL_END) exit(return_code);
        print_info(meta_db);
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
    meta_db.updated = time(NULL);
    FILE *file;
    open_database_file(&file);
    fseek(file, 0, SEEK_SET);
    fwrite(&meta_db, SIZE_META_DB, 1, file);
    fclose(file);
}

void set_tables_count(size_t count) {
    meta_db.tables_count = count;
    update_file_meta();
}

void set_first_table_offset(size_t offset) {
    meta_db.first_table_offset = offset;
    update_file_meta();
}

void alloc_db_free(size_t requested_size, size_t *start_sector, size_t * alloc_size) {
    FILE *f;
    open_database_file(&f);
    int fd = fileno(f);
    *start_sector = lseek(fd, 0, SEEK_END);
    *alloc_size = requested_size;
    fclose(f);
}

MetaTable * read_table_meta(FILE *f, size_t table_offset) {
    MetaTable * table_meta = malloc(SIZE_META_TABLE);
    fseek(f, table_offset, SEEK_SET);
    if (fread(table_meta, SIZE_META_TABLE, 1, f) > 0) return table_meta;
    perror(ERROR_MESSAGE_DATABASE_TABLE_READ);
    exit(ERROR_DATABASE_TABLE_READ);
}

MetaRow * read_row_meta(FILE *f, size_t row_offset) {
    MetaRow * row_meta = malloc(SIZE_META_ROW);
    memset(row_meta, 0, SIZE_META_ROW);
    fseek(f, row_offset, SEEK_SET);
    fread(row_meta, SIZE_META_ROW, 1, f);
    return row_meta;
}

MetaColumn * read_column_meta(FILE *f, size_t offset) {
    MetaColumn * column_meta = malloc(SIZE_META_COLUMN);
    memset(column_meta, 0, SIZE_META_COLUMN);
    fseek(f, offset, SEEK_SET);
    fread(column_meta, SIZE_META_COLUMN, 1, f);
    return column_meta;
}

void read_table_columns_meta(FILE *f, MetaTable table_meta, MetaColumn *** columns) {
    *columns = malloc(SIZE_META_COLUMN * table_meta.amount_columns);
    size_t offset = table_meta.first_column_offset;
    for (int i = 0; i < table_meta.amount_columns; ++i) {
        MetaColumn *column_meta = malloc(SIZE_META_COLUMN);
        memset(column_meta, 0, SIZE_META_COLUMN);
        fseek(f, offset, SEEK_SET);
        fread(column_meta, SIZE_META_COLUMN, 1, f);

        *columns[i] = column_meta;
        offset = column_meta->next_column_offset;
    }
}

char * read_row_data(FILE *f, MetaRow row_meta) {
    char * row_data = malloc(row_meta.row_size + 1);
    memset(row_data, 0, row_meta.row_size + 1);
    fseek(f, row_meta.first_data_offset, SEEK_SET);
    fread(row_data, row_meta.row_size, 1, f);
    return row_data;
}

void write_db_part(void * value, size_t size, size_t offset) {
    FILE *file;
    open_database_file(&file);
    fseek(file, offset, SEEK_SET);
    fwrite(value, size, 1, file);
    fclose(file);
}

void write_column_meta(MetaColumn * column_meta, size_t offset) {
    write_db_part(column_meta, SIZE_META_COLUMN, offset);
}

void write_table_meta(MetaTable * table_meta, size_t offset) {
    write_db_part(table_meta, SIZE_META_TABLE, offset);
}

void write_zeros_db_part(FILE * file, size_t size, size_t offset) {
    open_database_file(&file);
    char * zeros = malloc(size);
    memset(zeros, 0, size);
    fseek(file, offset, SEEK_SET);
    fwrite(zeros, SIZE_META_ROW, 1, file);
    free(zeros);
}

int find_table_by_name(char *table_name, MetaTable *p_target_table_meta, size_t * p_table_offset) {
    FILE *f;
    size_t table_offset = meta_db.first_table_offset;
    open_database_file(&f);
    while (table_offset > 0) {
        MetaTable table = *read_table_meta(f, table_offset);
        if (!string_equals(table.name, table_name)) {
            table_offset = table.next_table_offset;
            continue;
        }
        if (p_table_offset != NULL) *p_table_offset = table_offset;
        if (p_target_table_meta != NULL) *p_target_table_meta = table;
        fclose(f);
        return NORMAL_END;
    }
    fclose(f);
    return ERROR_TABLE_NOT_FOUND;
}

int find_last_table(MetaTable *p_target_table_meta, size_t * p_table_offset) {
    if (meta_db.tables_count == 0) return ERROR_NO_TABLES;
    FILE *f;
    open_database_file(&f);
    size_t offset = meta_db.first_table_offset;
    MetaTable table_meta;
    for (int i = meta_db.tables_count; i > 0; --i) {
        table_meta = *read_table_meta(f, offset);
        if (i == 0) break;
        offset = table_meta.next_table_offset;
    }
    if (p_table_offset != NULL) *p_table_offset = offset;
    if (p_target_table_meta != NULL) *p_target_table_meta = table_meta;
    return NORMAL_END;
}

int find_last_table_row(MetaTable table_meta, MetaRow * p_target_row_meta, size_t * p_row_offset) {
    if (meta_db.tables_count == 0) return ERROR_NO_TABLES;
    FILE *f;
    open_database_file(&f);
    size_t offset = table_meta.first_row_offset;
    MetaRow row_meta;
    for (int i = table_meta.amount_rows; i > 0; --i) {
        row_meta = *read_row_meta(f, offset);
        if (i == 0) break;
        offset = row_meta.next_row_offset;
    }
    if (p_row_offset != NULL) *p_row_offset = offset;
    if (p_target_row_meta != NULL) *p_target_row_meta = row_meta;
    return NORMAL_END;
}

int find_previous_table(size_t table_offset, MetaTable *p_target_table_meta, size_t * p_table_offset) {
    if (meta_db.tables_count == 0) return ERROR_NO_TABLES;
    FILE *f;
    open_database_file(&f);
    size_t offset = meta_db.first_table_offset;
    MetaTable table_meta;
    for (int i = meta_db.tables_count; i > 0; --i) {
        table_meta = *read_table_meta(f, offset);
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
    open_database_file(&f);
    for (int i = 0; i < table_meta.amount_columns; ++i) {
        MetaColumn column;
        fseek(f, offset, SEEK_SET);
        fread(&column, SIZE_META_COLUMN, 1, f);

        if (column_name != NULL && !string_equals(column.name, column_name)) {
            offset = column.next_column_offset;
            continue;
        }
        if (p_index_column != NULL) *p_index_column = i;
        if (p_target_column_meta != NULL) *p_target_column_meta = column;
        return NORMAL_END;
    }
    fclose(f);
    return ERROR_COLUMN_NOT_FOUND;
}

void parse_row(const char *row_data, int row_size, void *data[]) {
    int offset = 0;
    int data_elem = 0;
    for (int row_pos = 0; row_pos < row_size; ++row_pos) {
        if (row_data[row_pos] != STRING_END) continue;
        const char *item = row_data + offset;

        size_t size = strlen(item);
        if (size == 0) size = 1;
        data[data_elem] = malloc(size + 1);
        memset(data[data_elem], 0, size + 1);
        strcpy(data[data_elem], item);
        offset = (row_pos + 1);
        data_elem++;
    }
    data[data_elem] = malloc(strlen((row_data + offset)) + 1);
    memset(data[data_elem], 0, strlen((row_data + offset)) + 1);
    strcpy(data[data_elem], row_data + offset);
}

void find_table_row(
        MetaTable * table_meta,
        char * column_name,
        void * search_value,
        MetaRow **p_row_meta,
        char **p_row_data,
        int offset
) {
    MetaColumn column_meta;
    int index_column;

    int return_code = find_column_by_name(*table_meta, column_name, &column_meta, &index_column);
    if (return_code == ERROR_COLUMN_NOT_FOUND) {
        if (p_row_meta != NULL) *p_row_meta = NULL;
        if (p_row_data != NULL) *p_row_data = NULL;
        return;
    }

    FILE *f;
    open_database_file(&f);
    int tmp_offset = offset;
    size_t row_offset = table_meta->first_row_offset;

    for (int i = 0; i < table_meta->amount_rows; ++i) {
        MetaRow *row_meta = read_row_meta(f, row_offset);
        if (tmp_offset-- > 0) {
            row_offset = row_meta->next_row_offset;
            continue;
        }

        char *row_data = read_row_data(f, *row_meta);
        void *row_data_result[table_meta->amount_columns];
        parse_row(row_data, row_meta->row_size, row_data_result);
        char *data_value = row_data_result[index_column];

        if (search_value != NULL && !string_equals(data_value, search_value)) {
            row_offset = row_meta->next_row_offset;
            free(row_meta);
            free(row_data);
            continue;
        }

        fclose(f);
        if (p_row_meta != NULL) *p_row_meta = row_meta;
        if (p_row_data != NULL) *p_row_data = row_data;
        return;
    }

    fclose(f);
    if (p_row_meta != NULL) *p_row_meta = NULL;
    if (p_row_data != NULL) *p_row_data = NULL;
}

void find_all_table_rows(
        char *table_name,
        char *column_name,
        void *search_value,
        MetaColumn ***p_columns,
        MetaRow ***p_rows,
        char ***p_rows_data,
        int *rows_count,
        int *amount_columns
) {
    MetaColumn **ret_columns;
    MetaRow **ret_row = malloc(1024 * SIZE_META_ROW);
    char **ret_rows_data = malloc(1024 * SIZE_META_ROW);

    MetaTable table_meta;
    find_table_by_name(table_name, &table_meta, NULL);
    FILE *f;
    open_database_file(&f);
    read_table_columns_meta(f, table_meta, &ret_columns);
    fclose(f);
    *amount_columns = table_meta.amount_columns;

    int i = 0;
    while (true) {
        MetaRow *find;
        char *data;
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

void delete_table_columns(FILE *f, MetaTable * table_meta) {
    size_t offset = table_meta->first_column_offset;
    for (int i = 0; i < table_meta->amount_columns; ++i) {
        MetaColumn *column = read_column_meta(f, offset);
        write_zeros_db_part(f, SIZE_META_COLUMN, offset);
        offset = column->next_column_offset;
        free(column);
    }
}

void delete_table_rows(FILE *f, MetaTable * table_meta) {
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

    if (meta_db.first_table_offset == table_offset) {
        set_first_table_offset(table_meta.next_table_offset);
    } else {
        MetaTable previous_table;
        size_t previous_table_offset;
        find_previous_table(table_offset, &previous_table, &previous_table_offset);
        previous_table.next_table_offset = table_meta.next_table_offset;
        write_table_meta(&previous_table, previous_table_offset);
    }

    set_tables_count(meta_db.tables_count - 1);
    return NORMAL_END;
}

int create_table(char *name, int amount_columns, MetaColumn columns[]) {
    size_t offset = SIZE_META_DB;

    int return_code = find_table_by_name(name, NULL, NULL);
    if (return_code == NORMAL_END) return ERROR_TABLE_ALREADY_EXISTS;

    MetaTable last_table_meta;
    size_t last_table_offset;

    return_code = find_last_table(&last_table_meta, &last_table_offset);
    if (return_code != ERROR_NO_TABLES ) offset = last_table_offset;

    for (int i = 0; i < amount_columns; ++i) {
        size_t start = 0;
        size_t len = 0;
        alloc_db_free(SIZE_META_COLUMN, &start, &len);
        if (i != amount_columns - 1) columns[i + 1].next_column_offset = offset;
        write_column_meta(&columns[i], offset);
    }

    MetaTable *table_meta = malloc(SIZE_META_TABLE);
    memset(table_meta, 0, SIZE_META_TABLE);

    strcpy(table_meta->name, name);
    table_meta->first_column_offset = offset;
    table_meta->first_row_offset = 0;
    table_meta->amount_columns = amount_columns;
    table_meta->amount_rows = 0;
    table_meta->next_table_offset = 0;

    size_t start = 0;
    size_t len = 0;
    alloc_db_free(SIZE_META_TABLE, &start, &len);

    FILE *f;
    open_database_file(&f);
    fseek(f, offset, SEEK_SET);
    fwrite(table_meta, sizeof(*table_meta), 1, f);
    fclose(f);

    if (meta_db.tables_count == 0) set_first_table_offset(offset);
    else {
        last_table_meta.next_table_offset = offset;
        write_table_meta(&last_table_meta, last_table_offset);
    }
    set_tables_count(++tables_count);
    free(table_meta);
    return 0;
}

int insert_into_table(char *table_name, int items_count, EntryToInsert items[]) {
    MetaTable table_meta;
    size_t table_offset;
    int return_code = find_table_by_name(table_name, &table_meta, &table_offset);
    if (return_code != NORMAL_END) return return_code;

    size_t table_ptr = meta_db.first_table_offset;
    size_t row_size = TEXT_LENGTH_MAX * items_count + items_count;
    size_t str_offset = 0;
    char *row_data = malloc(row_size);
    memset(row_data, 0, row_size);

    MetaColumn column_meta;
    for (int k = 0; k < items_count; ++k) {
        EntryToInsert item = items[k];
        return_code = find_column_by_name(table_meta, item.column_name, &column_meta, NULL);

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

        if (strlen(item.value_data) == 0) {
            strcpy(row_data + str_offset, " ");
            str_offset += 1;
            strcpy(row_data + ++str_offset, "\0");
        } else {
            strcpy(row_data + str_offset, item.value_data);
            str_offset += strlen(item.value_data);
            strcpy(row_data + ++str_offset, "\0");
        }
    }

    int data_length = 0;
    for (data_length = 0; data_length < row_size; data_length++)
        if (row_data[data_length] == STRING_END &&
            data_length < row_size - 1 &&
            row_data[data_length + 1] == STRING_END
        ) break;

    size_t offset = SIZE_META_DB;
    size_t start = 0;
    size_t len = 0;
    alloc_db_free(sizeof_max(data_length), &start, &len);
    write_db_part(row_data, data_length, offset);
    MetaRow row_meta = {
            .row_size = data_length,
            .first_data_offset = offset,
            .next_row_offset = 0
    };
    alloc_db_free(SIZE_META_ROW, &start, &len);
    write_db_part(&row_meta, SIZE_META_ROW, offset);


    if (table_meta.amount_rows > 0) {
        MetaRow last_row_meta;
        size_t last_row_offset;
        find_last_table_row(table_meta, &last_row_meta, &last_row_offset);
        last_row_meta.next_row_offset = offset;
    } else {
        table_meta.first_row_offset = offset;
    }
    table_meta.amount_rows ++;
    write_table_meta(&table_meta, table_offset);
    return NORMAL_END;
}
