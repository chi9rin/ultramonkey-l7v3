use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 23;
use IO::Socket::INET;
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - check_http
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'GET'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_UP, 'check_http - GET ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_UP, 'check_http - HEAD ok';
    close_child($pid);
    close $sock;
}
{
    set_default();
    my $port = 63334;
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http//localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http - URL error';
}
{
    set_default();
    my $port = 63334;
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http - cannot connect';
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n", 2);
    set_default();
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http - timeout';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http - error response';
    close_child($pid);
    close $sock;
}
# https
TODO: {
#SKIP: {
    todo_skip 'not ready https response', 1;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'GET'};
    my $r = {url => "https://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_UP, 'check_http - GET ok (https)';
    close_child($pid);
    close $sock;
}
TODO: {
#SKIP: {
    todo_skip 'not ready https response', 1;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'HEAD'};
    my $r = {url => "https://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_UP, 'check_http - HEAD ok (https)';
    close_child($pid);
    close $sock;
}
{
    set_default();
    my $port = 63334;
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'HEAD'};
    my $r = {url => "https//localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http - URL error (https)';
}
{
    set_default();
    my $port = 63334;
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'HEAD'};
    my $r = {url => "https://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http - cannot connect (https)';
}
TODO: {
#SKIP: {
    todo_skip 'not ready https response', 1;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n", 2);
    set_default();
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'HEAD'};
    my $r = {url => "https://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http - timeout (https)';
    close_child($pid);
    close $sock;
}
TODO: {
#SKIP: {
    todo_skip 'not ready https response', 1;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'HEAD'};
    my $r = {url => "https://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http - error response (https)';
    close_child($pid);
    close $sock;
}
#############################################################################
### IPv6 Test
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket6', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'GET'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_UP, 'check_http IPv6 - GET ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket6', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_UP, 'check_http IPv6 - HEAD ok';
    close_child($pid);
    close $sock;
}
{
    set_default();
    my $port = 63334;
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http//localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http IPv6 - URL error';
}
{
    set_default();
    my $port = 63334;
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http IPv6 - cannot connect';
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket6', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n", 2);
    set_default();
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http IPv6 - timeout';
    close_child($pid);
    close $sock;
}
SKIP: {
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket6', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'http', httpmethod => 'HEAD'};
    my $r = {url => "http://localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http IPv6 - error response';
    close_child($pid);
    close $sock;
}
# https
TODO: {
#SKIP: {
    todo_skip 'not ready https response', 1;
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket6', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'GET'};
    my $r = {url => "https://localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_UP, 'check_http IPv6 - GET ok (https)';
    close_child($pid);
    close $sock;
}
TODO: {
#SKIP: {
    todo_skip 'not ready https response', 1;
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket6', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n");
    set_default();
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'HEAD'};
    my $r = {url => "https://localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_UP, 'check_http IPv6 - HEAD ok (https)';
    close_child($pid);
    close $sock;
}
{
    set_default();
    my $port = 63334;
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'HEAD'};
    my $r = {url => "https//localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http IPv6 - URL error (https)';
}
{
    set_default();
    my $port = 63334;
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'HEAD'};
    my $r = {url => "https://localhost:$port/", fail_counts => 0,
             server => {ip => '[::1]', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http IPv6 - cannot connect (https)';
}
TODO: {
#SKIP: {
    todo_skip 'not ready https response', 1;
    my $port = 63334;
    my $sock = create_sock6($port);
    skip 'cannot create socket6', 1 if !$sock;
    my $pid = prepare_child($sock, "HTTP/1.0 200 OK\r\n\r\n", 2);
    set_default();
    my $v = {negotiatetimeout => 1, service => 'https', httpmethod => 'HEAD'};
    my $r = {url => "https://localhost:$port/", fail_counts => 0,
             server => {ip => '127.0.0.1', port => $port }};
    my $got = check_http($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_http IPv6 - timeout (https)';
    close_child($pid);
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
    my $sock = IO::Socket::INET->new(
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
        <$s>;
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
