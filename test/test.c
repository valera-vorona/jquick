#include "quin.h"
#define JQ_WITH_IMPLEMENTATION
#include "../jquick.h"
#include <malloc.h>

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
    rv = malloc(sz);
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

TEST_CASE(test_correct)
    struct jq_handler h;
    jq_bool r;
    size_t sz;
    char *json = read_json("../assets/glossary.json", &sz);
    if (!json) return 0;
    jq_init(&h);
    r = jq_parse_buf(&h, json, sz);
    free(json);
    TEST_REQUIRE(r == JQ_TRUE);
TEST_CASE_END()
 
TEST_CASE(test_esc)
    struct jq_handler h;
    jq_bool r;
    size_t sz;
    char *json = read_json("../assets/glossary-esc.json", &sz);
    if (!json) return 0;
    jq_init(&h);
    r = jq_parse_buf(&h, json, sz);
    free(json);
    TEST_REQUIRE(r == JQ_TRUE);
TEST_CASE_END()

TEST_CASE(test_esc_unicode)
    struct jq_handler h;
    jq_bool r;
    size_t sz;
    char *json = read_json("../assets/glossary-esc-unicode.json", &sz);
    if (!json) return 0;
    jq_init(&h);
    r = jq_parse_buf(&h, json, sz);
    free(json);
    TEST_REQUIRE(r == JQ_TRUE);
TEST_CASE_END()

TEST_CASE(test_lexical_error)
    struct jq_handler h;
    jq_bool r;
    size_t sz;
    char *json = read_json("../assets/glossary-lexical-error.json", &sz);
    if (!json) return 0;
    jq_init(&h);
    r = jq_parse_buf(&h, json, sz);
    free(json);
    TEST_REQUIRE(r == JQ_FALSE);
    TEST_REQUIRE(jq_get_error(&h) == JQ_ERR_LEXER_UNKNOWN_TOKEN);
TEST_CASE_END()

TEST_CASE(test_grammar_error)
    struct jq_handler h;
    jq_bool r;
    size_t sz;
    char *json = read_json("../assets/glossary-grammar-error.json", &sz);
    if (!json) return 0;
    jq_init(&h);
    r = jq_parse_buf(&h, json, sz);
    free(json);
    TEST_REQUIRE(r == JQ_FALSE);
    TEST_REQUIRE(jq_get_error(&h) == JQ_ERR_PARSER_UNEXPECTED_TOKEN);
TEST_CASE_END()

TEST_CASE(test_webapp)
    struct jq_handler h;
    jq_bool r;
    size_t sz;
    char *json = read_json("../assets/web-app.json", &sz);
    if (!json) return 0;
    jq_init(&h);
    r = jq_parse_buf(&h, json, sz);
    free(json);
    TEST_REQUIRE(r == JQ_TRUE);
TEST_CASE_END()
 
/* test main function */

TEST(jquick)
    TEST_SUITE_RUN(suite_jquick);
TEST_END()

/* test suites */

TEST_SUITE(suite_jquick)
    TEST_CASE_RUN(test_correct);
    TEST_CASE_RUN(test_esc);
    TEST_CASE_RUN(test_esc_unicode);
    TEST_CASE_RUN(test_lexical_error);
    TEST_CASE_RUN(test_grammar_error);
    TEST_CASE_RUN(test_webapp);
TEST_SUITE_END()

int main() {
    return TEST_RUN(jquick);
}

