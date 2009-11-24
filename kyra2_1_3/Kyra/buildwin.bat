:Builds the windows binary

del \Releases\KyraBinWin\*.*
del \Releases\kyra_bin_win_2_1_3.zip \Releases\KyraBinWin
mkdir \Releases
mkdir \Releases\KyraBinWin

copy license.txt \Releases\KyraBinWin

:Copy in the executables, by hand.
copy .\tests\Release\krdemos.exe \Releases\KyraBinWin
copy .\tests\bem.dat \Releases\KyraBinWin
copy .\tests\font.dat \Releases\KyraBinWin
copy .\tests\space.dat \Releases\KyraBinWin
copy .\tests\standardtest.dat \Releases\KyraBinWin
copy .\tests\title.dat \Releases\KyraBinWin
copy readme.txt \Releases\KyraBinWin

:Zip it all up. WARNING: this line is changed by setversion.py.
:If it is changed, setversion should be updated
cd \Releases
zip -q -r -9 kyra_bin_win_2_1_3.zip \Releases\KyraBinWin
copy kyra_bin_win_2_1_3.zip \Releases\KyraBinWin
