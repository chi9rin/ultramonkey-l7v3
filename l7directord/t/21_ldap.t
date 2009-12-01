use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 7;
use IO::Socket::INET;
use IO::Handle;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our $module_error = 0;
eval { require Net::LDAP; };
if ($@) {
    $module_error = 1;
}

#...............................................
# test start
#   - check_ldap
SKIP: {
    skip q{Can't locate Net/LDAP.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    # 30 0C 02 01 01 61 07 0A 01 00 04 00 04 00 __ __ 0....a........
    my $normal_response = pack 'H*', '300C02010161070A010004000400';
    my $pid = prepare_child($sock, [$normal_response]);
    set_default();
    my $v = { negotiatetimeout => 3, };
    my $r = { server => {ip => '127.0.0.1', port => $port }, fail_counts => 0 };
    my $got = check_ldap($v, $r);
    is $got, $main::SERVICE_UP, 'check_ldap - connect ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Net/LDAP.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    # 30 0C 02 01 01 61 07 0A 01 00 04 00 04 00 __ __ 0....a........
    my $normal_response = pack 'H*', '300C02010161070A010004000400';
    my $pid = prepare_child($sock, [$normal_response]);
    set_default();
    my $v = { negotiatetimeout => 3, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_ldap($v, $r);
    is $got, $main::SERVICE_UP, 'check_ldap - checkport connect ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Net/LDAP.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    # 30 0C 02 01 01 61 07 0A 01 00 04 00 04 00 __ __ 0....a........
    my $normal_response = pack 'H*', '300C02010161070A010004000400';
    # 30 64 02 01 02 64 5F 04 11 64 63 3D 65 78 61 6D 0d...d_..dc=exam    
    # 70 6C 65 2C 64 63 3D 63 6F 6D 30 4A 30 27 04 0B ple,dc=com0J0'..    
    # 6F 62 6A 65 63 74 43 6C 61 73 73 31 18 04 08 64 objectClass1...d    
    # 63 4F 62 6A 65 63 74 04 0C 6F 72 67 61 6E 69 7A cObject..organiz    
    # 61 74 69 6F 6E 30 0F 04 02 64 63 31 09 04 07 65 ation0...dc1...e    
    # 78 61 6D 70 6C 65 30 0E 04 01 6F 31 09 04 07 65 xample0...o1...e    
    # 78 61 6D 70 6C 65 __ __ __ __ __ __ __ __ __ __ xample              
    my $search_response = pack 'H*',
        '3064020102645F041164633D6578616D706C652C64633D636F6D304A3027040B' .
        '6F626A656374436C6173733118040864634F626A656374040C6F7267616E697A' .
        '6174696F6E300F04026463310904076578616D706C65300E04016F3109040765' .
        '78616D706C65';
    my $pid = prepare_child($sock, [$normal_response, $search_response]);
    set_default();
    my $v = {negotiatetimeout => 3, };
    my $r = { server => {ip => '127.0.0.1', port => $port}, request => 'dc=example,dc=com' , fail_counts => 0 };
    my $got = check_ldap($v, $r);
    is $got, $main::SERVICE_UP, 'check_ldap - search ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Net/LDAP.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    # 30 0C 02 01 01 61 07 0A 01 00 04 00 04 00 __ __ 0....a........
    my $normal_response = pack 'H*', '300C02010161070A010004000400';
    # 30 64 02 01 02 64 5F 04 11 64 63 3D 65 78 61 6D 0d...d_..dc=exam    
    # 70 6C 65 2C 64 63 3D 63 6F 6D 30 4A 30 27 04 0B ple,dc=com0J0'..    
    # 6F 62 6A 65 63 74 43 6C 61 73 73 31 18 04 08 64 objectClass1...d    
    # 63 4F 62 6A 65 63 74 04 0C 6F 72 67 61 6E 69 7A cObject..organiz    
    # 61 74 69 6F 6E 30 0F 04 02 64 63 31 09 04 07 65 ation0...dc1...e    
    # 78 61 6D 70 6C 65 30 0E 04 01 6F 31 09 04 07 65 xample0...o1...e    
    # 78 61 6D 70 6C 65 __ __ __ __ __ __ __ __ __ __ xample              
    my $search_response = pack 'H*',
        '3064020102645F041164633D6578616D706C652C64633D636F6D304A3027040B' .
        '6F626A656374436C6173733118040864634F626A656374040C6F7267616E697A' .
        '6174696F6E300F04026463310904076578616D706C65300E04016F3109040765' .
        '78616D706C65';
    my $pid = prepare_child($sock, [$normal_response, $search_response]);
    set_default();
    my $v = {negotiatetimeout => 3};
    my $r = { server => {ip => '127.0.0.1', port => $port}, request => 'dc=example,dc=com', receive => 'example' , fail_counts => 0 };
    my $got = check_ldap($v, $r);
    is $got, $main::SERVICE_UP, 'check_ldap - response check ok';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Net/LDAP.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    # 30 0C 02 01 01 61 07 0A 01 00 04 00 04 00 __ __ 0....a........
    my $normal_response = pack 'H*', '300C02010161070A010004000400';
    my $pid = prepare_child($sock, [$normal_response], 2);
    set_default();
    my $v = {negotiatetimeout => 1};
    my $r = { server => {ip => '127.0.0.1', port => $port}, request => 'dc=example,dc=com', receive => 'example' , fail_counts => 0 };
    my $got = check_ldap($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ldap - timeout';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Net/LDAP.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    my $sock = create_sock($port);
    skip 'cannot create socket', 1 if !$sock;
    # 30 0C 02 01 01 61 07 0A 01 00 04 00 04 00 __ __ 0....a........
    my $normal_response = pack 'H*', '300C02010161070A010004000400';
    # 30 64 02 01 02 64 5F 04 11 64 63 3D 65 78 61 6D 0d...d_..dc=exam    
    # 70 6C 65 2C 64 63 3D 63 6F 6D 30 4A 30 27 04 0B ple,dc=com0J0'..    
    # 6F 62 6A 65 63 74 43 6C 61 73 73 31 18 04 08 64 objectClass1...d    
    # 63 4F 62 6A 65 63 74 04 0C 6F 72 67 61 6E 69 7A cObject..organiz    
    # 61 74 69 6F 6E 30 0F 04 02 64 63 31 09 04 07 65 ation0...dc1...e    
    # 78 61 6D 70 6C 65 30 0E 04 01 6F 31 09 04 07 65 xample0...o1...e    
    # 78 61 6D 70 6C 65 __ __ __ __ __ __ __ __ __ __ xample              
    my $search_response = pack 'H*',
        '3064020102645F041164633D6578616D706C652C64633D636F6D304A3027040B' .
        '6F626A656374436C6173733118040864634F626A656374040C6F7267616E697A' .
        '6174696F6E300F04026463310904076578616D706C65300E04016F3109040765' .
        '78616D706C65';
    my $pid = prepare_child($sock, [$normal_response, $search_response]);
    set_default();
    my $v = {negotiatetimeout => 3};
    my $r = { server => {ip => '127.0.0.1', port => $port}, request => 'dc=example,dc=com', receive => 'foo' , fail_counts => 0 };
    my $got = check_ldap($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ldap - response error';
    close_child($pid);
    close $sock;
}
SKIP: {
    skip q{Can't locate Net/LDAP.pm in @INC}, 1 if $module_error;
    my $port = 63334;
    set_default();
    my $v = {negotiatetimeout => 1, checkport => $port };
    my $r = { server => {ip => '127.0.0.1', port => 10000 }, fail_counts => 0 };
    my $got = check_ldap($v, $r);
    is $got, $main::SERVICE_DOWN, 'check_ldap - connect error';
}
# test end
#...............................................

L7lib::comment_in();

sub create_sock {
    my $port = shift;
    my $sock = IO::Socket::INET->new(
        Listen => 5,
        LocalAddr => 'localhost',
        LocalPort => $port,
        ReuseAddr => 1,
        Proto => 'tcp');
    return $sock;
}
sub prepare_child {
    my $sock = shift;
    my $res = shift;
    my $sleep = shift;
    my $pid = fork();
    if ($pid == 0) {
        my $s = $sock->accept;
        $s->autoflush(1);
        if ($sleep) { sleep $sleep; }
        for my $response (@$res) {
            my $length;
            read $s, $length, 1;
            read $s, $length, 1;
            $length = unpack('C', $length);
            #print "read length: $length\n";
            my $req;
            read $s, $req, $length;
            #print "read: ", unpack('H*', $req), "\n";
            my $num = substr $req, 2, 1;
            $response =~ s/^(....)./$1$num/;
            print $s $response;
        }
        close $s;
        exit;
    }
    return $pid;
}
sub close_child {
    my $pid = shift;
    kill 15, $pid;
    waitpid $pid, 0;
}
sub set_default {
}
sub override {
    *ld_log = \&__ld_log;
    *get_status = \&__get_status;
}
sub __get_status {
    return $main::SERVICE_UP;
}
sub __ld_log {
}
