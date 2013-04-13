// VisualCppJsonLite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../../jsonlite/include/jsonlite.h"

int _tmain(int argc, _TCHAR* argv[])
{
	char json[] = "{\"key\" : 12345, \"obj\": {}, \"array\":[null, true, false, \"string\"]}";
    jsonlite_parser p = jsonlite_parser_init(16);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json));
    jsonlite_parser_release(p);
	return 0;
}

