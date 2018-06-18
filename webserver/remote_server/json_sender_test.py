import urllib
from urllib.request import urlopen
import json
from json import JSONEncoder

body = {'node_id':'1','humidity':'1','water':'1','temperature':'1'}

myurl = "http://localhost:8000/set_sensor_data"
req = urllib.request.Request(myurl)
req.add_header('Content-Type', 'application/json; charset=utf-8')
jsondata = json.dumps(body)
jsondataasbytes = jsondata.encode('utf-8')   # needs to be bytes
req.add_header('Content-Length', len(jsondataasbytes))
print (jsondataasbytes)
response = urllib.request.urlopen(req, jsondataasbytes)
