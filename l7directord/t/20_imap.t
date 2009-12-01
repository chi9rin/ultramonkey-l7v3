use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 6;
use IO::Socket::INET;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our $module_error = 0;
eval { require Mail::IMAPClient; };
if ($@) {
     $module_error = 1;
}

#...............................................
# test start
#   - check_imap
SKIP: {
    skip q{Can't locate Mail/IMAPClient.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["* OK IMAP4 ready\n", "* BYE\n1 OK completed\n"]);
    set_default();
    my $v = { negotiatetimeout => 3, };
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
    my $got = check_imap($v, $r);
    is $got, $main::SERVICE_UP, 'check_imap - connect ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Mail/IMAPClient.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["* OK IMAP4 ready\n", "* BYE\n1 OK completed\n"]);
    set_default();
    my $v = { negotiatetimeout => 3, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_imap($v, $r);
    is $got, $main::SERVICE_UP, 'check_imap - checkport connect ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Mail/IMAPClient.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["* OK IMAP4 ready\n", "1 OK LOGIN completed\n", "* BYE\n2 OK completed\n"]);
    set_default();
    my $v = {negotiatetimeout => 3, login => 'foo', passwd => 'bar'};
    my $r = { server => {ip => '127.0.0.1', port => $port } , fail_counts => 0 };
    my $got = check_imap($v, $r);
    is $got, $main::SERVICE_UP, 'check_imap - login ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Mail/IMAPClient.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["* OK IMAP4 ready\n", "1 OK LOGIN completed\n", "* BYE\n2 OK completed\n"], 5);
    set_default();
    my $v = {negotiatetimeout => 1, login => 'foo', passwd => 'bar'};
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0  };
    my $got = check_imap($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_imap - timeout';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Mail/IMAPClient.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, ["* OK IMAP4 ready\n", "1 NO LOGIN Error\n", "* BYE\n2 OK completed\n"]);
    set_default();
    my $v = {negotiatetimeout => 1, login => 'foo', passwd => 'bar'};
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0  };
    my $got = check_imap($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_imap - response error';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Mail/IMAPClient.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    set_default();
    my $v = {negotiatetimeout => 1, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_imap($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_imap - connect error';
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
