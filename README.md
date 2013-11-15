### Introduction to jsonlite

* [What is jsonlite and JsonLite ObjC?](#what-is-jsonlite-and-jsonlite-objc)
* [Design Overview](#design-principles)
* [Examples](https://github.com/amamchur/jsonlite/wiki/Examples)
* [Code Coverage](#code-coverage)
* [License](#license)

#### What is jsonlite and JsonLite ObjC

jsonlite is JSON [tokenizer](http://en.wikipedia.org/wiki/Tokenization). It's lightweight C library that can be used for low-level JSON processing or parser development.

JsonLite ObjC is JSON parser base on jsonlite. It's the [fastest](https://github.com/amamchur/iJSONBenchmark) and flexible JSON parser for Objective-C.

#### Design Principles

_jsonlite_ designed as goto driven finite state machine. This approach provides a lot of benefits such as:

* Very very fast parsing
* Low memory footprint
* Streaming parsong
* Depth checking
* Size of JSON payload doesn't influence memory usage
* Recursion free
* Good testability

##### Lightweight

jsonlite is super lite parser. It's perfect works on micro-controllers with 2k RAM.

Memory usage for parsing depth 32:
* *64-bit platform* - 216 bytes
* *32-bit platform* - 124 bytes

##### Divide and Rule (divide et impera)

It's main principle of _jsonlite_ design. _jsonlite_ doesn't perform any decoding by itself, but provides reach API for token processing instead of that.
Let's look at the following example:
```
{
  "string":"Some\u0020string"
}
```

In this case we have string with escaped UNICODE character and _jsonlite_ will provide all information about this token.
```
token.start == 12; // Start of "Some\u0020string"
token.end == 28; // End of "Some\u0020string"
token.type.string == jsonlite_string_ascii | jsonlite_string_unicode_escape; // Token attributes
```

And now is your turn, you may:
* Decode token (jsonlite_token_to_uft8 or jsonlite_token_size_of_uft16) 
* Pass as raw value to jsonlite_builder (see [Beautifier example](https://github.com/amamchur/jsonlite/blob/master/Examples/Beautifier/Beautifier/main.c)) 
* Terminate parsing using jsonlite_parser_terminate
* Or something else

**jsonlite - divides; you - rule.**

#### Code Coverage

* 100% Function coverage
* 100% Statement coverage
* 100% Branch coverage. 

See [Code coverage configuration](https://github.com/amamchur/jsonlite/wiki/Code-coverage-configuration)

![Image](../master/tests/cov.png?raw=true)

#### License

jsonlite and JsonLite ObjC are licensed under the [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0)

Copyright 2012-2013, Andrii Mamchur
