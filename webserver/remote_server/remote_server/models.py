from django.db import models
from django.utils import timezone
class Sensor(models.Model):
    sensor_id = models.IntegerField(null=True)
    temperature = models.IntegerField(null=True)
    humidity = models.IntegerField(null=True)
    time_stamp = models.DateTimeField(default = timezone.now())

class Pump(models.Model):
    state = models.IntegerField(null=True)
    time_stamp = models.DateTimeField(default=timezone.now())
