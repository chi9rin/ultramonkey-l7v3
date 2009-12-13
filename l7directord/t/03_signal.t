use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use L7lib;
use Test::More tests => 26;
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';

override();

#...............................................
# test start
#   - set_ld_handler
set_ld_handler();
{
    default_value();
    # PIPE (ignore)
    kill 13, $$;
    is $main::PROC_STAT{halt  }, undef,  'signal PIPE, not halt';
    is $main::PROC_STAT{reload}, undef,  'signal PIPE, not reload';
}
#   - ld_handler_term
{
    default_value();
    # INT
    kill 2, $$;
    is $main::PROC_STAT{halt  }, 'INT',  'signal INT, halt';
    is $main::PROC_STAT{reload}, undef,  'signal INT, not reload';
}
{
    default_value();
    # QUIT
    kill 3, $$;
    is $main::PROC_STAT{halt  }, 'QUIT',  'signal QUIT, halt';
    is $main::PROC_STAT{reload},  undef,  'signal QUIT, not reload';
}
{
    default_value();
    # ILL
    kill 4, $$;
    is $main::PROC_STAT{halt  }, 'ILL',  'signal ILL, halt';
    is $main::PROC_STAT{reload}, undef,  'signal ILL, not reload';
}
{
    default_value();
    # ABRT
    kill 6, $$;
    is $main::PROC_STAT{halt  }, 'ABRT',  'signal ABRT, halt';
    is $main::PROC_STAT{reload},  undef,  'signal ABRT, not reload';
}
{
    default_value();
    # FPE
    kill 8, $$;
    is $main::PROC_STAT{halt  }, 'FPE',  'signal FPE, halt';
    is $main::PROC_STAT{reload}, undef,  'signal FPE, not reload';
}
{
    default_value();
    # SEGV
    kill 11, $$;
    is $main::PROC_STAT{halt  }, 'SEGV',  'signal SEGV, halt';
    is $main::PROC_STAT{reload},  undef,  'signal SEGV, not reload';
}
{
    default_value();
    # TERM
    kill 15, $$;
    is $main::PROC_STAT{halt  }, 'TERM',  'signal TERM, halt';
    is $main::PROC_STAT{reload},  undef,  'signal TERM, not reload';
}
{
    default_value();
    # BUS
    kill 7, $$;
    is $main::PROC_STAT{halt  }, 'BUS',  'signal BUS, halt';
    is $main::PROC_STAT{reload}, undef,  'signal BUS, not reload';
}
{
    default_value();
    # SYS
    kill 31, $$;
    is $main::PROC_STAT{halt  }, 'SYS',  'signal SYS, halt';
    is $main::PROC_STAT{reload}, undef,  'signal SYS, not reload';
}
{
    default_value();
    # XCPU
    kill 24, $$;
    is $main::PROC_STAT{halt  }, 'XCPU',  'signal XCPU, halt';
    is $main::PROC_STAT{reload},  undef,  'signal XCPU, not reload';
}
{
    default_value();
    # XFSZ
    kill 25, $$;
    is $main::PROC_STAT{halt  }, 'XFSZ',  'signal XFSZ, halt';
    is $main::PROC_STAT{reload},  undef,  'signal XFSZ, not reload';
}
#   - ld_handler_hup
{
    default_value();
    # HUP
    kill 1, $$;
    is $main::PROC_STAT{halt  }, undef,  'signal HUP, not halt';
    is $main::PROC_STAT{reload}, 'HUP',  'signal HUP, reload';
}
# test end
#...............................................

L7lib::comment_in();

sub default_value {
    undef $main::PROC_STAT{halt  };
    undef $main::PROC_STAT{reload};
}
sub override {
    *ld_log = \&__ld_log;
}
sub __ld_log {
}
