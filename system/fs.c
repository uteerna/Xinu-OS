#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;

#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void fs_print_fsd(void) {
	printf("fsd.ninodes: %d\n", fsd.ninodes);
	printf("sizeof(struct inode): %d\n", sizeof(struct inode));
	printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
	printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
	int mbyte, mbit;
	mbyte = b / 8;
	mbit = b % 8;

	fsd.freemask[mbyte] |= (0x80 >> mbit);
	return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
	int mbyte, mbit;
	mbyte = b / 8;
	mbit = b % 8;

	return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
	return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
	int mbyte, mbit, invb;
	mbyte = b / 8;
	mbit = b % 8;

	invb = ~(0x80 >> mbit);
	invb &= 0xFF;

	fsd.freemask[mbyte] &= invb;
	return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

int fs_open(char *filename, int flags) {
	int fileStatus = 0, filedesc, i, entCount;
	filedesc = next_open_fd;
	struct inode nodes;
	bs_bread(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
	entCount = fsd.root_dir.numentries; // Fetch entry count from root directory
	for (i = 0; i < entCount; i++) {
		// Checking for the matching file name.
		if (strcmp(fsd.root_dir.entry[i].name, filename) != 0) {
			printf("FS_OPEN: Invalid file name specified. Please check!!\n");
		}

		if(i == fsd.root_dir.numentries) {
			printf("No file with %s name exists", filename);
		    return SYSERR;
		}

		else {
		  fileStatus = 1;  
		  nodes.type = flags;
		  oft[filedesc].fileptr = 0;
		  fs_get_inode_by_num(0, fsd.root_dir.entry[i].inode_num, &nodes);
		  oft[filedesc].state = FSTATE_OPEN;
		  oft[filedesc].in = nodes; // Inode updation
		  oft[filedesc].de = &fsd.root_dir.entry[i];
		  fs_put_inode_by_num(0, fsd.root_dir.entry[i].inode_num, &nodes);
		}
	}

	// If file is found then increment 
	if (fileStatus) {
		next_open_fd++;
		return filedesc;
	}
	printf("FS_OPEN: Invalid Filename"); // If file is not found then return SYSERR
	return SYSERR;
}

int fs_close(int fd) {

	// Check if the file state is CLOSED
	if (oft[fd].state == FSTATE_CLOSED) {
		printf("FSTATE_CLOSED: File is closed already!!\n");
		return SYSERR;
	}

	// Check whether file descriptor is valid or not
	else if (((fd > 0) || fd < next_open_fd)) {
		next_open_fd--;
		oft[fd].state = FSTATE_CLOSED; // Changing the file state to CLOSED
		oft[fd].fileptr = 0; // Updating the file pointer.    
		return OK;
	}

	else {
		printf("File entry not found to close. Please check!!\n");
		return SYSERR;    
	}
}

int fs_create(char *filename, int mode) {
	int i = 0, entCount = fsd.root_dir.numentries,count = 1, filedesc, block_len;;
	struct filetable filetbl;
	struct inode nodes;

	// Check if the the file already exists or not.
	for (i = 0; i < entCount; i++) {
		if ((strcmp(filename, fsd.root_dir.entry[i].name) == 0)) {
			printf("File already exists with this name.\n");
		    return SYSERR;
		}
	}

	// Check if there are inodes available for use
	if (fsd.inodes_used > fsd.ninodes) {
		printf("No inodes available for use");
		return SYSERR;
	}

	if (mode != O_CREAT) {
		return SYSERR;
	}

	else {
		fsd.inodes_used++; // Increment incode counter
		filedesc = next_open_fd++;
		nodes.id = count++;
		nodes.type = INODE_TYPE_FILE;
		block_len = nodes.id / INODES_PER_BLOCK;
		block_len = block_len + FIRST_INODE_BLOCK;
		fs_setmaskbit(block_len);
		fs_put_inode_by_num(0, nodes.id, &nodes);    

		filetbl.state = FSTATE_OPEN; // Changing the file state to OPEN
		filetbl.de = &(fsd.root_dir.entry[entCount++]);
		filetbl.fileptr = 0; // Set the initial vale of the file pointer
		strcpy((filetbl.de)->name, filename); // Update file name
		memcpy(&(fsd.root_dir), &fsd.root_dir, sizeof(struct directory));
		memcpy(&(filetbl.in), &nodes, sizeof(struct inode));
		memcpy(&(filetbl.de->inode_num), &(nodes.id), sizeof(int));
		memcpy(&(fsd.root_dir.entry[fsd.root_dir.numentries - 1].inode_num), &(nodes.id), sizeof(int));
		memcpy(oft + filedesc, &filetbl, sizeof(struct filetable));
		return filedesc;    
	} 
}

int fs_seek(int fd, int offset) {
	offset = oft[fd].fileptr + offset;

	// Check if the file state is CLOSED
	if(oft[fd].state == FSTATE_CLOSED) {
		printf("FS_SEEK: File is not open\n");
		return SYSERR;
	}

	if (offset < 0) {
		printf("Resetting the offset");
		offset = 0;
	}
	memcpy(&((oft + fd)->fileptr), &offset, sizeof(int));
	return fd;
}

int fs_read(int fd, void *buf, int nbytes) {
	struct filetable filetbl = oft[fd];
	struct inode node = filetbl.in;
	int bytes ,original_nbytes = nbytes;
	int inode_block = (filetbl.fileptr / fsd.blocksz);
	int inode_offset = (filetbl.fileptr % fsd.blocksz);
	int dst_blk = filetbl.in.blocks[inode_block];
	int new_blocks = fsd.blocksz - inode_offset;

	// Check if the file state is CLOSED
	if(oft[fd].state == FSTATE_CLOSED) {
		printf("FS_READ: File is not open\n");
		return SYSERR;
	}

	// Check if the file is valid
	else if(fd < 0 || fd > NUM_FD) {
		printf("FS_READ: Invalid File\n");
		return SYSERR;
	}

	// Check the buffer length
	else if(nbytes <= 0) {
		printf("FS_READ: Input buffer length is less than 0\n");
		return SYSERR;
	}

	if (inode_block < INODEBLOCKS) {
		while (nbytes > 0) {
		  // If the data is in different blocks
		    if(new_blocks <= nbytes) {
		        if (inode_block == INODEBLOCKS - 1) {
			    	printf("FS_READ: Byte limit exceeded!!");
				    bytes = original_nbytes - nbytes;
				    return bytes;
		    	}
			    bs_bread(0, dst_blk, inode_offset, buf, new_blocks);
			    memcpy(oft + fd, &filetbl, sizeof(struct filetable));
			    dst_blk = node.blocks[++inode_block];
			    inode_offset = 0;
			    filetbl.fileptr = filetbl.fileptr + new_blocks;
			    buf = buf + new_blocks;
			    nbytes = nbytes - new_blocks;
			}

			// If data is in same block
			else if (new_blocks > nbytes) {
				bs_bread(0, dst_blk, inode_offset, buf, nbytes);
			    buf = buf + nbytes;
			    filetbl.fileptr = filetbl.fileptr + nbytes;
			    nbytes = 0;
			    return original_nbytes;
			}
		}
	}
	bytes = original_nbytes - nbytes;
	return bytes;
}

int fs_write(int fd, void *buf, int nbytes) {
	int i, bytes, original_nbytes = nbytes, blocks, index; 
	struct filetable filetbl = oft[fd];
	struct inode node = filetbl.in;
	int inode_offset = (filetbl.fileptr % fsd.blocksz);
	int inodeblock = (filetbl.fileptr / fsd.blocksz);
	int new_blocks = fsd.blocksz - inode_offset; 
	blocks = fsd.nblocks;

	// Check if the file state is CLOSED
	if (oft[fd].state == FSTATE_CLOSED) {
		printf("FS_WRITE: File is in Closed State");
		return SYSERR;
	}

	// Check if the file is empty
	else if(nbytes == 0) {
		printf("FS_WRITE: The file is empty!!");
		return SYSERR;
	}

	// Check if the file if valid
	else if(fd < 0 || fd > NUM_FD) {
		printf("FS_WRITE: Invalid File\n");
		return SYSERR;
	}

	else if (inodeblock < INODEBLOCKS) {
		while (nbytes > 0) {
		    if (node.blocks[inodeblock] > 0) {
		    	i = node.blocks[inodeblock];
		  	}
			else if (node.blocks[inodeblock] == 0) {
			    for (index = 0; index<blocks; index++) {
			        if(fs_getmaskbit(index) == 0) {
			            i = index;
			      	}
		    	}
			    memcpy(node.blocks + inodeblock, &i, sizeof(int));
			    memcpy(&((oft + fd)->in), &(node), sizeof(struct inode));
			    fs_put_inode_by_num(0, node.id, &node);
			    fs_setmaskbit(i);
			    filetbl.in = node;
		  	}
		  
		    if (new_blocks > nbytes) {
			    bs_bwrite(0, i, inode_offset, buf, nbytes);
			    filetbl.fileptr = filetbl.fileptr + nbytes;
			    memcpy(oft + fd, &filetbl, sizeof(struct filetable));
			    nbytes = 0;
			    return original_nbytes;
		    }

		    if (new_blocks <= nbytes) {
			    if (inodeblock == INODEBLOCKS - 1) {
			    	printf("FS_WRITE: Byte limit exceeded!!");
			        bytes = original_nbytes - nbytes;
			        return bytes;
			    }
			    bs_bwrite(0, i, inode_offset, buf, new_blocks);
			    memcpy(oft + fd, &filetbl, sizeof(struct filetable));
			    inode_offset = 0;
			    inodeblock++;
			    buf = buf + new_blocks;
			    nbytes = nbytes - new_blocks;
			    filetbl.fileptr = filetbl.fileptr + new_blocks;
		  	}
		}
	}
	bytes = original_nbytes - nbytes;
	return bytes;
}
#endif
