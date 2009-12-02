use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 33;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our $filename = undef;
our @message_only_args = ();
our @message_args = ();
our $init_error_arg = undef;
our $ld_rm_file_arg = undef;
our $ld_log_arg = undef;

#...............................................
# test start
#   - read_pid
{
    default_value();
    create_pidfile($$);
    local $0 = 'l7directord';
    my ($oldpid, $filepid) = read_pid();
    is $oldpid,  $$, 'read_pid - from /proc/pid';
    is $filepid, $$, 'read_pid - from pid file';
    remove_pidfile();
} 
{
    default_value();
    create_pidfile($$);
    local $0 = 'l7directord';
    my $oldpid = read_pid();
    is $oldpid,  $$, 'read_pid - get /proc/pid only';
    remove_pidfile();
} 
{
    default_value();
    create_pidfile($$);
    local $0 = 'foobar';
    my ($oldpid, $filepid) = read_pid();
    is $oldpid,  undef, 'read_pid - from /proc/pid, but process name mismatch';
    is $filepid, $$,    'read_pid - from pid file';
    remove_pidfile();
} 
{
    default_value();
    create_pidfile($$);
    local $0 = 'foobar';
    my $oldpid = read_pid();
    is $oldpid,  undef, 'read_pid - get /proc/pid only, but process name mismatch';
    remove_pidfile();
} 
{
    default_value();
    create_pidfile(99999);
    local $0 = 'l7directord';
    my ($oldpid, $filepid) = read_pid();
    is $oldpid,  undef, 'read_pid - from /proc/pid, but no such proc file.';
    is $filepid, 99999, 'read_pid - from pid file';
    remove_pidfile();
} 
{
    default_value();
    local $0 = 'l7directord';
    my ($oldpid, $filepid) = read_pid();
    is $oldpid,  undef, 'read_pid - from /proc/pid';
    is $filepid, undef, 'read_pid - from pid file, but no such pid file.';
    remove_pidfile();
} 
#   - write_pid
{
    default_value();
    remove_pidfile();
    local @message_only_args = ();
    local $init_error_arg    = undef;
    write_pid(1);
    my $written = read_pidfile();
    is $written, "1\n", 'write_pid - pid 1 specified';
    remove_pidfile();
}
{
    default_value();
    remove_pidfile();
    local @message_only_args = ();
    local $init_error_arg    = undef;
    write_pid(0);
    is_deeply \@message_only_args, ['ERR0412', 0], 'write_pid - pid 0 specified, error message';
    is $init_error_arg, "message_only", 'write_pid - pid 0 specified, init_error';
    remove_pidfile();
}
{
    default_value();
    remove_pidfile();
    local @message_only_args = ();
    local $init_error_arg    = undef;
    write_pid();
    is_deeply \@message_only_args, ['ERR0412', 'undef'], 'write_pid - pid not specified, error message';
    is $init_error_arg, "message_only", 'write_pid - pid not specified, init_error';
    remove_pidfile();
}
{
    default_value();
    remove_pidfile();
    local @message_only_args = ();
    local $init_error_arg    = undef;
    write_pid('foobar');
    is_deeply \@message_only_args, ['ERR0412', 'foobar'], 'write_pid - pid some word specified, error message';
    is $init_error_arg, "message_only", 'write_pid - pid some word specified, init_error';
    remove_pidfile();
}
{
    default_value();
    remove_pidfile();
    local @message_only_args = ();
    local $init_error_arg    = undef;
    local *close = \&__close;
    write_pid(1);
    is_deeply \@message_only_args, ['ERR0409', $filename, "close_error\n"], 'write_pid - close error';
    is $init_error_arg, "message_only", 'write_pid - close error, init_error';
    remove_pidfile();
}
#   - remove_pid
{
    default_value();
    local *ld_rm_file = \&__ld_rm_file;
    remove_pid();
    is $ld_rm_file_arg, $filename, 'remove_pid';
}
#   - ld_rm_file
{
    default_value();
    create_pidfile(1);
    my $got = ld_rm_file($filename);
    is $got, 0, 'ld_rm_file - success';
}
{
    default_value();
    my $got = ld_rm_file();
    is $got, -1, 'ld_rm_file - error no delete file';
    is_deeply \@message_args, ['ERR0411'], 'ld_rm_file - error message(1)';
    is $ld_log_arg, 'message', 'ld_rm_file - error log(1)';
}
SKIP: {
    default_value();
    skip '/tmp not found!', 3 if (!-d '/tmp');
    my $got = ld_rm_file('/tmp');
    is $got, -1, 'ld_rm_file - error directory';
    is_deeply \@message_args, ['ERR0401', '/tmp'], 'ld_rm_file - error message(2)';
    is $ld_log_arg, 'message', 'ld_rm_file - error log(2)';
}
SKIP: {
    default_value();
    skip 'file /asdfasdf exist', 3 if (-e '/asdfasdf');
    my $got = ld_rm_file('/asdfasdf');
    is $got, -1, 'ld_rm_file - error file not found';
    is_deeply \@message_args, ['ERR0402', '/asdfasdf'], 'ld_rm_file - error message(3)';
    is $ld_log_arg, 'message', 'ld_rm_file - error log(3)';
}
SKIP: {
    default_value();
    skip 'file /proc/version not exist', 3 if (!-f '/proc/version');
    my $got = ld_rm_file('/proc/version');
    is $got, -1, 'ld_rm_file - error cannot remove';
    is_deeply \@message_args, ['ERR0403', '/proc/version', $!], 'ld_rm_file - error message(4)';
    is $ld_log_arg, 'message', 'ld_rm_file - error log(4)';
}
# test end
#...............................................

L7lib::comment_in();

sub default_value {
    $main::PROC_ENV{pid_prefix}  = cwd;
    $main::CONFIG_FILE{filename} = time;
}
sub override {
    *_message_only = \&__message_only;
    *_message      = \&__message;
    *ld_log        = \&__ld_log;
    *init_error    = \&__init_error;
}
sub create_pidfile {
    my $pid = shift;
    $filename = $main::PROC_ENV{pid_prefix} . q{.} . $main::CONFIG_FILE{filename} . '.pid';
    if (open my $f, ">", $filename) {
        print $f $pid . "\n";
        close $f;
        return 1;
    }
    return 0;
}
sub read_pidfile {
    $filename = $main::PROC_ENV{pid_prefix} . q{.} . $main::CONFIG_FILE{filename} . '.pid';
    my $pidinfo = undef;
    if (open my $f, "<", $filename) {
        local $\ = undef;
        $pidinfo = <$f>;
        close $f;
    }
    return $pidinfo;
}
sub remove_pidfile {
    $filename = $main::PROC_ENV{pid_prefix} . q{.} . $main::CONFIG_FILE{filename} . '.pid';
    if (-f $filename) {
        unlink $filename;
    }
}
sub __message_only {
    @message_only_args = @_;
    return 'message_only';
}
sub __init_error {
    $init_error_arg = shift;
}
sub __close {
    die "close_error\n";
}
sub __ld_rm_file {
    $ld_rm_file_arg = shift;
}
sub __ld_log {
    $ld_log_arg = shift;
}
sub __message {
    @message_args = @_;
    return 'message';
}
