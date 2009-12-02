use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 8;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

#...............................................
# test start
#   - sleep_and_check_signal
{
    set_default();
    my $sec = undef;
    my $child = 0;
    my $got = sleep_and_check_signal($sec, $child);
    is $got, 'halt', 'sleep_and_check_signal - sleep time is undef';
}
{
    set_default();
    my $sec = 'ten';
    my $child = 0;
    my $got = sleep_and_check_signal($sec, $child);
    is $got, 'halt', 'sleep_and_check_signal - sleep time is invalid';
}
{
    set_default();
    my $sec = 0;
    my $child = 0;
    my $got = sleep_and_check_signal($sec, $child);
    is $got, 'run', 'sleep_and_check_signal - sleep time is zero';
}
{
    set_default();
    my $sec = 10;
    my $child = 0;
    local $main::PROC_STAT{reload} = 1;
    my $got = sleep_and_check_signal($sec, $child);
    is $got, 'reload', 'sleep_and_check_signal - sleep parent and reload';
    is $main::PROC_STAT, undef, 'sleep_and_check_signal - reload flag off';
}
{
    set_default();
    my $sec = 10;
    my $child = 0;
    local $main::PROC_STAT{halt} = 1;
    my $got = sleep_and_check_signal($sec, $child);
    is $got, 'halt', 'sleep_and_check_signal - sleep parent and halt';
}
{
    set_default();
    my $sec = 10;
    my $child = 1;
    local $main::PROC_STAT{halt} = 1;
    my $got = sleep_and_check_signal($sec, $child);
    is $got, 'halt', 'sleep_and_check_signal - sleep child and halt';
}
{
    set_default();
    my $sec = 3;
    my $child = 1;
    local $main::PROC_STAT{reload} = 1;
    my $got = sleep_and_check_signal($sec, $child);
    is $got, 'run', 'sleep_and_check_signal - sleep child and ignore reload';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
    $main::CONFIG_FILE{path} = 'config';
}
sub override {
    *ld_log = \&__ld_log;
}
sub __ld_log {
}
