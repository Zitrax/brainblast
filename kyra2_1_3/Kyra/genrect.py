for minbit in range( 0, 8 ):
	
	line = []

	for maxbit in range( 0, 8 ):
		
		val = 0
		for i in range( minbit, maxbit+1 ):
			val += pow( 2, i )

		line.append( val )

	print '{ %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d },' % ( line[0], line[1], line[2], line[3], line[4], line[5], line[6], line[7] )

