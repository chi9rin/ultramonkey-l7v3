use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 16;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our @get_ip_port_args = ();
our @get_ip_port_returns = ();
our @get_forward_flag_args = ();
our @get_forward_flag_returns = ();
our @system_wrapper_args = ();
our @system_wrapper_returns = ();
our @command_wrapper_args = ();
our @command_wrapper_returns = ();
#...............................................
# test start
#   - ld_read_l7vsadm
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = '/';
    my %expected = (
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - no l7vsadm ok(1)';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = '/proc/1';
    my %expected = (
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - no l7vsadm ok(2)';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = 't/lib/l7vsadm.1';
    my %expected = (
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - no virtual service ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = 't/lib/l7vsadm.2';
    my %expected = (
        'tcp:127.0.0.1:80:sessionless' => undef,
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - one virtual service ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = 't/lib/l7vsadm.3';
    my %expected = (
        'tcp:127.0.0.1:80:cinsert --cookie-name monkey' => {
            '192.168.0.1:8080' => {
                server => { ip => '192.168.0.1', port => 8080 },
                weight => 5,
                forward => 'Masq',
                option => { flags => '-r 192.168.0.1:8080',
                            forward => '-m' },
            },
        },
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - one virtual service, one real server ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m', '-m', '-m');
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = 't/lib/l7vsadm.4';
    my %expected = (
        'tcp:127.0.0.1:80:url --pattern-match monkey' => {
            '192.168.0.1:80' => {
                server => { ip => '192.168.0.1', port => 80 },
                weight => 3,
                forward => 'Masq',
                option => { flags => '-r 192.168.0.1:80',
                            forward => '-m' },
            },
            '192.168.0.2:80' => {
                server => { ip => '192.168.0.2', port => 80 },
                weight => 2,
                forward => 'Masq',
                option => { flags => '-r 192.168.0.2:80',
                            forward => '-m' },
            },
            '192.168.0.3:80' => {
                server => { ip => '192.168.0.3', port => 80 },
                weight => 1,
                forward => 'Masq',
                option => { flags => '-r 192.168.0.3:80',
                            forward => '-m' },
            },
        },
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - one virtual service, three real server ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = 't/lib/l7vsadm.5';
    my %expected = (
        'tcp:127.0.0.1:80:sessionless' => undef,
        'tcp:192.168.0.1:8080:sslid' => undef,
        'tcp:192.168.0.100:10000:cinsert --cookie-name monkey' => undef,
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - three virtual service ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m', '-m', '-m');
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = 't/lib/l7vsadm.6';
    my %expected = (
        'tcp:127.0.0.1:80:sessionless' => {
            '192.168.0.1:80' => {
                server => { ip => '192.168.0.1', port => 80 },
                weight => 1,
                forward => 'Masq',
                option => { flags => '-r 192.168.0.1:80',
                            forward => '-m' },
            },
            '192.168.0.2:80' => {
                server => { ip => '192.168.0.2', port => 80 },
                weight => 1,
                forward => 'Masq',
                option => { flags => '-r 192.168.0.2:80',
                            forward => '-m' },
            },
        },
        'tcp:10.10.10.10:10:sslid' => undef,
        'tcp:20.20.20.20:20:url --pattern-match monkey' => {
            '192.168.0.1:80' => {
                server => { ip => '192.168.0.1', port => 80 },
                weight => 1,
                forward => 'Masq',
                option => { flags => '-r 192.168.0.1:80',
                            forward => '-m' },
            },
        },
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - virtual 1: two real, virtual 2: no real, virtual 3: one real ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = 't/lib/l7vsadm.7';
    my %expected = (
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - no virtual, ignore real';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    local $main::PROC_ENV{l7vsadm} = 't/lib/l7vsadm.8';
    my %expected = (
        'tcp:192.168.0.1:8080:sessionless' => {
            '192.168.0.2:80' => {
                server => { ip => '192.168.0.2', port => 80 },
                weight => 1,
                forward => 'Masq',
                option => { flags => '-r 192.168.0.2:80',
                            forward => '-m' },
            },
        },
    );
    my $got = ld_read_l7vsadm();
    is_deeply $got, \%expected, 'ld_read_l7vsadm - ignore real, one virtual, one real ok';
}
#   - ld_add_virtual
#   - ld_operate_virtual
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('1.1.1.1:1');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @command_wrapper_args = ();
    local @command_wrapper_returns = (0, q{});
    local $main::PROC_ENV{l7vsadm} = 'l7vsadm';
    my $v = { module => { name => 'sslid' }, option => { flags => '-t 1.1.1.1:1 -m sslid' } };
    ld_add_virtual($v);
    is_deeply \@command_wrapper_args, ['l7vsadm -A -t 1.1.1.1:1 -m sslid 2>&1'], 'ld_add_virtual - ok';
}
#   - ld_edit_virtual
#   - ld_operate_virtual
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('1.1.1.1:1');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @command_wrapper_args = ();
    local @command_wrapper_returns = (0, q{});
    local $main::PROC_ENV{l7vsadm} = 'l7vsadm';
    my $v = { module => { name => 'sslid' }, option => { flags => '-t 1.1.1.1:1 -m sslid' } };
    ld_edit_virtual($v);
    is_deeply \@command_wrapper_args, ['l7vsadm -E -t 1.1.1.1:1 -m sslid 2>&1'], 'ld_edit_virtual - ok';
}
#   - ld_delete_virtual
#   - ld_operate_virtual
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('1.1.1.1:1');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @command_wrapper_args = ();
    local @command_wrapper_returns = (0, q{});
    local $main::PROC_ENV{l7vsadm} = 'l7vsadm';
    my $v = { module => { name => 'sslid' }, option => { main => '-t 1.1.1.1:1 -m sslid' } };
    ld_delete_virtual($v);
    is_deeply \@command_wrapper_args, ['l7vsadm -D -t 1.1.1.1:1 -m sslid 2>&1'], 'ld_delete_virtual - ok';
}
#   - ld_add_real
#   - ld_operate_real
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('1.1.1.1:1', '2.2.2.2:2');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @command_wrapper_args = ();
    local @command_wrapper_returns = (0, q{});
    local $main::PROC_ENV{l7vsadm} = 'l7vsadm';
    my $v = { module => { name => 'sslid' }, option => { main => '-t 1.1.1.1:1 -m sslid' } };
    my $r = { option => { flags => '-r 2.2.2.2:2' } };
    my $weight = 10;
    ld_add_real($v, $r, $weight);
    is_deeply \@command_wrapper_args, ['l7vsadm -a -t 1.1.1.1:1 -m sslid -r 2.2.2.2:2 -w 10 2>&1'], 'ld_add_real - ok';
}
#   - ld_edit_real
#   - ld_operate_real
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('1.1.1.1:1', '2.2.2.2:2');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @command_wrapper_args = ();
    local @command_wrapper_returns = (0, q{});
    local $main::PROC_ENV{l7vsadm} = 'l7vsadm';
    my $v = { module => { name => 'sslid' }, option => { main => '-t 1.1.1.1:1 -m sslid' } };
    my $r = { option => { flags => '-r 2.2.2.2:2' } };
    my $weight = 10;
    ld_edit_real($v, $r, $weight);
    is_deeply \@command_wrapper_args, ['l7vsadm -e -t 1.1.1.1:1 -m sslid -r 2.2.2.2:2 -w 10 2>&1'], 'ld_edit_real - ok';
}
#   - ld_delete_real
#   - ld_operate_real
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('1.1.1.1:1', '2.2.2.2:2');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    local @command_wrapper_args = ();
    local @command_wrapper_returns = (0, q{});
    local $main::PROC_ENV{l7vsadm} = 'l7vsadm';
    my $v = { module => { name => 'sslid' }, option => { main => '-t 1.1.1.1:1 -m sslid' } };
    my $r = { option => { flags => '-r 2.2.2.2:2' } };
    my $weight = 10;
    ld_delete_real($v, $r, $weight);
    is_deeply \@command_wrapper_args, ['l7vsadm -d -t 1.1.1.1:1 -m sslid -r 2.2.2.2:2 2>&1'], 'ld_delete_real - ok';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
    $main::PROC_ENV{l7vsadm} = 'l7vsadm';
}
sub override {
    *get_ip_port = \&__get_ip_port;
    *get_forward_flag = \&__get_forward_flag;
    *system_wrapper = \&__system_wrapper;
    *command_wrapper = \&__command_wrapper;
    *ld_log = \&__ld_log;
}
sub __get_ip_port {
    push @get_ip_port_args, @_;
    return shift @get_ip_port_returns;
}
sub __get_forward_flag {
    push @get_forward_flag_args, @_;
    return shift @get_forward_flag_returns;
}
sub __system_wrapper {
    push @system_wrapper_args, @_;
    return shift @system_wrapper_returns;
}
sub __command_wrapper {
    push @command_wrapper_args, @_;
    return shift @command_wrapper_returns;
}
sub __ld_log {
}
