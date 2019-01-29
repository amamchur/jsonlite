#ifndef JSONLITE_CALLBACK_RECORDER_HPP
#define JSONLITE_CALLBACK_RECORDER_HPP

#include <jsonlite.h>
#include <string>
#include <vector>

typedef enum {
    begin_object,
    begin_array,
    end_object,
    end_array,
    token_null,
    token_true,
    token_false,
    token_number,
    token_string,
    token_key,
    parse_finished
} record_type;

class record {
public:
    record(record_type type, std::string token);

    explicit record(record_type type);

    void expect_eq(record_type t) const;
    void expect_eq(record_type t, const std::string &tk) const;
    void expect_eq(const record &r) const;

    record_type type;
    std::string token;
};

class callback_recorder {
public:
    callback_recorder();

    void add_record(record_type type);
    void add_record(record_type type, jsonlite_token *t);
    void expect_finished() const;

    jsonlite_parser_callbacks cbs;
    std::vector<record> records;
};

std::vector<char> read_file_content(const std::string &rel_path);

#endif
