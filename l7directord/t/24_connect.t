use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 9;
use IO::Socket::INET6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - check_connect
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    set_default();
    my $v = { checktimeout => 3, protocol => 'tcp' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
##    my $r = { server => {ip => '192.168.0.11', port => $port }, fail_counts => 0 };
    my $got = check_connect($v, $r);
    is $got, $main::SERVICE_UP, 'check_connect - connect ok';
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    set_default();
    my $v = { checktimeout => 3, protocol => 'tcp', checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_connect($v, $r);
    is $got, $main::SERVICE_UP, 'check_connect - checkport connect ok';
    close $sock;
}
TODO: {
    todo_skip 'cannot half open socket', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    set_default();
    my $v = { checktimeout => 1, protocol => 'tcp' };
    my $r = { server => {ip => '127.0.0.1', port => $port } , fail_counts => 0 };
    my $got = check_connect($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_connect - timeout';
    close $sock;
}
{
    my $port = 63334;
    set_default();
    my $v = { checktimeout => 1, checkport => $port, protocol => 'tcp' };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_connect($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_connect - connect error';
}
SKIP: {
    my $port = 63335;
    my $sock = create_sock($port, 'udp');
    skip 'cannot create socket', 1 if !$sock;
    set_default();
    my $v = { checktimeout => 3, protocol => 'udp' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
    my $got = check_connect($v, $r);
    is $got, $main::SERVICE_UP, 'check_connect - connect ok (udp)';
    close $sock;
}
SKIP: {
    my $port = 63336;
    my $sock = create_sock($port, 'udp');
    skip 'cannot create socket', 1 if !$sock;
    set_default();
    my $v = { checktimeout => 3, protocol => 'udp', checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_connect($v, $r);
    is $got, $main::SERVICE_UP, 'check_connect - checkport connect ok (udp)';
    close $sock;
}
TODO: {
    todo_skip 'cannot half open socket', 1;
#SKIP: {
    my $port = 63337;
    my $sock = create_sock($port, 'udp');
    skip 'cannot create socket', 1 if !$sock;
    set_default();
    my $v = { checktimeout => 1, protocol => 'udp' };
    my $r = { server => {ip => '127.0.0.1', port => $port } , fail_counts => 0 };
    my $got = check_connect($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_connect - timeout (udp)';
    close $sock;
}
TODO: {
    todo_skip 'udp socket can connect any port?', 1;
    my $port = 63338;
    set_default();
    my $v = { checktimeout => 1, checkport => $port, protocol => 'udp' };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_connect($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_connect - connect error (udp)';
}
##################################################
### IPv6 ld_open_socket Check
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket!!!!', 1 if !$sock;
    set_default();
    my $v = { checktimeout => 3, protocol => 'tcp' };
    my $r = { server => {ip => '::1', port => $port }, fail_counts => 0 };
    my $got = check_connect($v, $r);
    is $got, $main::SERVICE_UP, 'check_connect(IPv6) - connect ok';
    close $sock;
}
# test end
#...............................................

L7lib::comment_in();

sub create_sock {
    my $port = shift;
    my $proto = shift || 'tcp';
    if ($proto eq 'tcp') {
        my $sock = IO::Socket::INET6->new(
            Listen => 5,
            LocalAddr => 'localhost',
            LocalPort => $port,
            ReuseAddr => 1,
            Proto => $proto);
        return $sock;
    }
    else {
        my $sock = IO::Socket::INET6->new(
            LocalAddr => 'localhost',
            LocalPort => $port,
            ReuseAddr => 1,
            Proto => $proto);
        return $sock;
    }
}
sub create_sock6 {
    my $port = shift;
    my $proto = shift || 'tcp';
    if ($proto eq 'tcp') {
        my $sock = IO::Socket::INET6->new(
            Listen => 5,
            LocalAddr => '::1',
            LocalPort => $port,
            ReuseAddr => 1,
            Proto => $proto);
        return $sock;
    }
    else {
        my $sock = IO::Socket::INET6->new(
            LocalAddr => '::1',
            LocalPort => $port,
            ReuseAddr => 1,
            Proto => $proto);
        return $sock;
    }
}
sub prepare_child {
    my $sock = shift;
    my $res = shift;
    my $sleep = shift;
    my $pid = fork();
    if ($pid == 0) {
        if ($sleep) { sleep $sleep; }
        my $s = $sock->accept;
        for (@$res) {
            print $s $_;
            <$s>;
        }
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
