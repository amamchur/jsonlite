#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <jsonlite.h>
#include <stdint.h>
#include <string>
#include "callback_recorder.hpp"

TEST(parser, should_parse_array_with_different_values) {
    std::string path = TEST_DIR;
    std::string file = path + "/success/array_values.json";

    std::fstream f(file, std::ios::in | std::ios::binary);
    char buffer[4096];
    f.read(buffer, sizeof(buffer));
    auto count = static_cast<size_t>(f.gcount());
    EXPECT_TRUE(count < sizeof(buffer));

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    callback_recorder cr;
    jsonlite_parser_set_callback(p, &cr.cbs);

    jsonlite_result result = jsonlite_parser_tokenize(p, buffer, count);
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ(cr.records.size(), 32);

    cr.expect_finished();
    cr.records[0].expect_eq(begin_array);
    cr.records[1].expect_eq(token_null);
    cr.records[2].expect_eq(token_true);
    cr.records[3].expect_eq(token_false);
    cr.records[4].expect_eq(token_string, "string");
    cr.records[5].expect_eq(token_number, "0");
    cr.records[6].expect_eq(token_number, "1");
    cr.records[7].expect_eq(token_number, "-1");
    cr.records[8].expect_eq(token_number, "123456789");
    cr.records[9].expect_eq(token_number, "-123456789");
    cr.records[10].expect_eq(token_number, "0.5");
    cr.records[11].expect_eq(token_number, "-0.5");
    cr.records[12].expect_eq(token_number, "1.5");
    cr.records[13].expect_eq(token_number, "-1.5");
    cr.records[14].expect_eq(token_number, "1.5e5");
    cr.records[15].expect_eq(token_number, "-1.5e5");
    cr.records[16].expect_eq(token_number, "1.5E5");
    cr.records[18].expect_eq(token_number, "1.5e+5");
    cr.records[19].expect_eq(token_number, "-1.5e+5");
    cr.records[20].expect_eq(token_number, "1.5e-5");
    cr.records[21].expect_eq(token_number, "-1.5e-5");
    cr.records[22].expect_eq(token_number, "0.5e5");
    cr.records[23].expect_eq(token_number, "-0.5e5");
    cr.records[24].expect_eq(token_number, "0.5E5");
    cr.records[25].expect_eq(token_number, "-0.5E5");
    cr.records[26].expect_eq(token_number, "0.5e-5");
    cr.records[27].expect_eq(token_number, "-0.5e-5");
    cr.records[28].expect_eq(token_number, "0.5E-5");
    cr.records[29].expect_eq(token_number, "-0.5E-5");
    cr.records[30].expect_eq(end_array);
    cr.records[31].expect_eq(parse_finished);
}

TEST(parser, should_parse_object_with_different_values) {
    std::string path = TEST_DIR;
    std::string file = path + "/success/object_values.json";

    std::fstream f(file, std::ios::in | std::ios::binary);
    char buffer[4096];
    f.read(buffer, sizeof(buffer));
    auto count = static_cast<size_t>(f.gcount());
    EXPECT_TRUE(count < sizeof(buffer));

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    callback_recorder cr;
    jsonlite_parser_set_callback(p, &cr.cbs);

    jsonlite_result result = jsonlite_parser_tokenize(p, buffer, count);
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ(cr.records.size(), 63);

    cr.expect_finished();
    cr.records[0].expect_eq(begin_object);
    cr.records[1].expect_eq(token_key, "v0");
    cr.records[2].expect_eq(token_null);
    cr.records[3].expect_eq(token_key, "v1");
    cr.records[4].expect_eq(token_true);
    cr.records[5].expect_eq(token_key, "v2");
    cr.records[6].expect_eq(token_false);
    cr.records[7].expect_eq(token_key, "v3");
    cr.records[8].expect_eq(token_string, "string");
    cr.records[9].expect_eq(token_key, "v4");
    cr.records[10].expect_eq(token_number, "0");
    cr.records[11].expect_eq(token_key, "v5");
    cr.records[12].expect_eq(token_number, "1");
    cr.records[13].expect_eq(token_key, "v6");
    cr.records[14].expect_eq(token_number, "-1");
    cr.records[15].expect_eq(token_key, "v7");
    cr.records[16].expect_eq(token_number, "123456789");
    cr.records[17].expect_eq(token_key, "v8");
    cr.records[18].expect_eq(token_number, "-123456789");
    cr.records[19].expect_eq(token_key, "v9");
    cr.records[20].expect_eq(token_number, "0.5");
    cr.records[21].expect_eq(token_key, "v10");
    cr.records[22].expect_eq(token_number, "-0.5");
    cr.records[23].expect_eq(token_key, "v11");
    cr.records[24].expect_eq(token_number, "1.5");
    cr.records[25].expect_eq(token_key, "v12");
    cr.records[26].expect_eq(token_number, "-1.5");
    cr.records[27].expect_eq(token_key, "v13");
    cr.records[28].expect_eq(token_number, "1.5e5");
    cr.records[29].expect_eq(token_key, "v14");
    cr.records[30].expect_eq(token_number, "-1.5e5");
    cr.records[31].expect_eq(token_key, "v15");
    cr.records[32].expect_eq(token_number, "1.5E5");
    cr.records[33].expect_eq(token_key, "v16");
    cr.records[34].expect_eq(token_number, "-1.5E5");
    cr.records[35].expect_eq(token_key, "v17");
    cr.records[36].expect_eq(token_number, "1.5e+5");
    cr.records[37].expect_eq(token_key, "v18");
    cr.records[38].expect_eq(token_number, "-1.5e+5");
    cr.records[39].expect_eq(token_key, "v19");
    cr.records[40].expect_eq(token_number, "1.5e-5");
    cr.records[41].expect_eq(token_key, "v20");
    cr.records[42].expect_eq(token_number, "-1.5e-5");
    cr.records[43].expect_eq(token_key, "v21");
    cr.records[44].expect_eq(token_number, "0.5e5");
    cr.records[45].expect_eq(token_key, "v22");
    cr.records[46].expect_eq(token_number, "-0.5e5");
    cr.records[47].expect_eq(token_key, "v23");
    cr.records[48].expect_eq(token_number, "0.5E5");
    cr.records[49].expect_eq(token_key, "v24");
    cr.records[50].expect_eq(token_number, "-0.5E5");
    cr.records[51].expect_eq(token_key, "v25");
    cr.records[52].expect_eq(token_number, "0.5e-5");
    cr.records[53].expect_eq(token_key, "v26");
    cr.records[54].expect_eq(token_number, "-0.5e-5");
    cr.records[55].expect_eq(token_key, "v27");
    cr.records[56].expect_eq(token_number, "0.5E-5");
    cr.records[57].expect_eq(token_key, "v28");
    cr.records[58].expect_eq(token_number, "-0.5E-5");
    cr.records[59].expect_eq(token_key, "v29");
    cr.records[60].expect_eq(token_number, "-123.1234e04");
    cr.records[61].expect_eq(end_object);
    cr.records[62].expect_eq(parse_finished);
}
