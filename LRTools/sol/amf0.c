#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "amf0.h"

const char *sol_header_version = "\x00\xbf";
const char *sol_header_signature = "TCSO\x00\x04\x00\x00\x00\x00";

struct amf0_obj *find_property(const char *name, struct amf0_obj *obj)
{
    struct amf0_obj *curr = obj->data.properties;
    while (curr) {
        if (strcmp(name, curr->name) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void reverse_bytes (void *buf, size_t num)
{
    uint8_t *ubuf = (uint8_t *)buf;
    for (int i = 0; i < num / 2; ++i) {
        uint8_t *a = ubuf + i;
        uint8_t *b = ubuf + (num - 1 - i);
        uint8_t tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

uint16_t read_u16 (FILE *f)
{
    uint8_t raw[2];
    if (fread(raw, 1, 2, f) != 2)
        return USHRT_MAX; // less likely to be a valid value
    reverse_bytes(raw, 2); // endian conversion
    return *((uint16_t *)(raw)); // bytes to uint conversion
}

uint32_t read_u32 (FILE *f)
{
    uint8_t raw[4];
    if (fread(raw, 1, 4, f) != 4)
        return UINT_MAX; // less likely to be a valid value
    reverse_bytes(raw, 4); // endian conversion
    return *((uint32_t *)(raw)); // bytes to uint conversion
}

double read_double (FILE *f)
{
    uint8_t raw[8];
    if (fread(raw, 1, 8, f) != 8)
        return -99999999.0;
    reverse_bytes(raw, 8);
    return *((double *)(raw));
}

void read_utf8_string (char *dst, size_t len, FILE *f)
{
    fread(dst, 1, len, f);
    dst[len] = '\0';
}

int check_sol_headers (FILE *sol_f)
{
    char buf[16];
    memset(buf, 0, sizeof(buf));
    
    // read in version
    if (fread(buf, sizeof(char), 2, sol_f) != 2)
        return 0;
    if (strcmp(buf, sol_header_version) != 0)
        return 0;
    
    // read in length
    uint32_t len = read_u32(sol_f);
    //printf("length: %u\n", len);
    
    // confirm length
    long curr_pos = ftell(sol_f);
    fseek(sol_f, 0, SEEK_END);
    long len_remaining = ftell(sol_f) - 6;
    if (len_remaining != len) {
        //printf("Invalid lengths: %u != %li\n", len, len_remaining);
        return 0;
    }
    fseek(sol_f, curr_pos, SEEK_SET); // jump back to where we were
    
    // signature
    memset(buf, 0, sizeof(buf));
    if (fread(buf, sizeof(char), 10, sol_f) != 10)
        return 0;
    if (strcmp(buf, sol_header_signature) != 0)
        return 0;
    
    return 1;
}

int read_amf0_number (struct amf0_obj *target, FILE *f)
{
    target->data.number = read_double(f);
    //printf("number %.2f\n", target->data.number);
    return 1;
}

int read_amf0_boolean (struct amf0_obj *target, FILE *f)
{
    target->data.boolean = fgetc(f);
    //printf("boolean %u\n", target->data.boolean);
    return 1;
}

int read_amf0_string (struct amf0_obj *target, FILE *f)
{
    uint16_t name_len = read_u16(f);
    target->data.string = malloc(name_len + 1);
    read_utf8_string(target->data.string, name_len, f);
    //printf("string %s\n", target->data.string);
    return 1;
}

int read_amf0_objects(struct amf0_obj *parent, FILE *f, int allow_eof)
{
    parent->data.properties = NULL;
    struct amf0_obj *curr = NULL;
    while (1) {
        struct amf0_obj *obj = malloc(sizeof(*obj));
        uint16_t name_len = read_u16(f);
        
        if (feof(f))
            if (allow_eof)
                return 1;
            else
                return 0;
        
        
        obj->name = malloc(name_len + 1);
        read_utf8_string(obj->name, name_len, f);
        obj->type = fgetc(f);
        obj->next = NULL;
        
        switch (obj->type) {
            case AMF0_NUMBER:
                if (!read_amf0_number(obj, f))
                    return 0;
                break;
            case AMF0_BOOLEAN:
                if (!read_amf0_boolean(obj, f))
                    return 0;
                break;
            case AMF0_STRING:
                if (!read_amf0_string(obj, f))
                    return 0;
                break;
            case AMF0_OBJECT:
                //printf("object\n");
                if (!read_amf0_objects(obj, f, 0))
                    return 0;
                break;
            case AMF0_ECMA_ARRAY:
                if (!read_amf0_ecma_array(obj, f))
                    return 0;
                break;
            case AMF0_NULL:
            case AMF0_UNDEFINED:
                break; // don't need to do a thing, nulls and undefineds have no data
            case AMF0_OBJECT_END:
                //printf("object end\n");
                free(obj->name);
                free(obj);
                return 1; // we're done
            default:
                //printf("unexpected type %u\n", obj->type);
                free(obj->name);
                free(obj);
                return 0; // unexpected type
        }
        
        if (parent->data.properties == NULL)
            parent->data.properties = obj;
        else
            curr->next = obj;
        curr = obj;
    }
    
    return 1;
}

int read_amf0_ecma_array (struct amf0_obj *parent, FILE *f)
{
    parent->length = read_u32(f);
    return read_amf0_objects(parent, f, 0);
}

struct amf0_obj *read_sol (const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }
    
    if (!check_sol_headers(f)) {
        //printf("Invalid headers.\n");
        fclose(f);
        return NULL;
    }
    
    struct amf0_obj *root = malloc(sizeof(*root));
    read_amf0_string(root, f);
    root->name = root->data.string; // steal the string out
    root->next = NULL;
    root->data.properties = NULL;
    
    //printf("%s\n", root->name);
    
    // 4 bytes of padding
    for (int i = 0; i < 4; ++i)
        fgetc(f);
    
    read_amf0_objects(root, f, 1);
    
    fclose(f);
    return root;
}

void write_u16 (uint16_t val, FILE *f)
{
    uint8_t raw[2];
    ((uint16_t *)raw)[0] = val; // copy in
    reverse_bytes(raw, 2); // endian conversion
    fwrite(raw, 2, 1, f);
    
}

void write_u32 (uint32_t val, FILE *f)
{
    uint8_t raw[4];
    ((uint32_t *)raw)[0] = val; // copy in
    reverse_bytes(raw, 4); // endian conversion
    fwrite(raw, 4, 1, f);
}

void write_double (double val, FILE *f)
{
    uint8_t raw[8];
    ((double *)raw)[0] = val; // copy in
    reverse_bytes(raw, 8); // endian conversion
    fwrite(raw, 8, 1, f);
}

void write_amf0_string (const char *str, FILE *f)
{
    write_u16(strlen(str), f);
    fwrite(str, strlen(str), 1, f);
}

void write_amf0_objects (struct amf0_obj *sol, FILE *f)
{
    struct amf0_obj *curr = sol->data.properties;
    while (curr) {
        // write out name
        write_amf0_string(curr->name, f);
        
        // write out type byte
        unsigned char type = curr->type;
        fwrite(&type, 1, 1, f);
        
        // and finally the value
        switch (curr->type) {
            case AMF0_NUMBER:
                write_double(curr->data.number, f);
                break;
            case AMF0_BOOLEAN:
                fwrite(&(curr->data.boolean), 1, 1, f);
                break;
            case AMF0_STRING:
                write_amf0_string(curr->data.string, f);
                break;
            case AMF0_ECMA_ARRAY:
                // length
                write_u32(curr->length, f);
                // fall through to object
            case AMF0_OBJECT:
                write_amf0_objects(curr, f);
                // obj terminator
                unsigned char termi[3] = { 0, 0, 9 };
                fwrite(termi, 3, 1, f);
                break;
            case AMF0_NULL:
            case AMF0_UNDEFINED:
            default:
                break; // pray
        }
        
        curr = curr->next;
    }
}

void write_sol (struct amf0_obj *sol, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (!f) {
        printf("Couldn't open %s for writing\n", path);
        return;
    }
    
    // headers
    fwrite(sol_header_version, 2, 1, f);
    // leave space for the size
    char buf[4];
    memset(buf, 0, 4);
    fwrite(buf, 4, 1, f);
    fwrite(sol_header_signature, 10, 1, f);
    write_amf0_string(sol->name, f); // sol name
    // 4 byte padding
    fwrite(buf, 4, 1, f);
    
    write_amf0_objects(sol, f);
    
    // ending crap
    fwrite(buf, 1, 1, f);
    
    // go back and write in the .sol's size at the start
    long length = ftell(f) - 6;
    fseek(f, 2, SEEK_SET); // jump back to the length
    
    write_u32(length, f); // conversions be damned
    
    fclose(f);
}
