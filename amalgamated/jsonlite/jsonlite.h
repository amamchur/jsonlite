//
//  Copyright 2012-2013, Andrii Mamchur
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

// #include "jsonlite_parser.h"
//
//  Copyright 2012-2013, Andrii Mamchur
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

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
// #include "jsonlite_token.h"
//
//  Copyright 2012-2013, Andrii Mamchur
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

#include <stdio.h>
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
    size_t jsonlite_token_decode_size_for_uft8(jsonlite_token *ts);
    
    /** @brief Converts specified token to UTF-8 string.
     *
     * Function converts specified token to UTF-8 string encoding and copy zero terminated string to buffer.
     * @note
     * Function will alloc memory by itself if *buffer == NULL.
     * In this case you are responsible for memory releasing by using free() function.
     * @param ts jsonlite token
     * @return length in bytes  of converted string.
     */
    size_t jsonlite_token_decode_to_uft8(jsonlite_token *ts, uint8_t **buffer);
    
    /** @brief Returns a size of memory that is required for token conversion to UTF-16 string.
     * @param ts jsonlite token
     * @return 0  if ts is NULL; otherwise required size of for token conversion.
     */
    size_t jsonlite_token_decode_size_for_uft16(jsonlite_token *ts);
    
    /** @brief Converts specified token to UTF-16 string.
     *
     * Function converts specified token to UTF-16 string encoding and copy zero terminated string to buffer.
     * @note
     * Function will alloc memory by itself if *buffer == NULL.
     * In this case you are responsible for memory releasing by using free() function.
     * @param ts jsonlite token
     * @return length in bytes of converted string.
     */
    size_t jsonlite_token_decode_to_uft16(jsonlite_token *ts, uint16_t **buffer);

    /** @brief Converts hex digit to integer value.
     *
     * @param c a ASCII character.
     * @return integer value of hex character, 
     * if character belongs to set [0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F,a,b,c,d,e,f]; otherwise 0xFF.
     */
    uint8_t jsonlite_hex_char_to_uint8(uint8_t c);
    
#ifdef __cplusplus
}
#endif

#endif

// #include "jsonlite_types.h"
//
//  Copyright 2012-2013, Andrii Mamchur
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
    jsonlite_result_invalid_argument,
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
    
    jsonlite_result_not_allowed
} jsonlite_result;


#endif


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

    /** @brief Estimates memory usage.
     * @note
     * This value depends on CPU architectures.
     * @param depth the parsing depth.
     * @return Estimated size in bytes.
     */
    size_t jsonlite_parser_estimate_size(size_t depth);

    /** @brief Creates and initializes new instance of parser object.
     *
     * You should release jsonlite_parser object using ::jsonlite_parser_release.
     * @see jsonlite_parser
     * @see jsonlite_parser_release
     * @param depth the parsing depth.
     * @return jsonlite_parser object.
     */
    jsonlite_parser jsonlite_parser_init(size_t depth);
    
    /** @brief Initializes memory for parser object.
     *
     * You should release internal resources using ::jsonlite_parser_cleanup
     * @see jsonlite_parser
     * @see jsonlite_parser_reset
     * @param memory the memory for parser.
     * @param size the memory size.
     * @return jsonlite_parser object.
     */
    jsonlite_parser jsonlite_parser_init_memory(void *memory, size_t size);
    
    /** \brief Copies provided callbacks structure to parser object.
     * @see jsonlite_parser
     * @see jsonlite_parser_callbacks
     * @see jsonlite_result
     * @param parser the parser object.
     * @param parser the callbacks object.
     * @return jsonlite_result_invalid_argument when parser or cbs are NULL; otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_parser_set_callback(jsonlite_parser parser, const jsonlite_parser_callbacks *cbs);
    
    /** \brief Returns result of last operation.
     * @see jsonlite_parser
     * @see jsonlite_result
     * @param parser the parser object.
     * @return jsonlite_result_invalid_argument when parser is NULL; otherwise s result of last operation.
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
     * @return JSON parsing result or jsonlite_result_invalid_argument when some parameter is invalid.
     * 
     * There is an example of JSON validation
     * @code{.c}
     * char json[] = "{\"key\" : 12345, \"obj\": {}, \"array\":[null, true, false, \"string\"]}";
     * jsonlite_parser p = jsonlite_parser_init(16);
     * jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json));
     * assert(result == jsonlite_result_ok);
     * jsonlite_parser_release(p);
     * @endcode
     *
     * There is an another example of JSON chunk parsing.
     * @code{.c}
     * char chunk1[] = "{\"key\" : 12345, \"obj\": {}, \"arr";
     * char chunk2[] = "ay\":[null, true, false, \"string\"]}";
     * jsonlite_parser p = jsonlite_parser_init(16);
     *
     * jsonlite_result result = jsonlite_parser_tokenize(p, chunk1, sizeof(chunk1) - 1);
     * assert(result == jsonlite_result_end_of_stream);
     * // Now you can release or reuse chunk1 buffer.
     *
     * result = jsonlite_parser_tokenize(p, chunk2, sizeof(chunk2) - 1);
     * assert(result == jsonlite_result_ok);
     *
     * jsonlite_parser_release(p);
     * @endcode
     */
    jsonlite_result jsonlite_parser_tokenize(jsonlite_parser parser, const void *buffer, size_t size);
    
    /** \brief Resumes JSON tokenization.
     * @see jsonlite_parser
     * @see jsonlite_result
     * @param parser the parser object.
     * @return JSON parsing result or jsonlite_result_invalid_argument when parser is NULL.
     */
    jsonlite_result jsonlite_parser_resume(jsonlite_parser parser);
    
    /** \brief Suspends JSON tokenization.
     *
     * You can continue tokenization later by calling ::jsonlite_parser_resume.
     * @see jsonlite_parser
     * @see jsonlite_result
     * @param parser the parser object.
     * @return jsonlite_result_invalid_argument when parser is NULL; 
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_parser_suspend(jsonlite_parser parser);
    
    /** \brief Terminate JSON tokenization.
     *
     * @see jsonlite_parser
     * @see jsonlite_result
     * @param parser the parser object.
     * @return jsonlite_result_invalid_argument when parser is NULL or result is jsonlite_result_unknown;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_parser_terminate(jsonlite_parser parser, jsonlite_result result);
    
    /** \brief Releases parser object.
     *
     * If parser is NULL, jsonlite_parser_release does nothing.
     * @see jsonlite_parser
     * @param parser the parser object.
     */
    void jsonlite_parser_release(jsonlite_parser parser);
    
    /** \brief Releases internal resources and states.
     *
     * If parser is NULL, jsonlite_parser_reset does nothing.
     * @see jsonlite_parser
     * @param parser the parser object.
     */
    void jsonlite_parser_cleanup(jsonlite_parser parser);

    /** \brief jsonlite_parser_callbacks structure initialized with callbacks that do nothing.
     */
    extern const jsonlite_parser_callbacks jsonlite_default_callbacks;
    
#ifdef __cplusplus
}
#endif

#endif

// #include "jsonlite_builder.h"
//
//  Copyright 2012-2013, Andrii Mamchur
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

// #include "jsonlite_stream.h"
//
//  Copyright 2012-2013, Andrii Mamchur
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

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct jsonlite_stream_struct;
    typedef struct jsonlite_stream_struct const * jsonlite_stream;

    typedef int (*jsonlite_stream_write_fn)(jsonlite_stream stream, const void *data, size_t length);
    typedef void (*jsonlite_stream_release_fn)(jsonlite_stream stream);

    int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length);
    void jsonlite_stream_release(jsonlite_stream stream);
    
    jsonlite_stream jsonlite_mem_stream_init(size_t block_size);
    size_t jsonlite_mem_stream_data(jsonlite_stream stream, uint8_t **data);
    
    jsonlite_stream jsonlite_static_mem_stream_init(void *buffer, size_t size);
    size_t jsonlite_static_mem_stream_written_bytes(jsonlite_stream stream);
    
    jsonlite_stream jsonlite_file_stream_init(FILE *file);
   
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

    /** @brief Creates and initializes new instance of builder object.
     *
     * You should release jsonlite_builder object using ::jsonlite_builder_release.
     * @see jsonlite_builder
     * @see jsonlite_builder_release
     * @param depth the builder depth
     * @return jsonlite_builder object
     */
    jsonlite_builder jsonlite_builder_init(size_t depth, jsonlite_stream stream);
    
    /** \brief Releases builder object.
     *
     * If builder is NULL, jsonlite_builder_release does nothing.
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_invalid_argument when builder is NULL; otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_release(jsonlite_builder builder);
       
    /** \brief Sets beautify indentation. Default is 0.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param indentation the beautify indentation; 0 - disabled
     * @return jsonlite_result_invalid_argument when builder is NULL; otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_set_indentation(jsonlite_builder builder, size_t indentation);
    
    /** \brief Sets format for double values. Default is "%.16g".
     *
     * jsonlite_builder_set_double_format copies format parameter and you can safety release it.
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param format the double format; see sprintf function for details
     * @return jsonlite_result_invalid_argument when builder or format are NULL; otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_set_double_format(jsonlite_builder builder, const char *format);
    
    /** \brief Begin JSON object.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_invalid_argument when builder is NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_object_begin(jsonlite_builder builder);
    
    /** \brief End JSON object.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_invalid_argument when builder is NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_object_end(jsonlite_builder builder);
    
    /** \brief Begin JSON array.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_invalid_argument when builder is NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_array_begin(jsonlite_builder builder);
    
    /** \brief End JSON array.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_invalid_argument when builder is NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
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
     * @return jsonlite_result_invalid_argument when builder or data are NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
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
     * @return jsonlite_result_invalid_argument when builder or data are NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_string(jsonlite_builder builder, const char *data, size_t length);
    
    /** \brief Write integer value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param value the integer value
     * @return jsonlite_result_invalid_argument when builder is NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_int(jsonlite_builder builder, long long value);
    
    /** \brief Write double value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @param value the double value
     * @return jsonlite_result_invalid_argument when builder is NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_double(jsonlite_builder builder, double value);
    
    /** \brief Write true value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_invalid_argument when builder is NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_true(jsonlite_builder builder);
    
    /** \brief Write false value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_invalid_argument when builder is NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_false(jsonlite_builder builder);
    
    /** \brief Write null value.
     *
     * @see jsonlite_builder
     * @see jsonlite_result
     * @param builder the builder object
     * @return jsonlite_result_invalid_argument when builder is NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
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
     * @return jsonlite_result_invalid_argument when builder or data are NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
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
     * @return jsonlite_result_invalid_argument when builder or data are NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
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
     * @return jsonlite_result_invalid_argument when builder or data are NULL;
     * jsonlite_result_not_allowed when operation is not allowed;
     * otherwise jsonlite_result_ok.
     */
    jsonlite_result jsonlite_builder_raw_value(jsonlite_builder builder, const void *data, size_t length);

#ifdef __cplusplus
}
#endif

#endif

// #include "jsonlite_token_pool.h"
//
//  Copyright 2012-2013, Andrii Mamchur
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
    
typedef void (*jsonlite_token_pool_release_value_fn)(void *);
typedef struct jsonlite_token_pool_struct* jsonlite_token_pool;
    
typedef struct jsonlite_token_bucket {
    const uint8_t *start;
    const uint8_t *end;
    
    ptrdiff_t hash;
    ptrdiff_t value_hash;
    
    const void *value;
} jsonlite_token_bucket;
    
jsonlite_token_pool jsonlite_token_pool_create(jsonlite_token_pool_release_value_fn release_fn);
void jsonlite_token_pool_copy_tokens(jsonlite_token_pool pool);
void jsonlite_token_pool_release(jsonlite_token_pool pool);
jsonlite_token_bucket* jsonlite_token_pool_get_bucket(jsonlite_token_pool pool, jsonlite_token *token);

#ifdef __cplusplus
}
#endif

#endif


#endif

/** @mainpage jsonlite Index Page
 *
 * @section intro_sec Introduction
 *
 * This is the introduction.
 *
 * @section install_sec Installation
 *
 * @subsection step1 Step 1: Opening the box
 *
 * etc...
 */
