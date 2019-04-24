# Add the following to crontab:

```
@reboot python3 /home/pi/token/checkForImageUpdate.py > /home/pi/logs/checkForImageUpdate.log 2>&1
@reboot python3 /home/pi/token/tokenFlasher.py > /home/pi/logs/tokenFlasher.log 2>&1
```

