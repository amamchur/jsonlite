iOS-JSON-Performance
====================

This project is a quick benchmark of JSON libraries for iOS.
I did this project because sometimes I've been questioned about which one is the best library for JSON deserialization/serialization so I decided to create a quick project to test all of them with different scenarios.

I used the following libraries:

* [SBJson](http://stig.github.com/json-framework) - *Previously known as json-framework* (latest version, 3.1)
* [JSONKit](https://github.com/johnezang/JSONKit) (latest version, 2.0)
* [NextiveJSON](https://github.com/nextive/NextiveJson) (latest version)
* [TouchJSON](https://github.com/TouchCode/TouchJSON) (latest version)
* [NSJSONSerialization](http://developer.apple.com/library/ios/#documentation/Foundation/Reference/NSJSONSerialization_Class/Reference/Reference.html) - *Public API since iOS5*


I also created a [post in my blog about it](http://www.bonto.ch/blog/2011/12/08/json-libraries-for-ios-comparison-updated/). Please feel free to add comments, suggestion and criticism to help me improving this basic project.


Latest Results
====================

![Twitter Timeline](http://bonto.ch/images/content/twitter_timeline_2_chart.png)

![100 Records with a Repeated Structure](http://bonto.ch/images/content/repeat_2_chart.png)

![1000 Records with Random Structure](http://bonto.ch/images/content/random_2_chart.png)

License
====================

The code not related to external libraries is free for any kind of use.<br/>
All the tested libraries and MBProgressHUD have their own license, please check it before you use one library in your project(s).

Credits
====================
Thanks to [johnezang](https://github.com/johnezang) for adding extra parsing methods for JSONKit.
Thanks to [stig](https://github.com/stig) for adding extra parsing methods for SBJson.