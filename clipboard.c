#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define allocate_at_a_time 10
//todo: add support for integer and float


//types o - object, l - list, s - dynamic string, b - boolean, n - null, i - integer, f - float

bool is_ignoring_char(char c){
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}


struct dynamic_string{
    char* value;
    int size;
    int capacity;
};
typedef struct dynamic_string string;

void init_dynamic_string(string* str){
    str->value = (char*)malloc(sizeof(char) * allocate_at_a_time);
    str->size = 0;
    str->capacity = allocate_at_a_time;
}

void append_dynamic_string(string* str, char c){
    if (str->size == str->capacity){
        str->capacity += allocate_at_a_time;
        str->value = (char*)realloc(str->value, sizeof(char) * str->capacity);
    }
    str->value[str->size] = c;
    str->size++;
}

char* string_to_char(string* str){

}

struct Value{
    void* value;
    char type;
};
typedef struct Value Value;


struct Entry{
    string* key;
    Value* value;
};
typedef struct Entry Entry;


struct Object{
    Entry** entries;
    int size;
    int capacity;
};
typedef struct Object Object;

void init_object(Object* obj){
    obj->entries = (Entry**)malloc(sizeof(Entry*) * allocate_at_a_time);
    obj->size = 0;
    obj->capacity = allocate_at_a_time;
}

void append_object(Object* obj, Entry* entry){
    if (obj->size == obj->capacity){
        obj->capacity += allocate_at_a_time;
        obj->entries = (Entry**)realloc(obj->entries, sizeof(Entry*) * obj->capacity);
    }
    obj->entries[obj->size] = entry;
    obj->size++;
}

struct List{
    Value** objects;
    int size;
    int capacity;
};
typedef struct List List;

void init_list(List* list){
    list->objects = (Value**)malloc(sizeof(Value*) * allocate_at_a_time);
    list->size = 0;
    list->capacity = allocate_at_a_time;
}

void append_list(List* list, Value* value){
    if (list->size == list->capacity){
        list->capacity += allocate_at_a_time;
        list->objects = (Value**)realloc(list->objects, sizeof(Value*) * list->capacity);
    }
    list->objects[list->size] = value;
    list->size++;
}


struct Bool{
    bool value;
};
typedef struct Bool Bool;


struct Null{
    char value[5];
};
typedef struct Null Null;

void* get_null(){
    Null* obj = (Null*)malloc(sizeof(Null));
    strcpy(obj->value, "null");
    return obj;
}


struct Int{
    int value;
};
typedef struct Int Int;

struct Float{
    float value;
};
typedef struct Float Float;


void* parse_value(FILE* fp, char* type);
Object* parse_object(FILE* fp);

void* parse_float(FILE* fp, int prev, bool is_negative){
    Float* obj = (Float*)malloc(sizeof(Float));
    obj->value = prev;
    float decimal = 0.1;
    char c = '\0';
    while(1){
        c = fgetc(fp);
        if (c >= '0' && c <= '9'){
            obj->value = obj->value + (c - '0') * decimal;
            decimal /= 10;
        }
        else if (is_ignoring_char(c)){
            continue;
        }
        else if(c == ',' || c == ']' || c == '}'){
            ungetc(c, fp);
            if (is_negative){
                obj->value = -obj->value;
            }
            return obj;
        }
    }
}


void* parse_int(FILE* fp, bool* is_float){
    Int* obj = (Int*)malloc(sizeof(Int));
    obj->value = 0;
    char c = '\0';
    c = fgetc(fp);
    bool is_negative = false;
    if (c == '-'){
        is_negative = true;
    }
    else{
        ungetc(c, fp);
    }
    while(1){
        c = fgetc(fp);
        if (c >= '0' && c <= '9'){
            obj->value = obj->value * 10 + (c - '0');
        }
        else if (c == '.'){
            *is_float = true;
            return parse_float(fp, obj->value, is_negative);
        }
        else{
            ungetc(c, fp);
            if (is_negative){
                obj->value = -obj->value;
            }
            return obj;
        }
    }

}

// It parses string
void* parse_string(FILE* fp){
    string* d_str = (string*)malloc(sizeof(string));
    init_dynamic_string(d_str);
    char c = '\0';
    c = fgetc(fp); //should be "
    while(1){
        c = fgetc(fp);
        if(c == '"'){
            append_dynamic_string(d_str, '\0');
            return d_str;
        }
        else if(c == EOF){
            printf("Invalid JSON\n");
            exit(1);
        }
        else{
            append_dynamic_string(d_str, c);
        }
    }
}

// It parses list
// Example: ["Hello", "World"] each entry can be list, object, string, numeric value, boolean, null
void* parse_list(FILE* fp){
    List* list = (List*)malloc(sizeof(List));
    init_list(list);
    char c = '\0';
    c = fgetc(fp); //should be [
    while(1){
        Value* entry = (Value*)malloc(sizeof(Value));
        entry->value = parse_value(fp, &entry->type);
        append_list(list, entry);
        c = fgetc(fp);
        if (c == ',' || is_ignoring_char(c)){
            continue;
        }
        else if (c == ']'){
            return list;
        }
        else{
            printf("Invalid JSON\n");
            exit(1);
        }
    }
    return list;

}

// It is just used to verify that:
//      The next characters in the file are same as the string
// Example: A function has already read t of true and it knows that 
// it is true or there is syntax error, it wants to verify rue
//          So, it will call verify_next(fp, "rue")
bool verify_next(FILE* fp, char* str){
    char c = '\0';
    for(int i = 0; i < strlen(str); i++){
        c = fgetc(fp);
        if(c != str[i]){
            return false;
        }
    }
    return true;
}

void* parse_value(FILE* fp, char* type){
    char c = '\0';
    while(1){
        c = fgetc(fp);
        if (is_ignoring_char(c)){
            continue;
        }
        else if(c == '"'){
            *type = 's';
            ungetc(c,fp);
            return parse_string(fp);
        }
        else if(c == '{'){
            *type = 'o';
            ungetc(c,fp);
            return parse_object(fp);
        }
        else if(c == '['){
            *type = 'l';
            ungetc(c,fp);
            return parse_list(fp);
        }
        else if(c == 't' || c == 'f'){
            *type = 'b';
            Bool* bool_obj = (Bool*)malloc(sizeof(Bool));
            if(verify_next(fp, "rue")){
                bool_obj->value = true;
            }
            else if(verify_next(fp, "alse")){
                bool_obj->value = false;
            }
            else{
                printf("Invalid boolean value.\n");
                exit(1);
            }
            return bool_obj;
        }
        else if(c == 'n'){
            void* obj = get_null();
            *type = 'n';
            if(!verify_next(fp, "ull")){
                printf("Invalid null value.\n");
                exit(1);
            }
            return obj;
        }
        else if(c == '-' || (c >= '0' && c <= '9')){
            *type = 'i';
            bool is_float = false;
            ungetc(c,fp);
            void* t = parse_int(fp, &is_float);
            if (is_float){
                *type = 'f';
            }
            return t;
        }
        else{
            printf("Invalid JSON\n");
            exit(1);
        }
    }
}

Entry* parse_entry(FILE* fp){
    Entry* obj = (Entry*)malloc(sizeof(Entry));
    obj->key = parse_string(fp);
    char c = '\0';
    while(1){
        c = fgetc(fp);
        if(is_ignoring_char(c)){
            continue;
        }
        else if(c == ':'){
            obj->value = (Value*)malloc(sizeof(Value));
            obj->value->value = parse_value(fp, &(obj->value->type));
            return obj;
        }
        else if (c == EOF){
            printf("Invalid JSON\n");
            exit(1);
        }
        else{
            printf("Invalid JSON\n");
            exit(1);
        }
    }
}

Object* parse_object(FILE* fp){
    Object* obj = (Object*)malloc(sizeof(Object));
    init_object(obj);
    char c = '\0';
    c = fgetc(fp); //should be '{'
    while(1){
        c = fgetc(fp);
        if (is_ignoring_char(c) || c == ','){
            continue;
        }
        else if (c == '"'){
            ungetc(c,fp);
            Entry* entry = parse_entry(fp);
            append_object(obj, entry);
        }
        else if(c == '}'){
            return obj;
        }
        else{
            printf("Invalid JSON\n");
            exit(1);
        }
    }
    return obj;
}

Value* parse_json(const char* filename){
    FILE* file;
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("File not found.\n");
        return NULL;
    }
    Value* obj = (Value*)malloc(sizeof(Value));
    while(1){
        char c = fgetc(file);
        if (c == EOF) {
            break;
        }
        if (is_ignoring_char(c)){
            continue;
        }
        else if (c == '{') {
            obj->type = 'o';
            ungetc(c,file);
            obj->value = parse_object(file);
        }
        else if (c == '[') {
            obj->type = 'l';
            ungetc(c,file);
            obj->value = parse_list(file);
        }
        else {
            printf("Invalid JSON\n");
            exit(1);
        }
    }
    return obj;
}


//get value from object
Value* _get_value(Value* obj,const char* key){
    Value* return_obj = (Value*)malloc(sizeof(Value));
    if (obj->type == 'o'){
        Object* object = (Object*)obj->value;
        for (int i = 0; i < object->size; i++){
            if (strcmp(object->entries[i]->key->value, key) == 0){
                return_obj->value = object->entries[i]->value->value;
                return_obj->type = object->entries[i]->value->type;
                return return_obj;
            }
        }
    }
    else if (obj->type == 'l'){
        List* list = (List*)obj->value;
        return_obj->value = ((Value*)list->objects[atoi(key)])->value;
        return_obj->type = ((Value*)list->objects[atoi(key)])->type;
        return return_obj;
    }
}

Value* get_value(Value* obj, int argc, char* argv[]){

    Value* return_obj = NULL;
    for (int i = 1; i < argc; i++){
        return_obj = _get_value(obj,argv[i]);
        obj = return_obj;
    }
    return obj;
}

//add a value
void add_value(Value* obj, void* item){
    if(obj->type == 'o'){
        Object* object = (Object*) obj->value;
        Entry* item = (Entry*) item;
        append_object(object,item);
    }
}

void print_JSON(Value* json){
    if(json->type == 's'){
        printf("%s\n",((string*)json->value)->value);
    }
    else if(json->type == 'o'){
        Object* obj = (Object*)json->value;
        for(int i = 0; i < obj->size; i++){
            printf("%s: ", obj->entries[i]->key->value);
            print_JSON(obj->entries[i]->value);
        }
    }
    else if(json->type == 'l'){
        List* list = (List*)json->value;
        for(int i = 0; i < list->size; i++){
            print_JSON(list->objects[i]);
        }
    }
    else if(json->type == 'b'){
        printf("%s\n", ((Bool*)json->value)->value ? "true" : "false");
    }
    else if(json->type == 'n'){
        printf("null\n");
    }
    else if(json->type == 'i'){
        printf("%d\n", ((Int*)json->value)->value);
    }
    else if(json->type == 'f'){
        printf("%f\n", ((Float*)json->value)->value);
    }
    else{
        printf("Invalid JSON\n");
    }
}


int main(int argc, char* argv[]){
    Value* obj = (Value*)malloc(sizeof(Value));
    obj = parse_json("D:\\CustomEnvVar\\temp.json");

    Value* obj2 = get_value(obj,argc, argv);
    print_JSON(obj2);

    
    
    return 0;
}