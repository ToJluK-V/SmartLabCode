import re
# имя файла
FILENAME = "sockets_state.txt"

# запись списка в файл
def writeState(socket, state):
 with open(FILENAME, "r") as file: #проверка на существование записи
  new_line = socket + " " + state + "\n"
  lines = file.readlines()
 file.close()
 with open(FILENAME, "w") as file: #записываем все строки, кроме той, которая совпала
  for line in lines:
   if(line.startswith(socket)):
    line = line.replace(line, '')
   file.write(line)
  file.write(new_line)
  file.close()

# считываем сообщения из файла
def readState(socket):
 with open(FILENAME, "r") as file:
  lines = file.readlines()
  for line in lines:
   if(line.startswith(socket)):
    return(line[(len(line)-2)])
  file.close()