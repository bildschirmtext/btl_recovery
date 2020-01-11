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

int check_and_mark_pointer(const int bn, const int known)
{
	if (known!=0) mark_chunk(bn,4,bn); //Mark known addresses even if they aren't used here
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
	int blocknumber=0;
	while (fread(block, sizeof(block), 1, stdin)>0) {
		memset(chunk_num, 0, sizeof(chunk_num));
		
		printf("Block: %d\n", blocknumber);

		check_and_mark_pointer(0x10, 1);
		check_and_mark_pointer(0x7e, 1);
		check_and_mark_pointer(0x88, 1);
		check_and_mark_pointer(0x8e, 1);
		check_and_mark_pointer(0x92, 1);
		check_and_mark_pointer(0x9a, 1);
		check_and_mark_pointer(0x9e, 1);
		check_and_mark_pointer(0xa2, 1);
		check_and_mark_pointer(0xa6, 1);


		int n;
		for (n=1; n<BLOCK_LEN; n++) {
			if (chunk_num[n]==0) {
				check_and_mark_pointer(n, 0);
			}
		}
		if (argc<2) return 0;

		set_bg_color(0);
		set_fg_color(7);
		
		printf("    ");
		for (n=0; n<32; n++) {
			printf("%02x ", n);
			if (n==15) printf(" ");
		}
		printf(" ");
		for (n=0; n<32; n++) {
			printf("%01x", n%16);
		}
		printf("\n");
		set_fg_color(0xf);
		for (n=0; n<BLOCK_LEN/32; n++) {
			printf("%03x ", n*32);
			int m;
			for (m=0; m<32; m++) {
				int p=m+n*32;
				set_chunk_color(chunk_num[p]);
				printf("%02x", block[p]);
				if (chunk_num[p]!=chunk_num[p+1]) set_chunk_color(0);
				printf(" ");
				if (m==15) printf(" ");
			}
			set_chunk_color(0);
			printf(" ");
			for (m=0; m<32; m++) {
				int p=m+n*32;
				char c=block[p];
				if (block[p]<32) c='.';
				if (block[p]>=0x7f) c='.';
				set_chunk_color(chunk_num[p]);
				printf("%c", c);
				if (m==15) {
					if (chunk_num[p]!=chunk_num[p+1]) set_chunk_color(0);
					printf(" ");
				}
			}
			set_chunk_color(0);
			printf("\n");

		}
		blocknumber=blocknumber+1;
	}

	return 0;

}
