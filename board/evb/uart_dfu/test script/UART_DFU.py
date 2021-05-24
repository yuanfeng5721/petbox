# -*- coding: utf-8 -*-
"""
Created on Fri Oct 26 10:54:53 2018

@author: lance_lan
"""
import serial
import serial.tools.list_ports
from time import sleep
import sys
class DFU():
    def __init__(self):
        
        self.boudrate = 9600
        self.com = 'com4'
        self.path = r'C:\Users\lance_lan\Desktop\app.bin'
        
        self.ic_type = 0x05
        self.payload_len=2000
        self.cmd_list = {'START_DFU':[0x03,0x01,0x12,0x0C,0x00],
                         'DFU_WRITE_IMAGE':[0x03,0x02,0x12,self.payload_len&0xff,self.payload_len>>8],
                         'DFU_VALID_IMAGE':[0x03,0x03,0x12,0x00,0x00],
                         'DFU_ACTIVE_RESET':[0x03,0x04,0x12,0x00,0x00],
                         'DFU_SYS_RESET':[0x03,0x05,0x12,0x00,0x00],
                         'READ_DEVICE_INFO':[0x03,0x06,0x12,0x02,0x00,0,0]
                         }
        self.response_list = {'START_DFU':[0x04,0x01,0x12,0x01,0x00,0x00],
                 'DFU_WRITE_IMAGE':[0x04,0x02,0x12,0x05,0x00],
                 'DFU_VALID_IMAGE':[0x04,0x03,0x12,0x01,0x00],
                 'DFU_ACTIVE_RESET':[0x04,0x04,0x12,0x01,0x00],
                 'DFU_SYS_RESET':[0x04,0x05,0x12,0x01,0x00],
                 'READ_DEVICE_INFO':[0x04,0x06,0x12,0x09,0x00,self.ic_type]
                 }
        self.ser = serial.Serial(self.com ,self.boudrate,timeout=3)
        if self.ser.isOpen():
            print("open success")
        else :
            print("open failed")
    def crc16(self,x):
        a = 0xFFFF
        b = 0xA001
        for byte in x:
            a ^= byte
            for i in range(8):
                last = a % 2
                a >>= 1
                if last == 1:
                    a ^= b
        return [a&0xff,a>>8]
    def check_READ_DEVICE_INFO(self,git_version):
        rlen = self.ser.inWaiting()
        data = self.ser.read(rlen)
        print(list(data),list(data[0:len(data)-2]))
        if list(data[0:6]) == self.response_list['READ_DEVICE_INFO'] and data[11] == 0 \
                        and self.crc_check(list(data[0:len(data)-2]),list(data[len(data)-2:len(data)])):
            self.payload_len = data[7]|data[8]<<8
            if git_version[0]|git_version[1]<<8 <= data[9]|data[10]<<8:
                print('git_version :',hex(git_version[0]|git_version[1]<<8))
#                sys.exit(1)
        else:
            return False
        return True
    def crc_check(self,payload,crc_value):
        if self.crc16(payload) == crc_value:
            return True
        else:
            return False
    def check_DFU_WRITE_IMAGE(self,pay_load,length):
        sleep(2)
        rlen = self.ser.inWaiting()
        data = self.ser.read(rlen)
        print(list(data))
        if list(data[0:5]) != self.response_list['DFU_WRITE_IMAGE'] and \
                        self.crc_check(list(data[0:len(data)-2]),list(data[len(data)-2:len(data)])):
            print('DFU_WRITE_IMAGE fail')
            sys.exit(1)
        if data[9] == 0:
            return True
        else:
            self.cmd_list['DFU_WRITE_IMAGE'][3] = length&0xff
            self.cmd_list['DFU_WRITE_IMAGE'][4] = length>>8
            next_send_offset =data[5]|data[6]<<8|data[7]<<16|data[8]<<24 
            self.ser.write(self.cmd_list['DFU_WRITE_IMAGE']+pay_load[next_send_offset:next_send_offset+length]+
                       self.crc16(self.cmd_list['DFU_WRITE_IMAGE']+pay_load[next_send_offset:next_send_offset+length]))
            return False
    def run(self):
        try:
            with open (self.path,'rb') as f:
                while True:
                    f.seek(0x60)
                    git_version = f.read(4)
                    f.seek(0x04)
                    signature = f.read(2)
                    self.cmd_list['READ_DEVICE_INFO'][5] = int(signature[0])
                    self.cmd_list['READ_DEVICE_INFO'][6] = int(signature[1])            
                    self.ser.write(self.cmd_list['READ_DEVICE_INFO']+self.crc16(self.cmd_list['READ_DEVICE_INFO']))
                    sleep(0.5)
                    if self.check_READ_DEVICE_INFO(git_version) == False:
                        print('READ_DEVICE_INFO fail')
                        sys.exit(1)
    
                    f.seek(0)
                    Image_header = f.read(12)
                    send_list = self.cmd_list['START_DFU']+list(Image_header[0:12])
                    send_list = send_list+self.crc16(send_list)              
                    self.ser.write(send_list)
                    sleep(0.5)
                    rlen = self.ser.inWaiting()
                    data = self.ser.read(rlen)
                    print(list(data))
                    if list(data[0:6]) != self.response_list['START_DFU'] \
                            and self.crc_check(list(data[0:len(data)-2]),list(data[len(data)-2:len(data)])):
                        print('START_DFU fail')
                        sys.exit(1)

                        
                    f.seek(0)
                    pay_load = list(f.read())
                    self.cmd_list['DFU_WRITE_IMAGE'][3] = self.payload_len&0xff
                    self.cmd_list['DFU_WRITE_IMAGE'][4] = self.payload_len>>8
                    
                    for i in range(len(pay_load)//self.payload_len):
                        self.ser.write(self.cmd_list['DFU_WRITE_IMAGE']+pay_load[i*self.payload_len:(i+1)*self.payload_len]+
                                       self.crc16(self.cmd_list['DFU_WRITE_IMAGE']+pay_load[i*self.payload_len:(i+1)*self.payload_len]))
    
                        while True:
                            if self.check_DFU_WRITE_IMAGE(pay_load,self.payload_len):
                                break
                    self.cmd_list['DFU_WRITE_IMAGE'][3] = (len(pay_load)%self.payload_len)&0xff
                    self.cmd_list['DFU_WRITE_IMAGE'][4] = (len(pay_load)%self.payload_len)>>8
                    self.ser.write(self.cmd_list['DFU_WRITE_IMAGE']+pay_load[(len(pay_load)//self.payload_len)*self.payload_len:len(pay_load)]+
                                       self.crc16(self.cmd_list['DFU_WRITE_IMAGE']+pay_load[(len(pay_load)//self.payload_len)*self.payload_len:len(pay_load)]))
                    while True:
                        if self.check_DFU_WRITE_IMAGE(pay_load,len(pay_load)%self.payload_len):
                            break
                        
                    self.ser.write(self.cmd_list['DFU_VALID_IMAGE']+self.crc16(self.cmd_list['DFU_VALID_IMAGE']))
                    sleep(0.5)
                    rlen = self.ser.inWaiting()
                    data = self.ser.read(rlen)
                    print(list(data))
                    
                    self.ser.write(self.cmd_list['DFU_ACTIVE_RESET']+self.crc16(self.cmd_list['DFU_ACTIVE_RESET']))
                    sleep(0.5)
                    if list(data[0:5]) != self.response_list['DFU_VALID_IMAGE'] \
                        and self.crc_check(list(data[0:len(data)-2]),list(data[len(data)-2:len(data)])):
                        print('DFU_VALID_IMAGE fail')
                        rlen = self.ser.inWaiting()
                        data = self.ser.read(rlen)
                        print(list(data))                 
                    else:
                        rlen = self.ser.inWaiting()
                        data = self.ser.read(rlen)
                        print(list(data)) 
                        if data[5] == 0:
                            break  
                
        finally:
            self.ser.close()
    
dfu = DFU()
dfu.run()