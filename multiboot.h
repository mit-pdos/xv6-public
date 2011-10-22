typedef struct Mbdata Mbdata;
struct Mbdata
{
	uint32	flags;
	uint32	mem_lower;  // flag 0
	uint32	mem_upper;  // flag 0
	uint32	boot_device;  // flag 1
	uint32	cmdline;  // flag 2
	uint32	mods_count;  // flag 3
	uint32	mods_addr;
	uint32	syms[4];  // flag 4, 5
	uint32	mmap_length;  // flag 6
	uint32	mmap_addr;
	uint32	drives_length;  // flag 7
	uint32	drives_addr;
	uint32	config_table;  // flag 8
	uint32	boot_loader_name;  // flag 9
	uint32	apm_table;  // flag 10
	uint32	vbe_control_info;  // flag 11
	uint32	vbe_mode_info;
	uint32	vbe_mode;
	uint32	vbe_interface_seg;
	uint32	vbe_interface_off;
	uint32	vbe_interface_len;
};

typedef struct Mbmem Mbmem;
struct Mbmem
{
	uint64	base;
	uint64	length;
	uint32	type;
};

typedef struct Mbmod Mbmod;
struct Mbmod
{
	uint32	start;
	uint32	end;
	uint32	name;
};

typedef struct Page Page;
struct Page
{
	Page	*next;  // only valid when page is on free list
};
