#!/usr/bin/perl
use strict;
use warnings;

my $timestamp = scalar localtime();
my $ip = $ENV{REMOTE_ADDR} // 'unknown';

print "Content-Type: text/html\r\n\r\n";
print "<!DOCTYPE html>";
print "<html><head><title>Hello Aryan</title></head><body>";
print "<h1 align=center>Hello Aryan</h1>";
print "<hr/>";
print "<p>Hello Aryan</p>";
print "<p>This page was generated with the Perl programming langauge by Aryan Palaskar for CSE 135.</p>";
print "<p>This program was generated at: $timestamp</p>";
print "<p>Your current IP Address is: $ip</p>";
print "</body></html>";
