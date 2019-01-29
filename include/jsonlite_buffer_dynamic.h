#ifndef JSONLITE_BUFFER_DYNAMIC__H
#define JSONLITE_BUFFER_DYNAMIC__H

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#endif

#define jsonlite_buffer_dynamic_size() (sizeof(jsonlite_buffer_struct))

jsonlite_buffer jsonlite_buffer_dynamic_init(void *mem);

void jsonlite_buffer_dynamic_cleanup(jsonlite_buffer buffer);

#endif
