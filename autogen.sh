#! /bin/sh
if [ -d .git ]; then
    git submodule init
    git submodule update
fi
if [ -f buildconf/autogen.sh ]
then
./buildconf/autogen.sh
else
aclocal \
&& autoheader \
&& automake  --add-missing \
&& autoconf
find . -name autogen.sh -execdir {} \;
fi
