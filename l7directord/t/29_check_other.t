use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 3;
use IO::Socket::INET;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - check_none
{
    set_default();
    my $v = { checktimeout => 3 };
    my $r = { server => {ip => '127.0.0.1'} , fail_counts => 0 };
    my $got = check_none($v, $r);
    is $got, $main::SERVICE_UP, 'check_none - ok';
}
#   - check_off
{
    set_default();
    my $v = { checktimeout => 3 };
    my $r = { server => {ip => '127.0.0.1'} , fail_counts => 0 };
    my $got = check_off($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_off - ok';
}
#   - check_on
{
    set_default();
    my $v = { checktimeout => 3 };
    my $r = { server => {ip => '127.0.0.1'} , fail_counts => 0 };
    my $got = check_on($v, $r);
    is $got, $main::SERVICE_UP, 'check_on - ok';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
}
sub override {
    *ld_log = \&__ld_log;
}
sub __ld_log {
}
