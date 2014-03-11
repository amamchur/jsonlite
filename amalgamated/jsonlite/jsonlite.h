//
//  Copyright 2012-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_H
#define JSONLITE_H

// #include "jsonlite_buffer.h"
//
//  Copyright 2012-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_BUFFER_H
#define JSONLITE_BUFFER_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct jsonlite_buffer_struct *jsonlite_buffer;
    typedef int (*jsonlite_buffer_mem_fn)(jsonlite_buffer buffer, const void *data, size_t length);
    typedef const void * (*jsonlite_buffer_data_fn)(jsonlite_buffer buffer);
    
    struct jsonlite_buffer_struct {
        uint8_t *mem;
        size_t size;
        size_t capacity;
        
        jsonlite_buffer_mem_fn set_mem;
        jsonlite_buffer_mem_fn append_mem;
    } jsonlite_buffer_struct;
    
    int jsonlite_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length);
    int jsonlite_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length);
    const void *jsonlite_buffer_data(jsonlite_buffer buffer);
    size_t jsonlite_buffer_size(jsonlite_buffer buffer);

    #define jsonlite_static_buffer_size() (sizeof(jsonlite_buffer_struct))
    jsonlite_buffer jsonlite_static_buffer_init(void *mem, size_t size);
    
    #define jsonlite_heap_buffer_size() (sizeof(jsonlite_buffer_struct))
    jsonlite_buffer jsonlite_heap_buffer_init(void *mem);
    void jsonlite_heap_buffer_cleanup(jsonlite_buffer buffer);
    
    extern jsonlite_buffer jsonlite_null_buffer;
    
#ifdef __cplusplus
}
#endif

#endif

// #include "jsonlite_builder.h"
//
//  Copyright 2012-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_BUILDER_H
#define JSONLITE_BUILDER_H

#include <stdio.h>
// #include "jsonlite_types.h"
//
//  Copyright 2012-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

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

// #include "jsonlite_stream.h"
//
//  Copyright 2012-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_STREAM_H
#define JSONLITE_STREAM_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct jsonlite_stream_struct;
    typedef struct jsonlite_stream_struct const * jsonlite_stream;
    typedef int (*jsonlite_stream_write_fn)(jsonlite_stream stream, const void *data, size_t length);

    struct jsonlite_stream_struct {
        jsonlite_stream_write_fn write;
    } jsonlite_stream_struct;
    
    int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length);
    
    typedef struct jsonlite_mem_stream_block {
        struct jsonlite_mem_stream_block *next;
        uint8_t *data;
    } jsonlite_mem_stream_block;
    
    typedef struct jsonlite_mem_stream {
        size_t block_size;
        uint8_t *cursor;
        uint8_t *limit;
        struct jsonlite_mem_stream_block *current;
        struct jsonlite_mem_stream_block *first;
    } jsonlite_mem_stream;

    jsonlite_stream jsonlite_mem_stream_alloc(size_t block_size);
    void jsonlite_mem_stream_free(jsonlite_stream stream);
    size_t jsonlite_mem_stream_data(jsonlite_stream stream, uint8_t **data, size_t extra_bytes);
    
    typedef struct jsonlite_static_mem_stream {
        uint8_t *buffer;
        size_t size;
        size_t written;
        uint8_t *limit;
        int enabled;
    } jsonlite_static_mem_stream;
    
    #define jsonlite_static_mem_stream_size() (sizeof(jsonlite_stream_struct) + sizeof(jsonlite_static_mem_stream))
    
    jsonlite_stream jsonlite_static_mem_stream_init(void *buffer, size_t size);
    size_t jsonlite_static_mem_stream_written_bytes(jsonlite_stream stream);
    const void * jsonlite_static_mem_stream_data(jsonlite_stream stream);
    
    jsonlite_stream jsonlite_file_stream_alloc(FILE *file);
    void jsonlite_file_stream_free(jsonlite_stream stream);
    
    extern jsonlite_stream jsonlite_null_stream;
    extern jsonlite_stream jsonlite_stdout_stream;

#ifdef __cplusplus
}
#endif

#endif


#ifdef __cplusplus
extern "C" {
#endif
    
    struct jsonlite_builder_struct;
    typedef struct jsonlite_builder_struct* jsonlite_builder;
    typedef uint16_t jsonlite_write_state;
    typedef struct jsonlite_builder_struct {
        jsonlite_write_state *state;
        jsonlite_write_state *limit;
        jsonlite_write_state *stack;
        jsonlite_stream stream;
        
        size_t indentation;
        char doubleFormat[8];
    } jsonlite_builder_struct;

    #define jsonlite_builder_estimate_size(depth) (sizeof(jsonlite_builder_struct) + (depth) * sizeof(jsonlite_write_state))
    
    jsonlite_builder jsonlite_builder_init(void *memory, size_t size, jsonlite_stream stream);
    
    /** \brief Sets beautify indentation. Default is 0.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param indentation the beautify indentation; 0 - disabled
     */
    void jsonlite_builder_set_indentation(jsonlite_builder builder, size_t indentation);
    
    /** \brief Sets format for double values. Default is "%.16g".
     *
     * jsonlite_builder_set_double_format copies format parameter and you can safety release it.
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param format the double format; see sprintf function for details
     */
    void jsonlite_builder_set_double_format(jsonlite_builder builder, const char *format);
    
    /** \brief Begin JSON object.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_object_begin(jsonlite_builder builder);
    
    /** \brief End JSON object.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_object_end(jsonlite_builder builder);
    
    /** \brief Begin JSON array.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_array_begin(jsonlite_builder builder);
    
    /** \brief End JSON array.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_array_end(jsonlite_builder builder);
    
    /** \brief Write JSON key.
     *
     * jsonlite_builder_key performs two-character sequence escape for 
     * U+0022, U+005C, U+002F, U+0008, U+000C, U+000A, U+000D and U+0009
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param data the UTF-8 encoded string
     * @param length the string length
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_key(jsonlite_builder builder, const char *data, size_t length);
    
    /** \brief Write string value.
     *
     * jsonlite_builder_key performs two-character sequence escape for
     * U+0022, U+005C, U+002F, U+0008, U+000C, U+000A, U+000D and U+0009
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param data the UTF-8 encoded string
     * @param length the string length
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_string(jsonlite_builder builder, const char *data, size_t length);
    
    /** \brief Write integer value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param value the integer value
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_int(jsonlite_builder builder, long long value);
    
    /** \brief Write double value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param value the double value
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_double(jsonlite_builder builder, double value);
    
    /** \brief Write true value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_true(jsonlite_builder builder);
    
    /** \brief Write false value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_false(jsonlite_builder builder);
    
    /** \brief Write null value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_null(jsonlite_builder builder);
    
    /** \brief Write raw key.
     *
     * jsonlite_builder_raw_key does not perform any transformation.
     * jsonlite_builder_raw_key wraps raw key with '"' (U+0022).
     * If data already was wrapped with '"' use following practice jsonlite_builder_raw_key(d, data + 1, size - 2);
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param data the raw data
     * @param length the data length
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_raw_key(jsonlite_builder builder, const void *data, size_t length);
    
    /** \brief Write raw string.
     *
     * jsonlite_builder_raw_string does not perform any transformation.
     * jsonlite_builder_raw_string wraps raw string with '"' (U+0022).
     * If data already was wrapped with '"' use following practice jsonlite_builder_raw_string(d, data + 1, size - 2);
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param data the raw data
     * @param length the data length
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_raw_string(jsonlite_builder builder, const void *data, size_t length);
    
    /** \brief Write raw value.
     *
     * jsonlite_builder_raw_value does not perform any transformation.
     * jsonlite_builder_raw_value does not wrap raw value with '"' (U+0022).
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param data the raw data
     * @param length the data length
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_raw_value(jsonlite_builder builder, const void *data, size_t length);
    
    jsonlite_result jsonlite_builder_base64_value(jsonlite_builder builder, const void *data, size_t length);

#ifdef __cplusplus
}
#endif

#endif

// #include "jsonlite_parser.h"
//
//  Copyright 2012-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_PARSER_H
#define JSONLITE_PARSER_H

#include <stddef.h>
#include <stdint.h>
// #include "jsonlite_token.h"
//
//  Copyright 2012-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_TOKEN_H
#define JSONLITE_TOKEN_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    struct jsonlite_token;
    struct jsonlite_parser_struct;

    /** @brief Provides the hints for number token parsing.
     * 
     * This values is valid for jsonlite_parser_callbacks::number_found callback only.
     */
    typedef enum {
        /** @brief Indicates that number token has integer part.
         * 
         * @note
         * This flag is always set because of JSON number always has integer part (value .123 is not allowed). 
         */
        jsonlite_number_int = 0x01,
        
        /** @brief Indicates that number token has fraction part.
         *
         * This flag will set if token has an fraction part. For example: 123.987;
         * in current case fraction part is .987.
         */
        jsonlite_number_frac = 0x02,
        
        /** @brief Indicates that number token has exponent part.
         *
         * This flag will set if token has an exponent part.
         *
         * For example: 
         * For integer values: 123e5, 123E5, 123E+5, 123e+5;
         * all of this numbers are equal to each other and has exponent part.
         *
         * An other case 12300000 is also equals to previous numbers but has no exponent part.
         *
         * For floating point values:  123.01e5, 123.01E5, 123.01E+5, 123.01e+5;
         * all of this number are equal to each other and has exponent part.
         * An other case 12301000 is also equals to previous numbers but has no exponent part.
         */
        jsonlite_number_exp = 0x04,
        
        /** @brief Indicates that number token has negative value.
         *
         * This flag will set if token starts with '-' character.
         */
        jsonlite_number_negative = 0x08,
        
        /** @brief Indicates that number token starts with zero character.
         */
        jsonlite_number_zero_leading = 0x10,
        \
        /** @brief Indicates that number token starts with digit that is greater 0.
         */
        jsonlite_number_digit_leading = 0x20
    } jsonlite_number_type;
    
    /** @brief Provides the hints for string token parsing.
     *
     * This values is valid for jsonlite_parser_callbacks::string_found
     * and jsonlite_parser_callbacks::key_found callbacks only.
     */
    typedef enum {
        /** @brief Indicates that string token contains ASCII characters.
         *
         * @note
         * This flag is always set because of JSON string always has ASCII characters.
         */
        jsonlite_string_ascii = 0x01,
        
        /** @brief Indicates that string token has the sequences of UTF-8 characters.
         *
         * @note
         * This flag will set if string token has 2, 3 or 4 subsequently.
         */
        jsonlite_string_utf8 = 0x02,
        
        /** @brief Indicates that string token has an escaped character(s).
         *
         * This flag will be set if string token has one or more following escaped character:
         * - \\"
         * - \\\\
         * - \\n
         * - \\r
         * - \\/
         * - \\b
         * - \\f
         * - \\t
         */
        jsonlite_string_escape = 0x04,
        
        /** @brief Indicates that string token has one or more unicode escaped character(s).
         *
         * This flag will be set if string token has \\uXXXX escape - where (XXXX is an unicode character code)
         */
        jsonlite_string_unicode_escape = 0x08,
        
        
        /** @brief Indicates that string token has one or more unicode noncharacter(s).
         *
         * This flag will be set if string token has \\uFDD0-\\uFDEF and \\uFFFE-\\uFFFF unicode character
         */
        jsonlite_string_unicode_noncharacter = 0x10
    } jsonlite_string_type;
    
    /** @brief Contains information about parsed token.
     */
    typedef struct jsonlite_token {
        /** @brief This variable is reserved for high-level libraries.
         */
        void *ext;
        
        /** @brief Contains the start position of token.
         */
        const uint8_t *start;
        
        /** @brief Contains the end position of tokens.
         *
         * End position does not below to token, it should be interpreted as position of zero character.
         * @note
         * To measure token length you can use following expression: token->end - token->start. 
         */
        const uint8_t *end;

        /** @brief Contains the hints for token parsing.
         */
        union {
            /** @brief Contains the hints for number token parsing.
             */
            jsonlite_number_type number;
            
            /** @brief Contains the hints for string token parsing.
             */
            jsonlite_string_type string;
        } type;
    } jsonlite_token;

    
    /** @brief Returns a size of memory that is required for token conversion to UTF-8 string.
     * @param ts jsonlite token
     * @return 0 if ts is NULL; otherwise required size of for token conversion.
     */
    size_t jsonlite_token_size_of_uft8(jsonlite_token *ts);
    
    /** @brief Converts specified token to UTF-8 string.
     *
     * Function converts specified token to UTF-8 string encoding and copy zero terminated string to buffer.
     * @note
     * Function will alloc memory by itself if *buffer == NULL.
     * In this case you are responsible for memory releasing by using free() function.
     * @param ts jsonlite token
     * @return length in bytes  of converted string.
     */
    size_t jsonlite_token_to_uft8(jsonlite_token *ts, uint8_t *buffer);
    
    /** @brief Returns a size of memory that is required for token conversion to UTF-16 string.
     * @param ts jsonlite token
     * @return 0  if ts is NULL; otherwise required size of for token conversion.
     */
    size_t jsonlite_token_size_of_uft16(jsonlite_token *ts);
    
    /** @brief Converts specified token to UTF-16 string.
     *
     * Function converts specified token to UTF-16 string encoding and copy zero terminated string to buffer.
     * @note
     * Function will alloc memory by itself if *buffer == NULL.
     * In this case you are responsible for memory releasing by using free() function.
     * @param ts jsonlite token
     * @return length in bytes of converted string.
     */
    size_t jsonlite_token_to_uft16(jsonlite_token *ts, uint16_t *buffer);
    
    size_t jsonlite_token_size_of_base64_binary(jsonlite_token *ts);
    size_t jsonlite_token_base64_to_binary(jsonlite_token *ts, void *buffer);
    
    long jsonlite_token_to_long(jsonlite_token *token);
    long long jsonlite_token_to_long_long(jsonlite_token *token);
    
#ifdef __cplusplus
}
#endif

#endif

// #include "jsonlite_types.h"

// #include "jsonlite_buffer.h"


#ifdef __cplusplus
extern "C" {
#endif
    
    struct jsonlite_parser_struct;
    typedef struct jsonlite_parser_struct* jsonlite_parser;
    
    /** @brief Contains callback information.
     */
    typedef struct {
        /** @brief jsonlite parser object that initiate callback.
         * @note
         * You can use ::jsonlite_parser_suspend to stop tokenization.
         */
        jsonlite_parser parser;
        
        /** @brief Reserved for client usage.
         */
        void *client_state;
    } jsonlite_callback_context;
    
    /** @brief Type of value callback function.
     */
    typedef void (*jsonlite_value_callback)(jsonlite_callback_context *, jsonlite_token *);
    
    /** @brief Type of state callback function.
     */
    typedef void (*jsonlite_state_callback)(jsonlite_callback_context *);
    
    /** @brief Contains references to client callback functions.
     * 
     * You can use the global jsonlite_default_callbacks constant to initialize default values.
     */
    typedef struct {
        /** @brief Called when parser finished tokenization.
         * You can retrieve result of parsing using jsonlite_parser_get_result.
         */
        jsonlite_state_callback parse_finished;
        
        /** @brief Called when parser found object start.
         */
        jsonlite_state_callback object_start;
        
        /** @brief Called when parser found object end.
         */
        jsonlite_state_callback object_end;
        
        /** @brief Called when parser found array start.
         */
        jsonlite_state_callback array_start;
        
        /** @brief Called when parser found array end.
         */
        jsonlite_state_callback array_end;
        
        /** @brief Called when parser found \a true token.
         */
        jsonlite_state_callback true_found;
        
        /** @brief Called when parser found \a false token.
         */
        jsonlite_state_callback false_found;
        
        /** @brief Called when parser found \a null token.
         */
        jsonlite_state_callback null_found;
        
        /** @brief Called when parser found key token.
         */
        jsonlite_value_callback key_found;
        
        /** @brief Called when parser found string token.
         */
        jsonlite_value_callback string_found;
        
        /** @brief Called when parser found number token.
         */
        jsonlite_value_callback number_found;
        
        /** @brief Callbacks' context, will be past as first parameter of callback function.
         */
        jsonlite_callback_context context;
    } jsonlite_parser_callbacks;

    typedef uint8_t parse_state;
    struct jsonlite_parser_struct {
        const uint8_t *cursor;
        const uint8_t *limit;
        const uint8_t *buffer;
        
        jsonlite_buffer rest_buffer;
        
        parse_state *current;
        parse_state *last;
        parse_state **control;
        
        jsonlite_result result;
        jsonlite_parser_callbacks callbacks;
    } jsonlite_parser_struct;
    
    /** @brief Initializes memory for parser object.
     *
     * @see jsonlite_parser
     * @see jsonlite_parser_reset
     * @param memory the memory for parser.
     * @param size the memory size.
     * @return jsonlite_parser object.
     */
    jsonlite_parser jsonlite_parser_init(void *memory, size_t size, jsonlite_buffer rest_buffer);
    
    /** \brief Copies provided callbacks structure to parser object.
     * @see jsonlite_parser
     * @see jsonlite_parser_callbacks
     * @see jsonlite_result
     * @param parser the parser object.
     * @param parser the callbacks object.
     */
    void jsonlite_parser_set_callback(jsonlite_parser parser, const jsonlite_parser_callbacks *cbs);
    
    /** \brief Returns result of last operation.
     * @see jsonlite_parser
     * @see jsonlite_result
     * @param parser the parser object.
     * @return Result of last operation.
     */
    jsonlite_result jsonlite_parser_get_result(jsonlite_parser parser);
    
    /** \brief Performs JSON tokenization.
     *
     * jsonlite is a chunk parser and you can use this function to parser a fragment of JSON.
     * @see jsonlite_parser
     * @see jsonlite_result
     * @param parser the parser object.
     * @param buffer the pointer to JSON payload buffer.
     * @param size the JSON payload buffer size.
     * @return JSON parsing result.
     * @endcode
     */
    jsonlite_result jsonlite_parser_tokenize(jsonlite_parser parser, const void *buffer, size_t size);
    
    /** \brief Resumes JSON tokenization.
     * @see jsonlite_parser
     * @see jsonlite_result
     * @param parser the parser object.
     * @return JSON parsing result.
     */
    jsonlite_result jsonlite_parser_resume(jsonlite_parser parser);
    
    /** \brief Suspends JSON tokenization.
     *
     * You can continue tokenization later by calling ::jsonlite_parser_resume.
     * @see jsonlite_parser
     * @see jsonlite_result
     * @param parser the parser object.
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_parser_suspend(jsonlite_parser parser);
    
    /** \brief Terminate JSON tokenization.
     *
     * @see jsonlite_parser
     * @see jsonlite_result
     * @param parser the parser object.
     * @return jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_parser_terminate(jsonlite_parser parser, jsonlite_result result);
    
    /** \brief jsonlite_parser_callbacks structure initialized with callbacks that do nothing.
     */
    extern const jsonlite_parser_callbacks jsonlite_default_callbacks;
    
#ifdef __cplusplus
}
#endif

#define jsonlite_parser_estimate_size(depth) (sizeof(jsonlite_parser_struct) + (depth) * sizeof(parse_state))

#endif

// #include "jsonlite_stream.h"

// #include "jsonlite_token.h"

// #include "jsonlite_token_pool.h"
//
//  Copyright 2012-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_TOKEN_POOL_H
#define JSONLITE_TOKEN_POOL_H

// #include "jsonlite_token.h"


#ifdef __cplusplus
extern "C" {
#endif
    
#define JSONLITE_TOKEN_POOL_FRONT 0x80
#define JSONLITE_TOKEN_POOL_FRONT_MASK (JSONLITE_TOKEN_POOL_FRONT - 1)
    
typedef void (*jsonlite_token_pool_release_value_fn)(void *);
typedef struct jsonlite_token_pool_struct* jsonlite_token_pool;
    
typedef struct jsonlite_token_bucket {
    const uint8_t *start;
    const uint8_t *end;
    
    ptrdiff_t hash;
    ptrdiff_t value_hash;
    
    const void *value;
} jsonlite_token_bucket;
    
typedef struct jsonlite_token_block {
    jsonlite_token_bucket *buckets;
    size_t capacity;
} jsonlite_token_block;

typedef struct jsonlite_token_pool_struct {
    jsonlite_token_block blocks[JSONLITE_TOKEN_POOL_FRONT];
    uint8_t *content_pool;
    size_t content_pool_size;   
} jsonlite_token_pool_struct;

#define jsonlite_token_pool_estimate_size(count) ((count) * sizeof(jsonlite_token_pool_struct))
    
size_t jsonlite_token_pool_init_memory(void *mem, size_t size, jsonlite_token_pool* pools);
void jsonlite_token_pool_copy_tokens(jsonlite_token_pool pool);
void jsonlite_token_pool_cleanup(jsonlite_token_pool* pools, size_t count, jsonlite_token_pool_release_value_fn release);
jsonlite_token_bucket* jsonlite_token_pool_get_bucket(jsonlite_token_pool pool, jsonlite_token *token);

#ifdef __cplusplus
}
#endif

#endif


extern const char *jsonlite_version;

#endif
