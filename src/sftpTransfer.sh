#!/bin/bash
sftp pi@10.0.0.57
lcd directoryOnRPi
cd directoryOnPC
put Pluto.bin.TOKEN_FULL
bye