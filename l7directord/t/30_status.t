use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 21;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our @get_health_check_id_str_args = ();
our @get_health_check_id_str_returns = ();
our @fallback_on_args = ();
our @fallback_off_args = ();
our @__restore_service_args = ();
our @__remove_service_args = ();

#...............................................
# test start
#   - service_set
#   - _service_up
#   - _service_down
#   - _status_up
#   - _status_down
#   - get_status
{
    set_default();
    my $v_r_list = undef;
    my $state = undef;
    service_set($v_r_list, $state);
    pass 'service_set - state is undef';
}
{
    set_default();
    my $v_r_list = undef;
    my $state = 'up';
    service_set($v_r_list, $state);
    pass 'service_set - v_r_list is undef(1)';
}
{
    set_default();
    my $v_r_list = undef;
    my $state = 'down';
    service_set($v_r_list, $state);
    pass 'service_set - v_r_list is undef(2)';
}
{
    local @get_health_check_id_str_args    = ();
    local @get_health_check_id_str_returns = ('id1');
    set_default();
    my $v_r_list = [ ['v', 'r'] ];
    my $state = 'up';
    local $main::HEALTH_CHECK{id1}{status} = $main::SERVICE_UP;
    service_set($v_r_list, $state);
    is_deeply \@get_health_check_id_str_args, [['v', 'r']], 'service_set - set up, but already up';
}
{
    local @get_health_check_id_str_args    = ();
    local @get_health_check_id_str_returns = ('id2');
    set_default();
    my $v_r_list = [ ['v', 'r'] ];
    my $state = 'down';
    local $main::HEALTH_CHECK{id2}{status} = $main::SERVICE_DOWN;
    service_set($v_r_list, $state);
    is_deeply \@get_health_check_id_str_args, [['v', 'r']], 'service_set - set down, but already down';
}
{
    local @get_health_check_id_str_args    = ();
    local @get_health_check_id_str_returns = ('id1', 'id1');
    local @fallback_off_args = ();
    local @__restore_service_args = ();
    set_default();
    my $v_r_list = [ ['v', 'r'] ];
    my $state = 'up';
    local $main::HEALTH_CHECK{id1}{status} = $main::SERVICE_DOWN;
    service_set($v_r_list, $state);
    is_deeply \@get_health_check_id_str_args, [['v', 'r'], ['v', 'r']], 'service_set - set up';
    is_deeply \@fallback_off_args, ['v'], 'service_set - set up - fallback off args';
    is_deeply \@__restore_service_args, ['v', 'r', 'real'], 'service_set - set up - restore service args';
    is $main::HEALTH_CHECK{id1}{status}, $main::SERVICE_UP, 'service_set - status is changed to up';
}
{
    local @get_health_check_id_str_args    = ();
    local @get_health_check_id_str_returns = ('id2', 'id2');
    local @fallback_on_args = ();
    local @__remove_service_args = ();
    set_default();
    my $v_r_list = [ ['v', 'r'] ];
    my $state = 'down';
    local $main::HEALTH_CHECK{id2}{status} = $main::SERVICE_UP;
    service_set($v_r_list, $state);
    is_deeply \@get_health_check_id_str_args, [['v', 'r'], ['v', 'r']], 'service_set - set down';
    is_deeply \@fallback_on_args, ['v'], 'service_set - set up - fallback on args';
    is_deeply \@__remove_service_args, ['v', 'r', 'real'], 'service_set - set up - remove service args';
    is $main::HEALTH_CHECK{id2}{status}, $main::SERVICE_DOWN, 'service_set - status is changed to down';
}
{
    local @get_health_check_id_str_args    = ();
    local @get_health_check_id_str_returns = ('id1', 'id1');
    local @fallback_off_args = ();
    local @__restore_service_args = ();
    set_default();
    my $v_r_list = [ ['v', 'r'] ];
    my $state = 'up';
    service_set($v_r_list, $state);
    is_deeply \@get_health_check_id_str_args, [['v', 'r'], ['v', 'r']], 'service_set - set up, but not defined id';
    is_deeply \@fallback_off_args, ['v'], 'service_set - set up - fallback off args';
    is_deeply \@__restore_service_args, ['v', 'r', 'real'], 'service_set - set up - restore service args';
    is $main::HEALTH_CHECK{id1}{status}, $main::SERVICE_UP, 'service_set - status is registerd as up';
}
{
    local @get_health_check_id_str_args    = ();
    local @get_health_check_id_str_returns = ('id2', 'id2');
    local @fallback_on_args = ();
    local @__remove_service_args = ();
    set_default();
    my $v_r_list = [ ['v', 'r'] ];
    my $state = 'down';
    service_set($v_r_list, $state);
    is_deeply \@get_health_check_id_str_args, [['v', 'r'], ['v', 'r']], 'service_set - set down, but not defined id';
    is_deeply \@fallback_on_args, ['v'], 'service_set - set up - fallback on args';
    is_deeply \@__remove_service_args, ['v', 'r', 'real'], 'service_set - set up - remove service args';
    is $main::HEALTH_CHECK{id2}{status}, $main::SERVICE_DOWN, 'service_set - status is registerd as down';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
}
sub override {
    *ld_log = \&__ld_log;
    *get_health_check_id_str = \&__get_health_check_id_str;
    *fallback_on = \&__fallback_on;
    *fallback_off = \&__fallback_off;
    *_restore_service = \&___restore_service;
    *_remove_service = \&___remove_service;
}
sub __get_health_check_id_str {
    push @get_health_check_id_str_args, [@_];
    return shift @get_health_check_id_str_returns;
}
sub __fallback_on {
    push @fallback_on_args, @_;
}
sub __fallback_off {
    push @fallback_off_args, @_;
}
sub ___restore_service {
    push @__restore_service_args, @_;
}
sub ___remove_service {
    push @__remove_service_args, @_;
}
sub __ld_log {
}
