use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use subs qw(fork);
use Test::More tests => 11;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our @check_func_args = ();
our @check_func_returns = ();
our @get_status_args = ();
our @get_status_returns = ();
our @get_ip_port_args = ();
our @get_ip_port_returns = ();
our @get_check_func_args = ();
our @get_check_func_returns = ();
our @service_set_args = ();
our @service_set_returns = ();
our @check_connect_args = ();
our @check_connect_returns = ();
our @sleep_and_check_signal_args = ();
our @sleep_and_check_signal_returns = ();
our $health_check_called = 0;
#...............................................
# test start
#   - health_check
{
    set_default();
    eval {
        health_check();
    };
    is $@, "1\n", 'health_check - no virtual real list';
}
{
    set_default();
    local @get_status_returns = ();
    local @get_check_func_returns = ( \&check_func );
    my $v = {};
    my $r = {};
    eval {
        health_check([ [] ]);
    };
    is $@, "2\n", 'health_check - no virtual real';
}
{
    set_default();
    local @get_status_returns = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_check_func_returns = ( \&check_func );
    local @check_func_returns = ( $main::SERVICE_DOWN );
    local @sleep_and_check_signal_returns = ( 'halt' );
    local @service_set_args = ();
    my $v = {
        checktype => 'connect',
        checkcount => 1,
        retryinterval => 5,
        checkinterval => 8,
             };
    my $r = {};
    eval {
        health_check([ [$v, $r] ]);
    };
    is $@, "0\n", 'health_check - no virtual real';
    is_deeply \@service_set_args, [ [[$v,$r]], 'down' ], 'health_check - service set down';
}
{
    set_default();
    local @get_status_returns = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_check_func_returns = ( \&check_func );
    local @check_func_returns = ( $main::SERVICE_UP,
                                  $main::SERVICE_DOWN,
                                  $main::SERVICE_DOWN,
                                  $main::SERVICE_DOWN,
                                );
    local @sleep_and_check_signal_returns = ( 'run',
                                              'run',
                                              'run',
                                              'halt',
                                            );
    local @service_set_args = ();
    my $v = {
        checktype => 'connect',
        checkcount => 3,
        retryinterval => 5,
        checkinterval => 8,
             };
    my $r = {};
    eval {
        health_check([ [$v, $r] ]);
    };
    is $@, "0\n", 'health_check - no virtual real';
    is_deeply \@service_set_args, [ [[$v,$r]], 'up', [[$v,$r]], 'down' ], 'health_check - service set up and down';
    is_deeply \@sleep_and_check_signal_args, [8,1,5,1,5,1,8,1], 'health_check - sleep time';
}
{
    set_default();
    local @get_status_returns = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_check_func_returns = ( \&check_func );
    local @check_func_returns = ( $main::SERVICE_DOWN,
                                  $main::SERVICE_DOWN,
                                  $main::SERVICE_UP,
                                  $main::SERVICE_DOWN,
                                );
    local @sleep_and_check_signal_returns = ( 'run',
                                              'run',
                                              'run',
                                              'halt',
                                            );
    local @service_set_args = ();
    my $v = {
        checktype => 'connect',
        checkcount => 3,
        retryinterval => 5,
        checkinterval => 8,
             };
    my $r = {};
    eval {
        health_check([ [$v, $r] ]);
    };
    is $@, "0\n", 'health_check - no virtual real';
    is_deeply \@service_set_args, [ [[$v,$r]], 'up' ], 'health_check - service set up and down';
    is_deeply \@sleep_and_check_signal_args, [5,1,5,1,8,1,5,1], 'health_check - sleep time';
}
{
    set_default();
    local $main::PROC_STAT{parent_pid} = 100000;
    local @get_status_returns = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_check_func_returns = ( \&check_func );
    local @check_func_returns = ( $main::SERVICE_DOWN );
    local @sleep_and_check_signal_returns = ( 'run' );
    local @service_set_args = ();
    my $v = {
        checktype => 'connect',
        checkcount => 3,
        retryinterval => 5,
        checkinterval => 8,
             };
    my $r = {};
    eval {
        health_check([ [$v, $r] ]);
    };
    is $@, "3\n", 'health_check - parent process is down';
}
# test end
#...............................................

L7lib::comment_in();

sub check_func {
    push @check_func_args, @_;
    return shift @check_func_returns;
}
sub set_default {
    $main::PROC_STAT{parent_pid} = $$;
    @get_status_args = ();
    @get_status_returns = ();
    @get_check_func_args = ();
    @get_check_func_returns = ();
    @get_ip_port_args = ();
    @get_ip_port_returns = ();
    @service_set_args = ();
    @service_set_returns = ();
    @check_connect_args = ();
    @check_connect_returns = ();
    $health_check_called = 0;
    @sleep_and_check_signal_args = ();
    @sleep_and_check_signal_returns = ();
}
sub override {
    *POSIX::_exit = \&__POSIX__exit;
    *get_status = \&__get_status;
    *get_check_func = \&__get_check_func;
    *service_set = \&__service_set;
    *check_connect = \&__check_connect;
    *sleep_and_check_signal = \&__sleep_and_check_signal;
    *ld_log = \&__ld_log;
}
sub __get_status {
    push @get_status_args, @_;
    return shift @get_status_returns;
}
sub __get_ip_port {
    push @get_ip_port_args, @_;
    return shift @get_ip_port_returns;
}
sub __get_check_func {
    push @get_check_func_args, @_;
    return shift @get_check_func_returns;
}
sub __service_set {
    push @service_set_args, @_;
    return shift @service_set_returns;
}
sub __check_connect {
    push @get_status_args, @_;
    return shift @get_status_returns;
}
sub __sleep_and_check_signal {
    push @sleep_and_check_signal_args, @_;
    return shift @sleep_and_check_signal_returns;
}
sub __POSIX__exit {
    die "$_[0]\n";
}
sub __ld_log {
}
