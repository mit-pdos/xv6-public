set architecture i386:x86-64:intel
target remote localhost:26000
symbol-file kernel
break main
continue
