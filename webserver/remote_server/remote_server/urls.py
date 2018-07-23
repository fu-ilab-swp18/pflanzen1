"""remote_server URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/1.11/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  url(r'^$', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  url(r'^$', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.conf.urls import url, include
    2. Add a URL to urlpatterns:  url(r'^blog/', include('blog.urls'))
"""
from django.conf.urls import url
from django.contrib import admin
from django.urls import path
from . import views

urlpatterns = [
    url(r'^admin/', admin.site.urls),
    path('', views.get_sensors_data, name='index'),
    path('set_sensor_data', views.set_sensor_data , name = 'set-sensor-data'),
    path('get_sensor_data', views.get_sensors_data , name = 'get-sensor-data'),
    path('set_pump_data', views.set_pump_data , name = 'set-pump-data'),
    path('get_pump_data', views.get_pump_data , name = 'get-pump-data'),
    path('open_pump', views.open_pump , name = 'open-pump'),
    path('close_pump', views.close_pump , name = 'close-pump'),
    
      
]
