#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>

#define BLOCK_LEN (2048)

uint8_t block[BLOCK_LEN];
int chunk_num[BLOCK_LEN];


void mark_chunk(const int start, const int len, const int bn)
{
	int n;
	for (n=0; n<len; n++) {
		chunk_num[start+n]=bn;
	}
}


int mark_chunk_if_empty(const int start, const int len, const int bn)
{
	int n;
	for (n=0; n<len; n++) {
		if (chunk_num[start+n]!=0) return 0;
	}
	mark_chunk(start, len, bn);
	return 1;
}

int check_and_mark_pointer(const int bn, const char *prefix)
{
	int s=block[bn+0] | ((int) block[bn+1])<<8 ;
	int l=block[bn+2] | ((int) block[bn+3])<<8 ;
	if (s==0) return 0;
	if (l<2) return 0;
	if (s>BLOCK_LEN) return 0;
	if (l>BLOCK_LEN) return 0;
	if (s+l>BLOCK_LEN) return 0;
	int res=mark_chunk_if_empty(s, l, bn);
	if (res==0) return res;
	mark_chunk(bn,4,bn);
	printf("Found pointer %03x\n", bn);
	char fn[256];
	snprintf(fn, sizeof(fn)-1, "%s_%03x.chunk", prefix, bn);
	FILE *f=fopen(fn, "w");
	int n;
	for (n=0; n<l; n++) {
		fprintf(f, "%c", block[s+n]);
	}
	fclose(f);
	return res;
}


void set_bg_color(const int c)
{
	printf("\x1b[48;5;%dm", c);
}

void set_fg_color(const int c)
{
	printf("\x1b[38;5;%dm", c);
}

void set_chunk_color(const int c)
{
	if (c==0) {
		set_bg_color(0);
		set_fg_color(0xf);
		return;
	}
	int fg=c/50*3+16;
	int bg=c%50*3+16;
	if (fg==bg) bg=bg+128;
	set_bg_color(bg);
	set_fg_color(fg);
}

int main(int argc, char *argv[])
{
	if (argc!=2) {
		printf("Usage: %s <prefix>\n", argv[0]);
		return 0;
	}
	char *prefix=argv[1];
	fread(block, sizeof(block), 1, stdin);
	memset(chunk_num, 0, sizeof(chunk_num));
	
	check_and_mark_pointer(0x10,prefix);
	check_and_mark_pointer(0x7e,prefix);
	check_and_mark_pointer(0x88,prefix);
	check_and_mark_pointer(0x8e,prefix);
	check_and_mark_pointer(0x92,prefix);
	check_and_mark_pointer(0x9a,prefix);
	check_and_mark_pointer(0x9e,prefix);
	check_and_mark_pointer(0xa2,prefix);
	check_and_mark_pointer(0xa6,prefix);


	int n;
	/*for (n=1; n<BLOCK_LEN; n++) {
		if (chunk_num[n]==0) {
			check_and_mark_pointer(n);
		}
	}
	if (argc<2) return 0;*/


	return 1;

}
