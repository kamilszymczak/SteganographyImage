#include <stdio.h>
#include <stdlib.h>
#include <math.h>    // ceil()
#include <string.h>

#define MAXJUMP 20

struct Pixel{
  int red, green, blue;
};

struct PPM{
  char p;
  int pVal;
  int width;
  int height;
  int color;
  char (*comments)[255];
  struct Pixel **pixels;
};

struct PPM getPPM(FILE * fin){

  struct PPM image;

  //Get PPM info
  fscanf (fin, "%c %d", &image.p, &image.pVal);

  //Check if file is P3
  if(image.p != 'P' && image.pVal != 3){
    fprintf(stderr, "PPM image is not P3\n");
    exit(0);
  }


  int mallocCounter = 1;
  image.comments = malloc(mallocCounter * sizeof(image.comments[0]));

  char buf[255];
  int count = 0;

  fgets(buf, 255, fin);
  while(buf[0] == '#' || buf[0] == '\n'){
    if(buf[0] == '\n'){
      fgets(buf, 255, fin);
      continue;
    }

    mallocCounter += 2;
    image.comments = realloc(image.comments, mallocCounter * sizeof(image.comments[0]));
    strcpy(image.comments[count++], buf);
    fgets(buf, 255, fin);
  }

  //Put pointer back since last while fgets holds pixel data not a comment
  fseek(fin, -(strlen(buf)), SEEK_CUR);
  //Comments are Now Gone


  fscanf (fin, "%d %d %d", &image.width, &image.height, &image.color);

  //Allocate memory for array of size height [this]
  image.pixels = malloc(image.height * sizeof(image.pixels));

  //Allocate memory for 2d array of size wdith [][this]
  for(int i = 0; i < image.height; i++){
      image.pixels[i] = malloc(image.width * sizeof(image.pixels) * 3);
  }


  for(int h = 0; h < image.height; h++){
    for(int w = 0; w < image.width; w++){
      fscanf(fin, "%d %d %d", &image.pixels[h][w].red, &image.pixels[h][w].green, &image.pixels[h][w].blue);
    }
  }

  return image;
}


void showPPM(struct PPM * im){

  printf("%c%d\n", im->p, im->pVal);


  int z = 0;
  while(im->comments[z][0] == '#'){
    printf("%s\n", im->comments[z]);
    z++;
  }


  printf("%d %d\n", im->width, im->height);
  printf("%d\n", im->color);

  for(int h = 0; h < im->height; h++){
    for(int w = 0; w < im->width; w++){
      printf("%d %d %d\n", im->pixels[h][w].red, im->pixels[h][w].green, im->pixels[h][w].blue);
    }
  }

} //End of showPPM


struct PPM * encode(struct PPM * im, char * message, unsigned int mSize, unsigned int secret){

  //Method to return array binary from char
  //Inpsired by - https://stackoverflow.com/questions/5666900/converting-c-string-to-binary-representation
  int* charToBinary(char * word, int wSize){
    int counter = 0;
    int *chars = malloc(wSize* 8 * sizeof(int));

    for(; *word != 0; ++word){
      for(int i = 7; i >= 0; i--){
        chars[counter] = (*word >> i) & 1;
        counter++;
      }
    }
    return chars;
  }

  //Create array of binary of all letters in message
  int *chars = charToBinary(message, mSize);

  srand(secret);

  //to get pos x and pos y if img width is 4 example below
  //5 = [1][1] - 5/4 = 1 r 1
  //8 = [2][0] = 8/4 = 2 r 0
  //11 = [2][3] = 11/4 = 2 r 3

  int curr = 0;
  int charCounter = 0;


  //If statement to round to the nearset next int
  //e.g. 2.3 would require 3
  int loopVal = (mSize*8) / 3;
  if((mSize*8) % 3 != 0){
    loopVal = floor(1+(mSize*8)/3);
  }



  //for number of letters * 8(8 bits) / 3 because 3 colors
  for(int i = 0; i < loopVal; i++){

    int jump = rand() % MAXJUMP;

    curr = curr + jump;
    int x = curr/im->width;
    int y = curr - (x*im->width);

    //If char is 1 then make char 1
    //else char is 0 then make char 0
    if(chars[charCounter] == 1){
      im->pixels[x][y].red |= 1;
    }else {
      im->pixels[x][y].red &= ~1;
    }
    charCounter++;

    if(chars[charCounter] == 1){
      im->pixels[x][y].green |= 1;
    }else {
      im->pixels[x][y].green &= ~1;
    }
    charCounter++;

    if(chars[charCounter] == 1){
      im->pixels[x][y].blue |= 1;
    }else {
      im->pixels[x][y].blue &= ~1;
    }
    charCounter++;

  } //End of for loop

}


char * decode(struct PPM * im, unsigned int secret){

  //Function to append char to char
  //Inspiration from - https://stackoverflow.com/questions/19891962/c-how-to-append-a-char-to-char
  char* append(char* array, char a){
    size_t len = strlen(array);
    char* ret = malloc(len+2);

    strcpy(ret, array);
    ret[len] = a;
    ret[len+1] = '\0';

    return ret;
  }

  int BinaryToInt(const char *s){
    return (int) strtol(s, NULL, 2);
  }


  srand(secret);

  //Count number of digits inside secret
  int msgLength = 0;
  while(secret != 0){
    secret /= 10;
    msgLength++;
  }


  //If statement to fix how much to loop for
  //e.g. 2.3 would require 3, round to the nearest next int
  int loopVal = (msgLength*8) / 3;
  if((msgLength*8) % 3 != 0){
    loopVal = floor(1+(msgLength*8)/3);
  }


  int charBinary[loopVal*3];
  int curr = 0;
  int counter = 0;

  for(int i = 0; i < loopVal; i++){
    int jump = rand() % MAXJUMP;
    //printf("Jump- %d\n", jump);

    curr = curr + jump;
    int x = curr/im->width;
    int y = curr - (x*im->width);
    //printf("X and Y - %d r- %d\n", x, y);

    charBinary[counter++] = (im->pixels[x][y].red >> 0) & 1;
    charBinary[counter++] = (im->pixels[x][y].green >> 0) & 1;
    charBinary[counter++] = (im->pixels[x][y].blue >> 0) & 1;
  }


  //size 8 because 8 bits plus \0
  //used to hold 8 bit which is ascii code for character
  char tempOneLetter[9];
  char* decodedWord = malloc(sizeof(char) * msgLength);
  counter = 0;

  for(int i = 0; i < msgLength; i++){

    for(int j = 0; j < 8; j++){
      tempOneLetter[j] = '0' + charBinary[counter];
      counter++;
    }

    decodedWord = append(decodedWord, BinaryToInt(tempOneLetter));
  }

  return decodedWord;
}

int main(int argc, char ** argv) {

  FILE * fin;
  FILE * fout;
  char* mode = argv[1];
  fin = fopen(argv[2], "r");

  if(!fin){
    fprintf(stderr, "can't open file %s\n", argv[2]);
    exit(0);
  }


  if(strcmp(mode, "e") == 0){

    if(argc != 3){
      fprintf(stderr, "%s\n", "Some arguments missing!");
      exit(0);
    }

    struct PPM image;
    image = getPPM(fin);

    int maxWords = -1;
    int totalBits = image.width * image.height * 3;

    do{
      maxWords++;
      totalBits = totalBits - (MAXJUMP*3);
      totalBits = totalBits - 8;
    } while (totalBits > 0);


    fprintf(stderr, "In the worst case you can hide maximum of %d words in this image, \n", maxWords);

    char msg[99];
    unsigned int secret;

    fprintf(stderr, "Please enter the message you would like to encode \n");
    scanf("%s", msg);

    fprintf(stderr, "Please enter the secret (secret must be %ld digits long) \n", strlen(msg));
    scanf("%u", &secret);

    //Count number of digits inside secret
    int secretLength = 0;
    unsigned int secretTemp = secret;
    while(secretTemp != 0){
      secretTemp /= 10;
      secretLength++;
    }


    if(secretLength != strlen(msg)){ fprintf(stderr, "Secret is not the same length as message! \n"); exit(0);}

    struct PPM *encoded; //Pointer bariable declaration
    encoded = &image; //store address of image in encoded (pointer variable)

    encoded = encode(&image, msg, strlen(msg), secret);

    showPPM(&image);

  } else if(strcmp(mode, "d") == 0){

    struct PPM image;
    image = getPPM(fin);

    unsigned int secret;

    fprintf(stderr, "Please enter the secret\n");
    scanf("%u", &secret);

    printf("The message is - %s\n", decode(&image, secret));

  } else{
    fprintf(stderr, "That mode does not exist\n");
  }


}
