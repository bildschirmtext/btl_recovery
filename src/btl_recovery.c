#include <stdint.h>
#include <stdio.h>
#include <string.h>

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




		int start=cept;
		printf("%s ", fn);
		int n;
		for (n=description; n<cept; n++) {
			printf("%c", block[n]);
		}
		printf("\n");
		uint8_t *sp=&(block[start]);
		FILE *f=fopen(fn,"w");
		fwrite(sp, len, 1, f);		
		fclose(f);

	}
}
