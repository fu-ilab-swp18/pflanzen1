from django.db import models

class Sensor(models.Model):
    sensor_id = models.IntegerField(default = 0)
    water = models.IntegerField(default=0)
    temperature = models.IntegerField(default=0)
    humidity = models.IntegerField(default=0)

class Pump(models.Model):
    state = models.IntegerField(default=0)
    date = models.DateTimeField('date captured')
