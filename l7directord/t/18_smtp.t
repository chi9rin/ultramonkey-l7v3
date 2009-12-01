use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 5;
use IO::Socket::INET;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - check_smtp
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "220\n250\n");
    set_default();
    my $v = {negotiatetimeout => 3};
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
    my $got = check_smtp($v, $r);
    is $got, $main::SERVICE_UP, 'check_smtp - connect ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "220\n250\n");
    set_default();
    my $v = {negotiatetimeout => 3, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_smtp($v, $r);
    is $got, $main::SERVICE_UP, 'check_smtp - checkport connect ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "420\n");
    set_default();
    my $v = {negotiatetimeout => 1, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_smtp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_smtp - error response';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "220\n250\n", 2);
    set_default();
    my $v = {negotiatetimeout => 1, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_smtp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_smtp - timeout';
    close_child($pid);
    close $sock;
}
{
    my $port = 63334;
    set_default();
    my $v = {negotiatetimeout => 1, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_smtp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_smtp - connect error';
}
# test end
#...............................................

L7lib::comment_in();

sub create_sock {
    my $port = shift;
    my $sock = IO::Socket::INET->new(
        Listen => 5,
        LocalAddr => 'localhost',
        LocalPort => $port,
        ReuseAddr => 1,
        Proto => 'tcp');
    return $sock;
}
sub prepare_child {
    my $sock = shift;
    my $res = shift;
    my $sleep = shift;
    my $pid = fork();
    if ($pid == 0) {
        my $s = $sock->accept;
        if ($sleep) { sleep $sleep; }
        print $s $res;
        close $s;
        exit;
    }
    return $pid;
}
sub close_child {
    my $pid = shift;
    kill 15, $pid;
    waitpid $pid, 0;
}
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
