# -*- coding: utf-8 -*-
import time
import socket
import os

""" Constantes """

addr = ("127.0.0.1", 12000)

""" Funciones auxiliares """

def clearScreen():
    if os.name == 'nt': 
        _ = os.system('cls') 

    # Para mac and linux (os.name is 'posix')
    else: 
        _ = os.system('clear') 


def makeRequest(message):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client_socket.settimeout(1.0)
    start = time.time()
    client_socket.sendto(message, addr)
    try:
        data, server = client_socket.recvfrom(1024)
        end = time.time()
        elapsed = end - start
        print(str(data, 'utf-8'))
    except socket.timeout:
        print('REQUEST TIMED OUT')

""" =========================================================================== """

clearScreen()

print("==================================================================")

# Prueba 1
# El usuario no existe
print("Prueba 1: El usuario no existe\n")
peticion = 'GET_USER mathias\n'
print("Peticion: "+peticion)
print("---------------------\n")
print("Respuesta:\n")
message1 = bytes(peticion, 'utf-8')
makeRequest(message1)

print("==================================================================")

# Prueba 2
# Usuario existe
print("Prueba 2: El usuario existe\n")
peticion = 'GET_USER matias\n'
print("Peticion: "+peticion)
print("---------------------\n")
print("Respuesta:\n")
message2 = bytes(peticion, 'utf-8')
makeRequest(message2)

print("==================================================================")

# Prueba 3
# Usuario existe en el segundo IMAP
print("Prueba 3: El usuario existe\n")
peticion = 'GET_USER lucia\n'
print("Peticion: "+peticion)
print("---------------------\n")
print("Respuesta:\n")
message3 = bytes(peticion, 'utf-8')
makeRequest(message3)

print("==================================================================")

# Prueba 3
# Usuario existe en el segundo IMAP
print("Prueba 4: Usuario rompe todo\n")
peticion = 'GET_USER matais'
print("Peticion: "+peticion)
print("---------------------\n")
print("Respuesta:\n")
message4 = bytes(peticion, 'utf-8')
makeRequest(message4)
