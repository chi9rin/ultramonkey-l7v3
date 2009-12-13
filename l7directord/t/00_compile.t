use strict;
use warnings;
use lib qw(t/lib lib);
use L7lib;
use Test::More tests => 1;
use Socket;
use Socket6;

L7lib::chdir();
L7lib::comment_out();

SKIP: {
    skip 'Test::More version 0.59_01 required', 1 if $Test::More::VERSION < 0.59_01;
    require_ok('./l7directord');
}

L7lib::comment_in();
