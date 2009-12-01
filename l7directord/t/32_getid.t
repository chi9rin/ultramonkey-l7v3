use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 41;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - get_ip_port
{
    my $host = undef;
    my $checkport = undef;
    my $got = get_ip_port($host, $checkport);
    is $got, '', 'get_ip_port - host and checkport is undef';
}
{
    my $host = { server => { ip => '127.0.0.1', port => undef } };
    my $checkport = undef;
    my $got = get_ip_port($host, $checkport);
    is $got, '', 'get_ip_port - host port and checkport is undef';
}
{
    my $host = { server => { ip => undef, port => undef } };
    my $checkport = 100;
    my $got = get_ip_port($host, $checkport);
    is $got, '', 'get_ip_port - host ip is undef';
}
{
    my $host = { server => { ip => '127.0.0.1', port => 80 } };
    my $checkport = undef;
    my $got = get_ip_port($host, $checkport);
    is $got, '127.0.0.1:80', 'get_ip_port - get host ip and port';
}
{
    my $host = { server => { ip => '127.0.0.1', port => 80 } };
    my $checkport = 8080;
    my $got = get_ip_port($host, $checkport);
    is $got, '127.0.0.1:8080', 'get_ip_port - get host ip and checkport';
}
{
    my $host = { server => { ip => '127.0.0.1', port => 0 } };
    my $checkport = undef;
    my $got = get_ip_port($host, $checkport);
    is $got, '127.0.0.1:0', 'get_ip_port - get host ip and port zero';
}
#   - get_health_check_id_str
{
    my $v = undef;
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, undef, 'get_health_check_id_str - v is undef';
}
{
    my $v = {};
    my $r = undef;
    my $got = get_health_check_id_str($v, $r);
    is $got, undef, 'get_health_check_id_str - r is undef';
}
{
    my $v = {};
    my $r = {};
    my $got = get_health_check_id_str($v, $r);
    is $got, undef, 'get_health_check_id_str - r->{server} is undef';
}
{
    my $v = {};
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::::::::::::::::::', 'get_health_check_id_str - all args is ok(empty)';
}
{
    my $v = {};
    my $r = { server => { ip => 1 } };
    my $got = get_health_check_id_str($v, $r);
    is $got, '1::::::::::::::::::', 'get_health_check_id_str - set real ip';
}
{
    my $v = {};
    my $r = { server => { port => 1 } };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':1:::::::::::::::::', 'get_health_check_id_str - set real port';
}
{
    my $v = { checkport => 1 };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':1:::::::::::::::::', 'get_health_check_id_str - set checkport';
}
{
    my $v = { checktype => 'connect' };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::connect::::::::::::::::', 'get_health_check_id_str - set checktype';
}
{
    my $v = { service => 'http' };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':::http:::::::::::::::', 'get_health_check_id_str - set service';
}
{
    my $v = { protocol => 'tcp' };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::::tcp::::::::::::::', 'get_health_check_id_str - set protocol';
}
{
    my $v = { num_connects => 1 };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':::::1:::::::::::::', 'get_health_check_id_str - set num_connects';
}
{
    my $v = {};
    my $r = { server => {}, request => '/' };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::::::/::::::::::::', 'get_health_check_id_str - set real request';
}
{
    my $v = {};
    my $r = { server => {}, receive => 'ok' };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':::::::ok:::::::::::', 'get_health_check_id_str - set real receive';
}
{
    my $v = { httpmethod => 'GET' };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::::::::GET::::::::::', 'get_health_check_id_str - set httpmethod';
}
{
    my $v = { virtualhost => 'virtual' };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':::::::::virtual:::::::::', 'get_health_check_id_str - set virtualhost';
}
{
    my $v = { login => 'ftp' };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::::::::::ftp::::::::', 'get_health_check_id_str - set login';
}
{
    my $v = { passwd => 'ftp' };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':::::::::::ftp:::::::', 'get_health_check_id_str - set passwd';
}
{
    my $v = { database => 'template' };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::::::::::::template::::::', 'get_health_check_id_str - set database';
}
{
    my $v = { customcheck => '/bin/echo' };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':::::::::::::/bin/echo:::::', 'get_health_check_id_str - set customcheck';
}
{
    my $v = { checkinterval => 1 };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::::::::::::::1::::', 'get_health_check_id_str - set checkinterval';
}
{
    my $v = { checkcount => 1 };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':::::::::::::::1:::', 'get_health_check_id_str - set checkcount';
}
{
    my $v = { checktimeout => 1 };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::::::::::::::::1::', 'get_health_check_id_str - set checktimeout';
}
{
    my $v = { negotiatetimeout => 1 };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, ':::::::::::::::::1:', 'get_health_check_id_str - set negotiatetimeout';
}
{
    my $v = { retryinterval => 1 };
    my $r = { server => {} };
    my $got = get_health_check_id_str($v, $r);
    is $got, '::::::::::::::::::1', 'get_health_check_id_str - set retryinterval';
}
{
    my $v = {
        checkport        => 10,
        checktype        => 'connect',
        service          => 'http',
        protocol         => 'tcp',
        num_connects     => 2,
        httpmethod       => 'GET',
        virtualhost      => 'virtual',
        login            => 'ftp',
        passwd           => 'ftp',
        database         => 'template',
        customcheck      => '/bin/echo',
        checkinterval    => 3,
        checkcount       => 4,
        checktimeout     => 5,
        negotiatetimeout => 6,
        retryinterval    => 7,
    };
    my $r = {
        server  => { ip => 1, port => 1 },
        request => '/',
        receive => 'ok',
    };
    my $got = get_health_check_id_str($v, $r);
    is $got, '1:10:connect:http:tcp:2:/:ok:GET:virtual:ftp:ftp:template:/bin/echo:3:4:5:6:7',
            'get_health_check_id_str - set all';
}
#   - get_virtual_id_str
{
    my $v = undef;
    my $got = get_virtual_id_str($v);
    is $got, undef, 'get_virtual_id_str - v is undef';
}
{
    my $v = {};
    my $got = get_virtual_id_str($v);
    is $got, undef, 'get_virtual_id_str - v->{module} is undef';
}
{
    my $v = { module => {} };
    my $got = get_virtual_id_str($v);
    is $got, '::', 'get_virtual_id_str - arg is ok(empty)';
}
{
    my $v = { module => {}, protocol => 'tcp' };
    my $got = get_virtual_id_str($v);
    is $got, 'tcp::', 'get_virtual_id_str - set protocol';
}
{
    my $v = { module => {}, server => { ip => '127.0.0.1', port => 80 } };
    my $got = get_virtual_id_str($v);
    is $got, ':127.0.0.1:80:', 'get_virtual_id_str - set server';
}
{
    my $v = { module => { name => 'cinsert', key => '--cookie-name monkey' } };
    my $got = get_virtual_id_str($v);
    is $got, '::cinsert --cookie-name monkey', 'get_virtual_id_str - set module';
}
{
    my $v = {
        server   => { ip => '127.0.0.1', port => 80 },
        module   => { name => 'cinsert', key => '--cookie-name monkey' },
        protocol => 'tcp',
    };
    my $got = get_virtual_id_str($v);
    is $got, 'tcp:127.0.0.1:80:cinsert --cookie-name monkey', 'get_virtual_id_str - set all';
}
#   - get_forward_flag
{
    my $got = get_forward_flag();
    is $got, '', 'get_forward_flag - forward is undef';
}
{
    my $got = get_forward_flag('MAsq');
    is $got, '-m', 'get_forward_flag - forward is masq';
}
{
    my $got = get_forward_flag('MAsqs');
    is $got, '', 'get_forward_flag - forward is otherwise';
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
