//
//  main.cpp
//  jsonlite_benchmark
//
//  Created by admin on 12/16/12.
//  Copyright (c) 2012 Andrii Mamchur. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "jsonlite.h"
#include "jsonlite_token.h"
#include "yajl_parse.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filestream.h"

using namespace std;

int token_count = 0;

char buffer[1024 * 1024];
size_t length;
const int COUNT = 1000;

void jsonlite_decode_token(jsonlite_callback_context *)  {
    token_count++;
}

void jsonlite_decode_number(jsonlite_callback_context *, jsonlite_token *ts) {
    token_count++;
}

// YAJL & rapidjson encode escaped string during parsing, jsonlite does not do it.
// To be fair we must do it in callback function
void jsonlite_decode_string(jsonlite_callback_context *, jsonlite_token *ts) {
    token_count++;
    if (ts->string_type & jsonlite_string_escape) {
        uint8_t *buffer = NULL;
        jsonlite_token_decode_to_uft8(ts, &buffer);
        free(buffer);
    }
}

void go_jsonlite() {
    token_count = 0;
    
    struct rusage r;
    struct timeval bu, bs;
    
    getrusage(RUSAGE_SELF, &r);
    bu = r.ru_utime;
    bs = r.ru_stime;
    
    bool success = true;
    jsonlite_parser_callbacks cbs = jsonlite_default_callbacks;
    cbs.object_start = &jsonlite_decode_token;
    cbs.object_end = &jsonlite_decode_token;
    cbs.array_start = &jsonlite_decode_token;
    cbs.array_end = &jsonlite_decode_token;
    cbs.true_found = &jsonlite_decode_token;
    cbs.false_found = &jsonlite_decode_token;
    cbs.null_found = &jsonlite_decode_token;
    cbs.key_found = &jsonlite_decode_string;
    cbs.string_found = &jsonlite_decode_string;
    cbs.number_found = &jsonlite_decode_number;
    for (int i = 0; i < COUNT; i++) {
        jsonlite_parser p = jsonlite_parser_init(512);
        jsonlite_parser_set_callback(p, &cbs);
        success = success && jsonlite_parser_tokenize(p, buffer, length) == jsonlite_result_ok;
        jsonlite_parser_release(p);
        
        if (!success) {
            break;
        }
    }
    
    if (success) {
        getrusage(RUSAGE_SELF, &r);
        
        long time = (r.ru_utime.tv_sec - bu.tv_sec) + (r.ru_stime.tv_sec - bs.tv_sec);
        time = time * 1000000;
        time += (r.ru_utime.tv_usec - bu.tv_usec) + (r.ru_stime.tv_usec - bs.tv_usec);
        
        double speed = ((double)length * COUNT / 1024 / 1024) / ((double)time / 1000000);
        std::cout << "jsonlite  - " << speed << " MBps" << std::endl;
        std::cout << "jsonlite tokens count - " << token_count << std::endl;
    } else {
        std::cout << "jsonlite  - fail!" << std::endl;
    }
}

static int handle_string (void *ctx, const unsigned char *string, size_t string_length)
{
    token_count++;
    return 1;
}

static int handle_number (void *ctx, const char *string, size_t string_length)
{
    token_count++;
    return 1;
}

static int handle_start_map (void *ctx)
{
    token_count++;
    return 1;
}

static int handle_end_map (void *ctx)
{
    token_count++;
    return 1;
}

static int handle_start_array (void *ctx)
{
    token_count++;
    return 1;
}

static int handle_end_array (void *ctx)
{
    token_count++;
    return 1;
}

static int handle_boolean (void *ctx, int boolean_value)
{
    token_count++;
    return 1;
}

static int handle_null (void *ctx)
{
    token_count++;
    return 1;
}

void go_yajl() {
    token_count = 0;
    
    struct rusage r;
    struct timeval bu, bs;
    
    getrusage(RUSAGE_SELF, &r);
    bu = r.ru_utime;
    bs = r.ru_stime;
    
    bool success = true;
    const yajl_callbacks callbacks =
    {
        /* null        = */ handle_null,
        /* boolean     = */ handle_boolean,
        /* integer     = */ NULL,
        /* double      = */ NULL,
        /* number      = */ handle_number,
        /* string      = */ handle_string,
        /* start map   = */ handle_start_map,
        /* map key     = */ handle_string,
        /* end map     = */ handle_end_map,
        /* start array = */ handle_start_array,
        /* end array   = */ handle_end_array
    };
    
    for (int i = 0; i < COUNT; i++) {
        yajl_handle h = yajl_alloc(&callbacks, NULL, NULL);
        yajl_config(h, yajl_allow_comments, 0);
        yajl_config(h, yajl_dont_validate_strings, 0);
        success = success && yajl_parse(h, (const unsigned char *)buffer, length) == yajl_status_ok;
        yajl_free(h);
        
        if (!success) {
            break;
        }
    }
    
    if (success) {
        getrusage(RUSAGE_SELF, &r);
    
        long time = (r.ru_utime.tv_sec - bu.tv_sec) + (r.ru_stime.tv_sec - bs.tv_sec);
        time = time * 1000000;
        time += (r.ru_utime.tv_usec - bu.tv_usec) + (r.ru_stime.tv_usec - bs.tv_usec);
        
        double speed = ((double)length * COUNT / 1024 / 1024) / ((double)time / 1000000);
        std::cout << "YAJL      - " << speed << " MBps" << std::endl;
        std::cout << "YAJL tokens count - " << token_count << std::endl;
    } else {
        std::cout << "YAJL      - fail!" << std::endl;
    }
}

template<typename Encoding = rapidjson::UTF8<> >
struct TestHandler {
	typedef typename Encoding::Ch Ch;
    
	virtual void Default() {}
	virtual void Null() { token_count++; }
	virtual void Bool(bool) { token_count++; }
	virtual void Int(int) { token_count++; }
	virtual void Uint(unsigned) { token_count++; }
	virtual void Int64(int64_t) { token_count++; }
	virtual void Uint64(uint64_t) { token_count++; }
	virtual void Double(double) { token_count++; }
	virtual void String(const Ch*, rapidjson::SizeType, bool) { token_count++; }
	virtual void StartObject() { token_count++; }
	virtual void EndObject(rapidjson::SizeType) { token_count++; }
	virtual void StartArray() { token_count++; }
	virtual void EndArray(rapidjson::SizeType) { token_count++; }
};

void go_rapidjson() {
    token_count = 0;
    
    struct rusage r;
    struct timeval bu, bs;
    
    getrusage(RUSAGE_SELF, &r);
    bu = r.ru_utime;
    bs = r.ru_stime;
    
    bool success = true;
    for (int i = 0; i < COUNT; i++) {
        rapidjson::StringStream s(buffer);
        TestHandler<> h;
        rapidjson::Reader reader;
        success = reader.Parse<rapidjson::kParseDefaultFlags>(s, h);
        if (!success) {
            break;
        }
    }
        
    if (success) {
        getrusage(RUSAGE_SELF, &r);
        long time = (r.ru_utime.tv_sec - bu.tv_sec) + (r.ru_stime.tv_sec - bs.tv_sec);
        time = time * 1000000;
        time += (r.ru_utime.tv_usec - bu.tv_usec) + (r.ru_stime.tv_usec - bs.tv_usec);
        
        double speed = ((double)length * COUNT / 1024 / 1024) / ((double)time / 1000000);
        std::cout << "rapidjson - " << speed << " MBps" << std::endl;
        std::cout << "rapidjson tokens count - " << token_count << std::endl;
    } else {
        std::cout << "rapidjson - fail!" << std::endl;
    }
}

int main(int argc, const char * argv[])
{
    fstream file("sample.json", fstream::in | fstream::binary);
    file.seekg (0, ios::end);
    length = file.tellg();
    file.seekg (0, ios::beg);    
    file.read(buffer,length);
    file.close();

    go_jsonlite();
    go_yajl();
    //go_rapidjson();
    
    return 0;
}

