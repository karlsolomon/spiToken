#!/usr/bin/env python3

import sys
import os
from pathlib import Path
import time
import re
import spidev
import RPi.GPIO as GPIO

SPI_BUS=0
SPI_DEVICE=0

PIN_CONFIG=GPIO.BOARD

BLUE_PIN=29
YELLOW_PIN=37
RED_PIN=35
GREEN_PIN=40

LOFO_PIN=12

PLUTO_BIN = 'Pluto_FULL_TOKEN.bin'
PLUTO_PATH = '/home/pi/token/'

def bytesToHex(Bytes):
    return ''.join(["0x%02X " % x for x in Bytes]).strip()

class Led():
    """LED class"""
    
    def __init__(self, pin):
        self.gpio = pin
        self.state = False
        GPIO.setup(pin, GPIO.OUT)
        self.off()

    def on(self):
        self.state = True
        GPIO.output(self.gpio, GPIO.HIGH)


    def off(self):
        self.state = False
        GPIO.output(self.gpio, GPIO.LOW)

    def isOn(self):
        return self.state

def isTokenInserted():
    if GPIO.input(LOFO_PIN) is 0:
        return True
    else:
        return False

def initAll():
    GPIO.setmode(PIN_CONFIG) 
    GPIO.setup(LOFO_PIN, GPIO.IN)

def tokenReadSR(spi):
    dataOut = [0x01]
    dataIn  = [0xFF, 0xFE]
    spi.writebytes(dataIn)
    spi.readbytes(len(dataIn))
    return dataIn

def programToken(fp):
    spi = spidev.SpiDev()
    spi.open(SPI_BUS, SPI_DEVICE)
    spi.max_speed_hz = 1000000
    spi.mode = 0b01

    # Erase token
    print('sending bytes')
    SR = tokenReadSR(spi)
    print("SR read")
    print("SR is: ", SR)

    # If file exists, open it.
    # Read in bytes, X at a time.
    # Program bytes to token.
    
    spi.close()
 

def main():

    initAll()

    blue = Led(BLUE_PIN)
    red = Led(RED_PIN)
    yellow = Led(YELLOW_PIN)
    green = Led(GREEN_PIN)

    try:
        while True:
        
            while isTokenInserted() is False:
                pass

            blue.on()
            time.sleep(0.2)

            if isTokenInserted() is True:
                yellow.on()
                time.sleep(1)
                path = PLUTO_PATH + PLUTO_BIN
                try: 
                    fp = open(path, "rb")
                    programToken(fp)
                    fp.close()
                    
                    green.on()     
                except:
                    print('File not found:', path)
                    red.on()
                
                yellow.off()

            while isTokenInserted() is True:
                pass

            red.off()
            yellow.off()
            green.off()
            blue.off()
            time.sleep(.5)

    except KeyboardInterrupt:
        print("Exiting...")
    
    red.off()
    green.off()
    blue.off()

    GPIO.cleanup()

    sys.exit(0)

if __name__=="__main__":
    main()
