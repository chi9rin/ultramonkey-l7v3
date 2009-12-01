use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 41;
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
#   - ip_to_int
{
    my $got = ip_to_int();
    is $got, -1, 'ip_to_int - undef is error';
}
{
    my $got = ip_to_int('0.0.0.0');
    is $got, 0, 'ip_to_int - 0.0.0.0 is ok';
}
{
    my $got = ip_to_int('-1.-1.-1.-1');
    is $got, -1, 'ip_to_int - -1.-1.-1.-1 is error';
}
{
    my $got = ip_to_int('255.255.255.255');
    is $got, 4294967295, 'ip_to_int - 255.255.255.255 is ok';
}
{
    my $got = ip_to_int('127.0.0.1');
    is $got, 2130706433, 'ip_to_int - 127.0.0.1 is ok';
}
{
    my $got = ip_to_int('256.256.256.256');
    is $got, -1, 'ip_to_int - 256.256.256.256 is error';
}
{
    my $got = ip_to_int('localhost');
    is $got, -1, 'ip_to_int - localhost(invalid format) is error';
}
#   - int_to_ip
{
    my $got = int_to_ip(0);
    is $got, '0.0.0.0', 'int_to_ip - 0 is ok';
}
{
    my $got = int_to_ip(-1);
    is $got, undef, 'int_to_ip - -1 is error';
}
{
    my $got = int_to_ip(2130706433);
    is $got, '127.0.0.1', 'int_to_ip - 2130706433 is ok';
}
{
    my $got = int_to_ip(4294967295);
    is $got, '255.255.255.255', 'int_to_ip - 4294967295 is ok';
}
SKIP: {
    my $got = int_to_ip(4294967296);
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
    my $got = int_to_ip('one');
    is $got, undef, 'int_to_ip - one(string) is error';
}
# test end
#...............................................

L7lib::comment_in();

sub override {
    *ld_log = \&__ld_log;
}
sub __ld_log {
}
