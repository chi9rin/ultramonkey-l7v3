use strict;
use warnings;
no warnings qw(redefine once);
use lib qw(t/lib lib);
use subs qw(print);
use Cwd;
use L7lib;
use Test::More tests => 31;

L7lib::chdir();
L7lib::comment_out();
require './l7directord';
override();

our @ld_read_l7vsadm_args = ();
our @ld_read_l7vsadm_returns = ();
our @get_virtual_id_str_args = ();
our @get_virtual_id_str_returns = ();
our @ld_add_virtual_args = ();
our @ld_add_virtual_returns = ();
our @ld_edit_virtual_args = ();
our @ld_edit_virtual_returns = ();
our @fallback_find_args = ();
our @fallback_find_returns = ();
our @get_ip_port_args = ();
our @get_ip_port_returns = ();
our @get_health_check_id_str_args = ();
our @get_health_check_id_str_returns = ();
our @ld_delete_virtual_args = ();
our @ld_delete_virtual_returns = ();
our @ld_delete_real_args = ();
our @ld_delete_real_returns = ();
#...............................................
# test start
#   - ld_start
{
    set_default();
    local @ld_read_l7vsadm_returns = ();
    ld_start();
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'ld_start - l7vsadm error';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( {} );
    $main::CONFIG{virtual} = [ { protocol => 'tcp' } ];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ { protocol => 'tcp' } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - l7vsadm has no service';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( {} );
    local @get_virtual_id_str_returns = ();
    $main::CONFIG{virtual} = [ { protocol => 'tcp' } ];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ { protocol => 'tcp' } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - get_virtual_id_error';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( { vid => undef } );
    local @get_virtual_id_str_returns = ('vid');
    $main::CONFIG{virtual} = [ { protocol => 'tcp' } ];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ { protocol => 'tcp' } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - edit virtual';
    is_deeply \@ld_edit_virtual_args, [ { protocol => 'tcp' } ], 'ld_start - edit virtual call';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( {} );
    local @get_virtual_id_str_returns = ('vid');
    $main::CONFIG{virtual} = [ { protocol => 'tcp' } ];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ { protocol => 'tcp' } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - add virtual';
    is_deeply \@ld_add_virtual_args, [ { protocol => 'tcp' } ], 'ld_start - add virtual call';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( { vid => { 'fbip:fbport' => undef } } );
    local @get_virtual_id_str_returns = ('vid');
    local @fallback_find_returns = ( {} );
    local @get_ip_port_returns = ( 'fbip:fbport' );
    $main::CONFIG{virtual} = [ { protocol => 'tcp' } ];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ { protocol => 'tcp' } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - fallback find';
    is_deeply \@ld_delete_real_args, [], 'ld_start - not delete fallback';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( { vid => { 'fbip:fbport' => undef, 'foo:bar' => { ip => 'foo' } } } );
    local @get_virtual_id_str_returns = ('vid');
    local @fallback_find_returns = ( {} );
    local @get_ip_port_returns = ( 'fbip:fbport' );
    $main::CONFIG{virtual} = [ { protocol => 'tcp' } ];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ { protocol => 'tcp' } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - delete not management real';
    is_deeply \@ld_delete_real_args, [ { protocol => 'tcp' }, { ip => 'foo' } ], 'ld_start - delete real';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( { vid => { 'foo:bar' => { ip => 'foo' } } } );
    local @get_virtual_id_str_returns = ('vid');
    local @get_health_check_id_str_returns = ();
    local @fallback_find_returns = ();
    local @get_ip_port_returns = ( 'foo:bar' );
    $main::CONFIG{virtual} = [ {
                protocol => 'tcp',
                real => [ 'real' ],
                             } ];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ {
                protocol => 'tcp',
                real => [ 'real' ],
                             } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - health_check_id_str error';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( { vid => { 'foo:bar' => { ip => 'foo' } } } );
    local @get_virtual_id_str_returns = ('vid');
    local @get_health_check_id_str_returns = ('hid');
    local @fallback_find_returns = ();
    local @get_ip_port_returns = ( 'foo:bar' );
    local %main::HEALTH_CHECK = ();
    $main::CONFIG{virtual} = [ {
                protocol => 'tcp',
                real => [ 'real' ],
                             } ];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ {
                protocol => 'tcp',
                real => [ 'real' ],
                             } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - new health check';
    is_deeply \%main::HEALTH_CHECK, { hid => { manage => [
                [ $expected{virtual}[0], $expected{virtual}[0]{real}[0] ]
                                                         ] } }, 'ld_start - new health check id';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( { vid => { 'foo:bar' => { ip => 'foo' } } } );
    local @get_virtual_id_str_returns = ('vid');
    local @get_health_check_id_str_returns = ('hid');
    local @fallback_find_returns = ();
    local @get_ip_port_returns = ( 'foo:bar' );
    local %main::HEALTH_CHECK = ( hid => undef );
    $main::CONFIG{virtual} = [ {
                protocol => 'tcp',
                real => [ 'real' ],
                             } ];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ {
                protocol => 'tcp',
                real => [ 'real' ],
                             } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - add health check';
    is_deeply \%main::HEALTH_CHECK, { hid => { manage => [
                [ $expected{virtual}[0], $expected{virtual}[0]{real}[0] ]
                                                         ] } }, 'ld_start - add health check id';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( { vid => { 'foo:bar' => { ip => 'foo' } } } );
    local @get_virtual_id_str_returns = ('vid');
    local @get_health_check_id_str_returns = ('hid');
    local @fallback_find_returns = ();
    local @get_ip_port_returns = ( 'foo:bar' );
    $main::CONFIG{virtual} = [ {
                protocol => 'tcp',
                real => [ 'real' ],
                             } ];
    local %main::HEALTH_CHECK = ( hid => { manage => [ [ $main::CONFIG{virtual}[0], $main::CONFIG{virtual}[0]{real}[0] ] ] } );
    my %expected = %main::GLOBAL;
    $expected{virtual} = [ {
                protocol => 'tcp',
                real => [ 'real' ],
                             } ];
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - ignore exists health check';
    is_deeply \%main::HEALTH_CHECK, { hid => { manage => [
                [ $expected{virtual}[0], $expected{virtual}[0]{real}[0] ]
                                                         ] } }, 'ld_start - ignore exists health check id';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( {} );
    $main::CONFIG{old_virtual} = [];
    my %expected = %main::GLOBAL;
    delete $expected{old_virtual};
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - empty old virtual';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( {} );
    local @get_virtual_id_str_returns = ( 'vid' );
    $main::CONFIG{old_virtual} = ['ov'];
    my %expected = %main::GLOBAL;
    delete $expected{old_virtual};
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - not exists old virtual';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ( { vid => { ip => 'foo' } } );
    local @get_virtual_id_str_returns = ( 'vid' );
    $main::CONFIG{old_virtual} = ['ov'];
    my %expected = %main::GLOBAL;
    delete $expected{old_virtual};
    ld_start();
    is_deeply \%main::CONFIG, \%expected, 'ld_start - exists old virtual';
    is_deeply \@ld_delete_virtual_args, ['ov'], 'ld_start - exists old virtual delete';
}
#   - ld_stop
{
    set_default();
    local @ld_read_l7vsadm_returns = ();
    ld_stop();
    is_deeply \%main::CONFIG, \%main::GLOBAL, 'ld_stop - l7vsadm no service';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ({});
    $main::CONFIG{virtual} = [];
    my %expected = %main::GLOBAL;
    $expected{virtual} = [];
    ld_stop();
    is_deeply \%main::CONFIG, \%expected, 'ld_stop - empty virtual';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ({});
    local @get_virtual_id_str_args = ();
    local @get_virtual_id_str_returns = ('vid');
    local @ld_delete_virtual_args = ();
    $main::CONFIG{virtual} = ['virtual'];
    my %expected = %main::GLOBAL;
    $expected{virtual} = ['virtual'];
    ld_stop();
    is_deeply \%main::CONFIG, \%expected, 'ld_stop - no real';
    is_deeply \@get_virtual_id_str_args, ['virtual'], 'ld_stop - get_virtual_id_str';
    is_deeply \@ld_delete_virtual_args, ['virtual'], 'ld_stop - delete virtual';
}
{
    set_default();
    local @ld_read_l7vsadm_returns = ({ vid => { rid1 => 'real1', rid2 => 'real2' } });
    local @get_virtual_id_str_args = ();
    local @get_virtual_id_str_returns = ('vid');
    local @ld_delete_virtual_args = ();
    local @ld_delete_real_args = ();
    $main::CONFIG{virtual} = ['virtual'];
    my %expected = %main::GLOBAL;
    $expected{virtual} = ['virtual'];
    ld_stop();
    is_deeply \%main::CONFIG, \%expected, 'ld_stop - exists real';
    is_deeply \@get_virtual_id_str_args, ['virtual'], 'ld_stop - get_virtual_id_str';
    is_deeply \@ld_delete_real_args, ['virtual', 'real2', 'virtual', 'real1'], 'ld_stop - delete real';
    is_deeply \@ld_delete_virtual_args, ['virtual'], 'ld_stop - delete virtual';
}
# test end
#...............................................

L7lib::comment_in();

sub set_default {
    %main::CONFIG = %main::GLOBAL;
    %main::HEALTH_CHECK = ();
}
sub override {
    *ld_read_l7vsadm = \&__ld_read_l7vsadm;
    *get_virtual_id_str = \&__get_virtual_id_str;
    *ld_add_virtual = \&__ld_add_virtual;
    *ld_edit_virtual = \&__ld_edit_virtual;
    *fallback_find = \&__fallback_find;
    *get_ip_port = \&__get_ip_port;
    *get_health_check_id_str = \&__get_health_check_id_str;
    *ld_delete_virtual = \&__ld_delete_virtual;
    *ld_delete_real = \&__ld_delete_real;
    *ld_log = \&__ld_log;
}
sub __ld_read_l7vsadm {
    push @ld_read_l7vsadm_args, @_;
    return shift @ld_read_l7vsadm_returns;
}
sub __get_virtual_id_str {
    push @get_virtual_id_str_args, @_;
    return shift @get_virtual_id_str_returns;
}
sub __ld_add_virtual {
    push @ld_add_virtual_args, @_;
    return shift @ld_add_virtual_returns;
}
sub __ld_edit_virtual {
    push @ld_edit_virtual_args, @_;
    return shift @ld_edit_virtual_returns;
}
sub __fallback_find {
    push @fallback_find_args, @_;
    return shift @fallback_find_returns;
}
sub __get_ip_port {
    push @get_ip_port_args, @_;
    return shift @get_ip_port_returns;
}
sub __get_health_check_id_str {
    push @get_health_check_id_str_args, @_;
    return shift @get_health_check_id_str_returns;
}
sub __ld_delete_virtual {
    push @ld_delete_virtual_args, @_;
    return shift @ld_delete_virtual_returns;
}
sub __ld_delete_real {
    push @ld_delete_real_args, @_;
    return shift @ld_delete_real_returns;
}
sub __ld_log {
}
