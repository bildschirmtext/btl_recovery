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
		int start=1;
		while (start<sizeof(block)) {
			if ( (block[start]==0x1f) && ((block[start+1]>0x20) && (block[start+1]<0x7f)) ) {
				break;
			}
			start=start+1;
		}
		if (start!=sizeof(block)) {
			uint8_t *sp=&(block[start]);
			int len=strnlen(sp, sizeof(block)-start);
			char fn[256];
			memset(fn, 0, sizeof(fn));
			snprintf(fn, sizeof(fn)-1, outf, blocknum);
			FILE *f=fopen(fn,"w");
			fwrite(sp, len, 1, f);
			//fwrite(block, sizeof(block), 1, f);
			fclose(f);
		}
		blocknum=blocknum+1;
	}
}
