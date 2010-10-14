#!/usr/bin/perl

use strict;
use warnings;
use IO::Socket::INET;

$| = 1;

my $socket = IO::Socket::INET->new(
	PeerAddr => "127.0.0.1:40001"
);

print $socket "POST / HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-length: 10\r\n\r\n";
print $socket "0123456789";

while (<$socket>) {
	print STDOUT $_;
}


