import serial
import sys
import time
import socket
import binascii

data_to_udp = None

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('0.0.0.0', 7899))
sock.setblocking(False)
            
ser = serial.serial_for_url('/dev/ttyUSB0', baudrate=9600, timeout = 0.1)

def ser_write(msg):
    print('UART TX: ' + str(msg))
    ser.write(msg)
    ser.flush()
    time.sleep(0.2)
            
while True:
    
    
    currentLine = ser.readline().decode('utf-8','ignore')

    if "AT+" in currentLine:
        print('UART RX: ' + str(currentLine).strip() + '\r\n')
        if "AT+CGATT?" in currentLine:
            msg = '+CGATT:1\r\n'.encode()
            ser_write(msg)
            
        if "AT+NSOST" in currentLine:
            nsost = currentLine.split(',')
            udp_msg_addr = nsost[1]
            udp_msg_port = nsost[2]
            udp_msg_len = nsost[3]
            udp_msg_payload = nsost[4].strip()
            print(udp_msg_payload)
            udp_msg_payload = bytearray.fromhex(udp_msg_payload)
            print(udp_msg_payload)
            sock.sendto(udp_msg_payload, (udp_msg_addr, int(udp_msg_port)))
        if "AT+NSORF" in currentLine:
            if data_to_udp:
                hex_udp_frame = str(binascii.hexlify(data_to_udp),'ascii')
                msg = ('+NSONMI:0,' + str(len(data_to_udp)) + '\r\n').encode()
                ser_write(msg)
                msg = ('0,198.199.120.16,2115,' + str(len(data_to_udp)) + ',' + hex_udp_frame + ',0\r\n').encode()
                ser_write(msg)
                data_to_udp = None

        msg = 'OK\r\n'.encode()
        ser_write(msg)
    udp_recv_data = None
    try:
        udp_recv_data = sock.recv(128)
    except:
        pass
    
    if udp_recv_data:
        data_to_udp = udp_recv_data
        
    
