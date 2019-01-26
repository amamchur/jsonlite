#include "callback_recorder.hpp"

#include <gtest/gtest.h>
#include <jsonlite.h>
#include <jsonlite_token.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace {
    class string_container {
    public:
        std::string utf8;
        std::wstring utf16;
    };
}

static char img[] = "iVBORw0KGgoAAAANSUhEUgA"
                    "AAAoAAAAKCAYAAACNMs+9AAAABmJLR0QA/wD/AP+gvaeTAAAAB3RJ"
                    "TUUH1ggDCwMADQ4NnwAAAFVJREFUGJWNkMEJADEIBEcbSDkXUnfSg"
                    "nBVeZ8LSAjiwjyEQXSFEIcHGP9oAi+H0Bymgx9MhxbFdZE2a0s9kT"
                    "Zdw01ZhhYkABSwgmf1Z6r1SNyfFf4BZ+ZUExcNUQUAAAAASUVORK5"
                    "CYII=";

static char img_scaped[] = "iVBORw0KGgoAAAANSUhEUgA"
                           "AAAoAAAAKCAYAAACNMs+9AAAABmJLR0QA\\/wD\\/AP+gvaeTAAAAB3RJ"
                           "TUUH1ggDCwMADQ4NnwAAAFVJREFUGJWNkMEJADEIBEcbSDkXUnfSg"
                           "nBVeZ8LSAjiwjyEQXSFEIcHGP9oAi+H0Bymgx9MhxbFdZE2a0s9kT"
                           "Zdw01ZhhYkABSwgmf1Z6r1SNyfFf4BZ+ZUExcNUQUAAAAASUVORK5"
                           "CYII=";

static uint8_t img_bytes[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x06,
    0x00, 0x00, 0x00, 0x8d, 0x32, 0xcf, 0xbd, 0x00, 0x00, 0x00, 0x06, 0x62, 0x4b, 0x47, 0x44, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0xa0, 0xbd, 0xa7, 0x93, 0x00,
    0x00, 0x00, 0x07, 0x74, 0x49, 0x4d, 0x45, 0x07, 0xd6, 0x08, 0x03, 0x0b, 0x03, 0x00, 0x0d, 0x0e, 0x0d, 0x9f, 0x00, 0x00, 0x00, 0x55, 0x49, 0x44, 0x41, 0x54,
    0x18, 0x95, 0x8d, 0x90, 0xc1, 0x09, 0x00, 0x31, 0x08, 0x04, 0x47, 0x1b, 0x48, 0x39, 0x17, 0x52, 0x77, 0xd2, 0x82, 0x70, 0x55, 0x79, 0x9f, 0x0b, 0x48, 0x08,
    0xe2, 0xc2, 0x3c, 0x84, 0x41, 0x74, 0x85, 0x10, 0x87, 0x07, 0x18, 0xff, 0x68, 0x02, 0x2f, 0x87, 0xd0, 0x1c, 0xa6, 0x83, 0x1f, 0x4c, 0x87, 0x16, 0xc5, 0x75,
    0x91, 0x36, 0x6b, 0x4b, 0x3d, 0x91, 0x36, 0x5d, 0xc3, 0x4d, 0x59, 0x86, 0x16, 0x24, 0x00, 0x14, 0xb0, 0x82, 0x67, 0xf5, 0x67, 0xaa, 0xf5, 0x48, 0xdc, 0x9f,
    0x15, 0xfe, 0x01, 0x67, 0xe6, 0x54, 0x13, 0x17, 0x0d, 0x51, 0x05, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};

static void string_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    string_container sc;
    {
        auto buffer = new char[jsonlite_token_size_of_uft8(t)];
        auto size = jsonlite_token_to_uft8(t, buffer);
        sc.utf8 = std::string(buffer, buffer + size / sizeof(buffer[0]));
        delete[] buffer;
    }

    {
        auto buffer = new uint16_t[jsonlite_token_size_of_uft16(t)];
        auto size = jsonlite_token_to_uft16(t, buffer);
        sc.utf16 = std::wstring(buffer, buffer + size / sizeof(buffer[0]));
        delete[] buffer;
    }

    auto vect = reinterpret_cast<std::vector<string_container> *>(ctx->client_state);
    vect->emplace_back(sc);
}

TEST(tokens, should_convert_number_to_long_long) {
    jsonlite_token token;
    token.start = (uint8_t *)"1234567890";
    token.end = token.start + strlen((char *)token.start);

    long long value = jsonlite_token_to_long_long(&token);
    EXPECT_EQ(value, 1234567890);

    token.start = (uint8_t *)"-1234567890";
    token.end = token.start + strlen((char *)token.start);
    token.type.number = jsonlite_number_negative;
    value = jsonlite_token_to_long_long(&token);
    EXPECT_EQ(value, -1234567890);
}

TEST(tokens, should_convert_number_to_long) {
    jsonlite_token token;
    token.start = (uint8_t *)"1234567890";
    token.end = token.start + strlen((char *)token.start);

    long value = jsonlite_token_to_long(&token);
    EXPECT_EQ(value, 1234567890);

    token.start = (uint8_t *)"-1234567890";
    token.end = token.start + strlen((char *)token.start);
    token.type.number = jsonlite_number_negative;
    value = jsonlite_token_to_long_long(&token);
    EXPECT_EQ(value, -1234567890);
}

TEST(tokens, should_convert_unicode_string) {
    auto data = read_file_content("/success/strings.json");

    EXPECT_FALSE(data.empty());
    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    std::vector<string_container> sc;

    jsonlite_parser_callbacks cbs;
    jsonlite_parser_callbacks_init(&cbs);
    cbs.string_found = &string_callback;
    cbs.context.client_state = &sc;
    jsonlite_parser_set_callback(p, &cbs);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ(sc.size(), 15);
    EXPECT_EQ(sc[0].utf8, "");
    EXPECT_EQ(sc[0].utf16, L"");
    EXPECT_EQ(sc[1].utf8, "Test");
    EXPECT_EQ(sc[1].utf16, L"Test");
    EXPECT_EQ(sc[2].utf8, "\nTe\nst\n");
    EXPECT_EQ(sc[2].utf16, L"\nTe\nst\n");
    EXPECT_EQ(sc[3].utf8, "\r\n\t\\/\b\f\"");
    EXPECT_EQ(sc[3].utf16, L"\r\n\t\\/\b\f\"");
    EXPECT_EQ(sc[4].utf8, " spaces ");
    EXPECT_EQ(sc[4].utf16, L" spaces ");
    EXPECT_EQ(sc[5].utf8, "/");
    EXPECT_EQ(sc[5].utf16, L"/");
    EXPECT_EQ(sc[6].utf8, "/unescaped/slashes");
    EXPECT_EQ(sc[6].utf16, L"/unescaped/slashes");
    EXPECT_EQ(sc[7].utf8, "/escaped/slashes");
    EXPECT_EQ(sc[7].utf16, L"/escaped/slashes");
    EXPECT_EQ(sc[8].utf8, "\xF0\x9D\x9B\xA2");
    EXPECT_EQ(sc[8].utf16, L"\xD835\xDEE2");
    EXPECT_EQ(sc[9].utf8, "\xC3\xA9");
    EXPECT_EQ(sc[9].utf16, L"\xE9");
    EXPECT_EQ(sc[10].utf8, "\xF0\x9D\x84\x9E");
    EXPECT_EQ(sc[10].utf16, L"\xD834\xDD1E");
    EXPECT_EQ(sc[11].utf8,
              "42\xC3\xA9"
              "42\xE2\x89\xA5"
              "42");
    EXPECT_EQ(sc[11].utf16,
              L"42\xE9"
              L"42\x2265"
              L"42");
    EXPECT_EQ(sc[12].utf8, "Begin Escapes \"\\\n\r/\b\f\t\xF0\x9D\x9B\xA2 End Escapes");
    EXPECT_EQ(sc[12].utf16, L"Begin Escapes \"\\\n\r/\b\f\t\xD835\xDEE2 End Escapes");
    EXPECT_EQ(sc[13].utf8, "begin escapes \"\\\n\r/\b\f\t\xF0\x9D\x9B\xA2 end escapes");
    EXPECT_EQ(sc[13].utf16, L"begin escapes \"\\\n\r/\b\f\t\xD835\xDEE2 end escapes");
    EXPECT_EQ(sc[14].utf8,
              "\v=>x\xED\x93\xAE\xE2\xA3\xABP1\xE0\xA0\xABLMMX'M\xE5\x88\xBC\xE5\x94\xB3\xEB\x90\xA4\xF0\x9D\x9B\xA2 \xD0\x9A \xE0\xAF\xB5 "
              "\xEE\x80\x80\xF0\x9D\x9B\xA2\n");
    EXPECT_EQ(sc[14].utf16, L"\v=>x\xD4EE\x28EBP1\x82BLMMX'M\x523C\x5533\xB424\xD835\xDEE2 \x41A \xBF5 \xE000\xD835\xDEE2\n");
}

TEST(tokens, should_convert_base64) {
    char buffer[256];
    jsonlite_token token;
    token.start = (uint8_t *)"TWFu";
    token.end = token.start + strlen((char *)token.start);

    size_t size = jsonlite_token_size_of_base64_binary(&token);
    EXPECT_EQ(size, 4);

    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 3);
    EXPECT_TRUE(memcmp(buffer, "Man", 3) == 0);

    token.start = (uint8_t *)"SGVsbG8=";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_size_of_base64_binary(&token);
    EXPECT_EQ(size, 8);

    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 5);
    EXPECT_TRUE(memcmp(buffer, "Hello", 5) == 0);

    token.start = (uint8_t *)"SGVsbG8hIQ==";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_size_of_base64_binary(&token);
    EXPECT_EQ(size, 12);

    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 7);
    EXPECT_TRUE(memcmp(buffer, "Hello!!", 7) == 0);

    token.start = (uint8_t *)img;
    token.end = token.start + sizeof(img) - 1;
    size = jsonlite_token_size_of_base64_binary(&token);
    EXPECT_EQ(size, 180);

    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_TRUE(size == sizeof(img_bytes));
    EXPECT_TRUE(memcmp(buffer, img_bytes, sizeof(img_bytes)) == 0);

    token.start = (uint8_t *)img_scaped;
    token.end = token.start + sizeof(img_scaped) - 1;
    size = jsonlite_token_size_of_base64_binary(&token);
    EXPECT_EQ(size, 184);

    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_TRUE(size == sizeof(img_bytes));
    EXPECT_TRUE(memcmp(buffer, img_bytes, sizeof(img_bytes)) == 0);
}

TEST(tokens, should_handle_base64_errors) {
    jsonlite_token token;
    char buffer[256];
    size_t size = 0;

    token.start = (uint8_t *)"";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 0);

    token.start = (uint8_t *)"=";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 0);

    token.start = (uint8_t *)" ";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 0);

    token.start = (uint8_t *)"}";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 0);

    token.start = (uint8_t *)"TWF";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 0);

    token.start = (uint8_t *)"TWF{";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 0);

    token.start = (uint8_t *)"TWF ";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 0);

    token.start = (uint8_t *)"SGVsbG8===";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 0);

    token.start = (uint8_t *)"aaaa\\ba";
    token.end = token.start + strlen((char *)token.start);
    size = jsonlite_token_base64_to_binary(&token, buffer);
    EXPECT_EQ(size, 0);
}
