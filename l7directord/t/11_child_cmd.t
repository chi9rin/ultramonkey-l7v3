use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 6;
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our @system_wrapper_args = ();
our @system_wrapper_returns = ();
#...............................................
# test start
#   - ld_cmd_children
{
    set_default();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    ld_cmd_children();
    is_deeply \@system_wrapper_args, [], 'ld_cmd_children - no execute';
}
{
    set_default();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    $main::CONFIG{execute} = { subconf1 => 1, subconf2 => 1 };
    ld_cmd_children();
    is_deeply \@system_wrapper_args, [], 'ld_cmd_children - exists execute, but no command';
}
{
    set_default();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    $main::CONFIG{execute} = { subconf1 => 1, subconf2 => 1 };
    ld_cmd_children('start');
    is_deeply \@system_wrapper_args, ['l7directord subconf1 start', 'l7directord subconf2 start'],
        'ld_cmd_children - exists execute and command';
}
{
    set_default();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    $main::CONFIG{execute} = { subconf1 => 1, subconf2 => 1 };
    ld_cmd_children('start', {});
    is_deeply \@system_wrapper_args, [], 'ld_cmd_children - specified empty config';
}
{
    set_default();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    my $execute = { subconf1 => 1, subconf2 => 1 };
    ld_cmd_children(undef, $execute);
    is_deeply \@system_wrapper_args, [], 'ld_cmd_children - specified config but not exist command';
}
{
    set_default();
    local @system_wrapper_args = ();
    local @system_wrapper_returns = ();
    my $execute = { subconf1 => 1, subconf2 => 1 };
    ld_cmd_children('start', $execute);
    is_deeply \@system_wrapper_args, ['l7directord subconf1 start', 'l7directord subconf2 start'],
        'ld_cmd_children - specified config and exists command';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
    %main::CONFIG = %main::GLOBAL;
    $main::PROC_ENV{l7directord} = 'l7directord';
}
sub override {
    *system_wrapper = \&__system_wrapper;
    *ld_log = \&__ld_log;
}
sub __system_wrapper {
    push @system_wrapper_args, @_;
    return shift @system_wrapper_returns;
}
sub __ld_log {
}
