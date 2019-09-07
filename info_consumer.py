import time
import socket

# Prueba 1
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(1.0)
print("==================================================================")
peticion = 'GET_USER-1\nmathias saihtam\n'
print("Peticion: "+peticion)
message = bytes(peticion, 'utf-8')
addr = ("127.0.0.1", 12000)

start = time.time()
client_socket.sendto(message, addr)
try:
    data, server = client_socket.recvfrom(1024)
    end = time.time()
    elapsed = end - start
    print(str(data, 'utf-8'))
except socket.timeout:
    print('REQUEST TIMED OUT')

# Prueba 2
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(1.0)
print("==================================================================")
peticion = 'GET_USER-1\nmatias saitam\n'
print("Peticion: "+peticion)
message = bytes(peticion, 'utf-8')
addr = ("127.0.0.1", 12000)

start = time.time()
client_socket.sendto(message, addr)
try:
    data, server = client_socket.recvfrom(1024)
    end = time.time()
    elapsed = end - start
    print(str(data, 'utf-8'))
except socket.timeout:
    print('REQUEST TIMED OUT')