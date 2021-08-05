#
LOADLIB=bin/wfwizbld

mkdir lists
mkdir bin

rm lists/*
rm bin/*

xlc++ -o $LOADLIB \
  -qrent -D_ISOC99_SOURCE -D_VARARG_EXT_ \
  "-Wc,LIST(./lists)" -qasm -qsuppress=CCN6608 \
  -W c,list -qcompress -qASMLIB=SYS1.MACLIB  \
  -I ../h ../c/*cpp

