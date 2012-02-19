typedef struct Mbdata Mbdata;
struct Mbdata
{
	u32	flags;
	u32	mem_lower;  // flag 0
	u32	mem_upper;  // flag 0
	u32	boot_device;  // flag 1
	u32	cmdline;  // flag 2
	u32	mods_count;  // flag 3
	u32	mods_addr;
	u32	syms[4];  // flag 4, 5
	u32	mmap_length;  // flag 6
	u32	mmap_addr;
	u32	drives_length;  // flag 7
	u32	drives_addr;
	u32	config_table;  // flag 8
	u32	boot_loader_name;  // flag 9
	u32	apm_table;  // flag 10
	u32	vbe_control_info;  // flag 11
	u32	vbe_mode_info;
	u32	vbe_mode;
	u32	vbe_interface_seg;
	u32	vbe_interface_off;
	u32	vbe_interface_len;
};

typedef struct Mbmem Mbmem;
struct Mbmem
{
	u64	base;
	u64	length;
	u32	type;
};

typedef struct Mbmod Mbmod;
struct Mbmod
{
	u32	start;
	u32	end;
	u32	name;
};
