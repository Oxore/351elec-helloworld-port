#!/bin/sh

. /etc/profile

init_port main alsa

/storage/roms/ports/demo/main

ret_code=$?

end_port

exit ${ret_code}
