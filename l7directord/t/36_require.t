use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 1;
use Config;
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';

override();

#...............................................
# test start
#####################
###  check_require_module
{
    default_value();
    $main::CONFIG{virtual} = [ undef, undef, {
        service   => 'sip',
        checktype => 'negotiate',
    } ];
##    $expected = [ undef, undef, {
##        service   => 'sip',
##        checktype => 'negotiate',
##    } ];
    eval {
        check_require_module();
    };
    is_deeply \%main::CONFIG, \%main::CONFIG, 'check_require_module - ok';
}
#####################################################################
# test end
#...............................................

L7lib::comment_in();

sub default_value {
    $main::CONFIG_FILE{path} = time;
    undef $main::CONFIG_FILE{stattime};
    undef $main::CONFIG_FILE{checksum};
    $main::PROC_STAT{initialized} = 0;
    %main::CONFIG = %main::GLOBAL;
}
sub create_config {
    my $input = shift;
    open my $f, ">", $main::CONFIG_FILE{path} or return 0;
    print $f $input;
    close $f;
    return 1;
}
sub remove_config {
    if (-f $main::CONFIG_FILE{path}) {
        unlink $main::CONFIG_FILE{path};
    }
}
sub override {
    *ld_log = \&__ld_log;
}
sub __ld_log {
}
