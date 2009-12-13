use strict; 
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;    
use L7lib;  
use Test::More tests => 57;
use Socket;
use Socket6;
            
L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override(); 
            
our @config_error_args = ();
our @ld_gethostservbyname_args = ();
our $ld_gethostservbyname_return = undef;
our @ld_gethostbyname_args = ();
our @ld_gethostbyname_returns = ();
our $ld_getservbyname_arg = undef;
our $ld_getservbyname_return = undef;
our @ip_to_int_args = ();
our @ip_to_int_returns = ();
our @int_to_ip_args = ();
our @int_to_ip_returns = ();
#...............................................
# test start
#   - parse_fallback
{           
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = 'foo:bar';
    my %expected = %main::REAL;
    $expected{server} = $ld_gethostservbyname_return;
    $expected{forward} = 'masq';
    my $line = 1;
    my $fallback = 'fallback masq';
    my $config_line = 'configline';
    my $ret = parse_fallback($line, $fallback, $config_line);
    is_deeply $ret, \%expected, 'parse_fallback - ok';
    is_deeply \@ld_gethostservbyname_args, [ 'fallback', 'tcp' ], 'parse_fallback - ld_gethostservbyname arg(1)';
}           
{
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = 'foo:bar';
    my %expected = %main::REAL;
    $expected{server} = $ld_gethostservbyname_return;
    my $line = 1;
    my $fallback = 'fallback';
    my $config_line = 'configline';
    my $ret = parse_fallback($line, $fallback, $config_line);
    is_deeply $ret, \%expected, 'parse_fallback - no forward mode ok';
    is_deeply \@ld_gethostservbyname_args, [ 'fallback', 'tcp' ], 'parse_fallback - ld_gethostservbyname arg(2)';
}
{
    local @config_error_args = ();
    my $line = 1;
    my $fallback = ' fallback ';
    my $config_line = 'configline';
    eval {
        my $ret = parse_fallback($line, $fallback, $config_line);
    };
    is $@, "config_error\n", 'parse_fallback - format error';
    is_deeply \@config_error_args, [1, 'ERR0114', 'configline'], 'parse_fallback - config_error args(1)';
}
{
    local @config_error_args = ();
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = undef;
    my $line = 1;
    my $fallback = 'fallback masq';
    my $config_line = 'configline';
    eval {
        my $ret = parse_fallback($line, $fallback, $config_line);
    };
    is $@, "config_error\n", 'parse_fallback - ld_gethostservbyname error';
    is_deeply \@config_error_args, [1, 'ERR0114', 'configline'], 'parse_fallback - config_error args(2)';
    is_deeply \@ld_gethostservbyname_args, [ 'fallback', 'tcp' ], 'parse_fallback - ld_gethostservbyname arg(3)';
}
{
    local @config_error_args = ();
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = 'foo:bar';
    my $line = 1;
    my $fallback = 'fallback direct';
    my $config_line = 'configline';
    eval {
        my $ret = parse_fallback($line, $fallback, $config_line);
    }; 
    is $@, "config_error\n", 'parse_fallback - forward mode error';
    is_deeply \@config_error_args, [1, 'ERR0107', 'configline'], 'parse_fallback - config_error args(3)';
    is_deeply \@ld_gethostservbyname_args, [ 'fallback', 'tcp' ], 'parse_fallback - ld_gethostservbyname arg(4)';
}
#   - parse_real
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('1.1.1.1', '1.1.1.2');
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = 80;
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (1,2);
    local @int_to_ip_args = ();
    local @int_to_ip_returns = ('1.1.1.1', '1.1.1.2');
    my %real = %main::REAL;
    $real{forward} = 'masq';
    $real{weight} = 100;
    $real{request} = 'request';
    $real{receive} = 'receive';
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = $ld_getservbyname_return;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{foo.bar.com->hoge.huga.com:serv  Masq  100  "request" , 'receive'};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - full parse ok';
    is $ld_getservbyname_arg, 'serv', 'parse_real - ld_getservbyname arg(1)';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com', 'hoge.huga.com'], 'parse_real - ld_gethostbyname arg(1)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(1)';
##    is_deeply \@int_to_ip_args, [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(1)';
}
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('1.1.1.1', '1.1.1.1');
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = 80;
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (1, 1);
    local @int_to_ip_args = ();
    local @int_to_ip_returns = ('1.1.1.1');
    my %real = %main::REAL;
    $real{forward} = 'masq';
    $real{weight} = 100;
    $real{request} = 'request';
    $real{receive} = 'receive';
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = $ld_getservbyname_return;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{foo.bar.com->hoge.huga.com:serv  Masq  100  "request" , 'receive'};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - same range ok';
    is $ld_getservbyname_arg, 'serv', 'parse_real - ld_getservbyname arg(2)';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com', 'hoge.huga.com'], 'parse_real - ld_gethostbyname arg(2)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(2)';
##    is_deeply \@int_to_ip_args, [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(2)';
}
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('1.1.1.1', '1.1.1.1');
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = 80;
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (1, 1);
    local @int_to_ip_args = ();
    local @int_to_ip_returns = ('1.1.1.1');
    my %real = %main::REAL;
    $real{forward} = 'masq';
    $real{weight} = 100;
    $real{request} = 'request';
    $real{receive} = 'receive';
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = $ld_getservbyname_return;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{foo.bar.com:serv  Masq  100  "request" , 'receive'};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - no range ok';
    is $ld_getservbyname_arg, 'serv', 'parse_real - ld_getservbyname arg(3)';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com'], 'parse_real - ld_gethostbyname arg(3)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(3)';
##    is_deeply \@int_to_ip_args, [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(3)';
}
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('1.1.1.1', '1.1.1.1');
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (1, 1);
    local @int_to_ip_args = ();
    local @int_to_ip_returns = ('1.1.1.1');
    my %real = %main::REAL;
    $real{forward} = 'masq';
    $real{weight} = 100;
    $real{request} = 'request';
    $real{receive} = 'receive';
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = undef;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{foo.bar.com  Masq  100  "request" , 'receive'};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - no port ok';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com'], 'parse_real - ld_gethostbyname arg(4)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(4)';
##    is_deeply \@int_to_ip_args, [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(4)';
}
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('1.1.1.1', '1.1.1.1');
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (1, 1);
    local @int_to_ip_args = ();
    local @int_to_ip_returns = ('1.1.1.1');
    my %real = %main::REAL;
    $real{weight} = 100;
    $real{request} = 'request';
    $real{receive} = 'receive';
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = undef;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{foo.bar.com  100  "request" , 'receive'};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - no forward mode ok';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com'], 'parse_real - ld_gethostbyname arg(5)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(5)';
##    is_deeply \@int_to_ip_args, [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(5)';
}
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('1.1.1.1', '1.1.1.1');
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (1, 1);
    local @int_to_ip_args = ();
    local @int_to_ip_returns = ('1.1.1.1');
    my %real = %main::REAL;
    $real{request} = 'request';
    $real{receive} = 'receive';
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = undef;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{foo.bar.com    "request" , 'receive'};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - no weight ok';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com'], 'parse_real - ld_gethostbyname arg(6)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(6)';
##    is_deeply \@int_to_ip_args, [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(6)';
}
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('1.1.1.1', '1.1.1.1');
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (1, 1);
    local @int_to_ip_args = ();
    local @int_to_ip_returns = ('1.1.1.1');
    my %real = %main::REAL;
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = undef;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{foo.bar.com};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - no request, receive ok';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com'], 'parse_real - ld_gethostbyname arg(7)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(7)';
##    is_deeply \@int_to_ip_args, [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(7)';
}
{
    local @config_error_args = ();
    my $line = 1;
    my $real = q{foo.bar.com:bad:format};
    my $config_line = 'configline';
    eval {
        my $ret = parse_real($line, $real, $config_line);
    };
    is $@, "config_error\n", 'parse_real - format error';
    is_deeply \@config_error_args, [1, 'ERR0114', 'configline'], 'parse_real - config_error args(1)';
}
{
    local @config_error_args = ();
    my $line = 1;
    my $real = q{foo.bar.com:bad direct};
    my $config_line = 'configline';
    eval {
        my $ret = parse_real($line, $real, $config_line);
    };
    is $@, "config_error\n", 'parse_real - forward mode error';
    is_deeply \@config_error_args, [1, 'ERR0107', 'configline'], 'parse_real - config_error args(2)';
}
{
    local @config_error_args = ();
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = undef;
    my $line = 1;
    my $real = q{foo.bar.com:bad masq};
    my $config_line = 'configline';
    eval {
        my $ret = parse_real($line, $real, $config_line);
    };
    is $@, "config_error\n", 'parse_real - ld_getservbyname error';
    is_deeply \@config_error_args, [1, 'ERR0108', 'configline'], 'parse_real - config_error args(3)';
    is $ld_getservbyname_arg, 'bad', 'parse_real - ld_getservbyname args(4)';
}
{
    local @config_error_args = ();
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = 80;
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ();
    my $line = 1;
    my $real = q{foo.bar.com:bad masq};
    my $config_line = 'configline';
    eval {
        my $ret = parse_real($line, $real, $config_line);
    };
    is $@, "config_error\n", 'parse_real - first ld_gethostbyname error';
    is_deeply \@config_error_args, [1, 'ERR0114', 'configline'], 'parse_real - config_error args(4)';
    is $ld_getservbyname_arg, 'bad', 'parse_real - ld_getservbyname args(5)';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com'], 'parse_real - ld_gethostbyname args(5)';
}
{
    local @config_error_args = ();
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = 80;
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('1.1.1.1');
    my $line = 1;
    my $real = q{foo.bar.com->hoge.huga.com:bad masq};
    my $config_line = 'configline';
    eval {
        my $ret = parse_real($line, $real, $config_line);
    };
    is $@, "config_error\n", 'parse_real - second ld_gethostbyname error';
    is_deeply \@config_error_args, [1, 'ERR0114', 'configline'], 'parse_real - config_error args(5)';
    is $ld_getservbyname_arg, 'bad', 'parse_real - ld_getservbyname args(6)';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com', 'hoge.huga.com'], 'parse_real - ld_gethostbyname args(6)';
}
{
    local @config_error_args = ();
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = 80;
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ( '2.2.2.2','1.1.1.1');
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (2, 1);
    my $line = 1;
    my $real = q{foo.bar.com->hoge.huga.com:bad masq};
    my $config_line = 'configline';
    my @gethost_r = @ld_gethostbyname_returns;
    eval {
        my $ret = parse_real($line, $real, $config_line);
    };
    is $@, "config_error\n", 'parse_real - second ld_gethostbyname error';
    is_deeply \@config_error_args, [1, 'ERR0115', '2.2.2.2', '1.1.1.1', 'configline'], 'parse_real - config_error args(5)';
    is $ld_getservbyname_arg, 'bad', 'parse_real - ld_getservbyname args(6)';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com', 'hoge.huga.com'], 'parse_real - ld_gethostbyname args(6)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(8)';
}
###################################################################
##  - parse_real (IPv6)
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('[2001::1]', '[2001::2]');
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = 80;
    local @ip_to_int_args = ('[2001::1]', '[2001::2]');
    local @ip_to_int_returns = (2306124484190404608,2306124484190404609);
    local @int_to_ip_args = (2306124484190404608,2306124484190404609);
    local @int_to_ip_returns = ('[2001::1]', '[2001::2]');
    my %real = %main::REAL;
    $real{forward} = 'masq';
    $real{weight} = 100;
    $real{request} = 'request';
    $real{receive} = 'receive';
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = $ld_getservbyname_return;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{foo.bar.com->hoge.huga.com:serv  Masq  100  "request" , 'receive'};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - full parse ok';
    is $ld_getservbyname_arg, 'serv', 'parse_real - ld_getservbyname arg(1)';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com', 'hoge.huga.com'], 'parse_real - ld_gethostbyname arg(1)';
    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(1)';
    is_deeply [$int_to_ip_args[0] .. $int_to_ip_args[1] ], [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(1)';
}
##  - parse_real (IPv6)
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('[2001::1]', '[2001::1]');
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = 80;
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (1,1);
    local @int_to_ip_args = ();
    local @int_to_ip_returns = ('[2001::1]', '[2001::1]');
    my %real = %main::REAL;
    $real{forward} = 'masq';
    $real{weight} = 100;
    $real{request} = 'request';
    $real{receive} = 'receive';
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = $ld_getservbyname_return;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{foo.bar.com->hoge.huga.com:serv  Masq  100  "request" , 'receive'};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - full parse ok';
    is $ld_getservbyname_arg, 'serv', 'parse_real - ld_getservbyname arg(1)';
    is_deeply \@ld_gethostbyname_args, ['foo.bar.com', 'hoge.huga.com'], 'parse_real - ld_gethostbyname arg(1)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(1)';
##    is_deeply \@int_to_ip_args, [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(1)';
}
{
    local @ld_gethostbyname_args = ();
    local @ld_gethostbyname_returns = ('[2001::1]', '[2001::2]');
    local $ld_getservbyname_arg = undef;
    local $ld_getservbyname_return = 80;
    local @ip_to_int_args = ();
    local @ip_to_int_returns = (1,2);
    local @int_to_ip_args = ();
    local @int_to_ip_returns = ('[2001::1]', '[2001::2]');
    my %real = %main::REAL;
    $real{forward} = 'masq';
    $real{weight} = 100;
    $real{request} = 'request';
    $real{receive} = 'receive';
    my @expected;
    my $c = 0;
    for ($ip_to_int_returns[0] .. $ip_to_int_returns[1]) {
        my %r = %real;
        push @expected, \%r;
        $expected[$c]->{server}{ip}   = $int_to_ip_returns[$c];
        $expected[$c]->{server}{port} = $ld_getservbyname_return;
        $c++;
    }
    my @gethost_r =  @ld_gethostbyname_returns;
    my @ip_to_r   =  @ip_to_int_returns;
    my $line = 1;
    my $real = q{[ffff:1:2:3:4:5:6:7:8]->[ffff:1:2:3:4:5:6:7:9]:serv  Masq  100  "request" , 'receive'};
    my $config_line = 'configline';
    my $ret = parse_real($line, $real, $config_line);
    is_deeply $ret, \@expected, 'parse_real - full parse ok';
    is $ld_getservbyname_arg, 'serv', 'parse_real - ld_getservbyname arg(1)';
    is_deeply \@ld_gethostbyname_args, ['[ffff:1:2:3:4:5:6:7:8]', '[ffff:1:2:3:4:5:6:7:9]'], 'parse_real - ld_gethostbyname arg(1)';
##    is_deeply \@ip_to_int_args, \@gethost_r, 'parse_real - ip_to_int arg(1)';
##    is_deeply \@int_to_ip_args, [ $ip_to_r[0] .. $ip_to_r[1] ], 'parse_real - int_to_ip arg(1)';
}
# test end
#...............................................

L7lib::comment_in();

sub override {
    *config_error         = \&__config_error;
    *ld_gethostservbyname = \&__ld_gethostservbyname;
    *ld_gethostbyname = \&__ld_gethostbyname;
    *ld_getservbyname = \&__ld_getservbyname;
##    *ip_to_int = \&__ip_to_int;
    *int_to_ip = \&__int_to_ip;
    *ld_log = \&__ld_log;
}
sub __config_error {
    @config_error_args = @_;
    die "config_error\n";
}           
sub __ld_gethostservbyname {
    push @ld_gethostservbyname_args, @_;
    return $ld_gethostservbyname_return;
}
sub __ld_gethostbyname {
    push @ld_gethostbyname_args, @_;
    return shift @ld_gethostbyname_returns;
}
sub __ld_getservbyname {
    $ld_getservbyname_arg = shift;
    return $ld_getservbyname_return;
}
sub __ip_to_int {
    push @ip_to_int_args, @_;
    return shift @ip_to_int_returns;
}
sub __int_to_ip {
    push @int_to_ip_args, @_;
    return shift @int_to_ip_returns;
}
sub __ld_log {
}
