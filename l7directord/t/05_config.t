use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 589;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our @config_error_args            = ();
our @ld_gethostservbyname_args    = undef;
our $ld_gethostservbyname_return  = undef;
our $ld_gethostservbyname_undef   = 0;
our $_ld_service_resolve_called   = 0;
our @parse_fallback_args          = ();
our $parse_fallback_return        = undef;
our @parse_real_args              = ();
our $parse_real_return            = undef;
our $ld_openlog_arg               = ();
our $ld_openlog_return            = undef;
our @get_hostservbyname_args      = ();
our $get_hostservbyname_return    = undef;
our @ld_cmd_children_args         = ();
our $system_wrapper_called        = 0;
our $system_wrapper_return        = undef;
our $ld_setup_die                 = 0;
our $read_config_die              = 0;
our $read_config_virtual          = [];
our $read_config_execute          = {};
#...............................................
# test start
#   - read_config
#   - validate_config
SKIP: {
    my $input = <<"CONFIG";
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    read_config();
    is $main::PROC_STAT{initialized}, 1, 'read_config - empty config';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(1)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
		# comment

      # comment

	# comment

  # comment

# comment
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    read_config();
    is $main::PROC_STAT{initialized}, 1, 'read_config - ignore comment';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(2)';
    remove_config();
}
{
    default_value();
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - no such config file';
    is $@, "config_error\n", 'read_config - raise config_error(3)';
    is_deeply \@config_error_args, [0, 'ERR0407', $main::CONFIG_FILE{path}], 'read_config - config_error args(3)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(3)';
}
SKIP: {
    my $input = <<"CONFIG";
    #
#
	    
virtual = virtual_value1	
virtual = "virtual_value2"     # comment
checktimeout =1   
negotiatetimeout=	2
checkinterval=3
retryinterval='4'
configinterval="5"
checkcount = 6
autoreload = 'no'
quiescent  = yes	# comment
fallback=fallback_value
callback=/bin/cat

execute= /etc/hosts
logfile=	"logfile_value"
		#
supervised

virtual = "virtual_value3"     # comment
	real = real_value1
        real = real_value2
	request='request'
    #
    receive = receive
		login= "login"

      passwd = "passwd"
	database = database
        virtualhost='virtualhost'
    checktype = CoNnEcT
	checktimeout =1   
        
	negotiatetimeout    =	2
	checkinterval=3
	retryinterval='4'
	checkcount = 6
	maxconn=7
        checkport = 10000
      scheduler = rr
    protocol= tcp
      service = http
        httpmethod = 'hEaD'
    fallback = fallback_value
      quiescent="yes"
 #
    module = url    --pattern-match   "/foo/bar" # comment

        sorryserver= sorryserver_value
    qosup  = '0'
    qosdown =999M

    realdowncallback=/bin/cat
    realrecovercallback = '/bin/cat'

        customcheck  = "/bin/echo"
    #

virtual = "virtual_value4"     # comment
        real = real_value3

    #
# end comment...
CONFIG
    my %config = (
        'autoreload' => 0,
        'checktimeout' => 1,
        'quiescent' => 1,
        'retryinterval' => 4,
        'negotiatetimeout' => 2,
        'checkinterval' => 3,
        'supervised' => 1,
        'fallback' => { 'tcp' => 'fallback_return' },
        'configinterval' => 5,
        'execute' => { '/etc/hosts' => 1 },
        'callback' => '/bin/cat',
        'checkcount' => 6,
        'logfile' => 'logfile_value',
        'virtual' => [
            {
                'protocol' => 'tcp',
                'httpmethod' => 'GET',
                'qosdown' => 0,
                'retryinterval' => undef,
                'negotiatetimeout' => undef,
                'checkinterval' => undef,
                'server' => {},
                'qosup' => 0,
                'real' => undef,
                'module' => { 'name' => 'sessionless', 'key' => '' },
                'checktimeout' => undef,
                'request' => undef,
                'checktype' => 'negotiate',
                'virtualhost' => undef,
                'quiescent' => undef,
                'realrecovercallback' => undef,
                'sorryserver' => undef,
                'service' => undef,
                'login' => '',
                'fallback' => undef,
                'passwd' => '',
                'receive' => undef,
                'database' => '',
                'maxconn' => 0,
                'checkcount' => undef,
                'checkport' => undef,
                'scheduler' => 'rr',
                'realdowncallback' => undef,
                'customcheck' => undef,
            },
            {
                'protocol' => 'tcp',
                'httpmethod' => 'GET',
                'qosdown' => 0,
                'retryinterval' => undef,
                'negotiatetimeout' => undef,
                'checkinterval' => undef,
                'server' => {},
                'qosup' => 0,
                'real' => undef,
                'module' => { 'name' => 'sessionless', 'key' => '' },
                'checktimeout' => undef,
                'request' => undef,
                'checktype' => 'negotiate',
                'virtualhost' => undef,
                'quiescent' => undef,
                'realrecovercallback' => undef,
                'sorryserver' => undef,
                'service' => undef,
                'login' => '',
                'fallback' => undef,
                'passwd' => '',
                'receive' => undef,
                'database' => '',
                'maxconn' => 0,
                'checkcount' => undef,
                'checkport' => undef,
                'scheduler' => 'rr',
                'realdowncallback' => undef,
                'customcheck' => undef,
            },
            {
                'protocol' => 'tcp',
                'httpmethod' => 'HEAD',
                'qosdown' => '999M',
                'retryinterval' => 4,
                'negotiatetimeout' => 2,
                'checkinterval' => 3,
                'server' => {},
                'qosup' => '0',
                'real' => ['real', 'real'],
                'module' => {
                    'name' => 'url',
                    'option' => '--pattern-match   /foo/bar',
                    'key' => '--pattern-match /foo/bar'
                    },
                'checktimeout' => 1,
                'request' => 'request',
                'checktype' => 'connect',
                'virtualhost' => 'virtualhost',
                'quiescent' => 1,
                'realrecovercallback' => '/bin/cat',
                'sorryserver' => {},
                'service' => 'http',
                'login' => 'login',
                'fallback' => { 'tcp' => 'fallback_return' },
                'passwd' => 'passwd',
                'receive' => 'receive',
                'database' => 'database',
                'maxconn' => 7,
                'checkcount' => 6,
                'checkport' => 10000,
                'scheduler' => 'rr',
                'realdowncallback' => '/bin/cat',
                'customcheck' => '/bin/echo',
            },
            {
                'protocol' => 'tcp',
                'httpmethod' => 'GET',
                'qosdown' => 0,
                'retryinterval' => undef,
                'negotiatetimeout' => undef,
                'checkinterval' => undef,
                'server' => {},
                'qosup' => 0,
                'real' => ['real'],
                'module' => { 'name' => 'sessionless', 'key' => '' },
                'checktimeout' => undef,
                'request' => undef,
                'checktype' => 'negotiate',
                'virtualhost' => undef,
                'quiescent' => undef,
                'realrecovercallback' => undef,
                'sorryserver' => undef,
                'service' => undef,
                'login' => '',
                'fallback' => undef,
                'passwd' => '',
                'receive' => undef,
                'database' => '',
                'maxconn' => 0,
                'checkcount' => undef,
                'checkport' => undef,
                'scheduler' => 'rr',
                'realdowncallback' => undef,
                'customcheck' => undef,
            }
        ]
    );
    default_value();
    local $ld_gethostservbyname_return = {};
    local @ld_gethostservbyname_args   = ();
    local $_ld_service_resolve_called  = 0;
    local @parse_fallback_args         = ();
    local $parse_fallback_return       = 'fallback_return';
    local @parse_real_args             = ();
    local $parse_real_return           = ['real'];
    local $ld_openlog_arg              = ();
    local $ld_openlog_return           = 0;
    create_config($input) or skip 'cannot create config', 7;
    read_config();
    is $main::PROC_STAT{initialized}, 1, 'read_config - full config';
    is_deeply \%main::CONFIG, \%config, 'read_config - set all value';
    is $_ld_service_resolve_called, 4, 'read_config - _ld_service_resolve called';
    is_deeply \@ld_gethostservbyname_args,
              ['virtual_value1',    'tcp', 'virtual_value2', 'tcp', 'virtual_value3', 'tcp',
               'sorryserver_value', 'tcp', 'virtual_value4', 'tcp'],
              'read_config - _ld_gethostservbyname called';
    is_deeply \@parse_fallback_args,
                [14, 'fallback_value', 'fallback=fallback_value',
                 46, 'fallback_value', '    fallback = fallback_value'],
                'read_config - parse_fallback called';
    is_deeply \@parse_real_args,
                [23, 'real_value1', '    real = real_value1',
                 24, 'real_value2', '        real = real_value2',
                 62, 'real_value3', '        real = real_value3'],
                'read_config - parse_real called';
    is $ld_openlog_arg, 'logfile_value', 'read_config - ld_openlog called';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checktimeout=1
CONFIG
    my %config = %main::GLOBAL;
    $config{checktimeout} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - checktimeout result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - checktimeout set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checktimeout
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - checktimeout not defined';
    is $@, "config_error\n", 'read_config - raise config_error(4)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'checktimeout'], 'read_config - config_error args(4)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(4)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checktimeout=yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - checktimeout not numeric';
    is $@, "config_error\n", 'read_config - raise config_error(5)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'checktimeout=yes'], 'read_config - config_error args(5)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(5)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checktimeout=0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - checktimeout zero';
    is $@, "config_error\n", 'read_config - raise config_error(6)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'checktimeout=0'], 'read_config - config_error args(6)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(6)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
negotiatetimeout=1
CONFIG
    my %config = %main::GLOBAL;
    $config{negotiatetimeout} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - negotiatetimeout result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - negotiatetimeout set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
negotiatetimeout
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - negotiatetimeout not defined';
    is $@, "config_error\n", 'read_config - raise config_error(7)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'negotiatetimeout'], 'read_config - config_error args(7)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(7)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
negotiatetimeout=yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - negotiatetimeout not numeric';
    is $@, "config_error\n", 'read_config - raise config_error(8)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'negotiatetimeout=yes'], 'read_config - config_error args(8)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(8)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
negotiatetimeout=0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - negotiatetimeout zero';
    is $@, "config_error\n", 'read_config - raise config_error(9)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'negotiatetimeout=0'], 'read_config - config_error args(9)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(9)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checkinterval=1
CONFIG
    my %config = %main::GLOBAL;
    $config{checkinterval} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - checkinterval result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - checkinterval set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checkinterval
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - checkinterval not defined';
    is $@, "config_error\n", 'read_config - raise config_error(10)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'checkinterval'], 'read_config - config_error args(10)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(10)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checkinterval=yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - checkinterval not numeric';
    is $@, "config_error\n", 'read_config - raise config_error(11)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'checkinterval=yes'], 'read_config - config_error args(11)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(11)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checkinterval=0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - checkinterval zero';
    is $@, "config_error\n", 'read_config - raise config_error(12)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'checkinterval=0'], 'read_config - config_error args(12)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(12)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
retryinterval=1
CONFIG
    my %config = %main::GLOBAL;
    $config{retryinterval} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - retryinterval result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - retryinterval set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
retryinterval
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - retryinterval not defined';
    is $@, "config_error\n", 'read_config - raise config_error(13)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'retryinterval'], 'read_config - config_error args(13)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(13)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
retryinterval=yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - retryinterval not numeric';
    is $@, "config_error\n", 'read_config - raise config_error(14)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'retryinterval=yes'], 'read_config - config_error args(14)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(14)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
retryinterval=0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - retryinterval zero';
    is $@, "config_error\n", 'read_config - raise config_error(15)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'retryinterval=0'], 'read_config - config_error args(15)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(15)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
configinterval=1
CONFIG
    my %config = %main::GLOBAL;
    $config{configinterval} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - configinterval result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - configinterval set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
configinterval
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - configinterval not defined';
    is $@, "config_error\n", 'read_config - raise config_error(16)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'configinterval'], 'read_config - config_error args(16)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(16)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
configinterval=yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - configinterval not numeric';
    is $@, "config_error\n", 'read_config - raise config_error(17)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'configinterval=yes'], 'read_config - config_error args(17)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(17)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
configinterval=0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - configinterval zero';
    is $@, "config_error\n", 'read_config - raise config_error(18)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'configinterval=0'], 'read_config - config_error args(18)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(18)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checkcount=1
CONFIG
    my %config = %main::GLOBAL;
    $config{checkcount} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - checkcount result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - checkcount set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checkcount
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - checkcount not defined';
    is $@, "config_error\n", 'read_config - raise config_error(19)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'checkcount'], 'read_config - config_error args(19)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(19)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checkcount=yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - checkcount not numeric';
    is $@, "config_error\n", 'read_config - raise config_error(20)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'checkcount=yes'], 'read_config - config_error args(20)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(20)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
checkcount=0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - checkcount zero';
    is $@, "config_error\n", 'read_config - raise config_error(21)';
    is_deeply \@config_error_args, [1, 'ERR0101', 'checkcount=0'], 'read_config - config_error args(21)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(21)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
autoreload=YeS
CONFIG
    my %config = %main::GLOBAL;
    $config{autoreload} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - autoreload yes result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - autoreload yes set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
autoreload=nO
CONFIG
    my %config = %main::GLOBAL;
    $config{autoreload} = 0;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - autoreload no result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - autoreload no set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
autoreload
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - autoreload not defined';
    is $@, "config_error\n", 'read_config - raise config_error(22)';
    is_deeply \@config_error_args, [1, 'ERR0102', 'autoreload'], 'read_config - config_error args(22)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(22)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
autoreload=YEAH
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - autoreload not lower yes or no(1)';
    is $@, "config_error\n", 'read_config - raise config_error(23)';
    is_deeply \@config_error_args, [1, 'ERR0102', 'autoreload=YEAH'], 'read_config - config_error args(23)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(23)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
autoreload=0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - autoreload not lower yes or no(2)';
    is $@, "config_error\n", 'read_config - raise config_error(24)';
    is_deeply \@config_error_args, [1, 'ERR0102', 'autoreload=0'], 'read_config - config_error args(24)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(24)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
quiescent=yEs
CONFIG
    my %config = %main::GLOBAL;
    $config{quiescent} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - quiescent yes result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - quiescent yes set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
quiescent=No
CONFIG
    my %config = %main::GLOBAL;
    $config{quiescent} = 0;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - quiescent no result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - quiescent no set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
quiescent
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - quiescent not defined';
    is $@, "config_error\n", 'read_config - raise config_error(25)';
    is_deeply \@config_error_args, [1, 'ERR0102', 'quiescent'], 'read_config - config_error args(25)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(25)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
quiescent=NOPE
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - quiescent not lower yes or no(1)';
    is $@, "config_error\n", 'read_config - raise config_error(26)';
    is_deeply \@config_error_args, [1, 'ERR0102', 'quiescent=NOPE'], 'read_config - config_error args(26)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(26)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
quiescent=0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - quiescent not lower yes or no(2)';
    is $@, "config_error\n", 'read_config - raise config_error(27)';
    is_deeply \@config_error_args, [1, 'ERR0102', 'quiescent=0'], 'read_config - config_error args(27)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(27)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
fallback
CONFIG
    my %config = %main::GLOBAL;
    $config{fallback} = { tcp => undef };
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @parse_fallback_args = ();
    local $parse_fallback_return = undef;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - fallback not defined';
    is_deeply \%main::CONFIG, \%config, 'read_config - fallback set undef..';
    is_deeply \@parse_fallback_args, [1, undef, 'fallback'], 'read_config - parse_fallback_args(1)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
fallback=localhost:http
CONFIG
    my %config = %main::GLOBAL;
    $config{fallback} = { tcp => 'fallback' };
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @parse_fallback_args = ();
    local $parse_fallback_return = 'fallback';
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - fallback ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - fallback set normal';
    is_deeply \@parse_fallback_args, [1, 'localhost:http', 'fallback=localhost:http'], 'read_config - parse_fallback_args(2)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
callback=/bin/sh
CONFIG
    my %config = %main::GLOBAL;
    $config{callback} = '/bin/sh';
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - callback result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - callback set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
callback
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - callback not defined';
    is $@, "config_error\n", 'read_config - raise config_error(28)';
    is_deeply \@config_error_args, [1, 'ERR0117', 'callback'], 'read_config - config_error args(28)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(28)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
callback=/proc/1
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - callback cannot execute';
    is $@, "config_error\n", 'read_config - raise config_error(29)';
    is_deeply \@config_error_args, [1, 'ERR0117', 'callback=/proc/1'], 'read_config - config_error args(29)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(29)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
callback=/proc/100000
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - callback no such file';
    is $@, "config_error\n", 'read_config - raise config_error(30)';
    is_deeply \@config_error_args, [1, 'ERR0117', 'callback=/proc/100000'], 'read_config - config_error args(30)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(30)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
callback=/proc
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - callback directory';
    is $@, "config_error\n", 'read_config - raise config_error(31)';
    is_deeply \@config_error_args, [1, 'ERR0117', 'callback=/proc'], 'read_config - config_error args(31)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(31)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
execute=/bin/sh
execute=/bin/bash
CONFIG
    my %config = %main::GLOBAL;
    $config{execute} = { '/bin/sh' => 1, '/bin/bash' => 1 };
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - execute result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - execute set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
execute
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - execute not defined';
    is $@, "config_error\n", 'read_config - raise config_error(32)';
    is_deeply \@config_error_args, [1, 'ERR0116', 'execute'], 'read_config - config_error args(32)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(32)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
execute=/proc/100000
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - execute no such file';
    is $@, "config_error\n", 'read_config - raise config_error(33)';
    is_deeply \@config_error_args, [1, 'ERR0116', 'execute=/proc/100000'], 'read_config - config_error args(33)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(33)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
execute=/proc
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - execute directory';
    is $@, "config_error\n", 'read_config - raise config_error(34)';
    is_deeply \@config_error_args, [1, 'ERR0116', 'execute=/proc'], 'read_config - config_error args(34)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(34)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
logfile=/bin/sh
CONFIG
    my %config = %main::GLOBAL;
    $config{logfile} = '/bin/sh';
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - logfile result ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - logfile set config ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
logfile=sys
CONFIG
    my %config = %main::GLOBAL;
    $config{logfile} = 'sys';
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local $ld_openlog_arg = undef;
    local $ld_openlog_return = 0;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - logfile ld_openlog ok';
    is_deeply \%main::CONFIG, \%config, 'read_config - logfile ld_openlog ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
logfile
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - logfile not defined';
    is $@, "config_error\n", 'read_config - raise config_error(35)';
    is_deeply \@config_error_args, [1, 'ERR0118', 'logfile'], 'read_config - config_error args(35)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(35)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
logfile=sys
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 5;
    local $ld_openlog_arg = undef;
    local $ld_openlog_return = 1;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - logfile ld_openlog fail';
    is $@, "config_error\n", 'read_config - raise config_error(36)';
    is_deeply \@config_error_args, [1, 'ERR0118', 'logfile=sys'], 'read_config - config_error args(36)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(36)';
    is $ld_openlog_arg, 'sys', 'read_config - ld_openlog arg';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
supervised
CONFIG
    my %config = %main::GLOBAL;
    $config{supervised} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - supervised result ok(1)';
    is_deeply \%main::CONFIG, \%config, 'read_config - supervised set config ok(1)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
supervised = yes
CONFIG
    my %config = %main::GLOBAL;
    $config{supervised} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - supervised result ok(2)';
    is_deeply \%main::CONFIG, \%config, 'read_config - supervised set config ok(2)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
supervised = no
CONFIG
    my %config = %main::GLOBAL;
    $config{supervised} = 1;
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - supervised result ok(3)';
    is_deeply \%main::CONFIG, \%config, 'read_config - supervised set config ok(3)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
unknown
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - unknown config';
    is $@, "config_error\n", 'read_config - raise config_error(37)';
    is_deeply \@config_error_args, [1, 'ERR0120', 'unknown'], 'read_config - config_error args(37)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(37)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 5;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = undef;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual not defined';
    is $@, "config_error\n", 'read_config - raise config_error(38)';
    is_deeply \@config_error_args, [1, 'ERR0114', 'virtual'], 'read_config - config_error args(38)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - not set anything(38)';
    is_deeply \@ld_gethostservbyname_args, [undef, 'tcp'], 'read_config - ld_gethostservbyname args(1)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual normal ok';
    is $main::CONFIG{virtual}[0]{server}{ip}, '127.0.0.1', 'read_config - set virtual ip ok';
    is $main::CONFIG{virtual}[0]{server}{port}, 80, 'read_config - set virtual port ok';
    is_deeply \@ld_gethostservbyname_args, ['localhost:http', 'tcp'], 'read_config - ld_gethostservbyname args(2)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
    real=localhost:http
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - no virtual line';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(1)';
    is_deeply \@config_error_args, [1, 'ERR0119', '    real=localhost:http'], 'read_config - virtual section - config_error args(1)';
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'read_config - virtual section - not set anything(1)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    unknown
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - unknown config';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(2)';
    is_deeply \@config_error_args, [2, 'ERR0120', '    unknown'], 'read_config - virtual section - config_error args(2)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    real="realhost1:ftp"
    real="realhost2:ftp"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 6;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    local @parse_real_args = ();
    local $parse_real_return = [ {server => { ip => '127.0.0.2', port => 21 } } ];
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - real ok';
    is $main::CONFIG{virtual}[0]{real}[0]{server}{ip}, '127.0.0.2', 'read_config - virtual section - set real ip ok(1)';
    is $main::CONFIG{virtual}[0]{real}[0]{server}{port}, 21, 'read_config - virtual section - set real port ok(1)';
    is $main::CONFIG{virtual}[0]{real}[1]{server}{ip}, '127.0.0.2', 'read_config - virtual section - set real ip ok(2)';
    is $main::CONFIG{virtual}[0]{real}[1]{server}{port}, 21, 'read_config - virtual section - set real port ok(2)';
    is_deeply \@parse_real_args, [2, 'realhost1:ftp', '    real="realhost1:ftp"',
                                  3, 'realhost2:ftp', '    real="realhost2:ftp"'], 'read_config - virtual section - parse_real args';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    real
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    local @parse_real_args = ();
    local $parse_real_return = undef;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - real not defined';
    is_deeply \@parse_real_args, [2, undef, '    real'], 'read_config - virtual section - parse_real args(3)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    request="/index.html"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - request ok';
    is $main::CONFIG{virtual}[0]{request}, '/index.html', 'read_config - virtual section - set request ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    request =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - request not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(4)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    request ='], 'read_config - virtual section - config_error args(4)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    request = ""
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - empty request';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(5)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    request = ""'], 'read_config - virtual section - config_error args(5)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    receive='body'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - receive ok';
    is $main::CONFIG{virtual}[0]{receive}, 'body', 'read_config - virtual section - set receive ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    receive =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - receive not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(4)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    receive ='], 'read_config - virtual section - config_error args(4)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    receive = ""
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - empty receive';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(5)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    receive = ""'], 'read_config - virtual section - config_error args(5)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    login= anonymous
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - login ok';
    is $main::CONFIG{virtual}[0]{login}, 'anonymous', 'read_config - virtual section - set login ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    login =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - login not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(4)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    login ='], 'read_config - virtual section - config_error args(4)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    login = ""
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - empty login';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(5)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    login = ""'], 'read_config - virtual section - config_error args(5)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    passwd = "ftp\@localhost"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - passwd ok';
    is $main::CONFIG{virtual}[0]{passwd}, 'ftp@localhost', 'read_config - virtual section - set passwd ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    passwd =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - passwd not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(4)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    passwd ='], 'read_config - virtual section - config_error args(4)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    passwd = ""
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - empty passwd';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(5)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    passwd = ""'], 'read_config - virtual section - config_error args(5)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    database = "template"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - database ok';
    is $main::CONFIG{virtual}[0]{database}, 'template', 'read_config - virtual section - set database ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    database =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - database not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(4)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    database ='], 'read_config - virtual section - config_error args(4)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    database = ""
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - empty database';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(5)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    database = ""'], 'read_config - virtual section - config_error args(5)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    virtualhost = "vip.domain.com"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - virtualhost ok';
    is $main::CONFIG{virtual}[0]{virtualhost}, 'vip.domain.com', 'read_config - virtual section - set virtualhost ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    virtualhost =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - virtualhost not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(4)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    virtualhost ='], 'read_config - virtual section - config_error args(4)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    virtualhost = ""
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - empty virtualhost';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(5)';
    is_deeply \@config_error_args, [2, 'ERR0103', '    virtualhost = ""'], 'read_config - virtual section - config_error args(5)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktype = CoNnEcT
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checktype connect ok';
    is $main::CONFIG{virtual}[0]{checktype}, 'connect', 'read_config - virtual section - set checktype connect ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktype = nEgOtIaTe
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checktype negotiate ok';
    is $main::CONFIG{virtual}[0]{checktype}, 'negotiate', 'read_config - virtual section - set checktype negotiate ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktype = piNG
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checktype ping ok';
    is $main::CONFIG{virtual}[0]{checktype}, 'ping', 'read_config - virtual section - set checktype ping ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktype = Off
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checktype off ok';
    is $main::CONFIG{virtual}[0]{checktype}, 'off', 'read_config - virtual section - set checktype off ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktype = oN
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checktype on ok';
    is $main::CONFIG{virtual}[0]{checktype}, 'on', 'read_config - virtual section - set checktype on ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktype = 1
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checktype combined ok';
    is $main::CONFIG{virtual}[0]{checktype}, '1', 'read_config - virtual section - set checktype combined ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktype =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checktype not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(6)';
    is_deeply \@config_error_args, [2, 'ERR0104', '    checktype ='], 'read_config - virtual section - config_error args(6)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktype = unknown
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checktype not valid';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(7)';
    is_deeply \@config_error_args, [2, 'ERR0104', '    checktype = unknown'], 'read_config - virtual section - config_error args(7)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktype = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checktype is zero';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(8)';
    is_deeply \@config_error_args, [2, 'ERR0104', '    checktype = 0'], 'read_config - virtual section - config_error args(8)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktimeout = '1'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checktimeout ok';
    is $main::CONFIG{virtual}[0]{checktimeout}, '1', 'read_config - virtual section - set checktimeout ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktimeout
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checktimeout not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(9)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    checktimeout'], 'read_config - virtual section - config_error args(9)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktimeout = yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checktimeout not numeric';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(10)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    checktimeout = yes'], 'read_config - virtual section - config_error args(10)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checktimeout = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checktimeout is zero';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(11)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    checktimeout = 0'], 'read_config - virtual section - config_error args(11)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    negotiatetimeout = '1'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - negotiatetimeout ok';
    is $main::CONFIG{virtual}[0]{negotiatetimeout}, '1', 'read_config - virtual section - set negotiatetimeout ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    negotiatetimeout
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - negotiatetimeout not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(12)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    negotiatetimeout'], 'read_config - virtual section - config_error args(12)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    negotiatetimeout = yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - negotiatetimeout not numeric';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(13)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    negotiatetimeout = yes'], 'read_config - virtual section - config_error args(13)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    negotiatetimeout = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - negotiatetimeout is zero';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(14)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    negotiatetimeout = 0'], 'read_config - virtual section - config_error args(14)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkinterval = '1'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checkinterval ok';
    is $main::CONFIG{virtual}[0]{checkinterval}, '1', 'read_config - virtual section - set checkinterval ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkinterval
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkinterval not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(15)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    checkinterval'], 'read_config - virtual section - config_error args(15)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkinterval = yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkinterval not numeric';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(16)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    checkinterval = yes'], 'read_config - virtual section - config_error args(16)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkinterval = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkinterval is zero';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(17)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    checkinterval = 0'], 'read_config - virtual section - config_error args(17)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    retryinterval = '1'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - retryinterval ok';
    is $main::CONFIG{virtual}[0]{retryinterval}, '1', 'read_config - virtual section - set retryinterval ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    retryinterval
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - retryinterval not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(18)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    retryinterval'], 'read_config - virtual section - config_error args(18)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    retryinterval = yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - retryinterval not numeric';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(19)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    retryinterval = yes'], 'read_config - virtual section - config_error args(19)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    retryinterval = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - retryinterval is zero';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(20)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    retryinterval = 0'], 'read_config - virtual section - config_error args(20)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkcount = "1"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checkcount ok';
    is $main::CONFIG{virtual}[0]{checkcount}, '1', 'read_config - virtual section - set checkcount ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkcount
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkcount not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(21)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    checkcount'], 'read_config - virtual section - config_error args(21)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkcount = yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkcount not numeric';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(22)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    checkcount = yes'], 'read_config - virtual section - config_error args(22)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkcount = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkcount is zero';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(23)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    checkcount = 0'], 'read_config - virtual section - config_error args(23)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    maxconn = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - maxconn ok';
    is $main::CONFIG{virtual}[0]{maxconn}, '0', 'read_config - virtual section - set maxconn ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    maxconn
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - maxconn not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(24)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    maxconn'], 'read_config - virtual section - config_error args(24)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    maxconn = yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - maxconn not numeric';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(25)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    maxconn = yes'], 'read_config - virtual section - config_error args(25)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    maxconn = -1
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - maxconn is minus';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(26)';
    is_deeply \@config_error_args, [2, 'ERR0101', '    maxconn = -1'], 'read_config - virtual section - config_error args(26)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkport = "1"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checkport min ok';
    is $main::CONFIG{virtual}[0]{checkport}, '1', 'read_config - virtual section - set checkport min ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkport = 65535
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - checkport max ok';
    is $main::CONFIG{virtual}[0]{checkport}, '65535', 'read_config - virtual section - set checkport max ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkport
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkport not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(27)';
    is_deeply \@config_error_args, [2, 'ERR0108', '    checkport'], 'read_config - virtual section - config_error args(27)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkport = yes
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkport not numeric';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(28)';
    is_deeply \@config_error_args, [2, 'ERR0108', '    checkport = yes'], 'read_config - virtual section - config_error args(28)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkport = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkport is zero';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(29';
    is_deeply \@config_error_args, [2, 'ERR0108', '    checkport = 0'], 'read_config - virtual section - config_error args(29)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    checkport = 65536
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - checkport is over range';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(30)';
    is_deeply \@config_error_args, [2, 'ERR0108', '    checkport = 65536'], 'read_config - virtual section - config_error args(30)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    scheduler = lC
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - scheduler lc ok';
    is $main::CONFIG{virtual}[0]{scheduler}, 'lc', 'read_config - virtual section - set scheduler lc ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    scheduler = 'Rr'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - scheduler rr ok';
    is $main::CONFIG{virtual}[0]{scheduler}, 'rr', 'read_config - virtual section - set scheduler rr ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    scheduler = "WRR"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - scheduler wrr ok';
    is $main::CONFIG{virtual}[0]{scheduler}, 'wrr', 'read_config - virtual section - set scheduler wrr ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    scheduler =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - scheduler not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(31)';
    is_deeply \@config_error_args, [2, 'ERR0105', '    scheduler ='], 'read_config - virtual section - config_error args(31)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    scheduler = unknown
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - scheduler not valid';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(32)';
    is_deeply \@config_error_args, [2, 'ERR0105', '    scheduler = unknown'], 'read_config - virtual section - config_error args(32)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    protocol = 'TcP'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - protocol ok';
    is $main::CONFIG{virtual}[0]{protocol}, 'tcp', 'read_config - virtual section - set protocol ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    protocol =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - protocol not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(33)';
    is_deeply \@config_error_args, [2, 'ERR0109', '    protocol ='], 'read_config - virtual section - config_error args(33)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    protocol = unknown
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - protocol not valid';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(34)';
    is_deeply \@config_error_args, [2, 'ERR0109', '    protocol = unknown'], 'read_config - virtual section - config_error args(34)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service=hTTp
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service http ok';
    is $main::CONFIG{virtual}[0]{service}, 'http', 'read_config - virtual section - set service http ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service=hTTpS
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service https ok';
    is $main::CONFIG{virtual}[0]{service}, 'https', 'read_config - virtual section - set service https ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service=LdAp
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service ldap ok';
    is $main::CONFIG{virtual}[0]{service}, 'ldap', 'read_config - virtual section - set service ldap ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service= "FTP"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service ftp ok';
    is $main::CONFIG{virtual}[0]{service}, 'ftp', 'read_config - virtual section - set service ftp ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service=  'SmtP'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service smtp ok';
    is $main::CONFIG{virtual}[0]{service}, 'smtp', 'read_config - virtual section - set service smtp ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service =   POp
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service pop ok';
    is $main::CONFIG{virtual}[0]{service}, 'pop', 'read_config - virtual section - set service pop ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service  = "ImAp"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service imap ok';
    is $main::CONFIG{virtual}[0]{service}, 'imap', 'read_config - virtual section - set service imap ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service  =  'NNtp'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service nntp ok';
    is $main::CONFIG{virtual}[0]{service}, 'nntp', 'read_config - virtual section - set service nntp ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service =DNS
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service dns ok';
    is $main::CONFIG{virtual}[0]{service}, 'dns', 'read_config - virtual section - set service dns ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service= mysql
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service mysql ok';
    is $main::CONFIG{virtual}[0]{service}, 'mysql', 'read_config - virtual section - set service mysql ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service=    'PGSQL'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service pgsql ok';
    is $main::CONFIG{virtual}[0]{service}, 'pgsql', 'read_config - virtual section - set service pgsql ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service= sip
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service sip ok';
    is $main::CONFIG{virtual}[0]{service}, 'sip', 'read_config - virtual section - set service sip ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service=  NONE
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - service none ok';
    is $main::CONFIG{virtual}[0]{service}, 'none', 'read_config - virtual section - set service none ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - service not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(31)';
    is_deeply \@config_error_args, [2, 'ERR0106', '    service ='], 'read_config - virtual section - config_error args(31)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    service = unknown
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - service not valid';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(32)';
    is_deeply \@config_error_args, [2, 'ERR0106', '    service = unknown'], 'read_config - virtual section - config_error args(32)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    httpmethod=get
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - httpmethod GET ok';
    is $main::CONFIG{virtual}[0]{httpmethod}, 'GET', 'read_config - virtual section - set httpmethod GET ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    httpmethod= 'HeaD'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - httpmethod HEAD ok';
    is $main::CONFIG{virtual}[0]{httpmethod}, 'HEAD', 'read_config - virtual section - set httpmethod HEAD ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    httpmethod =
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - httpmethod not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(31)';
    is_deeply \@config_error_args, [2, 'ERR0110', '    httpmethod ='], 'read_config - virtual section - config_error args(31)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    httpmethod = unknown
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - httpmethod not valid';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(32)';
    is_deeply \@config_error_args, [2, 'ERR0110', '    httpmethod = unknown'], 'read_config - virtual section - config_error args(32)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    fallback
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    local @parse_fallback_args = ();
    local $parse_fallback_return = undef;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - fallback not defined';
    is_deeply \@parse_fallback_args, [2, undef, '    fallback'], 'read_config - virtual section - parse_fallback_args(1)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    fallback=localhost:http
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    local @parse_fallback_args = ();
    local $parse_fallback_return = 'fallback';
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - fallback ok';
    is_deeply \@parse_fallback_args, [2, 'localhost:http', '    fallback=localhost:http'], 'read_config - virtual section - parse_fallback_args(2)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    quiescent= YEs
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - quiescent yes ok';
    is $main::CONFIG{virtual}[0]{quiescent}, '1', 'read_config - virtual section - set quiescent yes ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    quiescent= "NO"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - quiescent no ok';
    is $main::CONFIG{virtual}[0]{quiescent}, '0', 'read_config - virtual section - set quiescent no ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    quiescent
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - quiescent not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(33)';
    is_deeply \@config_error_args, [2, 'ERR0102', '    quiescent'], 'read_config - virtual section - config_error args(33)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    quiescent=YEP
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - quiescent not lower yes or no(1)';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(34)';
    is_deeply \@config_error_args, [2, 'ERR0102', '    quiescent=YEP'], 'read_config - virtual section - config_error args(34)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    quiescent=0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - quiescent not lower yes or no(2)';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(35)';
    is_deeply \@config_error_args, [2, 'ERR0102', '    quiescent=0'], 'read_config - virtual section - config_error args(35)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    module= URL  --reschedule --pattern-match  "foobar"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - module url ok';
    is_deeply $main::CONFIG{virtual}[0]{module}, { name => 'url', option => '--reschedule --pattern-match  foobar', key => '--pattern-match foobar' }, 'read_config - virtual section - set module url ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    module= 'cINSERT --cookie-name 'Monkey' --cookie-expire 864'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - module cinsert ok';
    is_deeply $main::CONFIG{virtual}[0]{module}, { name => 'cinsert', option => '--cookie-name Monkey --cookie-expire 864', key => '--cookie-name Monkey' }, 'read_config - virtual section - set module cinsert ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    module=  "sslID"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - module sslid ok';
    is_deeply $main::CONFIG{virtual}[0]{module}, { name => 'sslid', option => undef, key => '' }, 'read_config - virtual section - set module sslid ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    module=   SessionLess
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - module sessionless ok';
    is_deeply $main::CONFIG{virtual}[0]{module}, { name => 'sessionless', option => undef, key => '' }, 'read_config - virtual section - set module sessionless ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    module=  'Cinsert --reschedule'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - module cinsert omit ok';
    is_deeply $main::CONFIG{virtual}[0]{module}, { name => 'cinsert', option => '--cookie-name CookieName --reschedule', key => '--cookie-name CookieName' }, 'read_config - virtual section - set module cinsert omit ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    module
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - module not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(36)';
    is_deeply \@config_error_args, [2, 'ERR0111', '    module'], 'read_config - virtual section - config_error args(36)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    module = unknown
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - unknown module';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(37)';
    is_deeply \@config_error_args, [2, 'ERR0111', '    module = unknown'], 'read_config - virtual section - config_error args(37)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    module = url
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - url module key not exist';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(39)';
    is_deeply \@config_error_args, [2, 'ERR0112', 'url', q{--pattern-match' or `--uri-pattern-match' or `--host-pattern-match}, '    module = url'], 'read_config - virtual section - config_error args(39)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    sorryserver  =  sorryhost:ssh
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - sorryserver ok';
    is_deeply $main::CONFIG{virtual}[0]{sorryserver}, { ip => '127.0.0.1', port => 80 }, 'read_config - virtual section - set sorryserver ok';
    is_deeply \@ld_gethostservbyname_args, ['localhost:http', 'tcp', 'sorryhost:ssh', 'tcp'], 'read_config - virtual section - ld_gethostservbyname args(3)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    sorryserver
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    local $ld_gethostservbyname_undef  = 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - sorryserver not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(40)';
    is_deeply \@config_error_args, [2, 'ERR0114', '    sorryserver'], 'read_config - virtual section - config_error args(40)';
    is_deeply \@ld_gethostservbyname_args, ['localhost:http', 'tcp', undef, 'tcp'], 'read_config - virtual section - ld_gethostservbyname args(4)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    sorryserver = sorryhost:ftp
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 4;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    local $ld_gethostservbyname_undef  = 2;
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - sorryserver not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(41)';
    is_deeply \@config_error_args, [2, 'ERR0114', '    sorryserver = sorryhost:ftp'], 'read_config - virtual section - config_error args(41)';
    is_deeply \@ld_gethostservbyname_args, ['localhost:http', 'tcp', 'sorryhost:ftp', 'tcp'], 'read_config - virtual section - ld_gethostservbyname args(5)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosup 0 ok';
    is $main::CONFIG{virtual}[0]{qosup}, '0', 'read_config - virtual section - set qosup 0 ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup = "1k"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosup min kilo ok';
    is $main::CONFIG{virtual}[0]{qosup}, '1K', 'read_config - virtual section - set qosup min kilo ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup = '999K'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosup max kilo ok';
    is $main::CONFIG{virtual}[0]{qosup}, '999K', 'read_config - virtual section - set qosup max kilo ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup = 1m
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosup min mega ok';
    is $main::CONFIG{virtual}[0]{qosup}, '1M', 'read_config - virtual section - set qosup min mega ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup = "999M"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosup max mega ok';
    is $main::CONFIG{virtual}[0]{qosup}, '999M', 'read_config - virtual section - set qosup max mega ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup = '1G'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosup min giga ok';
    is $main::CONFIG{virtual}[0]{qosup}, '1G', 'read_config - virtual section - set qosup min giga ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup = 999g
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosup max giga ok';
    is $main::CONFIG{virtual}[0]{qosup}, '999G', 'read_config - virtual section - set qosup max giga ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - qosup not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(42)';
    is_deeply \@config_error_args, [2, 'ERR0113', '    qosup'], 'read_config - virtual section - config_error args(42)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup=100
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - qosup not valid format(1)';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(43-1)';
    is_deeply \@config_error_args, [2, 'ERR0113', '    qosup=100'], 'read_config - virtual section - config_error args(43-1)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosup=1000K
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - qosup not valid format(2)';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(43-2)';
    is_deeply \@config_error_args, [2, 'ERR0113', '    qosup=1000K'], 'read_config - virtual section - config_error args(43-2)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown = 0
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosdown 0 ok';
    is $main::CONFIG{virtual}[0]{qosdown}, '0', 'read_config - virtual section - set qosdown 0 ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown = "1k"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosdown min kilo ok';
    is $main::CONFIG{virtual}[0]{qosdown}, '1K', 'read_config - virtual section - set qosdown min kilo ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown = '999K'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosdown max kilo ok';
    is $main::CONFIG{virtual}[0]{qosdown}, '999K', 'read_config - virtual section - set qosdown max kilo ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown = 1m
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosdown min mega ok';
    is $main::CONFIG{virtual}[0]{qosdown}, '1M', 'read_config - virtual section - set qosdown min mega ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown = "999M"
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosdown max mega ok';
    is $main::CONFIG{virtual}[0]{qosdown}, '999M', 'read_config - virtual section - set qosdown max mega ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown = '1G'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosdown min giga ok';
    is $main::CONFIG{virtual}[0]{qosdown}, '1G', 'read_config - virtual section - set qosdown min giga ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown = 999g
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - qosdown max giga ok';
    is $main::CONFIG{virtual}[0]{qosdown}, '999G', 'read_config - virtual section - set qosdown max giga ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - qosdown not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(44)';
    is_deeply \@config_error_args, [2, 'ERR0113', '    qosdown'], 'read_config - virtual section - config_error args(44)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown=100
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - qosdown not valid format(1)';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(45-1)';
    is_deeply \@config_error_args, [2, 'ERR0113', '    qosdown=100'], 'read_config - virtual section - config_error args(45-1)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    qosdown= 1000M
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - qosdown not valid format(2)';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(45-2)';
    is_deeply \@config_error_args, [2, 'ERR0113', '    qosdown= 1000M'], 'read_config - virtual section - config_error args(45-2)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realdowncallback =  '/bin/sh'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - realdowncallback ok';
    is $main::CONFIG{virtual}[0]{realdowncallback}, '/bin/sh', 'read_config - virtual section - set realdowncallback ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realdowncallback
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - realdowncallback not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(46)';
    is_deeply \@config_error_args, [2, 'ERR0117', '    realdowncallback'], 'read_config - virtual section - config_error args(46)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realdowncallback=/proc/1
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - realdowncallback cannot execute';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(47)';
    is_deeply \@config_error_args, [2, 'ERR0117', '    realdowncallback=/proc/1'], 'read_config - virtual section - config_error args(47)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realdowncallback=/proc/100000
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - realdowncallback no such file';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(48)';
    is_deeply \@config_error_args, [2, 'ERR0117', '    realdowncallback=/proc/100000'], 'read_config - virtual section - config_error args(48)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realdowncallback=/proc
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - realdowncallback directory';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(49)';
    is_deeply \@config_error_args, [2, 'ERR0117', '    realdowncallback=/proc'], 'read_config - virtual section - config_error args(49)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realrecovercallback =  '/bin/sh'
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 2;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 1, 'read_config - virtual section - realrecovercallback ok';
    is $main::CONFIG{virtual}[0]{realrecovercallback}, '/bin/sh', 'read_config - virtual section - set realrecovercallback ok';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realrecovercallback
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - realrecovercallback not defined';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(50)';
    is_deeply \@config_error_args, [2, 'ERR0117', '    realrecovercallback'], 'read_config - virtual section - config_error args(50)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realrecovercallback=/proc/1
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - realrecovercallback cannot execute';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(51)';
    is_deeply \@config_error_args, [2, 'ERR0117', '    realrecovercallback=/proc/1'], 'read_config - virtual section - config_error args(51)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realrecovercallback=/proc/100000
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - realrecovercallback no such file';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(52)';
    is_deeply \@config_error_args, [2, 'ERR0117', '    realrecovercallback=/proc/100000'], 'read_config - virtual section - config_error args(52)';
    remove_config();
}
SKIP: {
    my $input = <<"CONFIG";
virtual=localhost:http
    realrecovercallback=/proc
CONFIG
    default_value();
    create_config($input) or skip 'cannot create config', 3;
    local @ld_gethostservbyname_args = ();
    local $ld_gethostservbyname_return = { ip => '127.0.0.1', port => 80 };
    eval {
        read_config();
    };
    is $main::PROC_STAT{initialized}, 0, 'read_config - virtual section - realrecovercallback directory';
    is $@, "config_error\n", 'read_config - virtual section - raise config_error(53)';
    is_deeply \@config_error_args, [2, 'ERR0117', '    realrecovercallback=/proc'], 'read_config - virtual section - config_error args(53)';
    remove_config();
}
#   - reread_config
{
    local *read_config = \&__read_config;
    local $main::CONFIG{virtual} = undef;
    local $read_config_virtual   = [ foo => 2, bar => 4 ];
    local $main::CONFIG{execute} = { 'foo.conf' => 1, 'bar.conf' => 1 };
    local $read_config_execute   = { 'foo.conf' => 1, 'hoge.conf' => 1 };
    local $system_wrapper_return = 1;
    local $system_wrapper_called = 0;
    local @ld_cmd_children_args  = ();
    reread_config();
    is $system_wrapper_called, 2, 'reread_config - virtual is undef';
    is_deeply \@ld_cmd_children_args, [ ['stop', {'bar.conf' => 1} ], ['start', {'hoge.conf' => 1} ] ], 'reread_config - 1 old config stop, 1 new config start';
}
{
    local *read_config = \&__read_config;
    local $main::CONFIG{virtual} = [ foo => 1, bar => 2 ];
    local $read_config_virtual   = [ foo => 2, bar => 4 ];
    local $main::CONFIG{execute} = undef;
    local $read_config_execute   = { 'foo.conf' => 1, 'hoge.conf' => 1 };
    local $system_wrapper_return = 1;
    local $system_wrapper_called = 0;
    local @ld_cmd_children_args  = ();
    reread_config();
    is $system_wrapper_called, 0, 'reread_config - execute is undef';
    is_deeply \@ld_cmd_children_args, [ ['stop', {} ], ['start', {'foo.conf' => 1, 'hoge.conf' => 1} ] ], 'reread_config - 0 old config stop, 2 new config start';
}
{
    local *read_config = \&__read_config;
    local $main::CONFIG{virtual} = [ foo => 1, bar => 2 ];
    local $read_config_virtual   = [ foo => 2, bar => 4 ];
    local $main::CONFIG{execute} = { 'foo.conf' => 1, 'bar.conf' => 1 };
    local $read_config_execute   = { 'foo.conf' => 1, 'hoge.conf' => 1 };
    local $system_wrapper_return = 1;
    local $system_wrapper_called = 0;
    local @ld_cmd_children_args  = ();
    reread_config();
    is $system_wrapper_called, 2, 'reread_config - 1 exist config reload, but failed and then start';
    is_deeply \@ld_cmd_children_args, [ ['stop', {'bar.conf' => 1} ], ['start', {'hoge.conf' => 1} ] ], 'reread_config - 1 old config stop, 1 new config start';
}
{
    local *read_config = \&__read_config;
    local $main::CONFIG{virtual} = [ foo => 1, bar => 2 ];
    local $read_config_virtual   = [ foo => 2, bar => 4 ];
    local $main::CONFIG{execute} = { 'foo.conf' => 1, 'bar.conf' => 1 };
    local $read_config_execute   = { 'foo.conf' => 1, 'hoge.conf' => 1 };
    local $system_wrapper_return = 0;
    local $system_wrapper_called = 0;
    local $read_config_die = 1;
    local @ld_cmd_children_args  = ();
    reread_config();
    is $system_wrapper_called, 2, 'reread_config - all old config reload(1)';
    is_deeply \@ld_cmd_children_args, [ ['stop', {} ], ['start', {} ] ], 'reread_config - no stop / start command(1)';
}
{
    local *read_config = \&__read_config;
    local $main::CONFIG{virtual} = [ foo => 1, bar => 2 ];
    local $read_config_virtual   = [ foo => 2, bar => 4 ];
    local $main::CONFIG{execute} = { 'foo.conf' => 1, 'bar.conf' => 1 };
    local $read_config_execute   = { 'foo.conf' => 1, 'hoge.conf' => 1 };
    local $system_wrapper_return = 0;
    local $system_wrapper_called = 0;
    local $read_config_die = 0;
    local $ld_setup_die = 1;
    local @ld_cmd_children_args  = ();
    reread_config();
    is $system_wrapper_called, 2, 'reread_config - all old config reload(2)';
    is_deeply \@ld_cmd_children_args, [ ['stop', {} ], ['start', {} ] ], 'reread_config - no stop / start command(2)';
}
#   - check_cfgfile
{
    local $main::CONFIG_FILE{path} = undef;
    my $got = check_cfgfile();
    is $got, 0, 'check_cfgfile - config file path is undef';
}
{
    default_value();
    create_config('sample');
    my $got = check_cfgfile();
    is $got, 0, 'check_cfgfile - first config file check(1)';
    ok $main::CONFIG_FILE{stattime} > 0, 'check_cfgfile - register mtime(1)';
    ok length $main::CONFIG_FILE{checksum} > 0, 'check_cfgfile - register checksum(1)';
    remove_config();
}
{
    default_value();
    create_config('sample');
    my $got = check_cfgfile();
    is $got, 0, 'check_cfgfile - first config file check(2)';
    ok $main::CONFIG_FILE{stattime} > 0, 'check_cfgfile - register mtime(2)';
    ok length $main::CONFIG_FILE{checksum} > 0, 'check_cfgfile - register checksum(2)';
    my $before_mtime = $main::CONFIG_FILE{stattime};
    my $before_checksum = $main::CONFIG_FILE{checksum};
    sleep 1;
    open my $f, ">", $main::CONFIG_FILE{path};
    print $f 'sample';
    close $f;
    $got = check_cfgfile();
    is $got, 0, 'check_cfgfile - mtime change but same';
    ok $main::CONFIG_FILE{stattime} > $before_mtime, 'check_cfgfile - mtime change';
    is $main::CONFIG_FILE{checksum}, $before_checksum, 'check_cfgfile - same checksum';
    remove_config();
}
{
    default_value();
    create_config('sample');
    my $got = check_cfgfile();
    is $got, 0, 'check_cfgfile - first config file check(3)';
    ok $main::CONFIG_FILE{stattime} > 0, 'check_cfgfile - register mtime(3)';
    ok length $main::CONFIG_FILE{checksum} > 0, 'check_cfgfile - register checksum(3)';
    my $before_mtime = $main::CONFIG_FILE{stattime};
    my $before_checksum = $main::CONFIG_FILE{checksum};
    sleep 1;
    open my $f, ">", $main::CONFIG_FILE{path};
    print $f 'sample2';
    close $f;
    $got = check_cfgfile();
    is $got, 0, 'check_cfgfile - config change, no autoreload, no callback';
    ok $main::CONFIG_FILE{stattime} > $before_mtime, 'check_cfgfile - mtime change';
    ok $main::CONFIG_FILE{checksum} ne $before_checksum, 'check_cfgfile - checksum change';
    remove_config();
}
{
    default_value();
    create_config('sample');
    $main::CONFIG{autoreload} = 1;
    my $got = check_cfgfile();
    is $got, 0, 'check_cfgfile - first config file check(4)';
    ok $main::CONFIG_FILE{stattime} > 0, 'check_cfgfile - register mtime(4)';
    ok length $main::CONFIG_FILE{checksum} > 0, 'check_cfgfile - register checksum(4)';
    my $before_mtime = $main::CONFIG_FILE{stattime};
    my $before_checksum = $main::CONFIG_FILE{checksum};
    sleep 1;
    open my $f, ">", $main::CONFIG_FILE{path};
    print $f 'test';
    close $f;
    $got = check_cfgfile();
    is $got, 1, 'check_cfgfile - config change, autoreload, no callback';
    ok $main::CONFIG_FILE{stattime} > $before_mtime, 'check_cfgfile - mtime change';
    ok $main::CONFIG_FILE{checksum} ne $before_checksum, 'check_cfgfile - checksum change';
    remove_config();
}
{
    default_value();
    create_config('sample');
    $main::CONFIG{autoreload} = 1;
    $main::CONFIG{callback} = '/bin/sh';
    my $got = check_cfgfile();
    is $got, 0, 'check_cfgfile - first config file check(4)';
    ok $main::CONFIG_FILE{stattime} > 0, 'check_cfgfile - register mtime(4)';
    ok length $main::CONFIG_FILE{checksum} > 0, 'check_cfgfile - register checksum(4)';
    my $before_mtime = $main::CONFIG_FILE{stattime};
    my $before_checksum = $main::CONFIG_FILE{checksum};
    sleep 1;
    open my $f, ">", $main::CONFIG_FILE{path};
    print $f 'callback';
    close $f;
    local $system_wrapper_called = 0;
    $got = check_cfgfile();
    is $got, 1, 'check_cfgfile - config change, autoreload, callback';
    ok $main::CONFIG_FILE{stattime} > $before_mtime, 'check_cfgfile - mtime change';
    ok $main::CONFIG_FILE{checksum} ne $before_checksum, 'check_cfgfile - checksum change';
    is $system_wrapper_called, 1, 'check_cfgfile - callback called';
    remove_config();
}
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
    *config_error         = \&__config_error;
    *ld_gethostservbyname = \&__ld_gethostservbyname;
    *parse_fallback       = \&__parse_fallback;
    *parse_real           = \&__parse_real;
    *ld_openlog           = \&__ld_openlog;
    *_ld_service_resolve  = \&__ld_service_resolve;
    *ld_setup             = \&__ld_setup;
    *ld_cmd_children      = \&__ld_cmd_children;
    *system_wrapper       = \&__system_wrapper;
    *ld_log               = \&__ld_log;
    *ld_start             = \&__ld_start;
    *check_require_module = \&__check_require_module;
}
sub __config_error {
    @config_error_args = @_;
    die "config_error\n";
}
sub __ld_gethostservbyname {
    push @ld_gethostservbyname_args, @_;
    if ($ld_gethostservbyname_undef != 0) {
        $ld_gethostservbyname_undef--;
        if ($ld_gethostservbyname_undef == 0) {
            return undef;
        }
    }
    return $ld_gethostservbyname_return;
}
sub __parse_fallback {
    push @parse_fallback_args, @_;
    return $parse_fallback_return;
}
sub __parse_real {
    push @parse_real_args, @_;
    return $parse_real_return;
}
sub __ld_openlog {
    $ld_openlog_arg = shift;
    return $ld_openlog_return;
}
sub __get_hostservbyname {
    @get_hostservbyname_args = @_;
    return $get_hostservbyname_return;
}
sub __ld_service_resolve {
    $_ld_service_resolve_called++;
}
sub __ld_cmd_children {
    push @ld_cmd_children_args, [@_];
}
sub __system_wrapper {
    $system_wrapper_called++;
    return $system_wrapper_return;
}
sub __ld_setup {
    die if $ld_setup_die;
}
sub __check_require_module {
}
sub __ld_start {
}
sub __read_config {
    $main::CONFIG{virtual} = $read_config_virtual;
    $main::CONFIG{execute} = $read_config_execute;
    die if $read_config_die;
}
sub __ld_log {
}
