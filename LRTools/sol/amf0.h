#ifndef AMF0_H_
#define AMF0_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
    
    enum amf0_type {
        AMF0_NUMBER,
        AMF0_BOOLEAN,
        AMF0_STRING,
        AMF0_OBJECT,
        AMF0_MOVIECLIP, /* not supported */
        AMF0_NULL,
        AMF0_UNDEFINED,
        AMF0_REFERENCE, /* not supported */
        AMF0_ECMA_ARRAY,
        AMF0_OBJECT_END,
        AMF0_STRICT_ARRAY, /* not supported */
        AMF0_DATE, /* not supported */
        AMF0_LONG_STRING, /* not supported */
        AMF0_UNSUPPORTED, /* not supported */
        AMF0_RECORDSET, /* not supported */
        AMF0_XML, /* not supported */
        AMF0_TYPED_OBJECT /* not supported */
    };
    
    struct amf0_obj {
        struct amf0_obj *next;
        char *name;
        enum amf0_type type;
        uint32_t length; /* used by arrays */
        union amf0_data {
            double number;
            uint8_t boolean;
            char *string;
            struct amf0_obj *properties; /* used by arrays and objects */
        } data;
    };
    
    struct amf0_obj *find_property(const char *name, struct amf0_obj *obj);
    void reverse_bytes (void *buf, size_t num);
    uint16_t read_u16 (FILE *f);
    uint32_t read_u32 (FILE *f);
    double read_double (FILE *f);
    void read_utf8_string (char *dst, size_t len, FILE *f);
    int check_sol_headers (FILE *sol_f);
    
    int read_amf0_number (struct amf0_obj *target, FILE *f);
    int read_amf0_boolean (struct amf0_obj *target, FILE *f);
    int read_amf0_string (struct amf0_obj *target, FILE *f);
    int read_amf0_objects (struct amf0_obj *target, FILE *f, int allow_eof);
    int read_amf0_ecma_array (struct amf0_obj *target, FILE *f);
    
    struct amf0_obj *read_sol (const char *path);
    void write_sol (struct amf0_obj *sol, const char *path);
    
#ifdef __cplusplus
}
#endif

#endif
