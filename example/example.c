#define JQ_WITH_IMPLEMENTATION
#define JQ_WITH_NULLTERM
#include "jquick.h"
#include <malloc.h>
#include <string.h>

char *read_json(const char *fname, size_t *rsz) {
    size_t sz;
    char *rv = NULL;
    FILE *fp = fopen(fname, "r");

    if (fp == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    rv = (char *)malloc(sz);
    if (rv == NULL) {
        perror("Error allocating memory");
    } else {
        *rsz = fread(rv, sizeof(char), sz, fp);
        if (sz != *rsz) {
            free(rv);
            perror("Error reading file");
        }
    }

    fclose(fp);

    return rv;
}

void margin(int n) {
    while (n--) printf("  ");
}

void cb(struct jq_handler *h, enum jq_event_type e) {
    static int depth = 0;
    static char *key = NULL;
    static jq_size klen;

    switch (e) {
    case JQ_E_OBJECT_BEGIN:
        margin(depth++);
        if (key) {
            key[klen] = '\0';
            printf("<object name=\"%s\">\n", key);
        } else {
            puts("<object>");
        }
        break;

    case JQ_E_OBJECT_KEY:   key = h->val; klen = strlen(key); break;
    case JQ_E_OBJECT_END:   margin(--depth);    puts("</object>"); break;
    case JQ_E_ARRAY_BEGIN:  margin(depth++);    puts("<array>"); break;
    case JQ_E_ARRAY_END:    margin(--depth);    puts("</array>"); break;
    case JQ_E_NULL:         margin(depth);      key[klen] = '\0'; printf("<string name=\"%s\" value=\"null\" />\n", key); break;
    case JQ_E_TRUE:         margin(depth);      key[klen] = '\0'; printf("<string name=\"%s\" value=\"true\" />\n", key); break;
    case JQ_E_FALSE:        margin(depth);      key[klen] = '\0'; printf("<string name=\"%s\" value=\"false\" />\n", key); break;
    case JQ_E_STRING:       margin(depth);      key[klen] = '\0'; printf("<string name=\"%s\" value=\"%s\" />\n", key, h->val); break;
    case JQ_E_NUMBER:       margin(depth);      key[klen] = '\0'; printf("<number name=\"%s\" value=\"%s\" />\n", key, h->val); break;
    }
}

int main() {
    struct jq_handler h;
    jq_bool r;
    size_t sz;
    char *json = read_json("../../test/assets/glossary.json", &sz);
    if (!json) return 0;

    jq_init(&h);
    jq_set_callback(&h, cb);
    r = jq_parse_buf(&h, json, sz);
    free(json);

    return 0;
}

