use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 83;
use Config;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';

override();

#...............................................
# test start
#   - _ld_service_resolve
{
    my %v = ();
    my $port = 80;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'http', '_ld_service_resolve - http ok';
}
{
    my %v = ();
    my $port = 443;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'https', '_ld_service_resolve - https ok';
}
{
    my %v = ();
    my $port = 21;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'ftp', '_ld_service_resolve - ftp ok';
}
{
    my %v = ();
    my $port = 25;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'smtp', '_ld_service_resolve - smtp ok';
}
{
    my %v = ();
    my $port = 110;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'pop', '_ld_service_resolve - pop ok';
}
{
    my %v = ();
    my $port = 119;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'nntp', '_ld_service_resolve - nntp ok';
}
{
    my %v = ();
    my $port = 143;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'imap', '_ld_service_resolve - imap ok';
}
{
    my %v = ();
    my $port = 389;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'ldap', '_ld_service_resolve - ldap ok';
}
{
    my %v = ();
    my $port = 53;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'dns', '_ld_service_resolve - dns ok';
}
{
    my %v = ();
    my $port = 3306;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'mysql', '_ld_service_resolve - mysql ok';
}
{
    my %v = ();
    my $port = 5432;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'pgsql', '_ld_service_resolve - pgsql ok';
}
{
    my %v = ();
    my $port = 5060;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'sip', '_ld_service_resolve - sip ok';
}
{
    my %v = ();
    my $port = 100000;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'none', '_ld_service_resolve - none ok';
}
{
    my $port = 100000;
    _ld_service_resolve(undef, $port);
}
{
    my %v = ();
    _ld_service_resolve(\%v, undef);
    is $v{service}, undef, '_ld_service_resolve - port is undef';
}
{
    my %v = ( service => 'pop' );
    my $port = 80;
    _ld_service_resolve(\%v, $port);
    is $v{service}, 'pop', '_ld_service_resolve - service is already exist';
}
#   - is_octet
{
    my $got = is_octet(0);
    is $got, 1, 'is_octet - 0 is ok';
}
{
    my $got = is_octet(-1);
    is $got, 0, 'is_octet - -1 is error';
}
{
    my $got = is_octet(255);
    is $got, 1, 'is_octet - 255 is ok';
}
{
    my $got = is_octet(256);
    is $got, 0, 'is_octet - 256 is error';
}
{
    my $got = is_octet();
    is $got, 0, 'is_octet - undef is error';
}
{
    my $got = is_octet('one');
    is $got, 0, 'is_octet - one(string) is error';
}
#   - is_ip
{
    my $got = is_ip();
    is $got, 0, 'is_ip - undef is error';
}
{
    my $got = is_ip('0.0.0.0');
    is $got, 1, 'is_ip - 0.0.0.0 is ok';
}
{
    my $got = is_ip('-1.-1.-1.-1');
    is $got, 0, 'is_ip - -1.-1.-1.-1 is error';
}
{
    my $got = is_ip('255.255.255.255');
    is $got, 1, 'is_ip - 255.255.255.255 is ok';
}
{
    my $got = is_ip('256.256.256.256');
    is $got, 0, 'is_ip - 256.256.256.256 is error';
}
{
    my $got = is_ip('localhost');
    is $got, 0, 'is_ip - localhost(invalid format) is error';
}
###############################################################################################
#   - is_ip6  IPv6 function
{
### Pattern ip:none
    my @got = is_ip6();
    my ($ret, @address ) = @got;
    is $ret, 0, 'is_ip6 - undef is error';
}
{
### Pattern ip:
    my @got = is_ip6('[::0]');
    my ($ret, @address ) = @got;
    is $ret, 1, 'is_ip6 - [::0] is ok';
}
{
### Pattern ip:[::1]
    my @got = is_ip6('[::1]');
    my ($ret, @address ) = @got;
    is $ret, 1, 'is_ip6 - [::1] is ok';
}
{
### Pattern ip:[2001::1]
    my @got = is_ip6('[2001::1]');
    my ($ret, @address ) = @got;
    is $ret, 1, 'is_ip6 - [2001::1] is ok';
}
{
### Pattern ip:[fe80::1%eth1]
    my @got = is_ip6('[fe80::1%eth1]');
    my ($ret, @address ) = @got;
    is $ret, 1, 'is_ip6 - [fe80::1%eth1] is ok';
    my $alleged_ip  = join ":", @address;
    is $alleged_ip, 'fe80:0:0:0:0:0:0:1%eth1', 'is_ip6 - [fe80::1%eth1] is ok';
}
{
### Pattern ip:[1234:5678:90ab:cdef:1234:5678:90ab:cdef]
    my @got = is_ip6('[1234:5678:90ab:cdef:1234:5678:90ab:cdef]');
    my ($ret, @address ) = @got;
    is $ret, 1, 'is_ip6 - [1234:5678:90ab:cdef:1234:5678:90ab:cdef] is ok';
}
{
### Pattern ip:[::90ab:cdef:1234:5678:90ab:cdef]
    my @got = is_ip6('[::90ab:cdef:1234:5678:90ab:cdef]');
    my ($ret, @address ) = @got;
    is $ret, 1, 'is_ip6 - [::90ab:cdef:1234:5678:90ab:cdef] is ok';
}
{
### Pattern ip:[1234:5678:90ab:cdef:1234:5678::]
    my @got = is_ip6('[1234:5678:90ab:cdef:1234:5678::]');
    my ($ret, @address ) = @got;
    is $ret, 1, 'is_ip6 - [1234:5678:90ab:cdef:1234:5678::] is ok';
}
{
### Pattern ip:[1234:5678:90ab:cdef:1234:5678::2]
    my @got = is_ip6('[1234:5678:90ab:cdef:1234:5678::2]');
    my ($ret, @address ) = @got;
    is $ret, 1, 'is_ip6 - [1234:5678:90ab:cdef:1234:5678::2] is ok';
}
{
### Pattern ip:[1234:5678:9012:3456:7890:1234:5678:9012:3456]
    my @got = is_ip6('[1234:5678:9012:3456:7890:1234:5678:9012:3456]');
    my ($ret, @address ) = @got;
    is $ret, 0, 'is_ip6 - [1234:5678:9012:3456:7890:1234:5678:9012:3456] is error';
}
{
### Pattern ip:[fffz::ffff]
    my @got = is_ip6('[fffz::ffff]');
    my ($ret, @address ) = @got;
    is $ret, 0, 'is_ip6 - [fffz::ffff] is error';
}
{
### Pattern ip:localhost
    my @got = is_ip6('localhost');
    my ($ret, @address ) = @got;
    is $ret, 0, 'is_ip6 - localhost(invalid format) is error';
}
###############################################################################################
#   - ip_to_int
{
    my @got = ip_to_int();
    my ($ipver, $addr ) = @got;
    is $addr, -1, 'ip_to_int - undef is error';
}
{
    my @got = ip_to_int('0.0.0.0');
    my ($ipver, $addr ) = @got;
    is $addr, 0, 'ip_to_int - 0.0.0.0 is ok';
}
{
    my @got = ip_to_int('-1.-1.-1.-1');
    my ($ipver, $addr ) = @got;
    is $addr, -1, 'ip_to_int - -1.-1.-1.-1 is error';
}
{
    my @got = ip_to_int('255.255.255.255');
    my ($ipver, $addr ) = @got;
    is $addr, 4294967295, 'ip_to_int - 255.255.255.255 is ok';
}
{
    my @got = ip_to_int('127.0.0.1');
    my ($ipver, $addr ) = @got;
    is $addr, 2130706433, 'ip_to_int - 127.0.0.1 is ok';
}
{
    my @got = ip_to_int('256.256.256.256');
    my ($ipver, $addr ) = @got;
    is $addr, -1, 'ip_to_int - 256.256.256.256 is error';
}
{
    my @got = ip_to_int('localhost');
    my ($ipver, $addr ) = @got;
    is $addr, -1, 'ip_to_int - localhost(invalid format) is error';
}
## IPv6 Function
{
    my @got = ip_to_int('::');
    my ($ipver, $addr ) = @got;
    is $addr, 0, 'ip_to_int - [::] is ok';
}
{
    my @got = ip_to_int('[::1]');
    my ($ipver, $addr ) = @got;
    is $addr, 1, 'ip_to_int - [::1] is ok';
}
{
    my @got = ip_to_int('[2001::1]');
    my ($ipver, $addr, $addr2 ) = @got;
    is $ipver, 'ipv6', 'ip_to_int - ipv6 is ok';
    is $addr, 1, 'ip_to_int - [2001::1] -> interface is ok';
    is $addr2, 2306124484190404608, 'ip_to_int - [2001::1] prefix is ok';
}
{
    my @got = ip_to_int('[1:2:3:4:5:6:7:8]');
    my ($ipver, $addr, $addr2 ) = @got;
    is $ipver, 'ipv6', 'ip_to_int - ipv6 is ok';
    is $addr, 1407400653815816, 'ip_to_int - [1:2:3:4:5:6:7:8] -> interface is ok';
    is $addr2, 281483566841860, 'ip_to_int - [1:2:3:4:5:6:7:8] is ok';
}
{
    my @got = ip_to_int('[1234:5678:90ab:cdef:1234:5678:90ab:cdef]');
    my ($ipver, $addr, $addr2 ) = @got;
    is $ipver, 'ipv6', 'ip_to_int - ipv6 is ok';
    is $addr, 1311768467294899695, 'ip_to_int - [1234:5678:90ab:cdef:1234:5678:90ab:cdef] -> interface is ok';
    is $addr2, 1311768467294899695, 'ip_to_int - [1234:5678:90ab:cdef:1234:5678:90ab:cdef] is ok';
}
{
    my @got = ip_to_int('[::90ab:cdef:1234:5678:90ab:cdef]');
    my ($ipver, $addr, $addr2 ) = @got;
    is $ipver, 'ipv6', 'ip_to_int - ipv6 is ok';
    is $addr, 1311768467294899695, 'ip_to_int - [::90ab:cdef:1234:5678:90ab:cdef] -> interface is ok';
    is $addr2, 2427178479, 'ip_to_int - [::90ab:cdef:1234:5678:90ab:cdef] is ok';
}
{
    my @got = ip_to_int('[1234:5678:90ab:cdef:1234:5678::]');
    my ($ipver, $addr, $addr2 ) = @got;
    is $ipver, 'ipv6', 'ip_to_int - ipv6 is ok';
    is $addr, 1311768464867721216, 'ip_to_int - [1234:5678:90ab:cdef:1234:5678::] -> interface is ok';
    is $addr2, 1311768467294899695, 'ip_to_int - [1234:5678:90ab:cdef:1234:5678::] is ok';
}
{
    my @got = ip_to_int('[1234:5678:90ab:cdef:1234:5678::2]');
    my ($ipver, $addr, $addr2 ) = @got;
    is $ipver, 'ipv6', 'ip_to_int - ipv6 is ok';
    is $addr, 1311768464867721218, 'ip_to_int - [1234:5678:90ab:cdef:1234:5678::2] -> interface is ok';
    is $addr2, 1311768467294899695, 'ip_to_int - [1234:5678:90ab:cdef:1234:5678::2] is ok';
}
{
    my @got = ip_to_int('[1234:5678:9012:3456:7890:1234:5678:9012:3456]');
    my ($ipver, $addr ) = @got;
    is $addr, -1, 'ip_to_int - [1234:5678:9012:3456:7890:1234:5678:9012:3456] is error';
}
{
    my @got = ip_to_int('fffz::ffff');
    my ($ipver, $addr ) = @got;
    is $addr, -1, 'ip_to_int - fffz::ffff is error';
}
###############################################################################################
#   - int_to_ip
#   - int_to_ip
{
    my $got = int_to_ip('ipv4', 0 );
    is $got, '0.0.0.0', 'int_to_ip - 0 is ok';
}
{
    my $got = int_to_ip('ipv4', -1);
    is $got, undef, 'int_to_ip - -1 is error';
}
{
    my $got = int_to_ip('ipv4', 2130706433);
    is $got, '127.0.0.1', 'int_to_ip - 2130706433 is ok';
}
{
    my $got = int_to_ip('ipv4', 4294967295);
    is $got, '255.255.255.255', 'int_to_ip - 4294967295 is ok';
}
SKIP: {
    my $got = int_to_ip('ipv4', 4294967296);
    if ( $Config{'use64bitint'} ) {
        is $got, '0.0.0.0', 'int_to_ip - 4294967296 is ok (overflow)';
    }
    else {
        is $got, '255.255.255.255', 'int_to_ip - 4294967296 is ok (overflow)';
    }
}
{
    my $got = int_to_ip();
    is $got, undef, 'int_to_ip - undef is error';
}
{
    my $got = int_to_ip('ipv4','one');
    is $got, undef, 'int_to_ip - one(string) is error';
}
###
### IPv6 Tests
###
{
    my $got = int_to_ip('ipv6', 0, 0);
    is $got, '0:0:0:0:0:0:0:0', 'int_to_ip - 0:0:0:0:0:0:0:0 is ok';
}
{
    my $got = int_to_ip('ipv6', 1, 0);
    is $got, '0:0:0:0:0:0:0:1', 'int_to_ip - 0:0:0:0:0:0:0:1 is ok';
}
{
    my $got = int_to_ip('ipv6', 1407400653815816,281483566841860);
    is $got, '1:2:3:4:5:6:7:8', 'int_to_ip - [1:2:3:4:5:6:7:8] is ok';
}
{
    my $got = int_to_ip('ipv6', 1311768467294899695,1311768467294899695);
    is $got, '1234:5678:90ab:cdef:1234:5678:90ab:cdef', 'int_to_ip - [1234:5678:90ab:cdef:1234:5678:90ab:cdef] is ok';
}
## ::90ab:cdef:1234:5678:90ab:cdef
{
    my $got = int_to_ip('ipv6', 1311768467294899695, 2427178479);
    is $got, '0:0:90ab:cdef:1234:5678:90ab:cdef', 'int_to_ip - [0:0:90ab:cdef:1234:5678:90ab:cdef] is ok';
}
## 1234:5678:90ab:cdef:1234:5678::
{
    my $got = int_to_ip('ipv6', 1311768464867721216,1311768467294899695);
    is $got, '1234:5678:90ab:cdef:1234:5678:0:0', 'int_to_ip - [1234:5678:90ab:cdef:1234:5678:0:0] is ok';
}
## 1234:5678:90ab:cdef:1234:5678::2
{
    my $got = int_to_ip('ipv6', 1311768464867721218, 1311768467294899695);
    is $got, '1234:5678:90ab:cdef:1234:5678:0:2', 'int_to_ip - [1234:5678:90ab:cdef:1234:5678:0:2] is ok';
}



#####################################################################
# test end
#...............................................

L7lib::comment_in();

sub override {
    *ld_log = \&__ld_log;
}
sub __ld_log {
}
