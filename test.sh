#!/bin/bash

# File che si occupa di far andare tutti i test
mkdir -p tmp/test;
for folder in test/test*; do
	bin/bindiff $folder/before $folder/after tmp/$folder;
	if [ "0" != "$?" ]; then
		exit 1;
	fi
	bin/binpatch $folder/before tmp/$folder tmp/$folder.new;
	if [ "0" != "$?" ]; then
		exit 1;
	fi

	if [ "" == "$(cmp -l $folder/after tmp/$folder.new)" ]; then
		echo "TEST PASSED";
	else
		echo "TEST FAILED";
		exit 1;
	fi
done
rm -rf tmp;
