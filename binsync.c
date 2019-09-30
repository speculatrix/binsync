// binsync is a tool for copying a binary file and synchonising changes
//
//

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/////////////////////////////////////////////////////////////////////////////
void	print_usage(char *argv[]) {

	printf("Usage\n");
	printf("\t%s [-limit N] <infile> <outfile>\n", argv[0]);
	printf("\t-limit N = compare the first N bytes\n");

}
/////////////////////////////////////////////////////////////////////////////
int	main(
	int	argc,
	char	*argv[]
	)
{
	int	fd_in, fd_out;
	int	block_size = 1024;	// one day this will be variable
	struct	stat stat_in, stat_out;
	off_t	file_pos = 0;		// current offset into file

	long	cmp_limit = 0;		// comparison limit, nz if active
	int	next_arg = 1;
	char	buff_in[block_size], buff_out[block_size];

	if (argc < 3) {
		print_usage(argv);
		return(0);
	}

	char *arg_lim = "-limit";
	if (!strncmp(argv[next_arg], arg_lim, strlen(arg_lim))) {
		if (argc < 5) {
			print_usage(argv);
			return(0);
		}
		cmp_limit = atol(argv[++next_arg]);
		printf("Dbg, cmp_limit is %ld\n", cmp_limit);
	}

	char *file_in = argv[++next_arg];
	if (stat(file_in, &stat_in)) {
		printf("Error, failed to get info on %s\n", file_in);
		return(1);
	}

	char *file_out = argv[++next_arg];
	if (stat(file_out, &stat_out)) {
		printf("Error, failed to get info on %s\n", file_out);
		return(1);
	}

	if (stat_in.st_size == 0) {
		printf("Error, input file %s is empty\n", file_in);
		return(1);
	}
	if (stat_in.st_size != stat_out.st_size) {
		printf("Error, input and output file size don't match\n");
		return(1);
	}
	
	if (!(fd_in = open(file_in, O_RDONLY))) {
		printf("Error, failed to open %s for reading\n", file_in);
		return(1);
	}

	if (!(fd_out = open(file_out, O_RDWR))) {
		printf("Error, failed to open %s for writing\n", file_out);
		return(1);
	}

	while (file_pos < stat_in.st_size) {
		int chunk_size = (file_pos < stat_in.st_size - block_size) ? block_size : stat_in.st_size - file_pos;
		//printf("Dbg, reading chunk size %d at position %ld\n", chunk_size, file_pos);
		read(fd_in, buff_in, chunk_size);
		read(fd_out, buff_out, chunk_size);
		if (memcmp(buff_in, buff_out, chunk_size)) {
			printf("Files are different somewhere starting at offset %ld chunk size %d\n", file_pos, chunk_size );

			// move output handle back to the start of block
			lseek(fd_out, file_pos, SEEK_SET);
			// and replace the chunk with the source data
			write(fd_out, buff_in, chunk_size);
		}
		file_pos += chunk_size;
	}

	close(fd_in);
	close(fd_out);
	return(0);
}
