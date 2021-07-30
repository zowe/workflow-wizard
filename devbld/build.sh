#

LOADLIB=CVI.WSRC0100.LOAD
USER=mainframe_user
HOST=mainframe_host
BLDPATH=/home/$USER/source/wf



if [ "$BLDPATH" = "" ]
then
   echo "Halting the build to prevent removing everything in root..."
   exit 8
fi

# Clean up old files
#ssh $USER@$HOST "rm -rf $BLDPATH/*"

# Transfer files to host
scp -r ../c ../h $USER@$HOST:$BLDPATH

# Actually perform build
ssh $USER@$HOST "cd $BLDPATH; 
mkdir build;
cd build;
rm *.lst;
xlc++ -o \"//'$LOADLIB(WFWIZBLD)'\" -qrent -D_ISOC99_SOURCE -D_VARARG_EXT_ -Wc,\"LIST(./)\" -qasm -qsuppress=CCN6608 -W c,list -qcompress -qASMLIB=SYS1.MACLIB  -I ../h ../c/*cpp"

#Pull listings locally
scp -C $USER@$HOST:$BLDPATH/build/*lst .
