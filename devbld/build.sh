#

LOADLIB=CVI.WSRC0100.LOAD

ssh mvssrc1@db2a "rm -rf /home/mvssrc1/source/wf/*"

scp -r ../c ../h mvssrc1@db2a:/home/mvssrc1/source/wf

# -qipa=map 
ssh mvssrc1@db2a "cd /home/mvssrc1/source/wf; 
mkdir build;
cd build;
rm *.lst;
xlc++ -o \"//'$LOADLIB(WFWIZBLD)'\" -qrent -D_ISOC99_SOURCE -D_VARARG_EXT_ -Wc,\"LIST(./)\" -qasm -qsuppress=CCN6608 -W c,list -qcompress -qASMLIB=SYS1.MACLIB  -I ../h ../c/*cpp"

scp -C mvssrc1@db2a:/home/mvssrc1/source/wf/build/*lst .
