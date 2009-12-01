use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 38;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our @get_ip_port_args = ();
our @get_ip_port_returns = ();
our @get_forward_flag_args = ();
our @get_forward_flag_returns = ();
#...............................................
# test start
#   - ld_setup
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - empty virtual ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $expected{virtual}     = [ undef, undef, undef ];
    $main::CONFIG{virtual} = [ undef, undef, undef ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - virtual undef list ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        protocol => 'tcp',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        option => { protocol => '-t' },
        protocol => 'tcp',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                                             } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - protocol tcp ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        option => { protocol => '-t' },
        module => { name => 'cinsert' },
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        option => {
            protocol => '-t',
            main  => '-t ip:port -m cinsert',
            flags => '-t ip:port -m cinsert',
        },
        module => { name => 'cinsert' },
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                                             } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - module minimum setup ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        option => { protocol => '-t' },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        option => {
            protocol => '-t',
            main  => '-t ip:port -m cinsert --cookie-name cookie',
            flags => '-t ip:port -m cinsert --cookie-name cookie',
        },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                                             } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - module option setup ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        option => { protocol => '-t' },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        option => {
            protocol => '-t',
            main  => '-t ip:port -m cinsert --cookie-name cookie',
            flags => '-t ip:port -m cinsert --cookie-name cookie -s rr',
        },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                                             } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - scheduler setup ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        option => { protocol => '-t' },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
        maxconn => 1000,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        option => {
            protocol => '-t',
            main  => '-t ip:port -m cinsert --cookie-name cookie',
            flags => '-t ip:port -m cinsert --cookie-name cookie -s rr -u 1000',
        },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
        maxconn => 1000,
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                                             } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - maxconn setup ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        option => { protocol => '-t' },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
        maxconn => 1000,
        sorryserver => { ip => 'sorryip', port => 'sorryport' },
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        option => {
            protocol => '-t',
            main  => '-t ip:port -m cinsert --cookie-name cookie',
            flags => '-t ip:port -m cinsert --cookie-name cookie -s rr -u 1000 -b sorryip:sorryport',
        },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
        maxconn => 1000,
        sorryserver => { ip => 'sorryip', port => 'sorryport' },
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                                             } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - sorryserver setup ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        option => { protocol => '-t' },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
        maxconn => 1000,
        sorryserver => { ip => 'sorryip', port => 'sorryport' },
        qosup => '100M',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        option => {
            protocol => '-t',
            main  => '-t ip:port -m cinsert --cookie-name cookie',
            flags => '-t ip:port -m cinsert --cookie-name cookie -s rr -u 1000 -b sorryip:sorryport -Q 100M',
        },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
        maxconn => 1000,
        sorryserver => { ip => 'sorryip', port => 'sorryport' },
        qosup => '100M',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                                             } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - qosup setup ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('ip:port');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        option => { protocol => '-t' },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
        maxconn => 1000,
        sorryserver => { ip => 'sorryip', port => 'sorryport' },
        qosup => '100M',
        qosdown => '100K',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        option => {
            protocol => '-t',
            main  => '-t ip:port -m cinsert --cookie-name cookie',
            flags => '-t ip:port -m cinsert --cookie-name cookie -s rr -u 1000 -b sorryip:sorryport -Q 100M -q 100K',
        },
        module => {
            name => 'cinsert',
            option => '--cookie-name cookie',
        },
        scheduler => 'rr',
        maxconn => 1000,
        sorryserver => { ip => 'sorryip', port => 'sorryport' },
        qosup => '100M',
        qosdown => '100K',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                                             } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - qosdown setup ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('fbip1', 'fbip1');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
                                             } ];
    $main::CONFIG{fallback} = { tcp => { server => 'fallback'} };
    $expected{virtual}     = [ undef, undef, {
        fallback => { tcp => { server => 'fallback', option => { flags => '-r fbip1' } } },
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    $expected{fallback} = { tcp => { server => 'fallback',
                                     option => { flags => '-r fbip1' } }
                          };
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - fallback overwrite ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('fbip1');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        fallback => { tcp => { server => 'fallback' } },
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        fallback => { tcp => { server => 'fallback', option => { flags => '-r fbip1' } } },
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - fallback ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        checktype => 1,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checktype => 'combined',
        num_connects => 1,
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - checktype ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        login => '',
        service => 'ftp',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        login => 'anonymous',
        service => 'ftp',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - login ftp ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        login => '',
        service => 'sip',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        login => 'l7directord@' . $main::PROC_ENV{hostname},
        service => 'sip',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - login sip ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        passwd => '',
        service => 'ftp',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        passwd => 'l7directord@' . $main::PROC_ENV{hostname},
        service => 'ftp',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - passwd ftp ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ();
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ();
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, undef ],
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, undef ],
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real undef list ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
            forward => 'forward',
              } ],
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            forward => 'forward',
            option => { forward => '-m', flags => '-r real' },
            weight => 1,
            fail_counts => 0,
            healthchecked => 0,
            server => { port => undef },
              } ],
        server => {},
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real forward, weight ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
            weight => 'foo',
              } ],
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            option => { flags => '-r real' },
            weight => 1,
            fail_counts => 0,
            healthchecked => 0,
            server => { port => undef },
              } ],
        server => {},
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real invalid weight ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
            weight => 10,
              } ],
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            option => { flags => '-r real' },
            weight => 10,
            fail_counts => 0,
            healthchecked => 0,
            server => { port => undef },
              } ],
        server => {},
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real valid weight ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
            server => { ip => 'r', port => 'p' },
              } ],
        service => 'http',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            server => { ip => 'r', port => 'p' },
            option => { flags => '-r real' },
            weight => 1,
            fail_counts => 0,
            healthchecked => 0,
            url => 'http://r:p/',
              } ],
        service => 'http',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real url ip, port ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
            server => { ip => 'r', port => 'p' },
              } ],
        service => 'http',
        checkport => '10000',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            server => { ip => 'r', port => 'p' },
            option => { flags => '-r real' },
            weight => 1,
            fail_counts => 0,
            healthchecked => 0,
            url => 'http://r:10000/',
              } ],
        service => 'http',
        checkport => '10000',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real url checkport ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
            server => { ip => 'r' },
              } ],
        service => 'http',
        server => { port => 'v' },
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            server => { ip => 'r', port => 'v' },
            option => { flags => '-r real' },
            weight => 1,
            fail_counts => 0,
            healthchecked => 0,
            url => 'http://r:v/',
              } ],
        server => { port => 'v' },
        service => 'http',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real url default port ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
            request => 'r',
              } ],
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            option => { flags => '-r real' },
            request => 'r',
            weight => 1,
            fail_counts => 0,
            healthchecked => 0,
            url => 'r',
            server => { port => undef },
              } ],
        server => {},
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real valid request ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
              } ],
        request => 'r',
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            option => { flags => '-r real' },
            request => 'r',
            weight => 1,
            fail_counts => 0,
            healthchecked => 0,
            url => 'r',
            server => { port => undef },
              } ],
        server => {},
        request => 'r',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real request overwrite ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
            server => { ip => 'r' },
              } ],
        request => 'ftp://foo',
        service => 'ftp',
        server => { port => 'v' },
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            option => { flags => '-r real' },
            request => 'ftp://foo',
            weight => 1,
            fail_counts => 0,
            healthchecked => 0,
            url => 'ftp://foo',
            server => { ip => 'r', port => 'v' },
              } ],
        server => { port => 'v' },
        request => 'ftp://foo',
        service => 'ftp',
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real request set to url ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        real => [ undef, undef, {
              } ],
        checktype => 3,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        real => [ undef, undef, {
            option => { flags => '-r real' },
            weight => 1,
            fail_counts => 0,
            healthchecked => 0,
            num_connects => undef,
            server => { port => undef },
              } ],
        server => {},
        checktype => 'combined',
        num_connects => 3,
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - real combined ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        checkcount => 100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => 100,
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - valid checkcount ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        checkcount => -100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - invalid checkcount ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        checktimeout => 100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => 100,
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - valid checktimeout ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        checktimeout => -100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - invalid checktimeout ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        negotiatetimeout => 100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => 100,
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - valid negotiatetimeout ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        negotiatetimeout => -100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - invalid negotiatetimeout ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        checkinterval => 100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => 100,
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - valid checkinterval ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        checkinterval => -100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - invalid checkinterval ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        retryinterval => 100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => 100,
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - valid retryinterval ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        retryinterval => -100,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => $main::CONFIG{quiescent},
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - invalid retryinterval ok';
}
{
    set_default();
    local @get_ip_port_args = ();
    local @get_ip_port_returns = ('real');
    local @get_forward_flag_args = ();
    local @get_forward_flag_returns = ('-m');
    my %expected = %main::GLOBAL;
    $main::CONFIG{virtual} = [ undef, undef, {
        quiescent => 0,
                                             } ];
    $expected{virtual}     = [ undef, undef, {
        checkcount => $main::CONFIG{checkcount},
        checktimeout => $main::CONFIG{checktimeout},
        negotiatetimeout => $main::CONFIG{negotiatetimeout},
        checkinterval => $main::CONFIG{checkinterval},
        retryinterval => $main::CONFIG{retryinterval},
        quiescent => 0,
                               } ];
    ld_setup();
    is_deeply \%main::CONFIG, \%expected, 'ld_setup - valid quiescent ok';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
    %main::CONFIG = %main::GLOBAL;
    $main::PROC_ENV{hostname} = 'test';
}
sub override {
    *get_ip_port = \&__get_ip_port;
    *get_forward_flag = \&__get_forward_flag;
    *ld_log = \&__ld_log;
}
sub __get_ip_port {
    push @get_ip_port_args, @_;
    return shift @get_ip_port_returns;
}
sub __get_forward_flag {
    push @get_forward_flag_args, @_;
    return shift @get_forward_flag_returns;
}
sub __ld_log {
}
