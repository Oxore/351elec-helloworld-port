#!/bin/sh

. /etc/profile

init_port main alsa

cd /storage/roms/ports/demo/
./main >/tmp/logs/DemoApplication-$(date -Iseconds).log 2>&1
ret_code=$?

end_port

exit ${ret_code}
