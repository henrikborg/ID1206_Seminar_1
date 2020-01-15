//#define ASCII
#define COLUMS 80
#define ROWS 25

char *name = "My OS is running...";
char *character_array = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWZ";
char character_array_length = 35;

typedef struct vga_char {
	char character;
	char colors;
} vga_char;

int rand(int seed);

void main(void) {
	// The VGA terminal mapped memory
	vga_char *vga = (vga_char*)0xb8000;
	int byte = 0;
	unsigned int seed = (int)((vga_char*)(vga + 600));
	char repeat_characters_per_colum[COLUMS];
	char start_character[COLUMS];	/* magic */

	for(int i = 0; i < COLUMS * ROWS; i++) {
		vga[i].character = ' ';
		vga[i].colors = 0x0a;
	}

	for(int col = 0; col < COLUMS; col++) {
		start_character[col] = ' ';
		repeat_characters_per_colum[col] = 0;
	}

	while(1) {
		/* Move charcters down - one row at a time */
		for(int row = ROWS - 1; row > 0; row --) {
			for(int col = 0; col < COLUMS; col++) {
				vga[row * 80 + col].character = vga[(row - 1) * 80 + col].character;
			}
		}

		/* Generate new characters on top row */
		for(int col = 0; col < COLUMS; col++) {
			if(start_character[col] == ' ') {
				/* Let space/blank repeat itself */
			} else {
				/* generate new character for this colum */
				seed = rand(seed);
#ifdef ASCII
				byte = seed % 0x5b; /* 0x5b for upper case letters, 0x7B for upper and lower case */
				if(byte < 0x30)
					vga[col].character = ' ';
				else
					vga[col].character = byte;
#else
				byte = seed % character_array_length;  /* Cannot include string.h */
        //vga[col].colors = 0x92;
				vga[col].character = character_array[byte];
#endif
			}

			repeat_characters_per_colum[col]--;

			/* Special thing for generating pattern */
			if(repeat_characters_per_colum[col] <= 0) {
				/* generate new character for this colum */
				int times;
				int blank_or_char;
				seed = rand(seed);
#ifdef ASCII
				byte = seed % 0x5b; /* 0x5b for upper case letters, 0x7B for upper and lower case */
#else
				byte = seed % character_array_length; /* Cannot include string.h */
#endif
				times = (seed >> 8) % 9; /* Times of characters repeat is not related to the character */

				repeat_characters_per_colum[col] = times;

				blank_or_char = (seed >> 16) % 10;
				if(blank_or_char < 8) {
					vga[col].character   = ' ';
					start_character[col] = ' ';
				} else {
#ifdef ASCII
					vga[col].character   = byte;
					start_character[col] = byte;
#else
          //vga[col].colors      = 0x32;
					vga[col].character   = character_array[byte];
					start_character[col] = character_array[byte];
#endif
				}

			}
		}

		for(int i = 0; i < 700000; i++)
			asm("");
	}

	return;
}

/***********************************************************
For the code:
Source: https://en.wikipedia.org/wiki/Linear_congruential_generator#Sample_Python_code
def lcg(modulus, a, c, seed):
  while True:
    seed = (a * seed + c) % modulus
    yield seed

For the numbers used:
Source: https://en.wikipedia.org/wiki/Linear_congruential_generator#Parameters_in_common_use
Row regarding glibc (used by GCC)[10]
***********************************************************/
int rand(int seed) {
	int modulus = 0x80000000;
	int a = 1103515245;
	int c = 12345;

	seed = (a * seed + c) % modulus;

	return seed;
}
