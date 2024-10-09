/*
///
/// # jquick
///
/// ## About
/// This is a single and quick json labrary written in ANCI C and 
/// licensed under the MIT License <http://opensource.org/licenses/MIT>.
/// SPDX-License-Identifier: MIT
///

/// This library is header only and consists of only one header file. So
/// you can just copy it near to your code and use.

MIT License

Copyright (c) 2024 valera-vorona

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

/// ## Features
/// * Single header library
/// * No dependences. By default it dosen't use any includes or library linkages
/// * No dynamic memory is allocated
///
/// ## Usage
/// You can just copy this file nearly to your code and include.
/// Macro JQ_WITH_IMPLEMENTATION should be used before including this file in only one
/// of the files that use this library. The others just include this file without
/// this macro.
/// This is how main.c can include jquick
/// ~~~
/// #define JQ_WITH_IMPLEMENTATION
/// #include "jquick.h"
/// ~~~
/// And the other files should include it without JQ_WITH_IMPLEMENTATION
/// ~~~
/// #include "jquick.h"
/// ~~~
*/

#ifndef __JQUICK_H__
#define __JQUICK_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==========================================================================
 *
 * INTERFACE
 *
 * ========================================================================== */

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199409L))
  #define JQ_API static
  #define JQ_INLINE static inline
#else
  #define JQ_API static
  #define JQ_INLINE static
#endif

#ifndef JQ_STACK_SIZE
  #define JQ_STACK_SIZE 4096
#endif /* JQ_STACK_SIZE */

struct jq_handler;

typedef char jq_char;
typedef unsigned long int jq_size;
typedef int jq_bool;

/*/// ## API
///
/// ### Enums, structs, typedefs
///
/// #### enum jq_bool
/// ~~~
/// enum jq_bool {
///    JQ_FALSE                        = 0,
///    JQ_TRUE                         = 1
/// };
/// ~~~
*/
enum jq_bool {
    JQ_FALSE                        = 0,
    JQ_TRUE                         = 1
};

/*//
/// #### enum jq_error
/// ~~~
/// enum jq_error {
///     JQ_ERR_OK                           = 0,
///     JQ_ERR_LEXER_UNKNOWN_TOKEN,
///     JQ_ERR_LEXER_UNKNOWN_ESCAPE_SYMBOL,
///     JQ_ERR_LEXER_UNKNOWN_HEX_SYMBOL,
///     JQ_ERR_LEXER_EXPONENT_ERROR,
///     JQ_ERR_PARSER_UNEXPECTED_TOKEN,
///     JQ_ERR_PARSER_NEED_MORE
/// };
/// ~~~
*/
enum jq_error {
    JQ_ERR_OK                           = 0,
    JQ_ERR_LEXER_UNKNOWN_TOKEN,
    JQ_ERR_LEXER_UNKNOWN_ESCAPE_SYMBOL,
    JQ_ERR_LEXER_UNKNOWN_HEX_SYMBOL,
    JQ_ERR_LEXER_EXPONENT_ERROR,
    JQ_ERR_PARSER_UNEXPECTED_TOKEN,
    JQ_ERR_PARSER_NEED_MORE
};

enum jq_token_type {
    JQ_T_UNDEFINED                      = -1,   /* Used at the beginning of scan */
    JQ_T_ERROR                          = 0,
    JQ_T_NEED_MORE                      = 256,
    JQ_T_NULL                           = 257,
    JQ_T_TRUE                           = 258,
    JQ_T_FALSE                          = 259,
    JQ_T_STRING                         = 260,
    JQ_T_NUMBER                         = 261,
    JQ_T_LEFT_BRACE                     = '{',
    JQ_T_RIGHT_BRACE                    = '}',
    JQ_T_LEFT_BRACKET                   = '[',
    JQ_T_RIGHT_BRACKET                  = ']',
    JQ_T_COLON                          = ':',
    JQ_T_COMMA                          = ','
};

/*///
/// #### enum jq_event_type
/// ~~~
/// enum jq_event_type {
///    JQ_E_NULL,
///    JQ_E_TRUE,
///    JQ_E_FALSE,
///    JQ_E_STRING,
///    JQ_E_NUMBER,
///    JQ_E_OBJECT_BEGIN,
///    JQ_E_OBJECT_END,
///    JQ_E_ARRAY_BEGIN,
///    JQ_E_ARRAY_END,
///    JQ_E_OBJECT_KEY
/// };
/// ~~~
*/
/* This enum is used in callbacks */
/* Except JQ_E_OBJECT_KEY it simply repeats some of jq_token_type constants */
/* IMPORTANT: except JQ_E_OBJECT_KEY the values must match those of jq_token_type! */
enum jq_event_type {
    JQ_E_NULL                           = JQ_T_NULL,
    JQ_E_TRUE                           = JQ_T_TRUE,
    JQ_E_FALSE                          = JQ_T_FALSE,
    JQ_E_STRING                         = JQ_T_STRING,
    JQ_E_NUMBER                         = JQ_T_NUMBER,
    JQ_E_OBJECT_BEGIN                   = '{',
    JQ_E_OBJECT_END                     = '}',
    JQ_E_ARRAY_BEGIN                    = '[',
    JQ_E_ARRAY_END                      = ']',
    JQ_E_OBJECT_KEY                     = 255 /* it must be unique */
};

enum jq_lexer_state {
    JQ_L_NORMAL                         = 0,
    JQ_L_STRING,
    JQ_L_ESCAPE,
    JQ_L_UNICODE,
    JQ_L_NULL,
    JQ_L_TRUE,
    JQ_L_FALSE,
    JQ_L_NUM_BEGIN,
    JQ_L_NUM_POINT,
    JQ_L_NUM_INT0_9,
    JQ_L_NUM_INT1_9,
    JQ_L_NUM_FRACTION,
    JQ_L_NUM_EXPO_PLUS_MINUS,
    JQ_L_NUM_EXPO_INT1,
    JQ_L_NUM_EXPO_INT
};

enum jq_parser_state {
    JQ_S_UNDEFINED                      = 0,
    JQ_S_OBJECT,
    JQ_S_ARRAY,
    JQ_S_COMPLETE
};

/*///
/// #### jq_callback
/// Callback function pointer typedef. 
/// ~~~
/// typedef void (*jq_callback)(struct jq_handler *h, enum jq_event_type);
/// ~~~
*/
typedef void (*jq_callback)(struct jq_handler *h, enum jq_event_type);

/*///
/// #### struct hq_handler
/// The main `jquick` handler.
/// ~~~
/// struct jq_handler;
/// ~~~
*/ 
struct jq_handler {
    jq_char *buf;                       /* input char buffer */
    jq_size buf_size;                   /* buf size */
    jq_size i;                          /* position in buf */
    jq_char cnt;                        /* Counts the element number in array */
                                        /* or object, should always be 0 >= cnt <= 4 */ 
    jq_char stack[JQ_STACK_SIZE];       /* jq_parser_state stack */
    jq_size stack_pos;                  /* position in stack */
    jq_char *val;                       /* value of the latest token */
#ifdef JQ_WITH_VLEN
    jq_size vlen;                       /* value length */
#endif
#ifdef JQ_WITH_LOCATION
    jq_size position;                   /* current position in buf from line beginning */
    jq_size line;                       /* current line in buf */
#endif
#ifdef JQ_WITH_NULLTERM
    jq_char subst_char;                 /* char temporary substituted */
                                        /* with '\0' */
    jq_size subst_pos;                  /* char position in buf */
#endif
    jq_callback callback;               /* callback function */
    enum jq_error error;                /* error code */
};

/*
/// ### Functions
///
/// #### jq_init
/// Initializes `jq_handler` struct which then is used in almost every `jquick` function.
/// If you want to reuse previously used `jq_handler` struct, you can just call this
/// function again and go on. No other cleanup is required.
/// ~~~
/// jq_bool jq_init(struct jq_handler *h);
/// ~~~
///
/// Parameter | Description
/// ----------|----------------------------------------------------------------
/// __h__     | Pointer to `jq_handler` struct
///
/// Returns `JQ_TRUE(1)` if ok, `JQ_FALSE(0)` if error occured.
///
*/
JQ_API jq_bool jq_init(struct jq_handler *h);

/*
/// #### jq_append_buf
///  Appends an input buffer which then can be parsed with
/// `jq_parse` or `jq_parse_buf()` function.
/// ~~~
/// void jq_append_buf(struct jq_handler *h, jq_char *src, jq_size sz);
/// ~~~
///
/// Parameter | Description
/// ----------|----------------------------------------------------------------
/// __h__     | Pointer to previously initialized `jq_handler` struct
/// __src__   | Pointer to source buffer
/// __sz__    | Size in bytes of source buffer
///
*/
JQ_INLINE void jq_append_buf(struct jq_handler *h, jq_char *src, jq_size sz);

/*
/// #### jq_set_callback
/// Sets the callback function pointer to handle `jquick` events, i.e. start of object,
/// end of array, null, false, true etc.
/// See enum `jq_event_type` for all the events this callback is called for.
/// ~~~
/// void jq_set_callback(struct jq_handler *h, jq_callback callback);
/// ~~~
///
/// Parameter       | Description
/// ----------------|----------------------------------------------------------------
/// __h__           | Pointer to previously initialized `jq_handler` struct
/// __callback__    | Pointer to callback function. See `jq_callback` typedef for prototype 
///
*/
JQ_INLINE void jq_set_callback(struct jq_handler *h, jq_callback callback);

/*
/// #### jq_parse
/// Parses previously appended with `jq_append_buf` json input buffer.
/// ~~~
/// jq_bool jq_parse(struct jq_handler *h);
/// ~~~
///
/// Parameter | Description
/// ----------|----------------------------------------------------------------
/// __h__     | Pointer to previously initialized `jq_handler` struct
///
/// Returns `JQ_TRUE(1)` if ok, `JQ_FALSE(0)` if error occured.
/// The error code can be retrieved with `jq_get_error()` function.
///
*/
JQ_API jq_bool jq_parse(struct jq_handler *h);

/*
/// #### jq_parse_buf
/// Parses json input buffer. This function is just a wrapper which calls
/// sequentially `jq_append_buf` and `jq_parse` returning what the below has returned.
/// ~~~
/// jq_bool jq_parse_buf(struct jq_handler *h, jq_char *src, jq_size sz);
/// ~~~
///
/// Parameter | Description
/// ----------|----------------------------------------------------------------
/// __h__     | Pointer to previously initialized `jq_handler` struct
/// __src__   | Pointer to source buffer
/// __sz__    | Size in bytes of source buffer
///
/// Returns `JQ_TRUE(1)` if ok, `JQ_FALSE(0)` if error occured.
/// The error code can be retrieved with `jq_get_error()` function.
///
*/
JQ_INLINE jq_bool jq_parse_buf(struct jq_handler *h, jq_char *src, jq_size sz);

/*
/// #### jq_get_error
/// Returns error code of the latest parsing operation.
/// It is implemented as a macro.
/// ~~~
/// enum jq_error jq_get_error(struct jq_handler *h) (h->error)
/// ~~~
///
/// Parameter | Description
/// ----------|----------------------------------------------------------------
/// __h__     | Pointer to previously initialized `jq_handler` struct
///
/// Returns error code defined in `enum jq_error`. 
///
*/
#define jq_get_error(h) ((h)->error)

/*
/// #### jq_errstr
/// Shows a readable error description.
/// ~~~
/// const char *jq_errstr(enum jq_error error);
/// ~~~
///
/// Parameter | Description
/// ----------|----------------------------------------------------------------
/// __error__ | Error code. It can be retrieved with `jq_get_error()` function
///
/// Returns a readable error description.
///
*/
JQ_API const char *jq_errstr(enum jq_error error);

/* ==========================================================================
 *
 * IMPLEMENTATION
 *
 * ========================================================================== */

/* ==========================================================================
 *
 * General
 *
 * ========================================================================== */

#ifdef JQ_WITH_IMPLEMENTATION

#ifdef JQ_WITH_STD_STRING
  #include <string.h>

  #define JQ_NULL    NULL
  #define JQ_MEMCMP(d, s, n)  memcmp(d, s, n)
  #define JQ_STRCHR(s, c)     strchr(s, c)
#elif defined(JQ_WITH_CUSTOM_STRING)
  /* you should define the following macros yourself: */
  #define JQ_NULL
  #define JQ_MEMCMP(d, s, n)
  #define JQ_STRCHR(s, c)
#else
  #define JQ_WITH_BUILTIN_STRING
  /* use builtin string functions */
  #define JQ_NULL 0

JQ_INLINE int
JQ_MEMCMP(const void *p1, const void *p2, size_t n) {
    while (n--) {
        if (*(jq_char *)(p1) != *(jq_char *)p2) return (jq_char *)p1 - (jq_char *)p2;
    }

    return 0;
}

JQ_INLINE jq_char *
JQ_STRCHR(jq_char *s, jq_char c) {
    while (*s) {
        if (*s == c) return s;
        ++s;
    }

    return JQ_NULL;
}

#endif /* JQ_WITH_STD_STRING */

JQ_API jq_bool
jq_init(struct jq_handler *h) {
    h->buf = JQ_NULL;
    h->buf_size = 0;
    h->i = 0;
    h->cnt = 0;
    h->stack[0] = JQ_S_UNDEFINED;
    h->stack_pos = 0;
    h->val = JQ_NULL;
#ifdef JQ_WITH_VLEN
    h->vlen = 0;
#endif
#ifdef JQ_WITH_LOCATION
    h->position = 0;
    h->line = 0;
#endif
#ifdef JQ_WITH_NULLTERM
    h->subst_char = '\0';
    h->subst_pos = 0; /* subst_pos init doesn't matter, only subst_char is checked */
#endif
    h->callback = JQ_NULL;
    h->error = JQ_ERR_OK;

    return JQ_TRUE;
}

JQ_INLINE void
jq_append_buf(struct jq_handler *h, jq_char *src, jq_size sz) {
    h->buf = src;
    h->buf_size = sz;
}

JQ_INLINE void
jq_set_callback(struct jq_handler *h, jq_callback callback) {
    h->callback = callback;
}

/* ==========================================================================
 *
 * Lexer
 *
 * ========================================================================== */

/* Forward declarations */
JQ_INLINE enum jq_token_type jq_finish_number(struct jq_handler *h);

#define jq_iswc(c) (JQ_STRCHR(" \n\r\t", c) != JQ_NULL)
#define jq_isesc(c) (JQ_STRCHR("\"\\/bfnrt", c) != JQ_NULL)
#define jq_isnum(c) (JQ_STRCHR("-0123456789", c) != JQ_NULL)
#define jq_isint(c) (JQ_STRCHR("123456789", c) != JQ_NULL)

/* This function cannot be implemented as a macro because c can be a return of a function */
JQ_INLINE jq_bool
jq_ishex(jq_char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

JQ_API int
jq_lexer_getchar(struct jq_handler *h) {
    if (h->i < h->buf_size) {
#ifdef JQ_WITH_LOCATION
        if (h->buf[h->i] == '\n') {
            h->position = 0;
            ++h->line;
        } else {
            ++h->position;
        }
#endif
#ifdef JQ_WITH_NULLTERM
        /* Restoring previously saved char */
        if (h->subst_char) {
            h->buf[h->subst_pos] = h->subst_char;
            h->subst_char = '\0';
        }
#endif

        return h->buf[h->i++];
    } else {
        return JQ_T_NEED_MORE;
    }
}

JQ_API void
jq_lexer_unget(struct jq_handler *h) {
/*
 * h->i can be 0 at the beginning of the buf! So this function should be called after a successive call of jq_getchar(jq_handler *h) only,
 * i.e. jq_getchar(jq_handler *h) should not have returned JQ_T_NEED_MORE!
*/
    --h->i;
#ifdef JQ_WITH_LOCATION
    if (h->buf[h->i] == '\n') {
        jq_char *b = &h->buf[h->i];
        do ++h->position; while (--*b != '\n' && b != h->buf); /* TODO: test it! */
        --h->line;
    } else {
        --h->position;
    }
#endif
}

JQ_API enum jq_token_type
jq_get_token(struct jq_handler *h) {
    int nft_cnt; /* current symbol inside null, true, false or unicode (\uxxxx) */
    static const char Null[] = "null";
    static const char True[] = "true";
    static const char False[] = "false";

    enum jq_lexer_state lexer_state = JQ_L_NORMAL;

    for (;;) {
        int c = jq_lexer_getchar(h);
        if (c == JQ_T_NEED_MORE) return c;

        switch (lexer_state) {
        case JQ_L_STRING:
            switch (c) {
            case '"':
#ifdef JQ_WITH_VLEN
                h->vlen = h->i - 1 - (h->val - h->buf);
#endif
#ifdef JQ_WITH_NULLTERM
                /* Remembering the char in h->subst_char and h->subst_pos */
                h->subst_pos = h->i - 1;
                h->subst_char = c;
                h->buf[h->subst_pos] = '\0'; /* and setting it to null terminator, then we should restore it */
#endif

                return JQ_T_STRING;

            case '\\':
                lexer_state = JQ_L_ESCAPE;
                continue;
            }
            break;

        case JQ_L_ESCAPE:
            if (c == 'u') {
                nft_cnt = 0;
                lexer_state = JQ_L_UNICODE;
            } else if (jq_isesc(c)) {
                lexer_state = JQ_L_STRING;
            } else {
                h->error = JQ_ERR_LEXER_UNKNOWN_ESCAPE_SYMBOL;
                return JQ_T_ERROR;
            }
            break;

        case JQ_L_UNICODE:
            if (nft_cnt++ < 4) {
                if (!jq_ishex(c)) {
                    h->error = JQ_ERR_LEXER_UNKNOWN_HEX_SYMBOL;
                    return JQ_T_ERROR;
                }
            } else {
                jq_lexer_unget(h);
                lexer_state = JQ_L_STRING;
            }
            break;

        case JQ_L_NULL:
            if (Null[nft_cnt]) {
                if (c == Null[nft_cnt]) {
                    ++nft_cnt;
                    continue;
                } else {
                    h->error = JQ_ERR_LEXER_UNKNOWN_TOKEN;
                    return JQ_T_ERROR;
                }
            } else {
                jq_lexer_unget(h);
                return JQ_T_NULL;
            }
            break;

        case JQ_L_TRUE:
            if (True[nft_cnt]) {
                if (c == True[nft_cnt]) {
                    ++nft_cnt;
                    continue;
                } else {
                    h->error = JQ_ERR_LEXER_UNKNOWN_TOKEN;
                    return JQ_T_ERROR;
                }
            } else {
                jq_lexer_unget(h);
                return JQ_T_TRUE;
            }
            break;

        case JQ_L_FALSE:
            if (False[nft_cnt]) {
                if (c == False[nft_cnt]) {
                    ++nft_cnt;
                    continue;
                } else {
                    h->error = JQ_ERR_LEXER_UNKNOWN_TOKEN;
                    return JQ_T_ERROR;
                }
            } else {
                jq_lexer_unget(h);
                return JQ_T_FALSE;
            }
            break;

        case JQ_L_NORMAL:
            if (jq_iswc(c)) continue;
            if (jq_isnum(c)) {
                jq_lexer_unget(h);
                lexer_state = JQ_L_NUM_BEGIN;
                continue;
            }

            switch (c) {
            case '"':
                h->val = &h->buf[h->i];
                lexer_state = JQ_L_STRING;
                continue;

            case '{': case '}': case '[': case ']': case ':': case ',':
                return c;

            case 'n':
                nft_cnt = 1;
                lexer_state = JQ_L_NULL;
                continue;

            case 't':
                nft_cnt = 1;
                lexer_state = JQ_L_TRUE;
                continue;

            case 'f':
                nft_cnt = 1;
                lexer_state = JQ_L_FALSE;
                continue;

            default:
                h->error = JQ_ERR_LEXER_UNKNOWN_TOKEN;
                return JQ_T_ERROR;
            }
            break;

        case JQ_L_NUM_BEGIN:
                h->val = &h->buf[h->i - 1];

                switch (c) {
                case '-':   lexer_state = JQ_L_NUM_INT1_9; break;
                case '0':   lexer_state = JQ_L_NUM_POINT; break;
                default:    jq_lexer_unget(h); lexer_state = JQ_L_NUM_INT1_9; break;
                }
            continue;

        case JQ_L_NUM_POINT:
            if (c == '.') {
                lexer_state = JQ_L_NUM_FRACTION;
            } else {
                return jq_finish_number(h);
            }
            continue;

        case JQ_L_NUM_INT0_9:
            if (JQ_STRCHR("0123456789", c) == JQ_NULL) {
                if (c == '.') {
                    lexer_state = JQ_L_NUM_FRACTION;
                } else {
                    return jq_finish_number(h);
                }
            }
            continue;

        case JQ_L_NUM_INT1_9:
            if (jq_isint(c)) {
                lexer_state = JQ_L_NUM_INT0_9;
            } else {
                if (c == '.') {
                    lexer_state = JQ_L_NUM_FRACTION;
                } else {
                    return jq_finish_number(h);
                }
            }
            continue;

        case JQ_L_NUM_FRACTION:
            if (JQ_STRCHR("0123456789", c) == JQ_NULL) {
                if (JQ_STRCHR("Ee", c) != JQ_NULL) {
                    lexer_state = JQ_L_NUM_EXPO_PLUS_MINUS;
                } else {
                    return jq_finish_number(h);
                }
            }
            continue;

        case JQ_L_NUM_EXPO_PLUS_MINUS:
            if (JQ_STRCHR("+-", c) != JQ_NULL) {
                lexer_state = JQ_L_NUM_EXPO_INT1;
            } else if (JQ_STRCHR("0123456789", c) != JQ_NULL) {
                lexer_state = JQ_L_NUM_EXPO_INT;
            } else {
                h->error = JQ_ERR_LEXER_EXPONENT_ERROR;
                return JQ_T_ERROR; /* Nothing found after exponent E(e) */
            }
            continue;

        case JQ_L_NUM_EXPO_INT1:
            if (JQ_STRCHR("0123456789", c) == JQ_NULL) {
                h->error = JQ_ERR_LEXER_EXPONENT_ERROR;
                return JQ_T_ERROR; /* Nothing found after exponent E(e)+- */
            } else {
                lexer_state = JQ_L_NUM_EXPO_INT;
            }
            continue;

        case JQ_L_NUM_EXPO_INT:
            if (JQ_STRCHR("0123456789", c) == JQ_NULL) {
                return jq_finish_number(h);
            }
            continue;
        }
    }
}

JQ_INLINE enum jq_token_type
jq_finish_number(struct jq_handler *h) {
    jq_lexer_unget(h);
#ifdef JQ_WITH_VLEN
    h->vlen = h->i - (h->val - h->buf);
#endif
#ifdef JQ_WITH_NULLTERM
    /* Remembering the char in h->subst_char and h->subst_pos */
    h->subst_pos = h->i;
    h->subst_char = h->buf[h->subst_pos];
    h->buf[h->subst_pos] = '\0'; /* and setting it to null terminator, then we should restore it */
#endif

    return JQ_T_NUMBER;
}

/* ==========================================================================
 *
 * Parser
 *
 * ========================================================================== */

/* Forward declarations */
JQ_INLINE enum jq_parser_state jq_parser_get_state(struct jq_handler *h);
JQ_INLINE void jq_parser_push_state(struct jq_handler *h, enum jq_parser_state state);
JQ_INLINE enum jq_parser_state jq_parser_pop_state(struct jq_handler *h);
#define jq_parser_inc_cnt(h) h->cnt = ++h->cnt & 3

JQ_API jq_bool
jq_parse(struct jq_handler *h) {
    enum jq_parser_state state = jq_parser_get_state(h);

    h->error = JQ_ERR_OK;

    for (;;) {
        enum jq_token_type token = jq_get_token(h);

        if (token == JQ_T_NEED_MORE) {
            if (state == JQ_S_COMPLETE) {
                return JQ_TRUE;
            } else {
                h->error = JQ_ERR_PARSER_NEED_MORE;
                return JQ_FALSE;
            }
        }

        if (state == JQ_S_COMPLETE) {
            h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN;
            return JQ_FALSE; /* Unexpected token after a valid json object */
        }

        switch (token) {
        case JQ_T_ERROR:
            return JQ_FALSE; /* Lexer already set h->error */

        case JQ_T_NULL: case JQ_T_TRUE: case JQ_T_FALSE: case JQ_T_NUMBER: case JQ_T_STRING:
            switch (state) {
            case JQ_S_OBJECT:
                switch (h->cnt) {
                case 0: if (token == JQ_T_STRING) {
                    if (h->callback) h->callback(h, JQ_E_OBJECT_KEY);
                } else {
                    h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN; /* Expected object key */
                    return JQ_FALSE;
                }
                break;

                case 1:
                    h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN; /* Expected ':' */
                    return JQ_FALSE;

                case 2:
                    if (h->callback) h->callback(h, token);
                    break;

                case 3:
                    h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN; /* Expected ',' or '}' */
                    return JQ_FALSE;
                }
                break;

            case JQ_S_ARRAY:
                if (h->cnt & 1) {
                    h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN; /* Expected ',' or ']' */
                    return JQ_FALSE;
                } else {
                    if (h->callback) h->callback(h, token);
                }
                break;

            case JQ_S_UNDEFINED:
                if (h->callback) h->callback(h, token);
                state = JQ_S_COMPLETE;
                break;    
            }
            break;

        case ':':
            if (state != JQ_S_OBJECT || h->cnt != 1) {
                h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN; /* Unexpected ':' */
                JQ_FALSE;
            }
            break;
                
        case ',':
            switch (state) {
            case JQ_S_OBJECT:
                if (h->cnt != 3) {
                    h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN; /* Unexpected ',' */
                    JQ_FALSE;
                }
                break;

            case JQ_S_ARRAY:
                if (!(h->cnt & 1)) {
                    h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN; /* Unexpected ',' array element delimeter */
                    return JQ_FALSE;
                }
                break;

            default:
                h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN; /* Unexpected ',' array element delimeter */
                return JQ_FALSE;

            }
            break;

        case '{':
            jq_parser_push_state(h, JQ_S_OBJECT);
            state = JQ_S_OBJECT;
            h->cnt = 3; /* jq_parser_inc_cnt() which is called at the bottom of this loop will make it 0 */
            if (h->callback) h->callback(h, token);
            break;

        case '[':
            jq_parser_push_state(h, JQ_S_ARRAY);
            state = JQ_S_ARRAY;
            h->cnt = 3; /* jq_parser_inc_cnt() which is called at the bottom of this loop will make it 0 */
            if (h->callback) h->callback(h, token);
            break;

        case '}': case ']':
            if (state == JQ_S_UNDEFINED) {
                h->error = JQ_ERR_PARSER_UNEXPECTED_TOKEN; /* Unexpected token and the beginning */ 

                return JQ_FALSE;
            }

            state = jq_parser_pop_state(h);
            if (h->callback) h->callback(h, token);

            h->cnt = 2; /* jq_parser_inc_cnt() which is called at the bottom of this loop will make it 3 */

            if (state == JQ_S_UNDEFINED) state = JQ_S_COMPLETE;
            break;
        }

        jq_parser_inc_cnt(h);
    }

    return JQ_TRUE;
}

JQ_INLINE jq_bool
jq_parse_buf(struct jq_handler *h, jq_char *src, jq_size sz) {
    jq_append_buf(h, src, sz);
    return jq_parse(h);
}

JQ_API const char *
jq_errstr(enum jq_error error) {
    switch (error) {
    case JQ_ERR_OK: return "Ok";
    case JQ_ERR_LEXER_UNKNOWN_TOKEN: return "Syntax error";
    case JQ_ERR_LEXER_UNKNOWN_ESCAPE_SYMBOL: return "Syntax error, unknown escape symbol";
    case JQ_ERR_LEXER_UNKNOWN_HEX_SYMBOL: return "Syntax error, unknown hex symbol after '\\u' escape symbol";
    case JQ_ERR_LEXER_EXPONENT_ERROR: return "Syntax error in exponent part";
    case JQ_ERR_PARSER_NEED_MORE: return "Unexpected end of file";
    case JQ_ERR_PARSER_UNEXPECTED_TOKEN: return "Unexpected token";
    }
}

JQ_INLINE enum jq_parser_state
jq_parser_get_state(struct jq_handler *h) {
    return h->stack[h->stack_pos];
}

JQ_INLINE void
jq_parser_push_state(struct jq_handler *h, enum jq_parser_state state) {
    h->stack[++h->stack_pos] = state;
}

JQ_INLINE enum jq_parser_state
jq_parser_pop_state(struct jq_handler *h) {
    return h->stack[--h->stack_pos];
}

#endif /* JQ_WITH_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* __JQUICK_H__ */

