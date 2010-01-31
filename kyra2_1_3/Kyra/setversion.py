# Python program to set the version.

##############################################


def fileProcess( name, lineFunction ):
	filestream = open( name, 'r' )
	if filestream.closed:
		print( "file " + name + " not open." )
		return

	output = ""
	print( "--- Processing " + name + " ---------" )
	while 1:
		line = filestream.readline()
		if not line: break
		output += lineFunction( line )
	filestream.close()
	
	if not output: return			# basic error checking
	
	print( "Writing file " + name )
	filestream = open( name, "w" );
	filestream.write( output );
	filestream.close()
	
	
def echoInput( line ):
	return line
	

import sys
major = input( "Major: " )
minor = input( "Minor: " )
build = input( "Build: " )

print "Setting filestream.h, buildwin, dox, buildlinux, engine makefile"
print "Version: " + `major` + "." + `minor` + "." + `build`

#### Write the engine.h ####

def engineRule( line ):

	matchMajor = "const int KyraVersionMajor"
	matchMinor = "const int KyraVersionMinor"
	matchBuild = "const int KyraVersionBuild"

	if line[0:len(matchMajor)] == matchMajor:
		print "1)engine.h Major found"
		return matchMajor + " = " + `major` + ";\n"

	elif line[0:len(matchMinor)] == matchMinor:
		print "2)engine.h Minor found"
		return matchMinor + " = " + `minor` + ";\n"

	elif line[0:len(matchBuild)] == matchBuild:
		print "3)engine.h Build found"
		return matchBuild + " = " + `build` + ";\n"

	else:
		return line;

fileProcess( "./engine/engine.h", engineRule )


#### Write the buildwin ####

def buildWinRule( line ):

	match = "kyra_bin_win_"

	if line.find( match ) > 0:
		print "zip file found match=" + match
		split = line.find( match )
		return line[0:split] + match + `major` + "_" + `minor` + "_" + `build` + ".zip \Releases\KyraBinWin\n"

	else:
		return line;


fileProcess( "buildwin.bat", buildWinRule )

#### Write the dox ####

def doxRule( line ):

	match = "PROJECT_NUMBER"

	if line[0:len( match )] == match:
		print "dox project found"
		return "PROJECT_NUMBER = " + `major` + "." + `minor` + "." + `build` + "\n"

	else:
		return line;

fileProcess( "dox", doxRule )


#### Write the buildlinux #####

def buildlinuxRule( line ):

	i = line.rfind( "_" )

	if i >= 4 and line[i] == "_" and line[i-2] == "_" and line[i-4] == "_":
		# This is ghetto. Should really use regular expressions.
		i -= 4
		print "buildlinux instance found"
		return line[0:i] + "_" + `major` + "_" + `minor` + "_" + `build` + line[i+6:]
	else:
		return line

fileProcess( "buildlinux", buildlinuxRule )


#### Write the engine Makefile #####

def MakefileRule( line ):

	i = line.find( "libkyra_" )

	if i > 0:
		print "Makefile instance found"
		#print( "beginning: " + line[0:i] )
		#print( "end: " + line[i+8+5:] )
		return ( line[0:i] + "libkyra_" + `major` + "_" + `minor` + "_" + `build` + line[i+8+5:] )
		#return line
	else:
		return line

fileProcess( "./engine/MakefileKyra", MakefileRule )


#### Fix up the configure.in file.

def configureinRule( line ):
	i = line.find( "Kyra, " )

	a = line.find( "KYRA_MAJOR_VERSION=" )
	b = line.find( "KYRA_MINOR_VERSION=" )
	c = line.find( "KYRA_BUILD_VERSION=" )

	if ( i > 0 ):
		print "Configure.in version found: 'Kyra, '"
		return ( line[0:i+6] + `major` + "." + `minor` + "." + `build` + line[i+6+5:] )
	elif ( a >= 0 ):
		print "Configure.in version found: MAJOR"
		return ( "KYRA_MAJOR_VERSION=" + `major` + '\n' )
	elif ( b >= 0 ):
		print "Configure.in version found: MINOR"
		return ( "KYRA_MINOR_VERSION=" + `minor` + '\n' )
	elif ( c >= 0 ):
		print "Configure.in version found: BUILD"
		return ( "KYRA_BUILD_VERSION=" + `build` + '\n' )
	else:
		return line

fileProcess( "configure.in", configureinRule )


#### Fix up the "tests" configure.in file.

def testConfigureinRule( line ):
	key = "Kyra-demo, "
	i = line.find( key )

	a = line.find( "KYRA_VERSION=" )

	if ( i > 0 ):
		print "Configure.in version found: 'Kyra-demo, '"
		return ( line[0:i+len( key )] + `major` + "." + `minor` + "." + `build` + line[i+len(key)+5:] )
	elif ( a >= 0 ):
		print "Configure.in version found: KYRA_VERSION"
		return ( "KYRA_VERSION=" + `major` + "." + `minor` + "." + `build` + "\n" )
	else:
		return line

fileProcess( "./tests/configure.in", testConfigureinRule )
