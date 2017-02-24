/*
 * This is a c++ source code of a encode/decode command line tool.
 * It uses it's own prime number-based cipher.
 *
 * author: Miloslav Ciz
 * year: 2012
 * version: 1.0
 */

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <limits>

using namespace std;

enum parameter_state
  {
    STATE_OK,
    STATE_WRONG_KEY,
    STATE_WRONG_ALPHABET,
    STATE_WRONG_NUMBER,
    STATE_WRONG_OTHER
  };

int eof_reached;                 // whether or not eof was reached
const long prime_numbers[] =
  {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61,
   67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137,
   139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211,
   223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283,
   293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379,
   383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461,
   463, 467, 479, 487, 491, 499, 503};

//----------------------------------------------------------------------

int word_is_ok(string word, string alphabet)

/*
 * Checks if the word is OK (contains only letters in given alphabet).
 */

 {
   int i;
   int j;
   int ok;

   for (i = 0; i < word.length(); i++)
     {
       ok = 0;

       for (j = 0; j < alphabet.length(); j++)
         if (word[i] == alphabet[j])
           {
             ok = 1;
             break;
           }

       if (!ok)
         return 0;
     }

   return 1;
 }

//----------------------------------------------------------------------

void print_help()

/*
 *  prints help message to standard output
 */

  {
    cout << "tf_code:" << endl << endl;
    cout << "This is a encode/decode command line tool which uses" << endl;
    cout << "it's own prime number-based cipher. It should be" << endl;
    cout << "used as follows:" << endl << endl;
    cout << "tf_code ([-e|-d] \"alphabet\" \"key\") | (-h)" << endl << endl;
    cout << "-e            encode mode (default)" << endl;
    cout << "-d            decode mode" << endl;
    cout << "-h            print this help" << endl;
    cout << "\"alphabet\"    characters used in the message" << endl;
    cout << "\"key\"         number key of the cipher" << endl << endl;
    cout << "Input text will be read from stdin and output will be" << endl;
    cout << "printed to stdout. Program returns either 0" << endl;
    cout << "(everything was OK) or 1 (something went wrong)." << endl << endl;
    cout << "The cipher divides the source text by fixed number of" << endl;
    cout << "characters, each of which is then assigned a prime" << endl;
    cout << "number. These are then multiplied to make set of" << endl;
    cout << "letters represented by one number. The order of" << endl;
    cout << "letters is represented by another number which is" << endl;
    cout << "a number of permutation in special permutation ordering" << endl;
    cout << "system. The key rotates the alphabet and is added to" << endl;
    cout << "every number representing letter set." << endl << endl;
    cout << "author: Miloslav Ciz, year: 2012, version: 1.0" << endl;
  }

//----------------------------------------------------------------------

string rotate_string(string what, int steps)

/*
 * Makes right circle rotation of the string by given number of steps.
 */

  {
    int position;
    int i;
    string result;

    if (steps < 0)
      return NULL;

    steps = steps % what.length();
    result = "";
    position = what.length() - steps;

    for (i = 0; i < what.length(); i++)
      {
        if (position >= what.length())
          position = 0;

        result += what[position];

        position++;
      }

    return result;
  }

//----------------------------------------------------------------------

unsigned long factorial(int number)

/*
 * Computes the number's factorial.
 */

  {
    unsigned long result = 1;

    while (number > 0)
      {
        result = result * number;
        number--;
      }

    return result;
  }

//----------------------------------------------------------------------

unsigned long permutation_to_number(int* permutation, unsigned int length)

/*
 * Converts given permutation to it's sequence number. The numbers in
 * the permutation must begin with 1, for instance {2,3,1}.
 */

  {
    unsigned long result = 0;
    int i;
    int j;
    int help_number;

    for (i = 0; i < length; i++)
      {
        help_number = permutation[i] - 1;

        for (j = 0; j < i; j++)
          if (permutation[j] < permutation[i])
            help_number--;

        result += help_number * factorial(length - i - 1);
      }

    return result;
  }

//----------------------------------------------------------------------

int is_number(string what)

/*
 * Checks if the string contains only digits.
 */

  {
    int i;

    for (i = 0; i < what.length(); i++)
      if (!isdigit(what[i]))
        return 0;

    return 1;
  }

//----------------------------------------------------------------------

void number_to_permutation(unsigned long number, int *destination, unsigned int length)

/*
 * Takes a sequence number and makes a permutation asociated to that
 * number as an array of numbers beginning with 1, for instance
 * {4,2,1,3}. The length must be less or equal to 20.
 */

  {
    unsigned long help_number;
    int fact;
    int i;
    int j;
    int help_set[20] =
      {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

    if (length > 20)
      return;

    help_number = number;

    for (i = 0; i < length; i++)
      {
        fact = factorial(length - i - 1);
        destination[i] = help_number / fact + 1;
        help_number = help_number % fact;
      }

    for (i = 0; i < length; i++)
      for (j = 0; j < 20; j++)
        {
          if (help_set[j] < 0)
            continue;

          destination[i]--;

          if (destination[i] == 0)
            {
              destination[i] = help_set[j];
              help_set[j] = -1;
              break;
            }
        }
  }

//----------------------------------------------------------------------

unsigned long letter_to_prime(char letter, string alphabet)

/*
 * Converts given character to a prime number acoording to given
 * alphabet.
 */

  {
    int i;
    unsigned long result = 0;

    for (i = 0; i < alphabet.length(); i++)
      if (letter == alphabet[i])
        {
          result = prime_numbers[i];
          break;
        }

    return result;
  }

//----------------------------------------------------------------------

int will_overflow(unsigned long number1, unsigned long number2)

/*
 * Checks if multiplication of two long numbers will overflow. Returns
 * 1 if the overflow would occur, otherwise 0.
 */

  {
    unsigned long long helpnumber;

    helpnumber = (unsigned long long) number1 * (unsigned long long) number2;

    return helpnumber >= 0x100000000ULL;
  }

//----------------------------------------------------------------------

unsigned long letters_to_number(string word, string alphabet)

/*
 * Converts given word to number representing the set of letters in the
 * word. If the word cannot be converted because the number would be
 * too large, the function returns 0;
 */

  {
    unsigned long result = 1;
    unsigned long prime;
    int i;

    for (i = 0; i < word.length(); i++)
      {
        prime = letter_to_prime(word[i],alphabet);

        if (will_overflow(result, prime))
          return 0;

        result = result * prime;
      }

    return result;
  }

//----------------------------------------------------------------------

char prime_to_letter(int prime_number, string alphabet)

/*
 * Converts given prime_number to a character asociated with it
 * acoording to given alphabet.
 */

  {
    int i;

    for (i = 0; i < 60; i++)   // check only the first 60 prime numbers
      if (prime_numbers[i] == prime_number)
        break;

    return i < alphabet.length() ? alphabet[i] : alphabet[0];
  }

//----------------------------------------------------------------------

void letters_to_permutation(string letters, int* destination, string alphabet)

/*
 * Computes the permutation of given word acoording to given alphabet.
 * The permutation is an array of numbers beginning with 1, for instance
 * {3,1,2,4,5}.
 */

  {
    int help_array[letters.length()];
    int i;
    int j;
    int temp;

    for (i = 0; i < letters.length(); i++)   // prepare things
      {
        destination[i] = i + 1;
        help_array[i] = letter_to_prime(letters[i],alphabet);
      }

    for (i = 0; i < letters.length(); i++)   // bubble sort the destination array by values of help array
      for (j = 0; j < i; j++)
        if (help_array[j] > help_array[i])
          {
            temp = help_array[j];
            help_array[j] = help_array[i];
            help_array[i] = temp;

            temp = destination[j];
            destination[j] = destination[i];
            destination[i] = temp;
          }
  }

//----------------------------------------------------------------------

string permutation_to_letters(int *permutation, string letters)

/*
 * Orders characters in string acoording to given permutation.
 */

  {
    string result;
    int i;
    result = letters;
    int index;

    for (i = 0; i < letters.length(); i++)
      {
        index = permutation[i] - 1;

        if (index >= 0 && index < result.length())
          result[index] = letters[i];
      }

    return result;
  }

//----------------------------------------------------------------------

string number_to_letters(unsigned long number, string alphabet)

/*
 * Converts given number to set of letters represented by string of
 * characters ordered acoording to given alphabet.
 */

 {
   int help_array[20];
   int length;                // length of help_array
   int i;
   int next_prime;
   string result;

   length = 0;
   result = "";

   for (i = 0; i < 50; i++)   // we check only first 50 prime numbers
     {
       if (number == 1)
         break;

       next_prime = 0;

       while (!next_prime)
         {
            if (number % prime_numbers[i] == 0)  // the prime is contained
              {
                help_array[length] = prime_numbers[i];
                length++;
                number = number / prime_numbers[i];
              }
            else
              {
                next_prime = 1;
              }
         }
     }

   for (i = 0; i < length; i++)
     result += prime_to_letter(help_array[i],alphabet);

   return result;
 }

//----------------------------------------------------------------------

string get_next_word()      // this function is currently not being used

/*
 * Gets the next word from stdin. When EOF is reached, it sets the
 * global variable. May return "" if eof was reached and nothing was
 * read. Words are considered characters separated by spaces or EOLs.
 */

  {
    string result = "";
    char character = 'a';
    int letter_reached = 0;   // to skip spaces at the beginning

    if (eof_reached)
      return "";

    while (1)
      {
        if ((character = getchar()) != EOF)
          {
            if (character == ' ' || character == '\n')
              {
                if (letter_reached)
                  break;
              }
            else
              {
                result += character;
                letter_reached = 1;
              }
          }
        else
          {
            eof_reached = 1;

            if (letter_reached)
              return result;
            else
              return "";
          }
      }

    return result;
  }

//----------------------------------------------------------------------

string get_next_n_letters(int n)

/*
 * Reads a maximum of n next characters from stdin. If eof is reached,
 * the global flag is set. EOLs will be converted to spaces.
 */

  {
    int i;
    string result;
    char character;

    if (eof_reached)
      return "";

    result = "";

    for (i = 0; i < n; i++)
      {
        if ((character = getchar()) == EOF)
          {
            eof_reached = 1;
            break;
          }
        else
          if (character == '\n')
            result += ' ';
          else
            result += character;
      }

    return result;
  }

//----------------------------------------------------------------------

long long get_next_number()

/*
 * Returns the next number read from stdin. If EOF is reached, the
 * global flag is set. Sometimes a negative value may be returned
 * which should be ignored.
 */

 {
   string word_read = "";
   char character = 'a';
   int digit_reached = 0;  // to skip non-digit characters at the beginning

   if (eof_reached)
     return -1;

   while (1)
      {
        if ((character = getchar()) != EOF)
          {
            if (!isdigit(character))
              {
                if (digit_reached)
                  break;
              }
            else
              {
                word_read += character;
                digit_reached = 1;
              }
          }
        else
          {
            eof_reached = 1;
            break;
          }
      }

    if (word_read.length() == 0)
      return -1;

    return strtoll(word_read.c_str(),NULL,10);
 }

//----------------------------------------------------------------------

int main(int argc, char** argv)

/*
 *  Main part of the program.
 */

  {
    string alphabet;        // character used in the message
    int mode;               // 1 = encode, 0 = decode
    int key;
    parameter_state state;
    string word_read;
    long long number_read;
    long long permutation_number;
    int permutation[20];
    unsigned long help_number;
    int first;              // to leave out a space after last word

    eof_reached = 0;
    mode = 1;
    key = 0;
    state = STATE_OK;
    first = 1;

    if (argc < 2 || argc > 4)
      state = STATE_WRONG_NUMBER;
                                                  // checking the parameters
    if (argc == 2 && strcmp(argv[1],"-h") == 0)   // -h
      {
        print_help();
        return 0;
      }

    if (argc == 3)
      {
        alphabet = argv[1];

        if (!is_number(argv[2]))
          state = STATE_WRONG_KEY;
        else
          key = strtol(argv[2],NULL,10);
      }

    if (argc == 4)
      {
        alphabet = argv[2];

        if (strcmp(argv[1],"-d") == 0)
          mode = false;
        else if (strcmp(argv[1],"-e") != 0)
          state = STATE_WRONG_OTHER;

        if (!is_number(argv[3]))
          state = STATE_WRONG_KEY;
        else
          key = strtol(argv[3],NULL,10);
      }

    switch (state)
      {
        case STATE_WRONG_ALPHABET:
          cerr << "error: the alphabet is too long" << endl;
          return 1;
          break;

        case STATE_WRONG_KEY:
          cerr << "error: the key is wrong" << endl;
          return 1;
          break;

        case STATE_WRONG_NUMBER:
          cerr << "error: wrong number of parameters" << endl;
          return 1;
          break;

        case STATE_WRONG_OTHER:
          cerr << "error: wrong argument format" << endl;
          return 1;
          break;
      }

    if (alphabet.length() > 40 || alphabet.length() == 0)
      {
        cerr << "error: the alphabet must be between 0 to 40 characters long" << endl;

        return 1;
      }

    alphabet = rotate_string(alphabet,key);

    if (mode) // encoding
      {
        while (!eof_reached)
          {
            word_read = get_next_n_letters(4);

            if (word_read.length() == 0)
              break;

            if (!word_is_ok(word_read,alphabet))
              {
                cerr << "error: the text contains characters that are not in the alphabet" << endl;
                return 1;
              }

            help_number = letters_to_number(word_read,alphabet);

            if (help_number == 0)
              {
                cerr << "error: the word \"" << word_read << "\" could not be converted ";
                cerr << "because it's number was too large, make shorter words or change ";
                cerr << "letters in it" << endl;
                return 1;
              }
            else
              {
                if (!first)
                  cout << " ";
                else
                  first = 0;

                cout << help_number + key << "|";
                letters_to_permutation(word_read,permutation,alphabet);
                cout << permutation_to_number(permutation,word_read.length());
              }
          }

        cout << endl;
      }
    else      // decoding
      {
        while (!eof_reached)
          {
            if ((number_read = get_next_number()) < 0)
              continue;

            if ((permutation_number = get_next_number()) < 0)
              continue;

            if (number_read - key <= 0)
              {
                cerr << "error: the message cannot be decoded with this key" << endl;
                return 1;
              }
            else
              {
                word_read = number_to_letters(number_read - key,alphabet);
                number_to_permutation(permutation_number,permutation,word_read.length());
                word_read = permutation_to_letters(permutation,word_read);
                cout << word_read;
              }
          }

        cout << endl;
      }

    return 0;
  }
