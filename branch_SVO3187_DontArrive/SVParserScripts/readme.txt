Updated May 1, 2007 - JMS

The SVPARSERSCRIPTS project is now buildable by using the build option in visual studio.

The SVEquation.lex file is compiled using the following commands:

echo flex SVEquation.lex
flex SVEquation.lex
echo Using ssed to comment out invalid istream definition
ssed "s/class istream;/\/\/class istream;/" lex.yy.cc > tmp.cc
echo Updating lex.yy.cc
del lex.yy.cc
rename tmp.cc lex.yy.cc

After the build is complete, line 24 (class istream;) in the lex.yy.cc file is commented out using the ssed command.

This line conflicts with the standard include for <iostream>.

The SVEquation.y file is compiled using the following commands:

echo byacc -dv SVEquation.y
byacc -dv SVEquation.y

Note: a directory called tmp needs to exist off the root of the drive where byaccc is executed from. This is required by byacc.

All of the support classes for LEX and YACC have been moved from SVObserver project to the SVEquationLibrary project.
