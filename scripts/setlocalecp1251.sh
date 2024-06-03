#!/bin/bash

luit -encoding cp1251
ret=$?
if [ $ret -eq 0 ]; then echo "Done!"
else echo "Error"; fi

exit $ret
