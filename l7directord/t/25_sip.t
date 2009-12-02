use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 8;
use IO::Socket::INET;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - check_sip
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["SIP/2.0 200 OK\n"]);
    set_default();
    my $v = { negotiatetimeout => 3, protocol => 'tcp', login => 'carol@chicago.com' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
    my $got = check_sip($v, $r);
    is $got, $main::SERVICE_UP, 'check_sip - response ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["SIP/2.0 200 OK\n"]);
    set_default();
    my $v = { negotiatetimeout => 3, protocol => 'tcp', checkport => $port, login => 'carol@chicago.com' };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_sip($v, $r);
    is $got, $main::SERVICE_UP, 'check_sip - checkport response ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["SIP/2.0 200 OK\n"], 2);
    set_default();
    my $v = { negotiatetimeout => 1, protocol => 'tcp', login => 'carol@chicago.com' };
    my $r = { server => {ip => '127.0.0.1', port => $port } , fail_counts => 0 };
    my $got = check_sip($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_sip - timeout';
    close_child($pid);
    close $sock;
}
{
    my $port = 63334;
    set_default();
    my $v = { negotiatetimeout => 1, checkport => $port, protocol => 'tcp', login => 'carol@chicago.com' };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_sip($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_sip - connect error';
}
TODO: {
    todo_skip 'not implement udp sip response', 1;
#SKIP: {
    my $port = 63335;
    my $sock = create_sock($port, 'udp');
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["SIP/2.0 200 OK\n"], 0, 'udp');
    set_default();
    my $v = { negotiatetimeout => 3, protocol => 'udp', login => 'carol@chicago.com' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
    my $got = check_sip($v, $r);
    is $got, $main::SERVICE_UP, 'check_sip - connect ok (udp)';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip 'not implement udp sip response', 1;
#SKIP: {
    my $port = 63336;
    my $sock = create_sock($port, 'udp');
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["SIP/2.0 200 OK\n"], 0, 'udp');
    set_default();
    my $v = { negotiatetimeout => 3, protocol => 'udp', checkport => $port, login => 'carol@chicago.com' };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_sip($v, $r);
    is $got, $main::SERVICE_UP, 'check_sip - checkport connect ok (udp)';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip 'not implement udp sip response', 1;
#SKIP: {
    my $port = 63337;
    my $sock = create_sock($port, 'udp');
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["SIP/2.0 200 OK\n"], 2, 'udp');
    set_default();
    my $v = { negotiatetimeout => 1, protocol => 'udp', login => 'carol@chicago.com' };
    my $r = { server => {ip => '127.0.0.1', port => $port } , fail_counts => 0 };
    my $got = check_sip($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_sip - timeout (udp)';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip 'udp socket can connect any port?', 1;
    my $port = 63338;
    set_default();
    my $v = { negotiatetimeout => 1, checkport => $port, protocol => 'udp', login => 'carol@chicago.com' };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_sip($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_sip - connect error (udp)';
}
# test end
#...............................................

L7lib::comment_in();

sub create_sock {
    my $port = shift;
    my $proto = shift || 'tcp';
    if ($proto eq 'tcp') {
        my $sock = IO::Socket::INET->new(
            Listen => 5,
            LocalAddr => 'localhost',
            LocalPort => $port,
            ReuseAddr => 1,
            Proto => $proto);
        return $sock;
    }
    else {
        my $sock = IO::Socket::INET->new(
            LocalAddr => 'localhost',
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
    my $udp = shift;
    my $pid = fork();
    if ($pid == 0) {
        if (!$udp) {
            my $s = $sock->accept;
            if ($sleep) { sleep $sleep; }
            for (@$res) {
                <$s>;
                print $s $_;
            }
            close $s;
            exit;
        } else {
            my $req;
            recv $sock, $req, 1500, 0;
            $req =~ /^Via.+(\d+\.\d+\.\d+\.\d+):(\d+)/;
            my $ret_sock = IO::Socket::INET->new(
                PeerAddr => $1,
                PeerPort => $2,
                Proto => 'udp',
            );
            if ($sleep) { sleep $sleep; }
            for (@$res) {
                send $ret_sock, $_, 0;
            }
            close $ret_sock;
            exit;
        }
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
