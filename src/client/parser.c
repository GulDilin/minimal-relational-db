#include "parser.h"

int count_siblings(struct ast_node * node) {
    int res = 0;
    while (node != NULL) {
        res ++;
        node = node->next;
    }
    return res;
}

int parse_select(struct ast_node * line, Common__Request * request) {
    debugp("SELECT COMMAND\n");
    request->command_code = COMMAND_CODE_SELECT;
    char * table_name = line->first->next->first->text;
    request->table_name = table_name;
    if (line->first->first->node_type == ALL) return 0;

    struct ast_node * first_column = line->first->first->first;
    int n_columns = count_siblings(first_column);
    request->n_columns = n_columns;
    request->columns = malloc(sizeof(Common__ColumnValue) * n_columns);

    struct ast_node * column = first_column;
    int i = 0;
    while (column != NULL) {
        Common__ColumnValue * column_value = malloc(sizeof(Common__ColumnValue));
        common__column_value__init(column_value);
        column_value->title = column->text;
        request->columns[i++] = column_value;
        column = column->next;
    }
    return 0;
}

int parse_create(struct ast_node * line, Common__Request * request) {
    debugp("CREATE COMMAND\n");
    request->command_code = COMMAND_CODE_CREATE;
    struct ast_node * table = line->first->first->first;
    char * table_name = table->text;
    debugf("Table name: %s\n", table_name);
    request->table_name = table_name;

    struct ast_node * first_column = table->first;
    int n_columns = count_siblings(first_column);
    request->n_columns = n_columns;
    debugf("N Columns: %d\n", n_columns);
    request->columns = malloc(sizeof(Common__ColumnValue) * n_columns);

    struct ast_node * column = first_column;
    int i = 0;
    while (column != NULL) {
        Common__ColumnValue * column_value = malloc(sizeof(Common__ColumnValue));
        common__column_value__init(column_value);
        struct ast_node * column_info = column->first->first;
        column_value->title = column_info->text;

        debugf("Column name: %s type: %d\n", column_info->text, column_info->next->node_type);
        switch(column_info->next->node_type) {
            case (TEXT_TYPE):
                column_value->column_type_code = COLUMN_TYPE_TEXT_N;
                break;
            case (NUMBER_TYPE):
                column_value->column_type_code = COLUMN_TYPE_TEXT_N;
                break;
        }
        column_value->column_type_code = column_info->next->node_type;

        request->columns[i++] = column_value;
        column = column->next;
    }
    return 0;
}

int parse_request_string(const char *in, Common__Request * request) {
    struct ast_node node;
    int rc = parse_string(in, &node);
    if (rc != 0) return rc;
    if (DEBUG) print_ast_tree(&node, 0);
    int node_type = node.first->first->node_type;
    debugf("Node type: %d\n", node_type);
    switch (node_type) {
        case SELECT:
            return parse_select(node.first, request);
        case CREATE:
            return parse_create(node.first, request);
        default:
            return rc;
    }
};
