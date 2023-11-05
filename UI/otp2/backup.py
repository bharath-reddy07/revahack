import serial

ser = serial.Serial('COM5', baudrate=115200)  # open serial port
f = open("backup.data", 'ab')

dataStr = ""
while(1):
    data = ser.read(64)
    
    dataStr +=str(data)
    print("\n\n",dataStr)

    f.write(data)
    # print(data)

    # sleep(0.5)


ser.close()   