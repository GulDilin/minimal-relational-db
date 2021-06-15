/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: common/net.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "common/net.pb-c.h"
void   common__column_value__init
                     (Common__ColumnValue         *message)
{
  static const Common__ColumnValue init_value = COMMON__COLUMN_VALUE__INIT;
  *message = init_value;
}
size_t common__column_value__get_packed_size
                     (const Common__ColumnValue *message)
{
  assert(message->base.descriptor == &common__column_value__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t common__column_value__pack
                     (const Common__ColumnValue *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &common__column_value__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t common__column_value__pack_to_buffer
                     (const Common__ColumnValue *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &common__column_value__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Common__ColumnValue *
       common__column_value__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Common__ColumnValue *)
     protobuf_c_message_unpack (&common__column_value__descriptor,
                                allocator, len, data);
}
void   common__column_value__free_unpacked
                     (Common__ColumnValue *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &common__column_value__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   common__request__init
                     (Common__Request         *message)
{
  static const Common__Request init_value = COMMON__REQUEST__INIT;
  *message = init_value;
}
size_t common__request__get_packed_size
                     (const Common__Request *message)
{
  assert(message->base.descriptor == &common__request__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t common__request__pack
                     (const Common__Request *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &common__request__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t common__request__pack_to_buffer
                     (const Common__Request *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &common__request__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Common__Request *
       common__request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Common__Request *)
     protobuf_c_message_unpack (&common__request__descriptor,
                                allocator, len, data);
}
void   common__request__free_unpacked
                     (Common__Request *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &common__request__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   common__response__init
                     (Common__Response         *message)
{
  static const Common__Response init_value = COMMON__RESPONSE__INIT;
  *message = init_value;
}
size_t common__response__get_packed_size
                     (const Common__Response *message)
{
  assert(message->base.descriptor == &common__response__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t common__response__pack
                     (const Common__Response *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &common__response__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t common__response__pack_to_buffer
                     (const Common__Response *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &common__response__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Common__Response *
       common__response__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Common__Response *)
     protobuf_c_message_unpack (&common__response__descriptor,
                                allocator, len, data);
}
void   common__response__free_unpacked
                     (Common__Response *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &common__response__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor common__column_value__field_descriptors[6] =
{
  {
    "title",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Common__ColumnValue, title),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "column_type_code",
    2,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_INT32,
    offsetof(Common__ColumnValue, has_column_type_code),
    offsetof(Common__ColumnValue, column_type_code),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "value",
    3,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Common__ColumnValue, value),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "is_key",
    4,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_BOOL,
    offsetof(Common__ColumnValue, has_is_key),
    offsetof(Common__ColumnValue, is_key),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "is_required",
    5,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_BOOL,
    offsetof(Common__ColumnValue, has_is_required),
    offsetof(Common__ColumnValue, is_required),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "is_start",
    6,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_BOOL,
    offsetof(Common__ColumnValue, has_is_start),
    offsetof(Common__ColumnValue, is_start),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned common__column_value__field_indices_by_name[] = {
  1,   /* field[1] = column_type_code */
  3,   /* field[3] = is_key */
  4,   /* field[4] = is_required */
  5,   /* field[5] = is_start */
  0,   /* field[0] = title */
  2,   /* field[2] = value */
};
static const ProtobufCIntRange common__column_value__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 6 }
};
const ProtobufCMessageDescriptor common__column_value__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "common.ColumnValue",
  "ColumnValue",
  "Common__ColumnValue",
  "common",
  sizeof(Common__ColumnValue),
  6,
  common__column_value__field_descriptors,
  common__column_value__field_indices_by_name,
  1,  common__column_value__number_ranges,
  (ProtobufCMessageInit) common__column_value__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor common__request__field_descriptors[3] =
{
  {
    "command_code",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(Common__Request, command_code),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "table_name",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Common__Request, table_name),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "columns",
    4,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(Common__Request, n_columns),
    offsetof(Common__Request, columns),
    &common__column_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned common__request__field_indices_by_name[] = {
  2,   /* field[2] = columns */
  0,   /* field[0] = command_code */
  1,   /* field[1] = table_name */
};
static const ProtobufCIntRange common__request__number_ranges[2 + 1] =
{
  { 1, 0 },
  { 4, 2 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor common__request__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "common.Request",
  "Request",
  "Common__Request",
  "common",
  sizeof(Common__Request),
  3,
  common__request__field_descriptors,
  common__request__field_indices_by_name,
  2,  common__request__number_ranges,
  (ProtobufCMessageInit) common__request__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor common__response__field_descriptors[4] =
{
  {
    "status_code",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(Common__Response, status_code),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "command_code",
    2,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_INT32,
    offsetof(Common__Response, has_command_code),
    offsetof(Common__Response, command_code),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "columns",
    3,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(Common__Response, n_columns),
    offsetof(Common__Response, columns),
    &common__column_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "text",
    4,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Common__Response, text),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned common__response__field_indices_by_name[] = {
  2,   /* field[2] = columns */
  1,   /* field[1] = command_code */
  0,   /* field[0] = status_code */
  3,   /* field[3] = text */
};
static const ProtobufCIntRange common__response__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor common__response__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "common.Response",
  "Response",
  "Common__Response",
  "common",
  sizeof(Common__Response),
  4,
  common__response__field_descriptors,
  common__response__field_indices_by_name,
  1,  common__response__number_ranges,
  (ProtobufCMessageInit) common__response__init,
  NULL,NULL,NULL    /* reserved[123] */
};
