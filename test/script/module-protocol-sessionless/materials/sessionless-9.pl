#!/usr/bin/perl

use strict;
use warnings;
use IO::Socket::INET;

$| = 1;

my $socket = IO::Socket::INET->new(
	PeerAddr => "127.0.0.1:40001"
);

print $socket "GET / HT";
print $socket "TP/1.0\r\n\r\n";

while (<$socket>) {
	print STDOUT $_;
}


