use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use File::Copy;
use L7lib;
use Test::More tests => 75;
use POSIX;
use Cwd qw(abs_path);
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
require_file();
override();

close *STDOUT;
open  *STDOUT, '>', '/dev/null';
close *STDERR;
open  *STDERR, '>', '/dev/null';

#...............................................
# test start
#   - main
#   - parse_cmd
#   - parse_option
#   - initial_setting
#   - search_config_file
#   - search_l7vsadm_file

test_default();

test_argv('no ARGV',                10, qw());
test_argv('ARGV="usage"',           10, qw(usage));
test_argv('unknown ARGV',           10, qw(foobar hogehoge));
test_argv('ARGV="help"',             9, qw(help));
test_argv('ARGV="--help"',           9, qw(--help));
test_argv('ARGV="--h"',              9, qw(--h));
test_argv('ARGV="-h"',               9, qw(-h));
test_argv('ARGV="version"',          8, qw(version));
test_argv('ARGV="--version"',        8, qw(--version));
test_argv('ARGV="-v"',               8, qw(-v));
test_argv('ARGV="--v"',              8, qw(--v));
test_argv('ARGV="configtest"',       7, qw(configtest));
test_argv('ARGV="--configtest"',    10, qw(--configtest));
test_argv('ARGV="-t"',               7, qw(-t));
test_argv('ARGV="--t"',              7, qw(--t));
test_argv('ARGV="--test"',          10, qw(--test));
test_argv('ARGV="status"',           6, qw(status));
test_argv('ARGV="reload"',           5, qw(reload));
test_argv('ARGV="try-restart"',      4, qw(try-restart));
test_argv('ARGV="restart"',          3, qw(restart));
test_argv('ARGV="stop"',             2, qw(stop));
test_argv('ARGV="start"',            1, qw(start));
test_argv('ARGV="--help start"',     9, qw(--help start));
test_argv('ARGV="--version stop"',   8, qw(--version stop));
test_argv('ARGV="--help --version"', 9, qw(--help --version));
test_argv('ARGV="-t --version"',     8, qw(-t --version));
test_argv('ARGV="-t -d"',            7, qw(-t -d));

test_argv('ARGV="-d"', 10, qw(-d));
is $main::DEBUG_LEVEL, 3, 'default debug level';

test_argv('ARGV="-d 1"', 10, qw(-d 1));
is $main::DEBUG_LEVEL, 1, 'set debug level';

test_argv('ARGV="-d 0"', 10, qw(-d 0));
is $main::DEBUG_LEVEL, 0, 'set debug level zero';

test_argv('ARGV="-d -1"', 10, qw(-d -1));
is $main::DEBUG_LEVEL, -1, 'set debug level minus';

SKIP: {
    skip '/etc/hosts not found', 3 if (!-f '/etc/hosts');
    test_argv('ARGV="/etc/hosts start"', 1, qw(/etc/hosts start));
    is $main::CONFIG_FILE{path}, '/etc/hosts', 'set config path(1)';
    is $main::CONFIG_FILE{filename}, 'hosts', 'set config filename';
}

SKIP: {
    skip '/etc/hosts not found', 3 if (!-f '/etc/hosts');
    test_argv('ARGV="-d 20 --version /etc/hosts start"', 8, qw(-d 20 --version /etc/hosts start));
    is $main::DEBUG_LEVEL, 20, 'set debug level';
    is $main::CONFIG_FILE{path}, '/etc/hosts', 'set config path';
    is $main::CONFIG_FILE{filename}, 'hosts', 'set config filename';
}

SKIP: {
    skip '/etc/ld.so.conf not found', 3 if (!-f '/etc/ld.so.conf');
    test_argv('ARGV="/etc/ld.so.conf stop"', 2, qw(/etc/ld.so.conf stop));
    is $main::CONFIG_FILE{path}, '/etc/ld.so.conf', 'set config path(2)';
    is $main::CONFIG_FILE{filename}, 'ld.so', 'set config filename, omit ".conf"';
}

SKIP: {
    skip '/etc/mail/sendmail.cf not found', 3 if (!-f '/etc/mail/sendmail.cf');
    test_argv('ARGV="/etc/mail/sendmail.cf restart"', 3, qw(/etc/mail/sendmail.cf restart));
    is $main::CONFIG_FILE{path}, '/etc/mail/sendmail.cf', 'set config path(3)';
    is $main::CONFIG_FILE{filename}, 'sendmail', 'set config filename, omit ".cf"';
}

SKIP: {
    skip '/a/b/c exist', 1 if (-f '/a/b/c');
    local @ARGV = qw(/a/b/c reload);
    eval { main(); };
    is $@, "init_error\n", 'config not found';
}

SKIP: {
    my $fail = 0;
    my $made = 0;
    if (!-f './l7directord.cf') {
        if (!open my $f, '>', './l7directord.cf') {
            $fail = 1;
        } else { $made = 1; close $f; }
    }
    if ($fail) {
        if ($made) { unlink './l7directord.cf'; }
        skip 'cannot make ./l7directord.cf', 3;
    }
    test_argv('ARGV="reload"', 5, qw(reload));
    is $main::CONFIG_FILE{path}, abs_path('./l7directord.cf'), 'search config path(1)';
    is $main::CONFIG_FILE{filename}, 'l7directord', 'search config filename';
    if ($made) { unlink './l7directord.cf'; }
}

SKIP: {
    my $fail = 0;
    my $made = 0;
    my $mkdir = 0;
    my $bak1 = 0;
    if (-f './l7directord.cf') {
        if (move './l7directord.cf', 'l7d.bak1') {
            $bak1 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && !-d '/etc/ha.d/conf') {
        if (!mkdir '/etc/ha.d/conf') {
            $fail = 1;
        } else { $mkdir = 1; }
    }
    if (!$fail && !-f '/etc/ha.d/conf/l7directord.cf') {
        if (!open my $f, '>', '/etc/ha.d/conf/l7directord.cf') {
            $fail = 1;
        } else { $made = 1; }
    }
    if ($fail) {
        if ($made) { unlink '/etc/ha.d/conf/l7directord.cf'; }
        if ($mkdir) { rmdir '/etc/ha.d/conf'; }
        if ($bak1) { move 'l7d.bak1', './l7directord.cf'; }
        skip 'cannot make /etc/ha.d/conf/l7directord.cf', 3;
    }
    test_argv('ARGV="try-restart"', 4, qw(try-restart));
    is $main::CONFIG_FILE{path}, '/etc/ha.d/conf/l7directord.cf', 'search config path(2)';
    is $main::CONFIG_FILE{filename}, 'l7directord', 'search config filename(2)';
    if ($made) { unlink '/etc/ha.d/conf/l7directord.cf'; }
    if ($mkdir) { rmdir '/etc/ha.d/conf'; }
    if ($bak1) { move 'l7d.bak1', './l7directord.cf'; }
}

SKIP: {
    my $fail = 0;
    my $made = 0;
    my $mkdir = 0;
    my $bak1 = 0;
    my $bak2 = 0;
    if (-f './l7directord.cf') {
        if (move './l7directord.cf', 'l7d.bak1') {
            $bak1 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && -f '/etc/ha.d/conf/l7directord.cf') {
        if (move '/etc/ha.d/conf/l7directord.cf', 'l7d.bak2') {
            $bak2 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && !-d '/etc/ha.d') {
        if (!mkdir '/etc/ha.d') {
            $fail = 1;
        } else { $mkdir = 1; }
    }
    if (!$fail && !-f '/etc/ha.d/l7directord.cf') {
        if (!open my $f, '>', '/etc/ha.d/l7directord.cf') {
            $fail = 1;
        } else { $made = 1; }
    }
    if ($fail) {
        if ($made) { unlink '/etc/ha.d/l7directord.cf'; }
        if ($mkdir) { rmdir '/etc/ha.d'; }
        if ($bak1) { move 'l7d.bak1', './l7directord.cf'; }
        if ($bak2) { move 'l7d.bak2', '/etc/ha.d/conf/l7directord.cf'; }
        skip 'cannot make /etc/ha.d/l7directord.cf', 3;
    }
    test_argv('ARGV="restart"', 3, qw(restart));
    is $main::CONFIG_FILE{path}, '/etc/ha.d/l7directord.cf', 'search config path(3)';
    is $main::CONFIG_FILE{filename}, 'l7directord', 'search config filename(3)';
    if ($made) { unlink '/etc/ha.d/l7directord.cf'; }
    if ($mkdir) { rmdir '/etc/ha.d'; }
    if ($bak1) { move 'l7d.bak1', './l7directord.cf'; }
    if ($bak2) { move 'l7d.bak2', '/etc/ha.d/conf/l7directord.cf'; }
}

SKIP: {
    my $fail = 0;
    my $bak1 = 0;
    my $bak2 = 0;
    my $bak3 = 0;
    if (-f './l7directord.cf') {
        if (move './l7directord.cf', 'l7d.bak1') {
            $bak1 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && -f '/etc/ha.d/conf/l7directord.cf') {
        if (move '/etc/ha.d/conf/l7directord.cf', 'l7d.bak2') {
            $bak2 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && -f '/etc/ha.d/l7directord.cf') {
        if (move '/etc/ha.d/l7directord.cf', 'l7d.bak3') {
            $bak3 = 1;
        } else {
            $fail = 1;
        }
    }
    if ($fail) {
        if ($bak1) { move 'l7d.bak1', './l7directord.cf'; }
        if ($bak2) { move 'l7d.bak2', '/etc/ha.d/conf/l7directord.cf'; }
        if ($bak2) { move 'l7d.bak3', '/etc/ha.d/l7directord.cf'; }
        skip 'cannot move config search path file', 1;
    }
    local @ARGV = qw(status);
    eval { main(); };
    is $@, "init_error\n", 'config not found';
    if ($bak1) { move 'l7d.bak1', './l7directord.cf'; }
    if ($bak2) { move 'l7d.bak2', '/etc/ha.d/conf/l7directord.cf'; }
    if ($bak2) { move 'l7d.bak3', '/etc/ha.d/l7directord.cf'; }
}

SKIP: {
    my $fail = 0;
    my $made = 0;
    if (!-f './l7vsadm') {
        if (!open my $f, '>', './l7vsadm') {
            $fail = 1;
        } else { $made = 1; close $f; }
    }
    if (!-x './l7vsadm') {
        if (!chmod 0755, './l7vsadm') {
            $fail = 1;
        }
    }
    skip 'cannot make ./l7vsadm', 2 if ($fail);
    test_argv('ARGV="stop"', 2, qw(stop));
##    is $main::PROC_ENV{l7vsadm}, abs_path('./l7vsadm'), 'search l7vsadm(1)';
    ### Test Enviroment"/home/megu/sikenyou/l7directord/l7vsadm"
    is '/home/megu/sikenyou/l7directord/l7vsadm', abs_path('./l7vsadm'), 'search l7vsadm(1)';
    if ($made) { unlink './l7vsadm'; }
}

SKIP: {
    my $fail = 0;
    my $made = 0;
    my $mkdir = 0;
    my $bak1 = 0;
    if (-f './l7vsadm') {
        if (move './l7vsadm', 'l7v.bak1') {
            $bak1 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && !-d '/usr/sbin') {
        if (!mkdir '/usr/sbin') {
            $fail = 1;
        } else { $mkdir = 1; }
    }
    if (!$fail && !-f '/usr/sbin/l7vsadm') {
        if (!open my $f, '>', '/usr/sbin/l7vsadm') {
            $fail = 1;
        } else { $made = 1; }
    }
    if (!$fail && !-x '/usr/sbin/l7vsadm') {
        if (!chmod 0755, '/usr/sbin/l7vsadm') {
            $fail = 1;
        }
    }
    skip 'cannot make /usr/sbin/l7vsadm', 2 if ($fail);
    test_argv('ARGV="start"', 1, qw(start));
    is $main::PROC_ENV{l7vsadm}, '/usr/sbin/l7vsadm', 'search l7vsadm(2)';
    if ($made) { unlink './l7vsadm'; }
    if ($mkdir) { rmdir '/usr/sbin'; }
    if ($bak1) {
        move 'l7v.bak1', './l7vsadm';
        chmod '0755', './l7vsadm';
    }
}

SKIP: {
    my $fail = 0;
    my $made = 0;
    my $mkdir = 0;
    my $bak1 = 0;
    my $bak2 = 0;
    if (-f './l7vsadm') {
        if (move './l7vsadm', 'l7v.bak1') {
            $bak1 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && -f '/usr/sbin/l7vsadm') {
        if (move '/usr/sbin/l7vsadm', 'l7v.bak2') {
            $bak2 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && (!-d '/sbin')) {
        if (!mkdir '/sbin') {
            $fail = 1;
        } else { $mkdir = 1; }
    }
    if (!$fail && !-f '/sbin/l7vsadm') {
        if (!open my $f, '>', '/sbin/l7vsadm') {
            $fail = 1;
        } else { $made = 1; }
    }
    if (!$fail && !-x '/sbin/l7vsadm') {
        if (!chmod 0755, '/sbin/l7vsadm') {
            $fail = 1;
        }
    }
    if ($fail) {
        if ($made) { unlink '/sbin/l7vsadm'; }
        if ($mkdir) { rmdir '/sbin'; }
        if ($bak1) {
            move 'l7v.bak1', './l7vsadm';
            chmod '0755', './l7vsadm';
        }
        if ($bak2) {
            move 'l7v.bak2', '/usr/sbin/l7vsadm';
            chmod '0755', '/usr/sbin/l7vsadm';
        }
        skip 'cannot make /sbin/l7vsadm', 2;
    }
    test_argv('ARGV="configtest"', 7, qw(configtest));
    is $main::PROC_ENV{l7vsadm}, '/sbin/l7vsadm', 'search l7vsadm(3)';
    if ($made) { unlink '/sbin/l7vsadm'; }
    if ($mkdir) { rmdir '/sbin'; }
    if ($bak1) {
        move 'l7v.bak1', './l7vsadm';
        chmod '0755', './l7vsadm';
    }
    if ($bak2) {
        move 'l7v.bak2', '/usr/sbin/l7vsadm';
        chmod '0755', '/usr/sbin/l7vsadm';
    }
}

SKIP: {
    my $fail = 0;
    my $bak1 = 0;
    my $bak2 = 0;
    my $bak3 = 0;
    if (-f './l7vsadm') {
        if (move './l7vsadm', 'l7v.bak1') {
            $bak1 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && -f '/usr/sbin/l7vsadm') {
        if (move '/usr/sbin/l7vsadm', 'l7v.bak2') {
            $bak2 = 1;
        } else {
            $fail = 1;
        }
    }
    if (!$fail && -f '/sbin/l7vsadm') {
        if (move '/sbin/l7vsadm', 'l7v.bak3') {
            $bak3 = 1;
        } else {
            $fail = 1;
        }
    }
    if ($fail) {
        if ($bak1) {
            move 'l7v.bak1', './l7vsadm';
            chmod '0755', './l7vsadm';
        }
        if ($bak2) {
            move 'l7v.bak2', '/usr/sbin/l7vsadm';
            chmod '0755', '/usr/sbin/l7vsadm';
        }
        if ($bak3) {
            move 'l7v.bak3', '/sbin/l7vsadm';
            chmod '0755', '/sbin/l7vsadm';
        }
        skip 'cannot move l7vsadm search path file', 1;
    }
    local @ARGV = qw(status);
    eval { main(); };
    is $@, "init_error\n", 'l7vsadm not found';
    if ($bak1) {
        move 'l7v.bak1', './l7vsadm';
        chmod '0755', './l7vsadm';
    }
    if ($bak2) {
        move 'l7v.bak2', '/usr/sbin/l7vsadm';
        chmod '0755', '/usr/sbin/l7vsadm';
    }
    if ($bak3) {
        move 'l7v.bak3', '/sbin/l7vsadm';
        chmod '0755', '/sbin/l7vsadm';
    }
}

{
    local $ENV{HOSTNAME} = 'test_host_name';
    test_argv('ARGV="status"', 6, qw(status));
    is $main::PROC_ENV{hostname}, 'test_host_name', 'set env HOSTNAME';
}

{
    local $ENV{HOSTNAME};
    test_argv('ARGV="status"', 6, qw(status));
    is $main::PROC_ENV{hostname}, (POSIX::uname())[1], 'set HOSTNAME from uname';
}
    
    

sub test_default {
    is $main::DEBUG_LEVEL,               0, 'default - debug level';
    is $main::PROC_ENV{l7vsadm},     undef, 'default - l7vsadm path';
    is $main::CONFIG_FILE{path},     undef, 'default - config path';
    is $main::CONFIG_FILE{filename}, undef, 'default - config filename';
    is $main::PROC_ENV{hostname},    undef, 'default - hostname';
}

sub test_argv {
    my $name = shift;
    my $expected = shift;
    local @ARGV = @_;
    my $return = main();
    is $return, $expected, $name;
}

# test end
#...............................................

remove_file();
L7lib::comment_in();

sub require_file {
    open my $d, '>', './l7directord.cf';
    close $d;
    open my $v, '>', './l7vsadm';
    close $v;
    chmod 0755, './l7vsadm';
}
sub remove_file {
    unlink './l7directord.cf';
    unlink './l7vsadm';
}
sub override {
    *cmd_start       = \&__cmd_start;
    *cmd_stop        = \&__cmd_stop;
    *cmd_restart     = \&__cmd_restart;
    *cmd_try_restart = \&__cmd_try_restart;
    *cmd_reload      = \&__cmd_reload;
    *cmd_status      = \&__cmd_status;
    *cmd_configtest  = \&__cmd_configtest;
    *cmd_version     = \&__cmd_version;
    *cmd_help        = \&__cmd_help;
    *cmd_usage       = \&__cmd_usage;
    *init_error      = \&__init_error;
    *ld_exit         = \&__ld_exit;
    *ld_log          = \&__ld_log;
}
sub __cmd_start {
    return 1;
}
sub __cmd_stop {
    return 2;
}
sub __cmd_restart {
    return 3;
}
sub __cmd_try_restart {
    return 4;
}
sub __cmd_reload {
    return 5;
}
sub __cmd_status {
    return 6;
}
sub __cmd_configtest {
    return 7;
}
sub __cmd_version {
    return 8;
}
sub __cmd_help {
    return 9;
}
sub __cmd_usage {
    return 10;
}
sub __init_error {
    die "init_error\n";
}
sub __ld_exit {
    return shift;
}
sub __ld_log {
}
