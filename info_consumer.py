import time
import socket
import os

if os.name == 'nt': 
    _ = os.system('cls') 

# Para mac and linux (os.name is 'posix')
else: 
    _ = os.system('clear') 

print("Formato de respuesta: \nNumero de secuencia\nCodigo de error\nIP del IMAP\nMensaje")

# Prueba 1
# El usuario no existe
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(1.0)
print("==================================================================")
peticion = 'GET_USER-1\nmathias saihtam\n'
print("Peticion: "+peticion)
print("El usuario no existe")
print("---------------------\nRespuesta:\n")
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
# Usuario existe, contraseña erronea
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(1.0)
print("==================================================================")
peticion = 'GET_USER-1\nlucia saitama\n'
print("Peticion: "+peticion)
print("Usuario existe pero contraseña erronea")
print("---------------------\nRespuesta:\n")
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


# Prueba 3
# Usuario existe, contraseña conrrecta
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(1.0)
print("==================================================================")
peticion = 'GET_USER-1\nlucia aicul\n'
print("Peticion: "+peticion)
print("Usuario existe y contraseña correcta")
print("---------------------\nRespuesta:\n")
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


# Prueba 4
# Comando incorrecto
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(1.0)
print("==================================================================")
peticion = 'GET_USERRRRR-1\nlucia aicul\n'
print("Peticion: "+peticion)
print("Comando incorrecto")
print("---------------------\nRespuesta:\n")
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

# Prueba 5
# Secuencia incorrecta
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(1.0)
print("==================================================================")
peticion = 'GET_USER-1aaa\nlucia aicul\n'
print("Peticion: "+peticion)
print("Secuencia incorrecta")
print("---------------------\nRespuesta:\n")
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