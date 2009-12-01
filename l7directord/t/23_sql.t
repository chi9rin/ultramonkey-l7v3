use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 14;
use IO::Socket::INET;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our $dbi_error = 0;
eval { require DBI; };
if ($@) {
    $dbi_error = 1;
}
our $pg_error = 0;
eval { require DBD::Pg; };
if ($@) {
    $pg_error = 1;
}
our $mysql_error = 0;
eval { require DBD::mysql; };
if ($@) {
    $mysql_error = 1;
}

#...............................................
# test start
#   - check_sql
#   - check_mysql
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/mysql.pm in @INC}, 1 if $mysql_error;
    todo_skip 'not ready mysql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create mysql response
    my $pid = prepare_child($sock, [$response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
    my $got = check_mysql($v, $r);
    is $got, $main::SERVICE_UP, 'check_mysql - connect ok';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/mysql.pm in @INC}, 1 if $mysql_error;
    todo_skip 'not ready mysql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create mysql response
    my $pid = prepare_child($sock, [$response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, checkport => $port, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_mysql($v, $r);
    is $got, $main::SERVICE_UP, 'check_mysql - checkport connect ok';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/mysql.pm in @INC}, 1 if $mysql_error;
    todo_skip 'not ready mysql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create mysql response
    my $query_response = ''; # TODO create mysql query response
    my $pid = prepare_child($sock, [$response, $query_response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'SELECT * from foo', fail_counts => 0 };
    my $got = check_mysql($v, $r);
    is $got, $main::SERVICE_UP, 'check_mysql - request ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    skip q{Can't locate DBD/mysql.pm in @INC}, 1 if $mysql_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create mysql response
    my $pid = prepare_child($sock, [$response], 5);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 1, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'SELECT * from foo', fail_counts => 0 };
    my $got = check_mysql($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_mysql - timeout';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/mysql.pm in @INC}, 1 if $mysql_error;
    todo_skip 'not ready mysql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create mysql response
    my $query_response = ''; # TODO create mysql query response (3rows)
    my $pid = prepare_child($sock, [$response, $query_response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'SELECT * from foo', receive => 3, fail_counts => 0 };
    my $got = check_mysql($v, $r);
    is $got, $main::SERVICE_UP, 'check_mysql - receive ok';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/mysql.pm in @INC}, 1 if $mysql_error;
    todo_skip 'not ready mysql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create mysql response
    my $query_response = ''; # TODO create mysql query response (3rows)
    my $pid = prepare_child($sock, [$response, $query_response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'SELECT * from foo', receive => 5, fail_counts => 0 };
    my $got = check_mysql($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_mysql - receive error';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    skip q{Can't locate DBD/mysql.pm in @INC}, 1 if $mysql_error;
    my $port = 63334;
    set_default();
    my $v = {negotiatetimeout => 1, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_mysql($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_mysql - connect error';
}
#   - check_pgsql
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/Pg.pm in @INC}, 1 if $pg_error;
    todo_skip 'not ready pgsql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create pgsql response
    my $pid = prepare_child($sock, [$response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
    my $got = check_pgsql($v, $r);
    is $got, $main::SERVICE_UP, 'check_pgsql - connect ok';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/Pg.pm in @INC}, 1 if $pg_error;
    todo_skip 'not ready pgsql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create pgsql response
    my $pid = prepare_child($sock, [$response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, checkport => $port, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_pgsql($v, $r);
    is $got, $main::SERVICE_UP, 'check_pgsql - checkport connect ok';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/Pg.pm in @INC}, 1 if $pg_error;
    todo_skip 'not ready pgsql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create pgsql response
    my $query_response = ''; # TODO create pgsql query response
    my $pid = prepare_child($sock, [$response, $query_response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'SELECT * from foo', fail_counts => 0 };
    my $got = check_pgsql($v, $r);
    is $got, $main::SERVICE_UP, 'check_pgsql - request ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    skip q{Can't locate DBD/Pg.pm in @INC}, 1 if $pg_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create pgsql response
    my $pid = prepare_child($sock, [$response], 5);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 1, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'SELECT * from foo', fail_counts => 0 };
    my $got = check_pgsql($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_pgsql - timeout';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/Pg.pm in @INC}, 1 if $pg_error;
    todo_skip 'not ready pgsql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create pgsql response
    my $query_response = ''; # TODO create pgsql query response (3rows)
    my $pid = prepare_child($sock, [$response, $query_response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'SELECT * from foo', receive => 3, fail_counts => 0 };
    my $got = check_pgsql($v, $r);
    is $got, $main::SERVICE_UP, 'check_pgsql - receive ok';
    close_child($pid);
    close $sock;
}
TODO: {
    todo_skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    todo_skip q{Can't locate DBD/Pg.pm in @INC}, 1 if $pg_error;
    todo_skip 'not ready pgsql response', 1;
#SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $response = ''; # TODO create pgsql response
    my $query_response = ''; # TODO create pgsql query response (3rows)
    my $pid = prepare_child($sock, [$response, $query_response]);
    set_default();
    my $v = { server => {ip => '127.0.0.1'}, negotiatetimeout => 3, login => 'postgres', passwd => '', database => 'template1' };
    my $r = { server => {ip => '127.0.0.1', port => $port }, request => 'SELECT * from foo', receive => 5, fail_counts => 0 };
    my $got = check_pgsql($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_pgsql - receive error';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate DBI.pm in @INC}, 1 if $dbi_error;
    skip q{Can't locate DBD/Pg.pm in @INC}, 1 if $pg_error;
    my $port = 63334;
    set_default();
    my $v = {negotiatetimeout => 1, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_pgsql($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_pgsql - connect error';
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
#            <$s>;
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
