jsonlite
========
Copyright © 2013, Andrii Mamchur.

Overview
--------
jsonlite - very high performance JSON tokenizer written in pure C.

Most important features:

strict RFC 4627
low memory usage
ability to process raw token data
streaming support
JSON building + pretty-printing
token pool
parse depth configuration
no external dependencies
no recursion
100% code coverage
Appropriate Uses
JSON validation

jsonlite is the best solution for JSON validation because of high performance, steaming processing and low memory usage. Of course there is no reason to collect data for validation - just use jsonlite with default callback.

JSON beautifying (see Beautifier)

Simple combination of jsonlite_parser + jsonlite_builder makes JSON beautifying without token transformation.

Transformation example: "tab char": "\u0009" -> "tab char": "\t" or use popular JSON validator http://jsonlint.com

Input:

{
    "tab char": "\u0009"
}
Output:

{
    "tab char": "\t"
}
Huge JSON payload processing

jsonlite was designed to work with huge JSON payload. Internal memory usage depends on maximal JSON depth only (See "jsonlite memory usage"). It does not collect data by itself, it just delegate this responsibility to high-level components.

jsonlite memory usage
Memory Usage = (1 x Depth + 25) x Word Size / 8 or use jsonlite_parser_estimate_size(depth)

Variable Name  Description
Memory Usage	 Memory used by jsonlite without JSON payload
Depth	 Parsing depth
Word Size	 Machine word size in bits
For example:

Parsing Twitter time line
Depth 16 - enough to process payload
Compiled for x86 architecture (32bit)
(1 x 16 + 25) x 32bit / 8 = 164 bytes

JsonLite Objective-C
====================
JsonLite Objective-C - power extension of jsonlite for Objective-C/Cocoa.

Most important features:

very fast JSON parsing (candidate for the fastest Objective-C parser, needed independent benchmarks)
streaming/chunk parsing
parse JSON to Cocoa collections and objects
serialize object to JSON
deserialize JSON to custom models
support NSDecimalNumber, NSDate and NSURL
provide ability to support any type of Objective-C
objects' pool
Situations where JsonLite Objective-C may not be suitable
NSJSONSerialization is enough for you.
Project use Automatic Reference Counting
Appropriate Uses
Objective-C models serialization/deserialization (bindings)

Currency in JSON

JsonLite Objective-C supports NSDecimalNumberv
