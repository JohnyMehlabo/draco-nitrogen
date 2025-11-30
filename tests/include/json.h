#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct _JSONObject JSONObject;
typedef struct _JSONArray JSONArray;

typedef enum {
    VALUE_ERROR = 0,

    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_BOOLEAN,
    VALUE_NULL,
    VALUE_ARRAY,
    VALUE_OBJECT,
} JSONValueType;

typedef struct {
    JSONValueType type;
    union {
        double number;
        uint8_t* string;
        bool boolean;
        JSONObject* object;
        JSONArray* array;
    };
} JSONValue;

[[nodiscard]] JSONValue json_parse(uint8_t* text);
[[nodiscard]] JSONValue json_parse_from_file(uint8_t* filepath);

[[nodiscard]] uint8_t* json_serialize(JSONValue value);
void json_serialize_to_file(JSONValue value, const uint8_t* filepath);

// JSON object-related functions
[[nodiscard]] JSONObject* json_object_create_empty();
// Does not transfer ownership so strings should be copied
JSONValue* json_object_get(const JSONObject* object, const uint8_t* key);
// WARNING: It is recommended to not use this function as the memory ownership system is complex. For instance, string literals for string values passed to this function must be duplicated.
void json_object_set(JSONObject* object, const uint8_t* key, JSONValue value);
void json_object_set_number(JSONObject* object, const uint8_t* key, double number);
void json_object_set_string(JSONObject* object, const uint8_t* key, const uint8_t* string);
void json_object_set_object(JSONObject* object, const uint8_t* key, JSONObject* member_object);
void json_object_set_array(JSONObject* object, const uint8_t* key, JSONArray* array);
void json_object_free(JSONObject* object);

// JSON array-related functions
[[nodiscard]] JSONArray* json_array_create_empty();
// Does not transfer ownership so strings should be copied
JSONValue* json_array_get(const JSONArray* array, uint32_t index);
uint32_t json_array_length(const JSONArray* array);
// WARNING: It is recommended to not use this function as the memory ownership system is complex. For instance, string literals for string values passed to this function must be duplicated.
void json_array_push(JSONArray* array, JSONValue value);
void json_array_push_number(JSONArray* array, double number);
void json_array_push_string(JSONArray* array, const uint8_t* string);
void json_array_push_object(JSONArray* array, JSONObject* object);
void json_array_push_array(JSONArray* array, JSONArray* element_array);
void json_array_free(JSONArray* array);

void json_free(JSONValue value);