import bluetooth

bd_addr = "98:D3:31:40:1E:18"
port = 1

sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((bd_addr, port))

print('Connected')

count = 0
while count < 10:
    data = sock.recv(12)
    code = int.from_bytes(b'\x01', byteorder="big", signed=False)
    print('received: %s', code)

    count += 1

sock.close()
