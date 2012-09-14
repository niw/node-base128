base128
=======

In many protocols or formats like JSON or early WebSocket, it requires String in UTF-8 representation, so we can not store raw binary data into String without encoding like Base64.

However, Base64 encodes binary into only 6 bits spece but we can actually use one more bit in valid UTF-8 string from U+0000 to U+007F, so using Base128 is better than Base64 (~ 16% more information we can store in the same space.)

This base128 module provides simple encode, decode interfaces between Buffer and String on Node.js which are implemented in C++.

Usage
-----

Use `npm` to grab a package then load by `require()`.

    $ npm install base128
    $ node
    > var base128 = require('base128')

Examples
--------

To Encode,

     var base128 = require('base128')
     var binary = new Buffer([0xFF, 0x01])
     var string = base128.encode(binary)
     console.log(string) #=> '\u007F\u0003\u0000' (These cahracters are invisible, though.)

To Decode,

     var base128 = require('base128')
     var string = '\u007F\u0003\u0000'
     var binary = base128.decode(string)
     console.log(binary) #=> '<Buffer ff 01>'
