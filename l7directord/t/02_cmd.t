use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use L7lib;
use Test::More tests => 73;
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();
default_value();

our $signal = undef;
our $read_config_called = 0;
our $ld_log_called = 0;
our @read_pid_return = ();
our @children_command = ();
our $system_command = undef;
our $cmd_start_return = undef;
our $cmd_stop_return = undef;
our $set_ld_handler_called = 0;
our $ld_setup_called = 0;
our $ld_daemon_called = 0;
our $run_pid = 0;
our $ld_main_called = 0;
our $remove_pid_called = 0;

close *STDOUT;
open  *STDOUT, '>', '/dev/null';
close *STDERR;
open  *STDERR, '>', '/dev/null';

#...............................................
# test start
#   - cmd_start
{
    local $set_ld_handler_called = 0;
    local $read_config_called    = 0;
    local $ld_setup_called       = 0;
    local @read_pid_return       = ();
    local $ld_log_called         = 0;
    local $ld_daemon_called      = 0;
    local $run_pid               = 0;
    local @children_command      = ();
    local $remove_pid_called     = 0;
    my $got = cmd_start();
    is $got, 0, 'cmd_start - run as daemon';
    is $set_ld_handler_called, 1, 'cmd_start - set_ld_handler';
    is $read_config_called, 1, 'cmd_start - read_config';
    is $ld_setup_called, 1, 'cmd_start - ld_setup';
    is $ld_log_called, 2, 'cmd_start - ld_log';
    is $ld_daemon_called, 1, 'cmd_start - ld_daemon';
    is $main::PROC_STAT{pid}, $$, 'cmd_start - save pid';
    is $run_pid, $$, 'cmd_start - write_pid';
    is_deeply \@children_command, ['start', 'stop'], 'cmd_start - ld_cmd_children';
    is $ld_main_called, 1, 'cmd_start - ld_main';
    is $remove_pid_called, 1, 'cmd_start - remove_pid';
}
{
    local $set_ld_handler_called = 0;
    local $read_config_called    = 0;
    local $ld_setup_called       = 0;
    local @read_pid_return       = ();
    local $ld_log_called         = 0;
    local $ld_daemon_called      = 0;
    local $run_pid               = 0;
    local @children_command      = ();
    local $ld_main_called        = 0;
    local $remove_pid_called     = 0;
    local $main::CONFIG{supervised} = 1;
    local $main::DEBUG_LEVEL     = 0;
    local $main::PROC_STAT{pid}  = 10000;
    my $got = cmd_start();
    is $got, 0, 'cmd_start - run supervised mode';
    is $set_ld_handler_called, 1, 'cmd_start - set_ld_handler';
    is $read_config_called, 1, 'cmd_start - read_config';
    is $ld_setup_called, 1, 'cmd_start - ld_setup';
    is $ld_log_called, 2, 'cmd_start - ld_log';
    is $ld_daemon_called, 0, 'cmd_start - no ld_daemon';
    is $run_pid, 10000, 'cmd_start - write_pid';
    is_deeply \@children_command, ['start', 'stop'], 'cmd_start - ld_cmd_children';
    is $ld_main_called, 1, 'cmd_start - ld_main';
    is $remove_pid_called, 1, 'cmd_start - remove_pid';
}
{
    local $set_ld_handler_called = 0;
    local $read_config_called    = 0;
    local $ld_setup_called       = 0;
    local @read_pid_return       = ();
    local $ld_log_called         = 0;
    local $ld_daemon_called      = 0;
    local $run_pid               = 0;
    local @children_command      = ();
    local $ld_main_called        = 0;
    local $remove_pid_called     = 0;
    local $main::CONFIG{supervised} = 0;
    local $main::DEBUG_LEVEL     = 3;
    local $main::PROC_STAT{pid}  = 20000;
    my $got = cmd_start();
    is $got, 0, 'cmd_start - run debug mode';
    is $set_ld_handler_called, 1, 'cmd_start - set_ld_handler';
    is $read_config_called, 1, 'cmd_start - read_config';
    is $ld_setup_called, 1, 'cmd_start - ld_setup';
    is $ld_log_called, 2, 'cmd_start - ld_log';
    is $ld_daemon_called, 0, 'cmd_start - no ld_daemon';
    is $run_pid, 20000, 'cmd_start - write_pid';
    is_deeply \@children_command, ['start', 'stop'], 'cmd_start - ld_cmd_children';
    is $ld_main_called, 1, 'cmd_start - ld_main';
    is $remove_pid_called, 1, 'cmd_start - remove_pid';
}
{
    local $set_ld_handler_called = 0;
    local $read_config_called    = 0;
    local $ld_setup_called       = 0;
    local @read_pid_return       = (100);
    my $got = cmd_start();
    is $got, 1, 'cmd_start - already running';
    is $set_ld_handler_called, 1, 'cmd_start - set_ld_handler';
    is $read_config_called, 1, 'cmd_start - read_config';
    is $ld_setup_called, 1, 'cmd_start - ld_setup';
}
#   - cmd_stop
{
    local @read_pid_return = ();
    my $got = cmd_stop();
    is $got, 2, 'cmd_stop - not running(1)';
}
{
    local @read_pid_return = (undef, undef);
    my $got = cmd_stop();
    is $got, 2, 'cmd_stop - not running(2)';
}
{
    local @read_pid_return = (undef, 100);
    my $got = cmd_stop();
    is $got, 2, 'cmd_stop - stale pid exist';
}
##{
##    local $SIG{TERM} = sub { $signal = shift; };
##    local @read_pid_return = ($$);
##    my $got = cmd_stop();
##    is $got, 0, 'cmd_stop - running';
##    is $signal, 'TERM', 'cmd_stop - signal send';
##}
{
    local @read_pid_return = (99999);
    local $ld_log_called = 0;
    my $got = cmd_stop();
    is $got, 3, 'cmd_stop - signal error';
    is $ld_log_called, 0, 'cmd_stop - ld_log';
}
#   - cmd_restart
{
    local *cmd_start = \&__cmd_start;
    local *cmd_stop  = \&__cmd_stop;
    {
        local $cmd_stop_return  = 0;
        local $cmd_start_return = 0;
        my $got = cmd_restart();
        is $got, 0, 'cmd_restart - stop success, start success';
    }
    {
        local $cmd_stop_return  = 0;
        local $cmd_start_return = 1;
        my $got = cmd_restart();
        is $got, 1, 'cmd_restart - stop success, start fail';
    }
    {
        local $cmd_stop_return  = 1;
        local $cmd_start_return = 0;
        my $got = cmd_restart();
        is $got, 0, 'cmd_restart - stop fail, start success';
    }
    {
        local $cmd_stop_return  = 1;
        local $cmd_start_return = 1;
        my $got = cmd_restart();
        is $got, 1, 'cmd_restart - stop fail, start fail';
    }
}
#   - cmd_try_restart
{
    local *cmd_start = \&__cmd_start;
    local *cmd_stop  = \&__cmd_stop;
    {
        local $cmd_stop_return  = 0;
        local $cmd_start_return = 0;
        my $got = cmd_try_restart();
        is $got, 0, 'cmd_try_restart - stop success, start success';
    }
    {
        local $cmd_stop_return  = 0;
        local $cmd_start_return = 1;
        my $got = cmd_try_restart();
        is $got, 1, 'cmd_try_restart - stop success, start fail';
    }
    {
        local $cmd_stop_return  = 2;
        my $got = cmd_try_restart();
        is $got, 2, 'cmd_try_restart - stop fail';
    }
}
#   - cmd_reload
{
    local $read_config_called = 0;
    local @read_pid_return = ();
    my $got = cmd_reload();
    is $got, 2, 'cmd_reload - not running(1)';
    is $read_config_called, 1, 'cmd_reload - read_config';
}
{
    local $read_config_called = 0;
    local @read_pid_return = (undef, undef);
    my $got = cmd_reload();
    is $got, 2, 'cmd_reload - not running(2)';
    is $read_config_called, 1, 'cmd_reload - read_config';
}
{
    local $read_config_called = 0;
    local @read_pid_return = (undef, 100);
    my $got = cmd_reload();
    is $got, 2, 'cmd_reload - stale pid exist';
    is $read_config_called, 1, 'cmd_reload - read_config';
}
{
    local $SIG{HUP} = sub { $signal = shift; };
    local $read_config_called = 0;
    local @read_pid_return = ($$);
    my $got = cmd_reload();
    is $got, 0, 'cmd_reload - running';
    is $read_config_called, 1, 'cmd_reload - read_config';
    is $signal, 'HUP', 'cmd_reload - signal send';
}
{
    local $read_config_called = 0;
    local $ld_log_called = 0;
    local @read_pid_return = (99999);
    my $got = cmd_reload();
    is $got, 3, 'cmd_reload - signal error';
    is $read_config_called, 1, 'cmd_reload - read_config';
    is $ld_log_called, 0, 'cmd_reload - ld_log';
}
#   - cmd_status
{
    local @read_pid_return = ();
    local @children_command = ();
    my $got = cmd_status();
    is $got, 2, 'cmd_status - not running(1)';
    is_deeply \@children_command, ['status'], 'status - ld_cmd_children';
}
{
    local @read_pid_return = (undef, undef);
    local @children_command = ();
    my $got = cmd_status();
    is $got, 2, 'cmd_status - not running(2)';
    is_deeply \@children_command, ['status'], 'status - ld_cmd_children';
}
{
    local @children_command = ();
    local @read_pid_return = (999);
    my $got = cmd_status();
    is $got, 0, 'cmd_status - running';
    is_deeply \@children_command, ['status'], 'status - ld_cmd_children';
}
{
    local @children_command = ();
    local @read_pid_return = (undef, 1000);
    my $got = cmd_status();
    is $got, 2, 'cmd_status - stale pid exist';
    is_deeply \@children_command, ['status'], 'status - ld_cmd_children';
}
#   - cmd_configtest
{
    local $read_config_called = 0;
    my $got = cmd_configtest();
    is $got, 0, 'cmd_configtest';
    is $read_config_called, 1, 'configtest - read_config';
}
#   - cmd_version
{
    my $got = cmd_version();
    is $got, 0, 'cmd_version';
}
#   - cmd_help
{
    local $system_command = undef;
    $main::PROC_ENV{l7directord} = '_l7directord_';
    my $got = cmd_help();
    is $got, 0, 'cmd_help';
    is $system_command, '/usr/bin/perldoc _l7directord_', 'perldoc command';
}
#   - cmd_usage
{
    my $got = cmd_usage();
    is $got, 0, 'cmd_usage';
}


# test end
#...............................................

L7lib::comment_in();

sub default_value {
    $main::CONFIG_FILE{filename} = 'filename';
    $main::CONFIG_FILE{path}     = '/path/filename';
    $main::CONFIG{supervised}    = 0;
    $main::DEBUG_LEVEL           = 0;
}
sub override {
    *set_ld_handler  = \&__set_ld_handler;
    *read_config     = \&__read_config;
    *ld_setup        = \&__ld_setup;
    *read_pid        = \&__read_pid;
    *write_pid       = \&__write_pid;
    *ld_cmd_children = \&__ld_cmd_children;
    *ld_main         = \&__ld_main;
    *remove_pid      = \&__remove_pid;
    *system_wrapper  = \&__system_wrapper;
    *ld_log          = \&__ld_log;
    *ld_daemon       = \&__ld_daemon;
}
sub __cmd_start {
    return $cmd_start_return;
}
sub __cmd_stop {
    return $cmd_stop_return;
}
sub __set_ld_handler {
    $set_ld_handler_called++;
}
sub __read_config {
    $read_config_called++;
}
sub __ld_setup {
    $ld_setup_called++;
}
sub __read_pid {
    return wantarray ? @read_pid_return : $read_pid_return[0];
}
sub __write_pid {
    $run_pid = shift;
}
sub __ld_cmd_children {
    push @children_command, @_;
}
sub __ld_main {
    $ld_main_called++;
}
sub __remove_pid {
    $remove_pid_called++;
}
sub __system_wrapper {
    $system_command = shift;
}
sub __ld_log {
    $ld_log_called++;
}
sub __ld_daemon {
    $ld_daemon_called++;
}
