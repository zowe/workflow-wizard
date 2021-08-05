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
ssh $USER@$HOST "rm -rf $BLDPATH/* $BLDPATH/.pax"

# Transfer files to host
scp -r ../c ../h ../build ../doc ../version.txt ../.pax $USER@$HOST:$BLDPATH

# Prepare workspace
ssh $USER@$HOST "cd $BLDPATH; .pax/prepare-workspace.sh;"

# Copy ASCII files...(already copied as ASCII from scp)
ssh $USER@$HOST "cd $BLDPATH/.pax/content; mv ../ascii/* .;"

# Build
ssh $USER@$HOST "cd $BLDPATH/.pax; ./pre-packaging.sh;"

# Actually perform build
#ssh $USER@$HOST "cd $BLDPATH/build; ./build.sh;"

#Pull listings locally
#scp -C $USER@$HOST:$BLDPATH/build/lists/*lst .


