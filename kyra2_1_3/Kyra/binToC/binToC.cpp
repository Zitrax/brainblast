#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

int main( int argc, const char* argv[] )
{
	if ( argc < 3 )
	{
		printf( "Usage: binToC filename.bin filename2.bin ... outfile [no extension]\n" );
		return 1;
	}

//	FILE* input = fopen( argv[1], "rb" );
//
	string cname = argv[argc-1];
	cname += ".cpp";
	string hname = argv[argc-1];
	hname += ".h";

	FILE* outputC = fopen( cname.c_str(), "w" );
	FILE* outputH = fopen( hname.c_str(), "w" );

	int inputIndex = 0;

	fprintf( outputH, "#ifndef %s_INCLUDED\n", argv[argc-1] );
	fprintf( outputH, "#define %s_INCLUDED\n", argv[argc-1] );

	fprintf( outputC, "#include \"%s.h\"\n\n", argv[argc-1] );

	while ( outputC && outputH  )
	{
		++inputIndex;
		if ( inputIndex == argc-1 )
			break;

		FILE* input = fopen( argv[inputIndex], "rb" );
		
		char iname[ 256 ];
		strcpy( iname, argv[inputIndex] );
		char* p = strchr( iname, '.' );
		*p = 0;
		
		fseek( input, 0, SEEK_END );
		int size = ftell( input );
		fseek( input, 0, SEEK_SET );

		fprintf( outputH, "const int %s_SIZE = %d;\n", iname, size );
		fprintf( outputH, "extern unsigned char %s_DAT[%d];\n", iname, size );

		fprintf( outputC, "unsigned char %s_DAT[%d] = \n{\n", iname, size );

		int count = 0;
		unsigned char byte;
		while ( fread( &byte, 1, 1, input ) )
		{
			if ( count == 0 )
				fprintf( outputC, "\t" );

			fprintf( outputC, "0x%02x, ", byte );

			++count;
			if ( count == 16 )
			{
				fprintf( outputC, "\n" );
				count = 0;
			}
		}

		fprintf( outputC, "\n};\n\n" );
	}
	fprintf( outputH, "#endif\n\n" );
	return 0;
}