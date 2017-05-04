#!/bin/bash

#
# Run this script to clean the project
#

rm client/*.o 2> /dev/null
rm client/*.save 2> /dev/null
rm client/*.layout 2> /dev/null
rm client/*.xml 2> /dev/null
rm client/ticket-client 2> /dev/null

rm server/*.o 2> /dev/null
rm server/*.save 2> /dev/null
rm server/*.layout 2> /dev/null
rm server/*.xml 2> /dev/null
rm server/ticket-server 2> /dev/null
