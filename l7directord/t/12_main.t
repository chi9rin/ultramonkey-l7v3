use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 29;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our $ld_start_called = 0;
our $ld_stop_called = 0;
our $reread_config_called = 0;
our @check_child_process_args = ();
our @check_child_process_returns = ();
our @create_check_process_args = ();
our @create_check_process_returns = ();
our @sleep_and_check_signal_args = ();
our @sleep_and_check_signal_returns = ();
our $check_cfgfile_called = 0;
our @check_cfgfile_returns = ();
#...............................................
# test start
#   - ld_main
{
    set_default();
    local @check_child_process_returns = ([]);
    local @sleep_and_check_signal_returns = ('halt');
    local $SIG{TERM} = sub { is $_[0], 'TERM', 'catch sigterm'; };
    $main::HEALTH_CHECK{id}{pid} = $$;
    ld_main();
    is $ld_start_called, 1, 'ld_main - start called';
    is $reread_config_called, 0, 'ld_main - reread_config called';
    is $check_cfgfile_called, 0, 'ld_main - check_cfgfile called';
    is $ld_stop_called, 1, 'ld_main - stop called';
}
{
    set_default();
    local @check_child_process_returns = (['id1', 'id2']);
    local @sleep_and_check_signal_returns = ('halt');
    local $SIG{TERM} = sub { is $_[0], 'TERM', 'catch sigterm'; };
    $main::HEALTH_CHECK{id}{pid} = $$;
    ld_main();
    is $ld_start_called, 1, 'ld_main - start called';
    is $reread_config_called, 0, 'ld_main - reread_config called';
    is $ld_stop_called, 1, 'ld_main - stop called';
    is $check_cfgfile_called, 0, 'ld_main - check_cfgfile called';
    is_deeply \@create_check_process_args, ['id1', 'id2'], 'ld_main - create_check_process args';
}
{
    set_default();
    local @check_child_process_returns = (['id1', 'id2'], [], []);
    local @sleep_and_check_signal_returns = (undef, undef , 'halt');
    local @create_check_process_args = ();
    local $SIG{TERM} = sub { is $_[0], 'TERM', 'catch sigterm'; };
    $main::HEALTH_CHECK{id}{pid} = $$;
    ld_main();
    is $ld_start_called, 1, 'ld_main - start called';
    is $reread_config_called, 0, 'ld_main - reread_config called';
    is $ld_stop_called, 1, 'ld_main - stop called';
    is $check_cfgfile_called, 2, 'ld_main - check_cfgfile called';
    is_deeply \@create_check_process_args, ['id1', 'id2'], 'ld_main - create_check_process args';
}
{
    set_default();
    local @check_child_process_returns = (['id1', 'id2'], [], []);
    local @sleep_and_check_signal_returns = (undef, 'reload' , 'halt');
    local @create_check_process_args = ();
    local $SIG{TERM} = sub { is $_[0], 'TERM', 'catch sigterm'; };
    $main::HEALTH_CHECK{id}{pid} = $$;
    ld_main();
    is $ld_start_called, 1, 'ld_main - start called';
    is $reread_config_called, 1, 'ld_main - reread_config called';
    is $ld_stop_called, 1, 'ld_main - stop called';
    is $check_cfgfile_called, 1, 'ld_main - check_cfgfile called';
    is_deeply \@create_check_process_args, ['id1', 'id2'], 'ld_main - create_check_process args';
}
{
    set_default();
    local @check_child_process_returns = (['id1', 'id2'], [], []);
    local @sleep_and_check_signal_returns = (undef, 'reload' , 'halt');
    local @check_cfgfile_returns = (1);
    local @create_check_process_args = ();
    local $SIG{TERM} = sub { is $_[0], 'TERM', 'catch sigterm'; };
    $main::HEALTH_CHECK{id}{pid} = $$;
    ld_main();
    is $ld_start_called, 1, 'ld_main - start called';
    is $reread_config_called, 2, 'ld_main - reread_config called';
    is $ld_stop_called, 1, 'ld_main - stop called';
    is $check_cfgfile_called, 1, 'ld_main - check_cfgfile called';
    is_deeply \@create_check_process_args, ['id1', 'id2'], 'ld_main - create_check_process args';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
    %main::CONFIG = %main::GLOBAL;
    %main::HEALTH_CHECK = ();
    $ld_start_called = 0;
    $ld_stop_called = 0;
    $reread_config_called = 0;
    $check_cfgfile_called = 0;
}
sub override {
    *ld_start = \&__ld_start;
    *check_child_process = \&__check_child_process;
    *create_check_process = \&__create_check_process;
    *sleep_and_check_signal = \&__sleep_and_check_signal;
    *check_cfgfile = \&__check_cfgfile;
    *reread_config = \&__reread_config;
    *ld_stop = \&__ld_stop;
    *ld_log = \&__ld_log;
}
sub __ld_start {
    $ld_start_called++;
}
sub __check_child_process {
    push @check_child_process_args, @_;
    my $return = shift @check_child_process_returns;
    return @$return;
}
sub __create_check_process {
    push @create_check_process_args, @_;
    return shift @create_check_process_returns;
}
sub __sleep_and_check_signal {
    push @sleep_and_check_signal_args, @_;
    return shift @sleep_and_check_signal_returns;
}
sub __check_cfgfile {
    $check_cfgfile_called++;
    return shift @check_cfgfile_returns;
}
sub __reread_config {
    $reread_config_called++;
}
sub __ld_stop {
    $ld_stop_called++;
}
sub __ld_log {
}
