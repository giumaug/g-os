#ifndef ATA_H                
#define ATA_H

typedef struct s_ata_request
{
	void *io_buffer;
	unsigned int lba;
	char sector_count;
	struct t_process_context *process_context; 
}
t_ata_request;

#endif
