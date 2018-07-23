from django.shortcuts import get_object_or_404, render
from django.template.loader import render_to_string
from django.shortcuts import render_to_response
from django.views.decorators.csrf import csrf_exempt
from django.http import HttpResponse, HttpResponseServerError
from django.utils import timezone
import remote_server

from remote_server.models import Sensor, Pump
import json
import os.path

class Sensor(object):	#incase json database used
    sensor_id = 0
    humidity = 0
    water = 0
    temperature = 0
 
    def __init__(self, sensor_id, humidity, water, temperature):
        self.sensor_id = sensor_id
        self.humidity = humidity
        self.water = water
        self.temperature = temperature

def index(request):
    return HttpResponse("Hello, world. You're at the pflanzen1 index.")

def get_sensors_data(request):
    db_files = [f for f in os.listdir(get_db_dir()) if f.endswith('.data')] 
    sensors_obj_array  = []
    for sensor_file in db_files:
        with open(os.path.join(get_db_dir(),sensor_file), "r") as f:
            lines = f.read().splitlines()
            last_line = lines[-1]
            sensor_data = json.loads(last_line)
            sensors_obj_array.append(Sensor(sensor_data['node_id'],sensor_data['humidity'],sensor_data['water'],sensor_data['temperature']))
            print(sensor_data)   
    return render(request,'sensors_table.html',{'sensors_details': sensors_obj_array, 'content': 'ok'})        

@csrf_exempt
def set_sensor_data(request):
    if request.method == 'POST':
        json_data = json.loads(request.read().decode('utf-8'))
        try:
            node_id = json_data['node_id']
            humidity = json_data['humidity']
            temperature = json_data['temperature']
            water = json_data['water']
            write_sensor_data_db(json_data)
        except KeyError:
            print("INVALID REQUEST")
            HttpResponseServerError("malformed data!")               
        return HttpResponse("Got json data")
    else:
        return HttpResponse("Invalid request.")
def get_db_dir():
    database_dir = 'sensors_database'
    here = os.path.dirname(os.path.realpath(__file__))
    return os.path.join(here,database_dir)

def get_sensor_file(note_id):
    return os.path.join(get_db_dir(), 'sensor_'+node_id+'.data')

def write_sensor_data_file(data):	
     node_id = data['node_id']
     humidity = data['humidity']
     temperature = data['temperature']
     water = data['water']
     sensor_file = get_sensor_file(node_id)
     try:
        file_read = open (sensor_file, 'r')
     except IOError:
        print("Sensor file not initialized.")
        return
 
     with open(sensor_file, "a") as f:
         f.write(json.dumps(data) + "\n")
         print("sensor data written to file")

def write_pump_data_db(data):
    p = Pump(state = data['state'], date = timezone.now())
    p.save()
def write_sensor_data_db(data):
     s = remote_server.models.Sensor(sensor_id = data['node_id'], water = data['water'], humidity = data['humidity'], temperature = data['temperature'])
     s.save()
