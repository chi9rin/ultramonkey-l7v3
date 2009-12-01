use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 2;
use IO::Socket::INET;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - check_ping
SKIP: {
    skip 'icmp ping requires root privilege', 1 if $< != 0;
    set_default();
    my $v = { checktimeout => 3 };
    my $r = { server => {ip => '127.0.0.1'} , fail_counts => 0 };
    my $got = check_ping($v, $r);
    is $got, $main::SERVICE_UP, 'check_ping - ping ok';
}
SKIP: {
    skip 'icmp ping requires root privilege', 1 if $< != 0;
    set_default();
    my $v = { checktimeout => 1 };
    my $r = { server => {ip => '128.0.0.1'} , fail_counts => 0 };
    my $got = check_ping($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ping - timeout';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
}
sub override {
    *ld_log = \&__ld_log;
    *get_status = \&__get_status;
}
sub __get_status {
    return $main::SERVICE_UP;
}
sub __ld_log {
}
