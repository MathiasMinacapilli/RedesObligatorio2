# -*- coding: utf-8 -*-
# info_server

""" 
El proxy le hace peticiones, consultas, a info_server. 
El formato de la consulta es el siguiente:
	GET_USER username \n

Se muestra a continuación el formato de la respuesta:
	[OK | NO] imap_server_ip \n

Retorna OK y la IP del servidor IMAP al que consultar por el usuario si 
existe el usuario enviado.
Retorna NO si no existe el usuario y retorna una IP de un servidor IMAP 
configurado por defecto.

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

IP = "127.0.0.1"
PORT = 12000
DATAGRAM_SIZE = 1024

IP_POR_DEFECTO = "192.168.56.102"

COMANDOS_DISPONIBLES = set([
    'GET_USER'
])

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
    error = 'NO ' if respuesta['error'] else 'OK '
    IP_IMAP = str(respuesta["IP_IMAP"])+"\n"

    string_respuesta = error+IP_IMAP
    
    return string_respuesta

""" 
==================================================================
======================= Programa principal =======================
==================================================================
"""
limpiarPantalla()

server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind(('', PORT))

if DEBUG: print("Servidor escuchando en el puerto "+str(PORT)+"...")

while True:
    
    # Recibo el datagrama
    mensaje, address = server_socket.recvfrom(DATAGRAM_SIZE)
    mensaje = str(mensaje, 'utf-8')

    print("=================================================================================================")
    if DEBUG: print(str(datetime.datetime.now())+" Atendiendo la solicitud (IP, puerto): "+str(address)+"...")

    # Estructura de mi respuesta
    respuesta = {
        "error": False, # Numero
        "IP_IMAP": IP_POR_DEFECTO, # String
    }

    error_solicitud = False

    # Proceso el mensaje
    mensaje_procesado = re.findall(r'.*\n', mensaje)
    mensaje = mensaje_procesado[0] if mensaje_procesado[0] else False
    if mensaje:

        if DEBUG: print(str(datetime.datetime.now())+" Procesando el mensaje...")

        # Proceso el comando
        comando_procesado = re.findall(r'^.* ', mensaje)
        comando = comando_procesado[0][:len(comando_procesado[0])-1] # -1 para olvidarse del espacio
        if comando is None or comando not in COMANDOS_DISPONIBLES:
            respuesta["error"] = True
            error_solicitud = True

        if DEBUG: print(str(datetime.datetime.now())+" Procesando el usuario...")

        # Obtengo usuario
        usuario = re.findall(r' .*$', mensaje)
        tengo_usuario =  False
        
        if not error_solicitud and usuario[0] != None:
            tengo_usuario = True
            usuario = usuario[0][1:len(usuario[0])] # Podriamos usar un lookbehind para que no tome el espacio en blanco
            if DEBUG: print(str(datetime.datetime.now())+"Obtenido el nombre de usuario: "+usuario+". Consultando datos...")
        elif not error_solicitud:
            respuesta["error"] = True

        
        # Proceso usuario
        if not error_solicitud and tengo_usuario:

            if DEBUG: print(str(datetime.datetime.now())+" Buscando usuario en el registro...")
            
            # Obtener los usuarios del archivo configuracion.json
            import json

            with open('configuracion.json') as archivo_json:
                usuarios = json.load(archivo_json)
                
                posicion_ususario = obtenerPosicionDeUsuario(usuarios, usuario)
                
                if(posicion_ususario is not False):
                    # Existe usuario           
                    if DEBUG: print(str(datetime.datetime.now())+" Existe usuario en el registro...")             
                    respuesta["error"] = False
                    respuesta["IP_IMAP"] = usuarios[posicion_ususario]["imapIP"]
                else:
                    if DEBUG: print(str(datetime.datetime.now())+" NO existe usuario en el registro...")             
                    respuesta["error"] = True
       
    else:
        respuesta["error"] = True

    if DEBUG: print(str(datetime.datetime.now())+" Creando respuesta...")

    string_respuesta = crearRespuesta(respuesta)
    
    # Enviar la respuesta
    server_socket.sendto(bytes(string_respuesta, 'utf-8'), address)