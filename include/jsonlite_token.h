//
//  Copyright 2012-2019, Andrii Mamchur
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
        /** @brief Initial value
         */
        jsonlite_none = 0x00,
        jsonlite_token_null = 0x01,
        jsonlite_token_true = 0x02,
        jsonlite_token_false = 0x03,
        jsonlite_token_key = 0x04,
        jsonlite_token_number = 0x05,
        jsonlite_token_string = 0x06,
        jsonlite_token_type_mask = 0x07,

        /** @brief Indicates that number token has integer part.
         *
         * @note
         * This flag is always set because of JSON number always has integer part (value .123 is not allowed).
         */
        jsonlite_number_int = 0x08,

        /** @brief Indicates that number token has fraction part.
         *
         * This flag will set if token has an fraction part. For example: 123.987;
         * in current case fraction part is .987.
         */
        jsonlite_number_frac = 0x10,

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
        jsonlite_number_exp = 0x20,

        /** @brief Indicates that number token has negative value.
         *
         * This flag will set if token starts with '-' character.
         */
        jsonlite_number_negative = 0x40,

        /** @brief Indicates that number token starts with zero character.
         */
        jsonlite_number_zero_leading = 0x80,


        /** @brief Indicates that number token starts with digit that is greater 0.
         */
        jsonlite_number_digit_leading = 0xA0,

        /** @brief Indicates that string token contains ASCII characters.
         *
         * @note
         * This flag is always set because of JSON string always has ASCII characters.
         */
        jsonlite_string_ascii = 0x08,

        /** @brief Indicates that string token has the sequences of UTF-8 characters.
         *
         * @note
         * This flag will set if string token has 2, 3 or 4 subsequently.
         */
        jsonlite_string_utf8 = 0x10,

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
        jsonlite_string_escape = 0x20,

        /** @brief Indicates that string token has one or more unicode escaped character(s).
         *
         * This flag will be set if string token has \\uXXXX escape - where (XXXX is an unicode character code)
         */
        jsonlite_string_unicode_escape = 0x40,


        /** @brief Indicates that string token has one or more unicode noncharacter(s).
         *
         * This flag will be set if string token has \\uFDD0-\\uFDEF and \\uFFFE-\\uFFFF unicode character
         */
        jsonlite_string_unicode_noncharacter = 0x80
    } jsonlite_token_type;

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
        jsonlite_token_type type;
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
    size_t jsonlite_token_to_uft8(jsonlite_token *ts, void *buffer);

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
    size_t jsonlite_token_to_uft16(jsonlite_token *ts, void *buffer);

    size_t jsonlite_token_size_of_base64_binary(jsonlite_token *ts);
    size_t jsonlite_token_base64_to_binary(jsonlite_token *ts, void *buffer);

    long jsonlite_token_to_long(jsonlite_token *token);
    long long jsonlite_token_to_long_long(jsonlite_token *token);

#ifdef __cplusplus
}
#endif

#endif
