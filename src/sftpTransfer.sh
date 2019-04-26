#!/usr/bin/expect -f
# sshpass -p 'raspberry' ssh -o StrictHostKeyChecking=no pi@10.0.0.57

spawn sftp pi@10.0.0.57
expect "password:"
send "raspberry\n"
expect "sftp>"
send "cd /home/pi/Documents/CODE/spiToken/src\n"
expect "sftp>"
send "put /home/ksolomon/git/spiToken/src/Pluto_FULL_TOKEN.bin\n"
expect "sftp>"
send "exit\n"
interact