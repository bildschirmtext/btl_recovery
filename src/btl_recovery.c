#include <stdint.h>
#include <stdio.h>
#include <string.h>


void dump_cept(FILE *f, uint8_t block[], int start, int len)
{
	if (f==NULL) return;
	if (block==NULL) return;
	fwrite(&(block[start]), len, 1, f);
}

void dump_cept_block(FILE *f, uint8_t block[], int offset)
{
	int start=(int)block[offset+0] | (int)block[offset+1]<<8;
	int len  =(int)block[offset+2] | (int)block[offset+3]<<8;
	if (start==0) return;
	if (start>=2048) return;
	if (len==0) return;
	if (start+len>=2048) return;
	dump_cept(f, block, start, len);
}

void print_description(char *fn, uint8_t block[], int offset)
{
	int start=(int)block[offset+0] | (int)block[offset+1]<<8;
	int len  =(int)block[offset+2] | (int)block[offset+3]<<8;
	if (start==0) return;
	if (start>=2048) return;
	if (len==0) return;
	if (start+len>=2048) return;
	if (len>127) return;
	char desc[128];
	memset(desc, 0, sizeof(desc));
	int n;
	for (n=0; n<len; n++) {
		desc[n]=block[n+start];
	}
	//trim the end of the string
	int slen=strlen(desc);
	while ((slen>0) && (desc[slen-1]==' ')) {
		desc[slen-1]=0;
		slen=strlen(desc);
	}
	if (slen<=0) return;
	printf("%s %02x %s\n", fn, offset, desc);
}

int main(int argc, char *argv[])
{
	if (argc!=2) {
		printf("usage: %s <printf-string for filename>\n\te.g. %s out-%%03d.cpt\n", argv[0], argv[0]);
		return 1;
	}
	char *outf=argv[1];
	int blocknum=0;
	uint8_t block[2048];
	while (fread(block, sizeof(block), 1, stdin)>0) {
		char fn[256];
		memset(fn, 0, sizeof(fn));
		snprintf(fn, sizeof(fn)-1, outf, blocknum);
		blocknum=blocknum+1;
		if (blocknum==1) continue; //ignore the first block
		//pointer to the description
		int description=(int)block[0x88] | (int)block[0x89]<<8;
		//if no description, it's not a valid block
		if (description<0x88) continue;
		if (description>2048) continue;

		//pointer to the cept block and its size
		int cept=(int)block[0x92] | (int)block[0x93]<<8;
		int len=(int)block[0x94] | (int)block[0x95]<<8;
		if (cept==0) {
			cept=(int)block[0x9A] | (int)block[0x9B]<<8;
			len=(int)block[0x9C] | (int)block[0x9D]<<8;
		}
		
		if (cept==0) continue;
		if (cept>2048) continue;

		print_description(fn, block, 0x88);
		print_description(fn, block, 0x8e);

		int start=cept;
		uint8_t *sp=&(block[start]);
		FILE *f=fopen(fn,"w");
		dump_cept_block(f, block, 0x92);
		dump_cept_block(f, block, 0x9a);
		dump_cept_block(f, block, 0xa2);	
		dump_cept_block(f, block, 0xae);
		fclose(f);

	}
}
