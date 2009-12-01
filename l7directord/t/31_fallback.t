use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 10;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our @_status_up_args = ();
our @_status_up_returns = ();
our @_status_down_args = ();
our @_status_down_returns = ();
our @__restore_service_args = ();
our @__remove_service_args = ();

#...............................................
# test start
#   - fallback_on
#   - fallback_off
#   - fallback_find
{
    set_default();
    my $v = undef;
    fallback_on($v);
    pass 'fallback_on - v is undef';
}
{
    set_default();
    my $v = undef;
    fallback_off($v);
    pass 'fallback_off - v is undef';
}
{
    set_default();
    local @_status_up_args = ();
    local @_status_up_returns = (1);
    local @__restore_service_args = ();
    my $v = { fallback => { tcp => 'r' } };
    fallback_on($v);
    is_deeply \@_status_up_args, [[[$v, 'r']], 'fallback'], 'fallback_on - status change to up';
    is_deeply \@__restore_service_args, [$v, 'r', 'fallback'], 'fallback_on - restore service args';
}
{
    set_default();
    local @_status_down_args = ();
    local @_status_down_returns = (1);
    local @__remove_service_args = ();
    my $v = { fallback => { tcp => 'r' } };
    fallback_off($v);
    is_deeply \@_status_down_args, [[[$v, 'r']], 'fallback'], 'fallback_off - status change to down';
    is_deeply \@__remove_service_args, [$v, 'r', 'fallback'], 'fallback_off - remove service args';
}
{
    set_default();
    local @_status_up_args = ();
    local @_status_up_returns = (0);
    local @__restore_service_args = ();
    my $v = { fallback => { tcp => 'r' } };
    fallback_on($v);
    is_deeply \@_status_up_args, [[[$v, 'r']], 'fallback'], 'fallback_on - status is not changed';
    is_deeply \@__restore_service_args, [], 'fallback_on - restore service no args';
}
{
    set_default();
    local @_status_down_args = ();
    local @_status_down_returns = (0);
    local @__remove_service_args = ();
    my $v = { fallback => { tcp => 'r' } };
    fallback_off($v);
    is_deeply \@_status_down_args, [[[$v, 'r']], 'fallback'], 'fallback_off - status is not changed';
    is_deeply \@__remove_service_args, [], 'fallback_off - remove service no args';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
}
sub override {
    *ld_log = \&__ld_log;
    *_status_up = \&___status_up;
    *_status_down = \&___status_down;
    *_restore_service = \&___restore_service;
    *_remove_service = \&___remove_service;
}
sub ___status_up {
    push @_status_up_args, @_;
    return shift @_status_up_returns;
}
sub ___status_down {
    push @_status_down_args, @_;
    return shift @_status_down_returns;
}
sub ___restore_service {
    push @__restore_service_args, @_;
}
sub ___remove_service {
    push @__remove_service_args, @_;
}
sub __ld_log {
}
