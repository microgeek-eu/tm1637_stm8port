@echo off

set CC_FLAGS=-mstm8 --std-c11 --opt-code-size --vc -DSTM8S103 -DUSE_STDPERIPH_DRIVER


echo.
echo Compiling modules...
sdcc -c %CC_FLAGS% hw_delay.c
sdcc -c %CC_FLAGS% TM1637.c


echo.
echo Compiling main file...
sdcc %CC_FLAGS% --out-fmt-ihx stm8s103.lib hw_delay.rel TM1637.rel main.c


echo.
echo Create Intel HEX output...
packihx main.ihx >main.hex
del *.ihx


echo.
echo Remove compiler artifact...
del *.asm
del *.lst
del *.rel
del *.rst
del *.sym
del *.cdb
del *.lk
del *.map

echo.
echo All done!
timeout /T 10