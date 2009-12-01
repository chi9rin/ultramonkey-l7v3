use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use subs qw(fork chdir);
use Test::More tests => 29;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our $health_check_called = 0;
our $fork_return = -1;
our $fork_called = 0;
our $ld_exit_called = 0;
our $setsid_return = 0;
our $chdir_return = 0;
#...............................................
# test start
#   - check_child_process
{
    set_default();
    my @got = check_child_process();
    is_deeply \@got, [], 'check_child_process - no process';
}
{
    set_default();
    %main::HEALTH_CHECK = ( id1 => { pid => $$ }, id2 => { pid => undef }, id3 => {pid => 100000 } );
    my @got = check_child_process();
    is_deeply \@got, ['id2', 'id3'], 'check_child_process - valid process, no process, invalid process';
}
#   - create_check_process
{
    set_default();
    create_check_process();
    is $fork_called, 0, 'create_check_process - no list';
    is $health_check_called, 0, 'create_check_process - no list';
}
{
    set_default();
    $fork_return = -1;
    create_check_process('id1', 'id2');
    is $fork_called, 2, 'create_check_process - fork error';
    is $health_check_called, 0, 'create_check_process - fork error';
    is_deeply \%main::HEALTH_CHECK, {}, 'create_check_process - fork error';
}
{
    set_default();
    $fork_return = 0;
    create_check_process('id1');
    is $fork_called, 1, 'create_check_process - fork child';
    is $health_check_called, 1, 'create_check_process - fork child';
    is_deeply \%main::HEALTH_CHECK, { id1 => {} }, 'create_check_process - fork child';
}
{
    set_default();
    $fork_return = 1000;
    create_check_process('id1', 'id2');
    is $fork_called, 2, 'create_check_process - fork parent';
    is $health_check_called, 0, 'create_check_process - fork parent';
    is_deeply \%main::HEALTH_CHECK, { id1 => {pid => 1000}, id2 => {pid => 1000} }, 'create_check_process - fork parent';
}
#   - ld_daemon_become_child
{
    set_default();
    local $fork_return = 0;
    eval { ld_daemon_become_child(); };
    is $fork_called, 1, 'ld_daemon_become_child - fork child';
    is $ld_exit_called, 0, 'ld_daemon_become_child - become child';
}
{
    set_default();
    local $fork_return = 1;
    eval { ld_daemon_become_child(); };
    is $fork_called, 1, 'ld_daemon_become_child - fork parent';
    is $ld_exit_called, 1, 'ld_daemon_become_child - parent ld_exit';
}
{
    set_default();
    local $fork_return = -1;
    eval { ld_daemon_become_child(); };
    is $fork_called, 1, 'ld_daemon_become_child - fork error';
    is $ld_exit_called, 1, 'ld_daemon_become_child - fork error ld_exit';
}
#   - ld_daemon
SKIP: {
    skip "need root privilege", 2 if ($> != 0);
    set_default();
    local $fork_return = 0;
    eval { ld_daemon(); };
    is $fork_called, 2, 'ld_daemon - fork child 2 times';
    is $ld_exit_called, 0, 'ld_daemon - become daemon';
}
{
    set_default();
    local $fork_return = 1;
    eval { ld_daemon(); };
    is $fork_called, 1, 'ld_daemon - fork parent';
    is $ld_exit_called, 1, 'ld_daemon - cannot become daemon';
}
{
    set_default();
    local $fork_return = -1;
    eval { ld_daemon(); };
    is $fork_called, 1, 'ld_daemon - fork error';
    is $ld_exit_called, 1, 'ld_daemon - cannot become daemon';
}
{
    set_default();
    local $fork_return = 0;
    local $setsid_return = -1;
    eval { ld_daemon(); };
    is $fork_called, 1, 'ld_daemon - setsid error';
    is $ld_exit_called, 1, 'ld_daemon - cannot become daemon';
}
{
    set_default();
    local $fork_return = 0;
    local $chdir_return = -1;
    eval { ld_daemon(); };
    is $fork_called, 2, 'ld_daemon - chdir error';
    is $ld_exit_called, 1, 'ld_daemon - cannot become daemon';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
    %main::HEALTH_CHECK = ();
    $health_check_called = 0;
    $fork_return = -1;
    $fork_called = 0;
    $ld_exit_called = 0;
}
sub override {
    *health_check = \&__health_check;
    *ld_log = \&__ld_log;
    *ld_exit = \&__ld_exit;
    *fork = \&__fork;
    *chdir = \&__chdir;
    *POSIX::setsid = \&__setsid;
}
sub __health_check {
    $health_check_called++;
}
sub __fork {
    $fork_called++;
    return $fork_return;
}
sub __ld_exit {
    $ld_exit_called++;
    die @_;
}
sub __setsid {
    return $setsid_return;
}
sub __chdir {
    return $chdir_return;
}
sub __ld_log {
}
