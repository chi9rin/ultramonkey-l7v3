use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 39;
use Config;
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

close *STDOUT;
open  *STDOUT, '>', '/dev/null';
close *STDERR;
open  *STDERR, '>', '/dev/null';

#...............................................
# test start
#   - command_wrapper
{
    my @got = command_wrapper();
    is_deeply \@got, [$?, ''], 'command_wrapper - args is undef';
}
{
    my @got = command_wrapper('ls');
    is $got[0], 0, 'command_wrapper - ls is ok';
    like $got[1], qr/Makefile.*l7directord.*t/s, 'command_wrapper - ls result';
}
{
    my @got = command_wrapper('env LANG=C ls /proc/100000 2>&1');
    if ( $Config{'use64bitint'} ) {
        is_deeply \@got, [512, "ls: /proc/100000: No such file or directory\n"], 'command_wrapper - ls /proc/100000(string) is ok';
    }
    else {
        is_deeply \@got, [256, "ls: /proc/100000: No such file or directory\n"], 'command_wrapper - ls /proc/100000(string) is ok';
    }
}
{
    my @got = command_wrapper('/etc');
    is_deeply \@got, [-1, undef], 'command_wrapper - no such command';
}
#   - system_wrapper
{
    my $got = system_wrapper();
    is $got, 65280, 'system_wrapper - args is undef';
}
{
    my $got = system_wrapper('ls');
    is $got, 0, 'system_wrapper - ls is ok';
}
{
    my $got = system_wrapper('ls /proc/100000');
    if ( $Config{'use64bitint'} ) {
        is $got, 512, 'system_wrapper - ls /proc/100000(string) is ok';
    }
    else {
        is $got, 256, 'system_wrapper - ls /proc/100000(string) is ok';
    }
}
{
    my $got = system_wrapper('ls', '/proc/100000');
    if ( $Config{'use64bitint'} ) {
        is $got, 512, 'system_wrapper - ls /proc/100000(array) is ok';
    }
    else {
        is $got, 256, 'system_wrapper - ls /proc/100000(array) is ok';
    }
}
{
    my $got = system_wrapper('/etc');
    is $got, -1, 'system_wrapper - no such command';
}
#   - exec_wrapper
{
    my $got = exec_wrapper();
    is $got, 0, 'exec_wrapper - args is undef';
}
{
    my $got = exec_wrapper('/etc');
    is $got, 0, 'exec_wrapper - no such command';
}
#   - ld_gethostbyname
{
    my $got = ld_gethostbyname();
    is $got, undef, 'ld_gethostbyname - name is undef';
}
{
    my $got = ld_gethostbyname('localhost');
    is $got, '127.0.0.1', 'ld_gethostbyname - resolve localhost';
}
{
    my $got = ld_gethostbyname('0.0.0.0');
    is $got, '0.0.0.0', 'ld_gethostbyname - already resolved';
}
{
    my $got = ld_gethostbyname('any host name');
    is $got, undef, 'ld_gethostbyname - cannot resolve';
}
#   - ld_getservbyname
{
    my $name = undef;
    my $protocol = 'tcp';
    my $got = ld_getservbyname($name, $protocol);
    is $got, undef, 'ld_getservbyname - name is undef';
}
{
    my $name = 'www';
    my $protocol = undef;
    my $got = ld_getservbyname($name, $protocol);
    is $got, 80, 'ld_getservbyname - protocol is undef';
}
{
    my $name = 0;
    my $protocol = 'tcp';
    my $got = ld_getservbyname($name, $protocol);
    is $got, 0, 'ld_getservbyname - name is 0(min)';
}
{
    my $name = 65535;
    my $protocol = 'tcp';
    my $got = ld_getservbyname($name, $protocol);
    is $got, 65535, 'ld_getservbyname - name is 65535(max)';
}
{
    my $name = 65536;
    my $protocol = 'tcp';
    my $got = ld_getservbyname($name, $protocol);
    is $got, undef, 'ld_getservbyname - name is 65536(error)';
}
{
    my $name = 'foobar';
    my $protocol = 'tcp';
    my $got = ld_getservbyname($name, $protocol);
    is $got, undef, 'ld_getservbyname - no such service name';
}
{
    my $name = 'www';
    my $protocol = 'foo';
    my $got = ld_getservbyname($name, $protocol);
    is $got, undef, 'ld_getservbyname - no such protocol name';
}
{
    my $name = 'www';
    my $protocol = 'tcp';
    my $got = ld_getservbyname($name, $protocol);
    is $got, 80, 'ld_getservbyname - resolve www tcp';
}
{
    my $name = 'www';
    my $protocol = 'udp';
    my $got = ld_getservbyname($name, $protocol);
    is $got, 80, 'ld_getservbyname - resolve www udp';
}
#   - ld_getservhostbyname
{
    my $hostserv = '127.0.0.1:80';
    my $protocol = 'tcp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is_deeply $got, { ip => '127.0.0.1', port => 80 }, 'ld_getservhostbyname - resolve localhost:www tcp';
}
{
    my $hostserv = 'localhost:www';
    my $protocol = 'tcp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is_deeply $got, { ip => '127.0.0.1', port => 80 }, 'ld_getservhostbyname - resolve localhost:www tcp';
}
{
    my $hostserv = 'localhost:www';
    my $protocol = 'udp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is_deeply $got, { ip => '127.0.0.1', port => 80 }, 'ld_getservhostbyname - resolve localhost:www udp';
}
{
    my $hostserv = 'localhost:www';
    my $protocol = undef;
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is_deeply $got, { ip => '127.0.0.1', port => 80 }, 'ld_getservhostbyname - protocol is undef';
}
{
    my $hostserv = 'localhost:www';
    my $protocol = 'foo';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is $got, undef, 'ld_getservhostbyname - no such protocol';
}
{
    my $hostserv = 'foo-bar-host:www';
    my $protocol = 'tcp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is $got, undef, 'ld_getservhostbyname - invalid host address';
}
{
    my $hostserv = 'localhost:foobar';
    my $protocol = 'tcp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is $got, undef, 'ld_getservhostbyname - invalid service name';
}
{
    my $hostserv = undef;
    my $protocol = 'tcp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is $got, undef, 'ld_getservhostbyname - hostserv is undef';
}
{
    my $hostserv = 'localhost';
    my $protocol = 'tcp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is $got, undef, 'ld_getservhostbyname - hostserv is invalid format';
}
##########################################################
### IPv6 
#   - ld_getservhostbyname
{
    my $hostserv = '[2001::11]:80';
    my $protocol = 'tcp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is_deeply $got, { ip => '2001::11', port => 80 }, 'ld_getservhostbyname - [2001::11]:80 tcp';
}
{
    my $hostserv = '[2001::11]:www';
    my $protocol = 'udp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is_deeply $got, { ip => '2001::11', port => 80 }, 'ld_getservhostbyname - resolve localhost:www udp';
}
{
    my $hostserv = 'um08eth1ipv6:www';
    my $protocol = undef;
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is_deeply $got, { ip => '2001::11', port => 80 }, 'ld_getservhostbyname - protocol is undef';
}
{
    my $hostserv = '10.144.169.222:www';
    my $protocol = 'foo';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is $got, undef, 'ld_getservhostbyname - no such protocol';
}
{
    my $hostserv = 'localhost6:80';
    my $protocol = 'tcp';
    my $got = ld_gethostservbyname($hostserv, $protocol);
    is_deeply $got, { ip => '::1'     , port => 80 }, 'ld_getservhostbyname - invalid host address';
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
