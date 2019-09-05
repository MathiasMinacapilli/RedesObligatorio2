# Piques tirados por el gran matias (no se el apellido)
- info_server
    - Para info server definir los mensajes y el comportamiento de cada uno (cliente y servidor). Sirve definir una maquina de estados.
    - El info_Server tiene que ser facilmente configurable. Los usuarios se tienen que obtener de un archivo por ejemplo, que sea dinamico.
    - PUEDE SER EN PYTHON
- proxy
    - Siempre tiene que estar escuchando peticiones de los clientes(En accept). (Cuando recibe una nueva peticion deberia hacer un nuevo hilo flags: -p threads para el compilador)
    - Importar las bibliotecas de threads
    - Sabemos que un mensaje IMAP termina (usa TCP) cuando llega un \n