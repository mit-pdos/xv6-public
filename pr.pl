#!/usr/bin/perl

use POSIX qw(strftime);

if($ARGV[0] eq "-h"){
	shift @ARGV;
	$h = $ARGV[0];
	shift @ARGV;
}else{
	$h = $ARGV[0];
}

$page = 0;
$now = strftime "%b %e %H:%M %Y", localtime;

@lines = <>;
for($i=0; $i<@lines; $i+=50){
	print "\n\n";
	++$page;
	print "$now  $h  Page $page\n";
	print "\n\n";
	for($j=$i; $j<@lines && $j<$i +50; $j++){
		print $lines[$j];
	}
	for(; $j<$i+50; $j++){
		print "\n";
	}
	$sheet = "";
	if($lines[$i] =~ /^([0-9][0-9])[0-9][0-9] /){
		$sheet = "Sheet $1";
	}
	print "\n\n";
	print "$sheet\n";
	print "\n\n";
}
