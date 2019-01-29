#ifndef JSONLITE_TYPES_H
#define JSONLITE_TYPES_H

typedef enum {
    jsonlite_result_unknown = -1,
    jsonlite_result_ok,
    jsonlite_result_end_of_stream,
    jsonlite_result_depth_limit,
    jsonlite_result_expected_object_or_array,
    jsonlite_result_expected_value,
    jsonlite_result_expected_key_or_end,
    jsonlite_result_expected_key,
    jsonlite_result_expected_colon,
    jsonlite_result_expected_comma_or_end,
    jsonlite_result_invalid_escape,
    jsonlite_result_invalid_number,
    jsonlite_result_invalid_token,
    jsonlite_result_invalid_utf8,
    jsonlite_result_suspended,

    jsonlite_result_not_allowed,
    jsonlite_result_out_of_memory,
} jsonlite_result;

#endif
