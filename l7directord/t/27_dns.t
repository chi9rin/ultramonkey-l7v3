use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 6;
use Socket;
use Socket6;
use IO::Socket::INET;
use IO::Socket::INET6;
L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our $module_error = 0;
eval { require Net::DNS::Resolver; };
if ($@) {
    $module_error = 1;
}

#...............................................
# test start
#   - check_dns
TODO: {
    todo_skip q{Can't locate Net/DNS.pm in @INC}, 1 if $module_error;
    todo_skip 'not implement dns response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, []);
    set_default();
    my $v = { negotiatetimeout => 3 };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'localhost', receive => '127.0.0.1' , fail_counts => 0 };
local $main::DEBUG_LEVEL = 100;
    my $got = check_dns($v, $r);
    is $got, $main::SERVICE_UP, 'check_dns - A ok';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate Net/DNS.pm in @INC}, 1 if $module_error;
    todo_skip 'not implement dns response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, []);
    set_default();
    my $v = { negotiatetimeout => 3, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000}, request => 'localhost', receive => '127.0.0.1' , fail_counts => 0 };
local $main::DEBUG_LEVEL = 100;
    my $got = check_dns($v, $r);
    is $got, $main::SERVICE_UP, 'check_dns - checkport A ok';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate Net/DNS.pm in @INC}, 1 if $module_error;
    todo_skip 'not implement dns response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, []);
    set_default();
    my $v = {negotiatetimeout => 3};
    my $r = { server => {ip => '127.0.0.1', port => $port}, request => '127.0.0.1', receive => 'localhost' , fail_counts => 0 };
local $main::DEBUG_LEVEL = 100;
    my $got = check_dns($v, $r);
    is $got, $main::SERVICE_UP, 'check_dns - PTR ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Net/DNS.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [], 2);
    set_default();
    my $v = {negotiatetimeout => 1};
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'localhost', receive => '127.0.0.1' , fail_counts => 0 };
    my $got = check_dns($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_dns - timeout';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate Net/DNS.pm in @INC}, 1 if $module_error;
    todo_skip 'not implement dns response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, []);
    set_default();
    my $v = {negotiatetimeout => 1};
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'localhost', receive => '128.0.0.1' , fail_counts => 0 };
local $main::DEBUG_LEVEL = 100;
    my $got = check_dns($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_dns - A error';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Net/DNS.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    set_default();
    my $v = {negotiatetimeout => 1, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_dns($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_dns - connect error';
}
# test end
#...............................................

L7lib::comment_in();

sub create_sock {
    my $port = shift;
    my $sock = IO::Socket::INET->new(
        LocalAddr => 'localhost',
        LocalPort => $port,
        ReuseAddr => 1,
        Proto => 'udp');
    return $sock;
}
sub create_sock6 {
    my $port = shift;
    my $sock = IO::Socket::INET6->new(
        LocalAddr => 'localhost6',
        LocalPort => $port,
        ReuseAddr => 1,
        Proto => 'udp');
    return $sock;
}
sub prepare_child {
    my $sock = shift;
    my $res = shift;
    my $sleep = shift;
    my $pid = fork();
    if ($pid == 0) {
        if ($sleep) { sleep $sleep; }
        for (@$res) {
            my $req;
            recv $sock, $req, 1500, 0;
            send $sock, $_, 0;
        }
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
