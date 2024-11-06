# EDAoogle

El código se divide en dos partes: Un módulo que arma el índice y otro que corre una instancia del buscador.

-ÍNDICE: 
  Es necesario modificar la variable "path" con la dirección absoluta del directorio /www .
  El código itera por todos los archivos que encuentra en ese directorio. Por cada uno, extrae 
  todo el cuerpo del texto omitiendo los delimitadores de HTML, "<" y ">". Luego, mediante Expresiones
  Regulares, extraemos los caracteres especiales (Los cuales podemos detectar porque, al ser extraídos
  de cada archivo, son parseados con el cuerpo "&#...;") y las comillas dobles y simples, las cuales sino 
  serían interpretadas como comandos separados en la String que le pasamos a SQLite3.
  Luego de tener eso, insertamos este cuerpo, junto con su título y Path, a una tabla virtual creada con FTS5.

-BUSCADOR:
  Es necesario modificar la variable "path" con la dirección absoluta del directorio /www .
  Al recibir el string de entrada quitamos las comillas para evitar las inyecciones de SQL.
  Utilizamos el algoritmo bm25() proveído por fts5 para acceder a los resultados mas conectados con la consulta realizada.
  Se utilizo la libreria chronos para llevar el tiempo de consulta a la base de datos.
  Cada respuesta se da con su respectivo titulo y tiene un link a la página correspondiente.
  EL bm25() permite realizar operaciones de OR y AND.
  Al poner "dominios anidados" se obtiene Evolución Biológica.
  

  
