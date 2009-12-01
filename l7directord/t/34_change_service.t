use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 27;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our $get_virtual_id_str_arg    = undef;
our $get_virtual_id_str_return = undef;
our $ld_read_l7vsadm_return    = undef;
our $ld_add_real_arg           = ();
our $ld_edit_real_arg          = ();
our $ld_delete_real_arg        = ();
our $system_wrapper_arg        = ();

#...............................................
# test start
#   - _remove_service
{
    set_default();
    my $v = undef;
    my $r = undef;
    my $tag = undef;
    _remove_service($v, $r, $tag);
    pass '_remove_service - args is undef';
}
{
    set_default();
    my $v = {};
    my $r = {};
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = {};
    _remove_service($v, $r, $tag);
    pass '_remove_service - not exist vip id in l7vsadm';
}
{
    set_default();
    my $v = { quiescent => 0, module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => {} };
    _remove_service($v, $r, $tag);
    pass '_remove_service - not quiescent and not exist real';
}
{
    set_default();
    my $v = { quiescent => 1, module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 0, option => { forward => '-m' } } } };
    _remove_service($v, $r, $tag);
    pass '_remove_service - quiescent and exist real and weight 0, same forward mode';
}
{
    set_default();
    my $v = { quiescent => 1, module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 1, option => { forward => '-m' } } } };
    _remove_service($v, $r, $tag);
    is_deeply $ld_edit_real_arg, [$v, $r, 0], '_remove_service - quiescent and exist real, no realdowncallback';
}
{
    set_default();
    my $v = { quiescent => 1, realdowncallback => '/bin/sh', module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 1, option => { forward => '-m' } } } };
    _remove_service($v, $r, $tag);
    is_deeply $ld_edit_real_arg, [$v, $r, 0], '_remove_service - quiescent and exist real, realdowncallback, first healthcheck';
}
{
    set_default();
    my $v = { quiescent => 1, realdowncallback => '/bin/sh', module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' }, healthchecked => 1 };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 1, option => { forward => '-m' } } } };
    _remove_service($v, $r, $tag);
    is_deeply $ld_edit_real_arg, [$v, $r, 0], '_remove_service - quiescent and exist real, realdowncallback, not first healthcheck';
    is_deeply $system_wrapper_arg, ['/bin/sh', '127.0.0.1:80'], '_remove_service - run realdowncallback';
}
{
    set_default();
    my $v = { quiescent => 1, module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => {} };
    _remove_service($v, $r, $tag);
    is_deeply $ld_add_real_arg, [$v, $r, 0], '_remove_service - quiescent and not exist real, no realdowncallback';
}
{
    set_default();
    my $v = { quiescent => 1, realdowncallback => '/bin/sh', module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => {} };
    _remove_service($v, $r, $tag);
    is_deeply $ld_add_real_arg, [$v, $r, 0], '_remove_service - quiescent and not exist real, realdowncallback, first healthcheck';
}
{
    set_default();
    my $v = { quiescent => 1, realdowncallback => '/bin/sh', module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' }, healthchecked => 1 };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => {} };
    _remove_service($v, $r, $tag);
    is_deeply $ld_add_real_arg, [$v, $r, 0], '_remove_service - quiescent and not exist real, realdowncallback, not first healthcheck';
    is_deeply $system_wrapper_arg, ['/bin/sh', '127.0.0.1:80'], '_remove_service - run realdowncallback';
}
{
    set_default();
    my $v = { quiescent => 0, module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 1, option => { forward => '-m' } } } };
    _remove_service($v, $r, $tag);
    is_deeply $ld_delete_real_arg, [$v, $r], '_remove_service - no quiescent and exist real, no realdowncallback';
}
{
    set_default();
    my $v = { quiescent => 0, realdowncallback => '/bin/sh', module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 1, option => { forward => '-m' } } } };
    _remove_service($v, $r, $tag);
    is_deeply $ld_delete_real_arg, [$v, $r], '_remove_service - no quiescent and exist real, realdowncallback, first healthcheck';
}
{
    set_default();
    my $v = { quiescent => 0, realdowncallback => '/bin/sh', module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, option => { forward => '-m' }, healthchecked => 1 };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 1, option => { forward => '-m' } } } };
    _remove_service($v, $r, $tag);
    is_deeply $ld_delete_real_arg, [$v, $r], '_remove_service - no quiescent and exist real, realdowncallback, not first healthcheck';
    is_deeply $system_wrapper_arg, ['/bin/sh', '127.0.0.1:80'], '_remove_service - run realdowncallback';
}
#   - _restore_service
{
    set_default();
    my $v = undef;
    my $r = undef;
    my $tag = undef;
    _restore_service($v, $r, $tag);
    pass '_restore_service - args is undef';
}
{
    set_default();
    my $v = {};
    my $r = {};
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = {};
    _restore_service($v, $r, $tag);
    pass '_restore_service - not exist vip id in l7vsadm';
}
{
    set_default();
    my $v = { module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, weight => 1, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 1, option => { forward => '-m' } } } };
    _restore_service($v, $r, $tag);
    pass '_restore_service - exist real and same weight, same forward mode';
}
{
    set_default();
    my $v = { module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, weight => 1, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 2, option => { forward => '-m' } } } };
    _restore_service($v, $r, $tag);
    is_deeply $ld_edit_real_arg, [$v, $r, 1], '_restore_service - change weight, no realrecovercallback';
}
{
    set_default();
    my $v = { module => { name => 'cinsert', key => '' }, realrecovercallback => '/bin/sh' };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, weight => 1, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 2, option => { forward => '-m' } } } };
    _restore_service($v, $r, $tag);
    is_deeply $ld_edit_real_arg, [$v, $r, 1], '_restore_service - change weight, realrecovercallback, first healthcheck';
}
{
    set_default();
    my $v = { module => { name => 'cinsert', key => '' }, realrecovercallback => '/bin/sh' };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, weight => 1, option => { forward => '-m' }, healthchecked => 1 };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => { '127.0.0.1:80' => { weight => 2, option => { forward => '-m' } } } };
    _restore_service($v, $r, $tag);
    is_deeply $ld_edit_real_arg, [$v, $r, 1], '_restore_service - change weight, realrecovercallback, not first healthcheck';
    is_deeply $system_wrapper_arg, ['/bin/sh', '127.0.0.1:80'], '_restore_service - run realrecovercallback';
}
{
    set_default();
    my $v = { module => { name => 'cinsert', key => '' } };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, weight => 1, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => {} };
    _restore_service($v, $r, $tag);
    is_deeply $ld_add_real_arg, [$v, $r, 1], '_restore_service - add real, no realrecovercallback';
}
{
    set_default();
    my $v = { module => { name => 'cinsert', key => '' }, realrecovercallback => '/bin/sh' };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, weight => 1, option => { forward => '-m' } };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => {} };
    _restore_service($v, $r, $tag);
    is_deeply $ld_add_real_arg, [$v, $r, 1], '_restore_service - add real, realrecovercallback, first healthcheck';
}
{
    set_default();
    my $v = { module => { name => 'cinsert', key => '' }, realrecovercallback => '/bin/sh' };
    my $r = { server => { ip => '127.0.0.1', port => 80 }, weight => 1, option => { forward => '-m' }, healthchecked => 1 };
    my $tag = undef;
    local $get_virtual_id_str_return = 'vipid';
    local $ld_read_l7vsadm_return    = { vipid => {} };
    _restore_service($v, $r, $tag);
    is_deeply $ld_add_real_arg, [$v, $r, 1], '_restore_service - add real, realrecovercallback, not first healthcheck';
    is_deeply $system_wrapper_arg, ['/bin/sh', '127.0.0.1:80'], '_restore_service - run realrecovercallback';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
    $get_virtual_id_str_arg    = undef;
    $get_virtual_id_str_return = undef;
    $ld_read_l7vsadm_return    = undef;
    $ld_add_real_arg           = ();
    $ld_edit_real_arg          = ();
    $ld_delete_real_arg        = ();
    $system_wrapper_arg        = ();
}
sub override {
    *ld_log             = \&__ld_log;
    *get_virtual_id_str = \&__get_virtual_id_str;
    *ld_read_l7vsadm    = \&__ld_read_l7vsadm;
    *ld_add_real        = \&__ld_add_real;
    *ld_edit_real       = \&__ld_edit_real;
    *ld_delete_real     = \&__ld_delete_real;
    *system_wrapper     = \&__system_wrapper;
}
sub __get_virtual_id_str {
    $get_virtual_id_str_arg = shift;
    return $get_virtual_id_str_return;
}
sub __ld_read_l7vsadm {
    return $ld_read_l7vsadm_return;
}
sub __ld_add_real {
    $ld_add_real_arg = [@_];
}
sub __ld_edit_real {
    $ld_edit_real_arg = [@_];
}
sub __ld_delete_real {
    $ld_delete_real_arg = [@_];
}
sub __system_wrapper {
    $system_wrapper_arg = [@_];
}
sub __ld_log {
}
