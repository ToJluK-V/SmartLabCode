import re
from typing import NamedTuple
import time
from socketUnit import writeState
from socketUnit import readState
from validationUnit import validate


from influxdb import InfluxDBClient

INFLUXDB_ADDRESS = '192.168.100.4'
INFLUXDB_USER = 'ivan'
INFLUXDB_PASSWORD = 'test8888'
INFLUXDB_DATABASE = 'smartlabdb'

influxdb_client = InfluxDBClient(INFLUXDB_ADDRESS, 8086, INFLUXDB_USER, INFLUXDB_PASSWORD, None)


class SensorData(NamedTuple):
    location: str
    measurement: str
    value: float

def getGETType(message):
    first_arg = str(message[1:message.find("?")])

    if (first_arg == 'validate'):
        validate(str(message[message.find("=")+1:]))
        return 'validation'
    if (first_arg == 'socket'):
        writeState(str(message[message.find("?")+1:message.find("=")]), str(message[message.find("=")+1:]))
        return 'socket'
    if (first_arg == 'socket_state'):
        return 'socket_state'
    else:
        return 'measurement'

def message_parser(message):
    print(message)

    first_arg = str(message[1:message.find("?")])

    location = first_arg
    measurement = message[message.find("?")+1:message.find("=")]
    value = float(message[message.find("=")+1:])

    return SensorData(location, measurement, value)

def _send_sensor_data_to_influxdb(sensor_data):
    json_body = [
        {
            'measurement': sensor_data.measurement,
            'tags': {
                'location': sensor_data.location
            },
            'fields': {
                'value': sensor_data.value
            }
        }
    ]
    print(json_body)
    influxdb_client.write_points(json_body)


def _init_influxdb_database():
    databases = influxdb_client.get_list_database()
    if len(list(filter(lambda x: x['name'] == INFLUXDB_DATABASE, databases))) == 0:
        influxdb_client.create_database(INFLUXDB_DATABASE)
    influxdb_client.switch_database(INFLUXDB_DATABASE)

#!/usr/bin/env python3

from http.server import BaseHTTPRequestHandler, HTTPServer
import logging

class S(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def _set_response_err(self):
        self.send_response(404)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):

        if (self.path=='/favicon.ico'):
            return

        getType = getGETType(str(self.path))

        if (getType=='measurement'):
            _send_sensor_data_to_influxdb(message_parser(str(self.path)))
            logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))
            self._set_response()
            self.wfile.write("GET request for {}".format(self.path).encode('utf-8'))
        elif (getType=='validation'):
            logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))

            if (validate(str(self.path[self.path.find("=")+1:]).lower())):
                _send_sensor_data_to_influxdb(SensorData("505", "UID_ok", str(self.path[self.path.find("=")+1:]).lower()))
                self._set_response()
                print('valid ', str(self.path[self.path.find("=")+1:]).lower())
                self.wfile.write(str('OK').encode('utf-8'))
            else:
                _send_sensor_data_to_influxdb(SensorData("505", "UID_bad", str(self.path[self.path.find("=")+1:]).lower()))
                self._set_response_err()
                print('invalid '+str(self.path[self.path.find("=")+1:]).lower())
                self.wfile.write('Invalid ID'.encode('utf-8'))

        elif (getType=='socket'):
            logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))

            if (str(self.path[self.path.find("=")+1:])=="1"):
#               _send_sensor_data_to_influxdb(SensorData("505", "Turn_on", str(self.path[self.path.find("=")+1:])))
               self._set_response()
               print('socket '+ str(self.path[self.path.find("?")+1:self.path.find("=")]), ' - turn on')
               self.wfile.write(str('On').encode('utf-8'))
            if (str(self.path[self.path.find("=")+1:])=="0"):
                self._set_response()
                print('socket ' + str(self.path[self.path.find("?")+1:self.path.find("=")]), ' - turn off')
                self.wfile.write('Off'.encode('utf-8'))

        elif (getType=='socket_state'):
            logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))

            if (readState(str(self.path[self.path.find("?")+1:self.path.find("=")]))=="1"):
               self._set_response()
               print('socket '+ str(self.path[self.path.find("?")+1:self.path.find("=")]), ' - turn on')
               self.wfile.write('On'.encode('utf-8'))
            if (readState(str(self.path[self.path.find("?")+1:self.path.find("=")]))=="0"):
                self._set_response()
                print('socket '+ str(self.path[self.path.find("?")+1:self.path.find("=")]), ' - turn off')
                self.wfile.write('Off'.encode('utf-8'))

    def do_POST(self):
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        post_data = self.rfile.read(content_length) # <--- Gets the data itself
        logging.info("POST request,\nPath: %s\nHeaders:\n%s\n\nBody:\n%s\n",
                str(self.path), str(self.headers), post_data.decode('utf-8'))

        self._set_response()
        self.wfile.write("POST request for {}".format(self.path).encode('utf-8'))
        
def run(server_class=HTTPServer, handler_class=S, port=8082):
    _init_influxdb_database()
    logging.basicConfig(level=logging.INFO)
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    logging.info('Starting httpd...\n')
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    logging.info('Stopping httpd...\n')

if __name__ == '__main__':
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
