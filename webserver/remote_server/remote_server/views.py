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

class Sensor_obj(object):	#Helpful for building html template. see sensors_table.html
    sensor_id = 0
    humidity = 0
    temperature = 0
    timestamp = ""
    def __init__(self, sensor_id, humidity, temperature, timestamp):
        self.sensor_id = sensor_id
        self.humidity = humidity
        self.temperature = temperature
        self.timestamp = timestamp

class Pump_obj(object):
    state = -1
    time_stamp = timezone.now()
    def __init__(self, state):
        self.state = state

@csrf_exempt
def get_pump_data(request):
    pump_obj_array  = []
    for line in Pump.objects.all():
          pump_obj_arr.append(Pump_obj(line.state,line.timestamp))
    return render(request,'sensors_table.html',{'sensors_details': pump_obj_array, 'content': 'ok'})        


def get_sensors_data(request):
    sensors_obj_array  = []
    for line in Sensor.objects.all():  
        sensors_obj_array.append(Sensor_obj(line.sensor_id, line.humidity, line.temperature, line.time_stamp))
    return render(request,'sensors_table.html',{'sensors_details': sensors_obj_array, 'content': 'ok'})        

def open_pump(request):
    HttpResponse("Pump opened") 

def close_pump(request):
    HttpResponse("Pump closed")

@csrf_exempt
def set_pump_data(request):
    if request.method == 'POST':
        json_data = json.loads(request.read().decode('utf-8'))
        try:
            node_id = json_data['state']
            write_pump_data_db(json_data)
        except KeyError:
            HttpResponseServerError("malformed data!")
            raise Exception(KeyError)               
        return HttpResponse("Got json data")
    else:
        return HttpResponseServerError("Invalid request.")


@csrf_exempt
def set_sensor_data(request):
    if request.method == 'POST':
        json_data = json.loads(request.read().decode('utf-8'))
        try:
            node_id = json_data['node_id']
            humidity = json_data['humidity']
            temperature = json_data['temperature']
            write_sensor_data_db(json_data)
        except KeyError:
            print("INVALID REQUEST")
            HttpResponseServerError("malformed data!")               
        return HttpResponse("Got json data")
    else:
        return HttpResponseServerError("Invalid request.")


def write_pump_data_db(data):
    p = Pump(state = data['state'])
    p.save()
def write_sensor_data_db(data):
     s = remote_server.models.Sensor(sensor_id = data['node_id'], humidity = data['humidity'], temperature = data['temperature'])
     s.save()
