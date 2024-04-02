
# Remote GUI window protocol

## General rules

Peers comunicating with messages. Message consists with message type, then may be message-subtype and then message data. Message data can consists with integers, strings or binary arrays.

Messages are split into two types - requests and responses/payloads. Requests usually use lowercase message type and responses/payloads usually use uppercase message type.

## Tokens

Messages are documented using *notation*. Notation is a sequence of *tokens*. When message is documented tokens can be separated with spaces, but it is only for readability, actual sent characters are only tokens.

### Message type
Message type is a one ascii letter `[A-Za-z]`

**Example:** \
`k` - is a message of type `k`

**Notation:** `<Type>`

### Message subtype
Some messages may require subtype. Message subtype is a one ascii letter or one ascii digit `[A-Za-z0-9]`

**Example:** \
`b1` - is a message of type `b` and subtype `1`

**Notation:** `<SubType>`

### Integer
Integer can be any positive or negative number from `-999999999` to `+999999999`. Integer is passed as a ascii string (not binary) with length of 10. first character is always sigh: `+` or `-`. Then number is zero extended to width of 9 characters. Number 0 can have either `+` or `-` sign.

**Example:** \
`+000000001` - is encoded number `1`
`-000000123` - is encoded number `-123`
`+000000000` - is encoded number `0`

**Notation:**: `<Number>`

### String
String is an array of ascii letters of any lenth (length should be specified using any other way). String must not end with zero byte at the end of transmission.

**Example:** \
`qwerty` - is a string with length of 5

**Notation:** `<String>`

### Binary array
Binary array is an array of bytes of any length (length should be specified using any other way).

**Example:** \
`123\0456\0` - byte array with 2 zero bytes

**Notation:** `<BinaryArray>`

## Message types

### Resolution payload

**Notation:** `<Type: R> <Number: width> <Number: height>`

 - *width* - width of the creating window
 - *height* - height of the creating window

### Keyboard event payload

**Notation:** `<Type> <Number: keycode>`

- *Type* is `K` if key is pressed and is `k` if key is released

Event is sent if keyboard or mouse button is pressed or released.

Keyborad and mouse buttons are not distinguished, they just have different keycodes: mouse buttons are from 0 to 7 and other are keyboard buttons.

### Mouse position request

**Notation:** `<Type: m>`

After mouse position requst client will send **Mouse position response**

### Mouse position response

**Notation:** `<Type: M> <Number: x> <Number: y>`

 - *x* - horizontal position of mouse
 - *y* - vertical position of mouse

### Pickle screen content payload

**Notation:** `<Type: P> <Number: length> <BinaryArray: payload>`

 - *length* - length of the *payload* in bytes
 - *payload* - encoded screen contents

Scren contents is a python numpy array with shape (window_height, window_width, 3) serialized using `pickle.dumps()`. Last dimention holds red, green and blue colors of the pixel. At index [0] is color red, at index [1] - green, and at index [2] - blue.

### Binary screen content payload

**Notation:**: `<Type: B> <SubType: compression> <Number: length> <BinaryArray: payload>`

 - *compression* a digit that describes the amount of compression.
 - *length* - length of the *payload* in bytes
 - *payload* - encoded screen contents

**Compression 0**
when *compression* is 0 - *payload* will have following structure: *length* will be equal to the `window_width * window_height * 3`. Pixels will be sent in rows starting from
top row and ending with bottom row. Each row will consist of pixels starting from most left pixel in the row and ending with most right pixel in the row. Each pixel consist of 3 bytes. First byte is red value, second is a blue value and third - is a green value. Each value is an one byte unsigned number from 0 to 255.
