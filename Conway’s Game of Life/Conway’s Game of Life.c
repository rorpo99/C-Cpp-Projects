#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//14.exe --input 14l.bmp --output C:\\Users\\Sonya\\Desktop\\14\\Game --max_iter 10 --dump_freq 1

union FileHeader {
	char buffer[14];
	struct {
		char type[2];
		unsigned int size;
		unsigned short reserved1;
		unsigned short reserved2;
		unsigned int offset;
	} BITMAPFILEHEADER;
};

union InfoHeader {
	char buffer[40];
	struct {
		unsigned int size;
		unsigned int width;
		unsigned int height;
		char other_info[28];
	} BITMAPINFOHEADER;
};

struct RGB {
	unsigned int blue;
	unsigned int green;
	unsigned int red;
};

struct RGB** pixelArray(FILE* file, int width, int height) {
	struct RGB** pixels = (struct RGB**)malloc(height * sizeof(struct RGB*));
	int i, j;
	for (i = 0; i < height; i++)
		pixels[i] = (struct RGB*)malloc(width * sizeof(struct RGB));
	for (i = height - 1; i >= 0; i--) {
		for (j = 0; j < width; j++) {
			pixels[i][j].blue = getc(file);
			pixels[i][j].green = getc(file);
			pixels[i][j].red = getc(file);
		}
	}
	return pixels;
}

void create_bmp(FILE* file, union FileHeader fheader, union InfoHeader iheader, struct RGB**
	pixels, int width, int height) {
	fwrite(&fheader.buffer, sizeof(char), 14, file);
	fwrite(&iheader.buffer, sizeof(char), 40, file);
	int i, j;
	for (i = height - 1; i >= 0; i--) {
		for (j = 0; j < width; j++) {
			putc(pixels[i][j].blue, file);
			putc(pixels[i][j].green, file);
			putc(pixels[i][j].red, file);
		}
	}
}

struct RGB** conv_to_pixels(int** in, int width, int height) {
	struct RGB** pixels = (struct RGB**)malloc(height * sizeof(struct RGB*));
	int i, j;
	for (i = 0; i < height; ++i)
		pixels[i] = (struct RGB*)malloc(width * sizeof(struct RGB));
	for (i = height - 1; i >= 0; i--) {
		for (j = 0; j < width; j++) {
			if (in[i][j] == 1) {
				pixels[i][j].blue = 0;
				pixels[i][j].green = 0;
				pixels[i][j].red = 0;
			}
			else
			{
				pixels[i][j].blue = 255;
				pixels[i][j].green = 255;
				pixels[i][j].red = 255;
			}
		}
	}
	return pixels;
}

int check_black(struct RGB pixel) {
	if (pixel.blue == 0 && pixel.green == 0 && pixel.red == 0)
		return 1;
	return 0;
}

int** read_files_pixels(struct RGB** pixels, int width, int height) {
	int** pixels_array = (int**)malloc(height * sizeof(int*));
	int i, j;
	for (i = 0; i < height; i++)
		pixels_array[i] = (int*)malloc(width * sizeof(int));
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			pixels_array[i][j] = (check_black(pixels[i][j]) == 1 ? 1 : 0);
	return pixels_array;
}

int main(int argc, char* argv[]) {

	char* file_name;
	char* output_directory;
	int i, j, k;
	int max_iter = 100, dump_freq = 1;
	
	for (i = 1; i < argc; i += 2) {
		if (strcmp(argv[i], "--input") == 0)
			file_name = argv[i + 1];
		if (strcmp(argv[i], "--output") == 0) {
			output_directory = argv[i + 1];
			mkdir(output_directory);
		}
		if (strcmp(argv[i], "--max_iter") == 0)
			sscanf(argv[i + 1], "%d", &max_iter);
		if (strcmp(argv[i], "--dump_freq") == 0)
			sscanf(argv[i + 1], "%d", &dump_freq);
	}
	
	FILE* file = fopen(file_name, "r");
	
	union FileHeader fheader;
	fread(fheader.buffer, sizeof(char), 14, file);
	
	union InfoHeader iheader;
	fread(iheader.buffer, sizeof(char), 40, file);
	
	unsigned int width = iheader.BITMAPINFOHEADER.width;
	unsigned int height = iheader.BITMAPINFOHEADER.height;
	struct RGB** pixelscolors = pixelArray(file, width, height);
	int** pixels = read_files_pixels(pixelscolors, width, height);
	int** new_pixels = read_files_pixels(pixelscolors, width, height);
	
	fclose(file);
	
	int up, left, down, right, cnt;
	
	for (k = 0; k < max_iter; k++) {
		for (i = 0; i < height; i++) {
			up = i - 1;
			down = i + 1;
			if (i == 0)
				up = height - 1;
			if (i == height - 1)
				down = 0;
			for (j = 0; j < width; j++) {
				left = j - 1;
				right = j + 1;
				if (j == 0)
					left = width - 1;
				if (j == width - 1)
					right = 0;
				cnt = pixels[up][left] + pixels[up][j] + pixels[up][right] + pixels[i][left] +
					pixels[i][right] + pixels[down][left] + pixels[down][j] + pixels[down][right];
				if (pixels[i][j] == 1) {
					if (cnt < 2 || cnt > 3)
						new_pixels[i][j] = 0;
					else
						new_pixels[i][j] = 1;
				}
				else {
					if (cnt == 3)
						new_pixels[i][j] = 1;
					else
						new_pixels[i][j] = 0;
				}
			}
		}
		for (i = 0; i < height; i++)
			for (j = 0; j < width; j++)
				pixels[i][j] = new_pixels[i][j];

		if (k % dump_freq == 0) {
			char s[50];
			char way[100];
			sprintf(s, "\\%d.bmp", k + 1);
			strcpy(way, output_directory);
			strcat(way, s);
			file = fopen(way, "w");
			create_bmp(file, fheader, iheader, conv_to_pixels(pixels, width, height), width, height);
			fclose(file);
		}
	}
	return 0;
}
