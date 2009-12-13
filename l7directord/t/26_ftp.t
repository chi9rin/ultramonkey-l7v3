use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 19;
use IO::Socket::INET;
use IO::Socket::INET6;
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - check_ftp
#220 (vsFTPd 2.0.5)
#USER ftp
#331 Please specify the password.
#PASS ftp
#230 Login successful.
#CWD /
#250 Directory successfully changed.
#TYPE I
#200 Switching to Binary mode.
#PASV
#227 Entering Passive Mode (127,0,0,1,191,154)
#RETR test.dat
#150 Opening BINARY mode data connection for test.dat (9 bytes).
#226 File send OK.
#QUIT
#221 Goodbye.
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "221 Goodbye.\n",
        ]);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_UP, 'check_ftp - login ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "221 Goodbye.\n",
        ]);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass', checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_UP, 'check_ftp - checkport login ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "221 Goodbye.\n",
        ], 2);
    set_default();
    my $v = {negotiatetimeout => 1, login => 'ftp', passwd => 'ftppass'};
    my $r = { server => {ip => '127.0.0.1', port => $port } , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - timeout';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    my $dataport = 53332;
    my $datasock = create_sock($dataport);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "530 Login incorrect.\n",
        "221 Goodbye.\n",
        ]);
    set_default();
    my $v = {negotiatetimeout => 1, login => 'foo', passwd => 'bar'};
    my $r = { server => {ip => '127.0.0.1', port => $port } , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - login error';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "550 Failed to change directory.\n",
        "221 Goodbye.\n",
        ]);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass', checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - checkport cwd error';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    my $dataport = 53334;
    my $datasock = create_sock($dataport);
    skip 'cannot create socket', 1 if !$sock;
    skip 'cannot create data socket', 1 if !$datasock;
    my $pasvport = sprintf "%d,%d", $dataport / 256, $dataport % 256;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "200 Switching to Binary mode.\n",
        "227 Entering Passive Mode (127,0,0,1,$pasvport)\n",
        "150 Opening BINARY mode data connection for test.dat (9 bytes).\n226 File send OK.\n",
        "221 Goodbye.\n",
        ]);
    my $datapid = prepare_child($datasock, ["test.dat\n"], 0, 1);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'test.dat' , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_UP, 'check_ftp - get ok';
    close_child($pid);
    close_child($datapid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    my $dataport = 53335;
    my $datasock = create_sock($dataport);
    skip 'cannot create socket', 1 if !$sock;
    skip 'cannot create socket', 1 if !$datasock;
    my $pasvport = sprintf "%d,%d", $dataport / 256, $dataport % 256;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "200 Switching to Binary mode.\n",
        "227 Entering Passive Mode (127,0,0,1,$pasvport)\n",
        "550 Failed to open file.\n",
        "221 Goodbye.\n",
        ]);
    my $datapid = prepare_child($datasock, ["test.dat\n"]);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'test.dat' , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - get error';
    close_child($pid);
    close_child($datapid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    my $dataport = 53334;
    my $datasock = create_sock($dataport);
    skip 'cannot create socket', 1 if !$sock;
    skip 'cannot create data socket', 1 if !$datasock;
    my $pasvport = sprintf "%d,%d", $dataport / 256, $dataport % 256;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "200 Switching to Binary mode.\n",
        "227 Entering Passive Mode (127,0,0,1,$pasvport)\n150 Opening BINARY mode data connection for test.dat (9 bytes).\n",
        "226 File send OK.\n",
        "221 Goodbye.\n",
        ]);
    my $datapid = prepare_child($datasock, ["test.dat\n"], 0, 1);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'test.dat', receive => 'test' , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_UP, 'check_ftp - receive ok';
    close_child($pid);
    close_child($datapid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    my $dataport = 53334;
    my $datasock = create_sock($dataport);
    skip 'cannot create socket', 1 if !$sock;
    skip 'cannot create data socket', 1 if !$datasock;
    my $pasvport = sprintf "%d,%d", $dataport / 256, $dataport % 256;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "200 Switching to Binary mode.\n",
        "227 Entering Passive Mode (127,0,0,1,$pasvport)\n150 Opening BINARY mode data connection for test.dat (9 bytes).\n",
        "226 File send OK.\n",
        "221 Goodbye.\n",
        ]);
    my $datapid = prepare_child($datasock, ["test.dat\n"], 0, 1);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'test.dat', receive => 'foo' , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - receive error';
    close_child($pid);
    close_child($datapid);
    close $sock;
}
{
    my $port = 63334;
    set_default();
    my $v = {negotiatetimeout => 1, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - connect error';
}
########################################################################
#### IPv6 Test
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "221 Goodbye.\n",
        ]);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '[::1]', port => $port }, fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_UP, 'check_ftp - login ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "221 Goodbye.\n",
        ]);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass', checkport => $port };
    my $r = { server => {ip => '[::1]', port => 10000 }, fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_UP, 'check_ftp - checkport login ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "221 Goodbye.\n",
        ], 2);
    set_default();
    my $v = {negotiatetimeout => 1, login => 'ftp', passwd => 'ftppass'};
    my $r = { server => {ip => '[::1]', port => $port } , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - timeout';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    my $dataport = 53332;
    my $datasock = create_sock6($dataport);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "530 Login incorrect.\n",
        "221 Goodbye.\n",
        ]);
    set_default();
    my $v = {negotiatetimeout => 1, login => 'foo', passwd => 'bar'};
    my $r = { server => {ip => '[::1]', port => $port } , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - login error';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "550 Failed to change directory.\n",
        "221 Goodbye.\n",
        ]);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass', checkport => $port };
    my $r = { server => {ip => '[::1]', port => 10000 }, fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - checkport cwd error';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    my $dataport = 53334;
    my $datasock = create_sock6($dataport);
    skip 'cannot create socket', 1 if !$sock;
    skip 'cannot create data socket', 1 if !$datasock;
    my $pasvport = sprintf "%d,%d", $dataport / 256, $dataport % 256;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "200 Switching to Binary mode.\n",
        "227 Entering Passive Mode (::1,$pasvport)\n",
        "150 Opening BINARY mode data connection for test.dat (9 bytes).\n226 File send OK.\n",
        "221 Goodbye.\n",
        ]);
    my $datapid = prepare_child($datasock, ["test.dat\n"], 0, 1);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '[::1]', port => $port }, request => 'test.dat' , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_UP, 'check_ftp - get ok';
    close_child($pid);
    close_child($datapid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    my $dataport = 53335;
    my $datasock = create_sock6($dataport);
    skip 'cannot create socket', 1 if !$sock;
    skip 'cannot create socket', 1 if !$datasock;
    my $pasvport = sprintf "%d,%d", $dataport / 256, $dataport % 256;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "200 Switching to Binary mode.\n",
        "227 Entering Passive Mode (::1,$pasvport)\n",
        "550 Failed to open file.\n",
        "221 Goodbye.\n",
        ]);
    my $datapid = prepare_child($datasock, ["test.dat\n"]);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '[::1]', port => $port }, request => 'test.dat' , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - get error';
    close_child($pid);
    close_child($datapid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    my $dataport = 53334;
    my $datasock = create_sock6($dataport);
    skip 'cannot create socket', 1 if !$sock;
    skip 'cannot create data socket', 1 if !$datasock;
    my $pasvport = sprintf "%d,%d", $dataport / 256, $dataport % 256;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "200 Switching to Binary mode.\n",
        "227 Entering Passive Mode (::1,$pasvport)\n150 Opening BINARY mode data connection for test.dat (9 bytes).\n",
        "226 File send OK.\n",
        "221 Goodbye.\n",
        ]);
    my $datapid = prepare_child($datasock, ["test.dat\n"], 0, 1);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '[::1]', port => $port }, request => 'test.dat', receive => 'test' , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_UP, 'check_ftp - receive ok';
    close_child($pid);
    close_child($datapid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    my $dataport = 53334;
    my $datasock = create_sock6($dataport);
    skip 'cannot create socket', 1 if !$sock;
    skip 'cannot create data socket', 1 if !$datasock;
    my $pasvport = sprintf "%d,%d", $dataport / 256, $dataport % 256;
    my $pid = prepare_child($sock, [
        "220 (vsFTPd 2.0.5)\n",
        "331 Please specify the password\n",
        "230 Login successful.\n",
        "250 Directory successfully changed.\n",
        "200 Switching to Binary mode.\n",
        "227 Entering Passive Mode (::1,$pasvport)\n150 Opening BINARY mode data connection for test.dat (9 bytes).\n",
        "226 File send OK.\n",
        "221 Goodbye.\n",
        ]);
    my $datapid = prepare_child($datasock, ["test.dat\n"], 0, 1);
    set_default();
    my $v = { negotiatetimeout => 3, login => 'ftp', passwd => 'ftppass' };
    my $r = { server => {ip => '[::1]', port => $port }, request => 'test.dat', receive => 'foo' , fail_counts => 0 };
    my $got = check_ftp($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ftp - receive error';
    close_child($pid);
    close_child($datapid);
    close $sock;
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

sub create_sock6 {
    my $port = shift;
    my $sock = IO::Socket::INET6->new(
        Listen => 5,
        LocalAddr => '::1',
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
        my $read = 0;
        for (@$res) {
            if ($read) {
                my $req = <$s>;
            }
            print $s $_;
            $read = 1;
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
