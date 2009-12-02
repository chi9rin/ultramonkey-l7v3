use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 34;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - get_check_func
{
    set_default();
    my $got = get_check_func();
    is $got, \&main::check_off, 'get_check_func - virtual is undef';
}
{
    set_default();
    my $v = {};
    my $got = get_check_func($v);
    is $got, \&main::check_none, 'get_check_func - no checktype';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'http'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_http, 'get_check_func - negotiate http';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'https'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_http, 'get_check_func - negotiate https';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'pop'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_pop, 'get_check_func - negotiate pop';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'imap'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_imap, 'get_check_func - negotiate imap';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'smtp'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_smtp, 'get_check_func - negotiate smtp';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'ftp'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_ftp, 'get_check_func - negotiate ftp';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'ldap'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_ldap, 'get_check_func - negotiate ldap';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'nntp'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_nntp, 'get_check_func - negotiate nntp';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'dns'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_dns, 'get_check_func - negotiate dns';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'sip'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_sip, 'get_check_func - negotiate sip';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'mysql'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_mysql, 'get_check_func - negotiate mysql';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'pgsql'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_pgsql, 'get_check_func - negotiate pgsql';
}
{
    set_default();
    my $v = {
        checktype => 'negotiate',
        service => 'unknown'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_none, 'get_check_func - negotiate unknown';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'http'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_http, 'get_check_func - combined http';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'https'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_http, 'get_check_func - combined https';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'pop'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_pop, 'get_check_func - combined pop';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'imap'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_imap, 'get_check_func - combined imap';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'smtp'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_smtp, 'get_check_func - combined smtp';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'ftp'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_ftp, 'get_check_func - combined ftp';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'ldap'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_ldap, 'get_check_func - combined ldap';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'nntp'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_nntp, 'get_check_func - combined nntp';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'dns'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_dns, 'get_check_func - combined dns';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'sip'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_sip, 'get_check_func - combined sip';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'mysql'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_mysql, 'get_check_func - combined mysql';
}
TODO:{
    todo_skip 'should check closure.', 1 if 1;
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'pgsql'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_pgsql, 'get_check_func - combined pgsql';
}
{
    set_default();
    my $v = {
        checktype => 'combined',
        service => 'unknown'
            };
    my $got = get_check_func($v);
    is $got, \&main::check_none, 'get_check_func - combined unknown';
}
{
    set_default();
    my $v = {
        checktype => 'connect',
            };
    my $got = get_check_func($v);
    is $got, \&main::check_ping, 'get_check_func - connect, but no protocol';
}
{
    set_default();
    my $v = {
        checktype => 'connect',
        protocol => 'udp',
            };
    my $got = get_check_func($v);
    is $got, \&main::check_ping, 'get_check_func - connect, protocol is not tcp';
}
{
    set_default();
    my $v = {
        checktype => 'connect',
        protocol => 'tcp',
            };
    my $got = get_check_func($v);
    is $got, \&main::check_connect, 'get_check_func - connect, protocol is tcp';
}
{
    set_default();
    my $v = {
        checktype => 'ping',
            };
    my $got = get_check_func($v);
    is $got, \&main::check_ping, 'get_check_func - ping';
}
{
    set_default();
    my $v = {
        checktype => 'off',
            };
    my $got = get_check_func($v);
    is $got, \&main::check_off, 'get_check_func - off';
}
{
    set_default();
    my $v = {
        checktype => 'on',
            };
    my $got = get_check_func($v);
    is $got, \&main::check_on, 'get_check_func - on';
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
