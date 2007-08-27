#!/usr/bin/perl

$| = 1;

sub writefile($@){
	my ($file, @lines) = @_;
	
	sleep(1);
	open(F, ">$file") || die "open >$file: $!";
	print F @lines;
	close(F);
}

# Cut out #include lines that don't contribute anything.
for($i=0; $i<@ARGV; $i++){
	$file = $ARGV[$i];
	if(!open(F, $file)){
		print STDERR "open $file: $!\n";
		next;
	}
	@lines = <F>;
	close(F);
	
	$obj = "$file.o";
	$obj =~ s/\.c\.o$/.o/;
	system("touch $file");

	if(system("make CC='gcc -Werror' $obj >/dev/null 2>\&1") != 0){
		print STDERR "make $obj failed: $rv\n";
		next;
	}

	system("cp $file =$file");
	for($j=@lines-1; $j>=0; $j--){
		if($lines[$j] =~ /^#include/){
			$old = $lines[$j];
			$lines[$j] = "/* CUT-H */\n";
			writefile($file, @lines);
			if(system("make CC='gcc -Werror' $obj >/dev/null 2>\&1") != 0){
				$lines[$j] = $old;
			}else{
				print STDERR "$file $old";
			}
		}
	}
	writefile($file, grep {!/CUT-H/} @lines);
	system("rm =$file");
}
