#ifndef READ_FILES
#define READ_FILES

#include "types.h"

/* reads files and puts the info to the array of fileNodes, returns number of files(tables) stored in memory */
int read_files( fileNode **files, FILE *fp);

#endif
