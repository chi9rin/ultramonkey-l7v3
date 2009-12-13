use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(exit print);
use Cwd;
use L7lib;
use Test::More tests => 33;
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our $ld_log_called = 0;

close *STDOUT;
open  *STDOUT, '>', '/dev/null';
close *STDERR;
open  *STDERR, '>', '/dev/null';

#...............................................
# test start
#   - ld_openlog
{
    local *ld_log = \&__ld_log;
    my $got = ld_openlog();
    is $got, 1, 'ld_openlog - log_config is undef';
}
{
    local *ld_log = \&__ld_log;
    local $main::DEBUG_LEVEL = 1;
    my $got = ld_openlog('logfile');
    is $got, 0, 'ld_openlog - debug mode';
}
{
    local *ld_log = \&__ld_log;
    local $main::CONFIG{supervised} = 1;
    my $got = ld_openlog('logfile');
    is $got, 0, 'ld_openlog - supervised mode';
}
{
    local *ld_log = \&__ld_log;
    my $log_config = '/tmp/' . time;
    my $got = ld_openlog($log_config);
    is $got, 0, 'ld_openlog - open normal logfile';
    unlink $log_config;
}
{
    local *ld_log = \&__ld_log;
    my $log_config = '/tmp';
    my $got = ld_openlog($log_config);
    is $got, 1, 'ld_openlog - cannot open normal logfile';
}
{
    local *ld_log = \&__ld_log;
    my $log_config = 'local0';
    my $got = ld_openlog($log_config);
    is $got, 0, 'ld_openlog - open syslog';
}
#   - ld_log
SKIP: {
    skip 'aboid logging', 1;
    my $got = ld_log();
    is $got, 1, 'ld_log - message is undef';
}
{
    local $main::PROC_STAT{pid}  = 12345;
    local $main::CONFIG{logfile} = '/tmp/' . time;
    my $got = ld_log('test');
    is $got, 0, 'ld_log - write message to logfile';
    unlink $main::CONFIG{logfile};
}
{
    local $main::PROC_STAT{pid}  = 12345;
    local $main::CONFIG{logfile} = '/tmp';
    my $got = ld_log('test');
    is $got, 1, 'ld_log - cannot write message to logfile';
}
{
    local $main::PROC_STAT{pid}  = 12345;
    local $main::CONFIG{logfile} = 'local0';
    my $got = ld_log('test');
    is $got, 0, 'ld_log - write message to syslog';
}
{
    local $main::CONFIG{supervised} = 1;
    local $main::PROC_STAT{pid}  = 12345;
    local $main::CONFIG{logfile} = 'local0';
    my $got = ld_log('test');
    is $got, 0, 'ld_log - write message to stderr(supervised mode)';
}
#   - ld_debug
{
    local *ld_log = \&__ld_log;
    ld_debug();
    pass 'ld_debug - args is undef';
}
{
    local *ld_log = \&__ld_log;
    ld_debug(undef, 'test');
    pass 'ld_debug - priority is undef';
}
{
    local *ld_log = \&__ld_log;
    ld_debug('fatal', 'test');
    pass 'ld_debug - priority is string';
}
{
    local *ld_log = \&__ld_log;
    ld_debug(3, undef);
    pass 'ld_debug - message is undef';
}
{
    local *ld_log = \&__ld_log;
    local $main::DEBUG_LEVEL = 10;
    ld_debug(3, 'debug');
    pass 'ld_debug - priority and message is ok';
}
#   - ld_exit
{
    local *ld_log = \&__ld_log;
    local $main::DEBUG_LEVEL = 0;
    local $ld_log_called = 0;
    ld_exit();
    is $ld_log_called, 0, 'ld_exit - args is undef';
}
{
    local *ld_log = \&__ld_log;
    local $main::DEBUG_LEVEL = 0;
    local $ld_log_called = 0;
    ld_exit(undef, 'exit');
    is $ld_log_called, 0, 'ld_exit - status is undef';
}
{
    local *ld_log = \&__ld_log;
    local $main::DEBUG_LEVEL = 0;
    local $ld_log_called = 0;
    ld_exit(1, undef);
    is $ld_log_called, 0, 'ld_exit - message is undef';
}
{
    local *ld_log = \&__ld_log;
    local $main::DEBUG_LEVEL = 0;
    local $ld_log_called = 0;
    ld_exit(0, 'exit');
    is $ld_log_called, 1, 'ld_exit - status is 0 (normal exit)';
}
{
    local *ld_log = \&__ld_log;
    local $main::DEBUG_LEVEL = 3;
    local $ld_log_called = 0;
    ld_exit(0, 'exit');
    is $ld_log_called, 1, 'ld_exit - debug enable';
}
{
    local *ld_log = \&__ld_log;
    local $main::DEBUG_LEVEL = 0;
    local $ld_log_called = 0;
    ld_exit(1, 'exit');
    is $ld_log_called, 1, 'ld_exit - status is 1 (some error)';
}
#   - init_error
{
    local *ld_log = \&__ld_log;
    local *ld_exit = \&__ld_exit;
    local $main::DEBUG_LEVEL = 0;
    local $ld_log_called = 0;
    init_error();
    is $ld_log_called, 0, 'init_error - message is undef';
}
{
    local *ld_log = \&__ld_log;
    local *ld_exit = \&__ld_exit;
    local $main::DEBUG_LEVEL = 0;
    local $ld_log_called = 0;
    init_error('init error');
    is $ld_log_called, 1, 'init_error - no debug';
}
{
    local *ld_log = \&__ld_log;
    local *ld_exit = \&__ld_exit;
    local $main::DEBUG_LEVEL = 1;
    local $ld_log_called = 0;
    init_error('init error');
    is $ld_log_called, 1, 'init_error - debug mode';
}
#   - config_error
{
    local *ld_log  = \&__ld_log;
    local *ld_exit = \&__ld_exit;
    local $main::CONFIG_FILE{path} = 'filepath';
    local $main::PROC_STAT{initialized} = 1;
    local $main::DEBUG_LEVEL = 1;
    local $ld_log_called = 0;
    eval { config_error(1, 'WRN0004'); };
    is $ld_log_called, 0, 'config_error - debug mode';
    ok $@, 'config_error - initialized and died';
}
{
    local *ld_log  = \&__ld_log;
    local *ld_exit = \&__ld_exit;
    local $main::CONFIG_FILE{path} = 'filepath';
    local $main::PROC_STAT{initialized} = 0;
    local $main::DEBUG_LEVEL = 1;
    local $ld_log_called = 0;
    eval { config_error(1, 'WRN0004'); };
    is $ld_log_called, 0, 'config_error - debug mode';
    is $@, '', 'config_error - not initialized exit';
}
{
    local *ld_log  = \&__ld_log;
    local *ld_exit = \&__ld_exit;
    local $main::CONFIG_FILE{path} = 'filepath';
    local $main::PROC_STAT{initialized} = 1;
    local $main::DEBUG_LEVEL = 0;
    local $ld_log_called = 0;
    eval { config_error(0, 'WRN0004'); };
    is $ld_log_called, 1, 'config_error - no debug mode and line is 0';
    ok $@, 'config_error - initialized and died';
}
{
    local *ld_log  = \&__ld_log;
    local *ld_exit = \&__ld_exit;
    local $main::CONFIG_FILE{path} = 'filepath';
    local $main::PROC_STAT{initialized} = 1;
    local $main::DEBUG_LEVEL = 0;
    local $ld_log_called = 0;
    eval { config_error(1, 'WRN0004'); };
    is $ld_log_called, 2, 'config_error - no debug mode and line is 1';
    ok $@, 'config_error - initialized and died';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
}
sub override {
    *exit = \&__exit;
}
sub __exit {
}
sub __ld_exit {
}
sub __ld_log {
    $ld_log_called++;
}
