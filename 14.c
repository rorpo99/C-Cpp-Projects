#include <stdio.h>
#include <stdlib.h>
#include <string.h>

union Header {
	char buffer[10];
	struct {
		unsigned char version[3];
		unsigned char major_version;
		unsigned char revision_number;
		unsigned char flags;
		int size;
	} data;
};

union FrameHeader {
	char buffer[10];
	struct {
		unsigned char name[4];
		unsigned int size;
		unsigned short flags;
	} data;
};

unsigned int reverse_bytes(unsigned int n) {
	return (n >> 24) | ((n >> 8) & 0x0000ff00) | ((n << 8) & 0x00ff0000) | ((n << 24) & 0xff000000);
}


void show(char* file_n) {
	
	FILE* file;	
	file = fopen(file_n, "rb");
	fseek(file, 0, SEEK_SET);
	
	union Header hdr;
	fread(hdr.buffer, sizeof(char), 10, file);
	printf("%sv%d.%d\n", hdr.data.version, hdr.data.major_version, hdr.data.revision_number);
	
	unsigned int tag_size = reverse_bytes(hdr.data.size);
	while(ftell(file) < tag_size + 10) {
		union FrameHeader frame_hdr;
		fread(frame_hdr.buffer, sizeof(char), 10, file);
		printf("%s ", frame_hdr.data.name);
		if(!frame_hdr.data.name[0])
			break;
		unsigned char* info;
		unsigned int frame_size = reverse_bytes(frame_hdr.data.size);
		info = (unsigned char*)malloc(sizeof(char) * frame_size);
		fread(info, sizeof(char), frame_size, file);
		int i;
		for(i = 0; i < frame_size; i++)
			printf("%c", info[i]);
		printf("\n");	
		free(info);
	}
	fclose(file);
}

void get(char* file_n, char frame_n[4]){
	FILE* file;
	file = fopen(file_n, "rb");

	union Header hdr;
	fread(hdr.buffer, sizeof(char), 10, file);

	unsigned int tag_size = reverse_bytes(hdr.data.size);
	while (ftell(file) < tag_size + 10) {
		union FrameHeader frame_hdr;
		fread(frame_hdr.buffer, sizeof(char), 10, file);
		unsigned int frame_size = reverse_bytes(frame_hdr.data.size);
		if (strcmp((char*)frame_hdr.data.name, frame_n) == 0){
			unsigned char* info;
			info = (unsigned char*)malloc(sizeof(char) * frame_size);
			fread(info, sizeof(char), frame_size, file);
			printf("%s ", frame_n);
			unsigned int i;
			for (i = 0; i < frame_size; ++i)
				printf("%c", info[i]);
			printf("\n");
			free(info);
			fclose(file);
			return;
		}
		fseek(file, frame_size, SEEK_CUR);
	}
	fclose(file);
}

void copy_file(FILE* inp, FILE* outp) {
	int c;
	while ((c = getc(inp)) != EOF) 
		putc(c, outp);
}

void set(char* file_n, char frame_n[4], char* value) {
	FILE* file;
	file = fopen(file_n, "rb");
	
	union Header hdr;
	fread(hdr.buffer, sizeof(char), 10, file);

	unsigned int tag_size = reverse_bytes(hdr.data.size);

	unsigned int frame_start = 0;
	unsigned int frame_old_size = 0;

	while (ftell(file) < tag_size + 10){
		union FrameHeader frame_hdr;
		fread(frame_hdr.buffer, sizeof(char), 10, file);
		unsigned int frame_size = reverse_bytes(frame_hdr.data.size);
		if (strcmp((char*)frame_hdr.data.name, frame_n) == 0){
			frame_start = ftell(file) - 10;
			frame_old_size = frame_size;
			break;
		}
		if (frame_hdr.data.name[0] == 0)
			break;
		fseek(file, frame_size, SEEK_CUR);
	}

	unsigned int value_size = strlen(value);
	unsigned int new_tag_size = tag_size - frame_old_size + value_size;

	FILE* file_copy;
	file_copy = fopen("t.mp3", "wb");

	fseek(file, 0, SEEK_SET);
	fseek(file_copy, 0, SEEK_SET);
	copy_file(file, file_copy);

	fclose(file);
	fclose(file_copy);

	file_copy = fopen("t.mp3", "rb");
	file = fopen(file_n, "wb");

	hdr.data.size = reverse_bytes(new_tag_size);
	fwrite(hdr.buffer, sizeof(char), 10, file);
	
	fseek(file_copy, 10, SEEK_SET);
	unsigned int i;
	for (i = 0; i < frame_start - 10; i++){
		int c;
		c = getc(file_copy);
		putc(c, file);
	}
	
	union FrameHeader frame_hdr;
	for (i = 0; i < 4; i++)
		frame_hdr.data.name[i] = frame_n[i];
	frame_hdr.data.size = reverse_bytes(value_size);
	frame_hdr.data.flags = 0;
	fwrite(frame_hdr.buffer, sizeof(char), 10, file);

	fwrite(value, sizeof(char), value_size, file);
	
	fseek(file_copy, frame_start + 10 + frame_old_size, SEEK_SET);

	while(!feof(file_copy)) {
		unsigned short int c;
		c = getc(file_copy);
		putc(c, file);
	}
	printf("Done");
	
	copy_file(file_copy, file);
	fclose(file);
	fclose(file_copy);
	remove("t.mp3");
}



int main(int argc, char** argv) {
	char* file_n; 
	char* frame_n;
	char* value;
	int fshow = 0, fget = 0, fset = 0;
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--show") == 0)
			fshow = 1;
		if (argv[i][2] == 'f')
			file_n = strpbrk(argv[i], "=") + 1;
		if (argv[i][2] == 'g') {
			fget = 1;
			frame_n = strpbrk(argv[i], "=") + 1;
		}
		if (argv[i][2] == 's') {
			fset = 1;
			frame_n = strpbrk(argv[i], "=") + 1;
		}
		if (argv[i][2] == 'v')
			value = strpbrk(argv[i], "=") + 1;
	}
	if (fshow)
		show(file_n);
	if (fget)
		get(file_n, frame_n);
	if (fset)
		set(file_n, frame_n, value);

	return 0;
}
