#
USER=mvssrc1
HOST=db2a
BLDPATH=/home/$USER/source/wf

if [ "$BLDPATH" = "" ]
then
   echo "Halting the build to prevent removing everything in root..."
   exit 8
fi

# Clean up old files
ssh $USER@$HOST "rm -rf $BLDPATH/*"

# Transfer files to host
scp -r ../c ../h ../build $USER@$HOST:$BLDPATH

# Actually perform build
ssh $USER@$HOST "cd $BLDPATH/build; ./build.sh;"

#Pull listings locally
scp -C $USER@$HOST:$BLDPATH/build/lists/*lst .


