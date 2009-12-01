#!/usr/bin/perl 
package L7lib;
use strict;
use warnings;
use Cwd;

sub chdir {
    my $pwd = cwd;
    if ($pwd =~ m{/t$}) {
        chdir '..';
    }
    if ($pwd =~ m{/t/lib$}) {
        chdir '../..';
    }
}

sub comment_in {
    replace( qr{^#main}, q{main} );
}

sub comment_out {
    replace( qr{^main}, q{#main} );
}

sub replace {
    my ($before, $after) = @_;

    my $file = -f 'l7directord'    ? 'l7directord'
             : -f '../l7directord' ? '../l7directord'
             :                       die 'l7directord not found'
             ;
    local $/ = undef;
    open my $f, '+<', $file or die $!;
    flock $f, 2;
    my $org = <$f>;
    $org =~ s{$before}{$after}m;
    seek $f, 0, 0;
    print $f $org;
    truncate $f, tell $f;
    close $f;
}
1;
