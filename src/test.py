
import os, time
import subprocess
import shutil
import serial 

import openpyxl
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders
from shutil import copy

 
mail=smtplib.SMTP_SSL('smtp.gmail.com',465) #try 465 or 578
time.sleep(2)
mail.login('Stryker.endo.CI@gmail.com', 'Strykerendo9')
time.sleep(2)
text = "hello there.this is michael!how's it going?"
mail.sendmail("Stryker.endo.CI@gmail.com","michael.karachewski@stryker.com",text)
mail.close()

        
    

        
    