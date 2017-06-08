#include <stdio.h>
#include <string.h>
#include "makefs.h"

char disk[totalsize+1];

int main()
{
	char *super_block = "00000077696E697857494E49585F524F4F544653000000000000000000000000000000000000010000000000000000F0000000000000010000000000000001EF0000000000000FEF00000000000001000000000000000080000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	char *super_block2 = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	char *block_bitmap = "FFFFFFFFFFFFFFFF0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	char *block_bitmap2 = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	char *inode_bitmap = "C0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	char *inode_bitmap2 = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	int block_size = 1024;
	
	char *pdisk = disk;
	int sb = 1024;
	int blockmap = 1024;
	int inodemap = 1024;
	int inode_tablesize = 496*128; //at 64th block
	long remaining = totalsize - sb - blockmap - inodemap - inode_tablesize;
	int i=0;

	strcat(pdisk,super_block);
	strcat(pdisk,super_block2);
	strcat(pdisk,block_bitmap);
	strcat(pdisk,block_bitmap2);
	strcat(pdisk,inode_bitmap);
	strcat(pdisk,inode_bitmap2);
	while(*pdisk++);
	pdisk--;

	// printf("%d %d %d %d\n",strlen(super_block),strlen(block_bitmap), strlen(block_bitmap2),strlen(inode_bitmap) );
	// printf("%s%s%s%s",super_block,block_bitmap,block_bitmap2,inode_bitmap);
	for(;i<inode_tablesize;i++){
		if( i % 128==0){
			sprintf(pdisk,"%08x",i/128+1);
			pdisk += 8;
			if(i==128){ //the first one is left as empty deliberately
				sprintf(pdisk,"%08x",i/128+1);
				pdisk += 8;
				*pdisk++ = '.';
				sprintf(pdisk,"%08x",i/128+1);
				pdisk += 8;
				*pdisk++ = '.';
				*pdisk++ = '.';
				i += 19;
			}
			i+=7;
		}
		else{
			*pdisk++ = '0';
		}
	}
	for(;i<inode_tablesize + remaining;i++){
		*pdisk++ = '0';
	}
	*pdisk = '\0';
	pdisk = disk;
	printf("%lu, %d\n",strlen(pdisk),totalsize);
	int curr = sb-1;
	printf("\nsb 0 - 0x%x\n", curr);
	curr++;
	printf("block map 0x%x - 0x%x\n",curr, curr+blockmap );
	curr += blockmap;
	printf("inode map 0x%x - 0x%x\n",curr, curr+inodemap );
	curr += inodemap;
	printf("inode table 0x%x - 0x%x\n",curr, curr+inode_tablesize );
	curr += inode_tablesize;
	printf("data block 0x%x - 0x%lx, free %ld\n",curr, curr+remaining, remaining/1024 );
	curr += remaining;
	
	return 0;
	// return disk;
}