# -*- coding: utf-8 -*-
# info_server

""" 
El proxy le hace peticiones, consultas, a info_server. 
El formato de la consulta es el siguiente:
	GET_USER-seq_number- \n
    user_example password_example \n

Se muestra a continuación el formato de la respuesta:
	seq_number \n
    error_code [Si error_code == 2] imap_server_ip

Existen 3 tipos de respuesta de info_server:
- Usuario no existe: se devuelve el código de error 0 
    que indica que el usuario consultado no existe.
- Contraseña incorrecta: se devuelve el código 1 
    que indica que la contraseña no es correcta para el usuario ingresado.
- Comando no existe: se devuelve el código 2
    que indica que no existe el comando enviado.
- Correcto: se devuelve el código de error 3 
    que indica que el usuario consultado no existe y 
    además se retorna la IP del servidor IMAP al que debe consultar el usuario.
 """

import os
import socket
import re
import datetime

""" 
==========================================================
======================= Constantes =======================
==========================================================
"""
DEBUG = True

PORT = 12000
DATAGRAM_SIZE = 1024

COMANDOS_DISPONIBLES = [
    'GET_USER'
]

""" 
====================================================================
======================= Funciones auxiliares =======================
====================================================================
"""
""" Limpia la pantalla de la consola """
def limpiarPantalla():
    # Para windows 
    if os.name == 'nt': 
        _ = os.system('cls') 
  
    # Para mac and linux (os.name is 'posix')
    else: 
        _ = os.system('clear') 

""" Busca al usuario $usuario en el arreglo de usuarios $usuarios en cada posicion
del arreglo fijandose por la clave "username" """
def obtenerPosicionDeUsuario(usuarios, usuario):
    encontre = False
    i = 0
    while(not encontre and i < len(usuarios)):
        if(usuarios[i]["username"] == usuario):
            encontre = True
        else:
            i += 1
    if(encontre):
        return i
    else:
        return False

""" Crea la respuesta como string utilizando el objeto respuesta
pasado por parametro """
def crearRespuesta(respuesta):
    return str(respuesta["numero_secuencia"])+"\n"+str(respuesta["codigo_error"])+"\n"+str(respuesta["mensaje"])

""" 
==================================================================
======================= Programa principal =======================
==================================================================
"""
limpiarPantalla()

server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind(('', PORT))

if DEBUG: print("Servidor escuchando...")

while True:

    # Recibo el datagrama
    mensaje, address = server_socket.recvfrom(DATAGRAM_SIZE)
    mensaje = str(mensaje, 'utf-8')

    print("=================================================================================================")
    if DEBUG: print(str(datetime.datetime.now())+" Atendiendo la solicitud (IP, puerto): "+str(address)+"...")

    # Defino la estructura de mi respuesta
    respuesta = {
        "numero_secuencia": -1, # Numero
        "codigo_error": -1, # Numero
        "mensaje": "" # String
    }

    error_solicitud = False

    # Proceso el mensaje
    mensaje_procesado = re.findall(r'.*\n', mensaje)
    mensaje_cabezal = mensaje_procesado[0] if mensaje_procesado[0] else False
    mensaje_credenciales = mensaje_procesado[1] if mensaje_procesado[1] else False
    if mensaje_cabezal:

        if DEBUG: print("Procesando el cabezal...")

        # Proceso el comando
        comando_procesado = re.findall(r'.*(?<=-)', mensaje_cabezal)
        
        if comando_procesado[0] != None:
            comando = comando_procesado[0]
        else:
            error_solicitud = True
            respuesta["mensaje"] = "No se pudo obtener el comando"
            respuesta["codigo_error"] = 0

        # Proceso el numero de secuencia
        secuencia_procesada = re.findall(r'(?<=-).*', mensaje_cabezal)

        if not error_solicitud and secuencia_procesada[0] != None:
            numero_secuencia = secuencia_procesada[0]
            respuesta["numero_secuencia"] = int(numero_secuencia)
        else:
            error_solicitud = True
            respuesta["mensaje"] = "No se pudo obtener la secuencia"
            respuesta["codigo_error"] = 1

        if DEBUG: print("Procesando las credenciales...")

        # Obtengo usuario y contraseña
        usuario = re.findall(r'.* ', mensaje_credenciales)
        tengo_usuario =  False
        contrasenia = re.findall(r' .*$', mensaje_credenciales)
        tengo_contrasenia = False
        
        if not error_solicitud and usuario[0] != None:
            tengo_usuario = True
            usuario = usuario[0][:len(usuario[0])-1] # Podriamos usar un lookbehind para que no tome el espacio en blanco
        else:
            respuesta["mensaje"] = "No se pudo obtener el usuario"
            respuesta["codigo_error"] = 99
            
        if not error_solicitud and contrasenia[0] != None:
            tengo_contrasenia = True
            contrasenia = contrasenia[0][1:] # Podriamos usar un lookbehind para que no tome el espacio en blanco
        else:
            respuesta["mensaje"] = "No se pudo obtener la contrasenia"
            respuesta["codigo_error"] = 99

        # Proceso usuario y contraseña
        if not error_solicitud and tengo_usuario and tengo_contrasenia:

            if DEBUG: print("Comparando las credenciales...")
            
            # Obtener los usuarios del archivo configuracion.json
            import json

            with open('configuracion.json') as archivo_json:
                usuarios = json.load(archivo_json)
                
                posicion_ususario = obtenerPosicionDeUsuario(usuarios, usuario)
                if(posicion_ususario):
                    if(usuarios[posicion_ususario]["password"] == contrasenia):
                        respuesta["mensaje"] = "Coooorrecto!"
                        respuesta["codigo_error"] = 4
                    else:
                        respuesta["mensaje"] = "Contraseña incorrecta"
                        respuesta["codigo_error"] = 3
                else:
                    respuesta["mensaje"] = "No existe el usuario ingresado"
                    respuesta["codigo_error"] = 2
            
    else:
        respuesta["mensaje"] = "Ha ocurrido un error al obtener el mensaje"

    if DEBUG: print("Creando respuesta...")
    string_respuesta = crearRespuesta(respuesta)

    # Enviar la respuesta
    server_socket.sendto(bytes(string_respuesta, 'utf-8'), address)