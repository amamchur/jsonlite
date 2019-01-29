#ifndef JSONLITE_PARSER_H
#define JSONLITE_PARSER_H

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#include "jsonlite_token.h"
#include "jsonlite_types.h"
#endif

#include <stdint.h>

struct jsonlite_parser_struct;
typedef struct jsonlite_parser_struct *jsonlite_parser;

typedef enum {
    jsonlite_event_finished,
    jsonlite_event_object_start,
    jsonlite_event_object_end,
    jsonlite_event_array_start,
    jsonlite_event_array_end
} jsonlite_event;

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
typedef void (*jsonlite_token_callback)(jsonlite_callback_context *, jsonlite_token *);

/** @brief Type of state callback function.
 */
typedef void (*jsonlite_event_callback)(jsonlite_callback_context *, jsonlite_event event);

/** @brief Contains references to client callback functions.
 *
 * You can use the global jsonlite_default_callbacks constant to initialize default values.
 */
typedef struct {
    /** @brief
    */
    jsonlite_event_callback event_occurred;

    /** @brief Called when parser found JSON token.
     */
    jsonlite_token_callback token_found;

    /** @brief Callbacks' context, will be past as first parameter of callback function.
     */
    jsonlite_callback_context context;
} jsonlite_parser_callbacks;

typedef uint8_t parse_state;
typedef struct jsonlite_parser_struct {
    const uint8_t *cursor;
    const uint8_t *limit;
    const uint8_t *buffer;

    jsonlite_buffer rest_buffer;

    parse_state *current;
    parse_state *last;
    parse_state *control;

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
 * @param cbs the callbacks object.
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

/** \brief Initialize callbacks with default values
 * @param cbs the parser callbacks object.
 */
void jsonlite_parser_callbacks_init(jsonlite_parser_callbacks *cbs);

#define jsonlite_parser_estimate_size(depth) (sizeof(jsonlite_parser_struct) + (depth) * sizeof(parse_state))

#endif
