#!/usr/bin/env python3

import os
import sys
import time
import datetime
from pathlib import Path
import shutil

PLUTO_BIN = 'Pluto.bin.TOKEN_FULL'
PLUTO_PATH_REMOTE = '/home/pi/Desktop/'
PLUTO_PATH_LOCAL = '/home/pi/token/'

CHECK_NETWORK_EVERY_X_SECONDS = 1

class FileWatcher(object):
    running = True
    refresh_delay_secs = CHECK_NETWORK_EVERY_X_SECONDS

    def __init__(self, watch_file, call_func_on_change=None, *args, **kwargs):
        self._cached_stamp = 0
        self.filename = watch_file
        self.call_func_on_change = call_func_on_change
        self.args = args
        self.kwargs = kwargs

    # Look for changes
    def look(self):
        stamp = os.stat(self.filename).st_mtime
        if stamp != self._cached_stamp:
            self._cached_stamp = stamp
            # File has changed.
            if self.call_func_on_change is not None:
                self.call_func_on_change(*self.args, **self.kwargs)

    # Keep watching in a loop
    def watch(self):
        while self.running: 
            try: 
                # Look for changes
                time.sleep(self.refresh_delay_secs) 
                self.look()
            except KeyboardInterrupt: 
                print('\nKeyboard interrupt detected.')
                break 
            except FileNotFoundError:
                # Action on file not found
                pass
            except: 
                print('Unhandled error: %s' % sys.exc_info()[0])

# Copy the file from the network to the local computer.
def copyFile(filename, src, dst):
    try:
        now = datetime.datetime.now()
        print(now, "Copying file", src+filename, "to", dst+filename)
        shutil.copy2(src+filename, dst)
    except:
        print("Failed to copy file")


def main():
    
    time.sleep(5)
    now = datetime.datetime.now()

    try:
        print('Trying to connect to the network...')
    except:
        print('Failed to connect to the network. :(')

    try:
        print(now, " Running file watcher")
        fileWatcher = FileWatcher(PLUTO_PATH_REMOTE + PLUTO_BIN, copyFile, filename=PLUTO_BIN, src=PLUTO_PATH_REMOTE, dst=PLUTO_PATH_LOCAL)
        fileWatcher.watch()
    except:
        print("Failed file watch; exception thrown.")
    
    sys.exit(0)

if __name__=="__main__":
    main()

