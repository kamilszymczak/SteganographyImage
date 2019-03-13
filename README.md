## Steganography System in C
Program to encode a text message into an image in PPM format by entering a secret key and decode the message from the image using the key specified at encoding stage or a wrong message will be decoded.
Because of the way the program was designed the maximum length of message can be 10 characters.

## Installation
Compile
```shell
$ gcc -o steg steg.c
```

## Usage
Run in encode mode
```shell
$ ./steg e <file_name.ppm> > <image_with_msg.ppm>
```

Run in decode mode
```shell
$ ./steg d <image_with_msg.ppm>
```
