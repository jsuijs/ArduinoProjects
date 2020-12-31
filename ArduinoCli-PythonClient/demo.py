# example how to use the client (should be renamed...)
import sys, json

# add path, to support imports from client package.
sys.path.append('C:/GitHub/ArduinoProjects/ArduinoCli-PythonClient')

# import, create instance & setup internal grpc clients
import client
Acc = client.ArduinoCliClient()

#
TestSketchPath = 'C:\\RobotLib\\Boards\\ArduinoStm32\\Sample_project_1'
TestFqbn       = 'STM32:stm32:GenF4:pnum=DIYMORE_F407VGT,upload_method=swdMethod,xserial=none,usb=none,xusb=FS,opt=osstd,rtlib=nano'
TestPort       = ''

print("calling SetValue")
# create in root of settings dict:
# "foo": {
#     "data": "aap",
#     "downloads": "noot",
#     "user": "mies"
# }
JsonData = '{"user": "C:/RobotLib/Boards/ArduinoStm32/Sample_project_1"}'
Acc.SetValue('directories', JsonData) # Set value

print("calling LoadSketch")
print(Acc.LoadSketch(TestSketchPath))

# print all config data
#print("calling GetAll")
#JsonData = Acc.GetAll() # All the settings
#print(json.dumps(JsonData, indent=4, sort_keys=True))

print("calling Compile")
r = Acc.Compile(sketch_path=TestSketchPath, fqbn=TestFqbn, clean=False)

client.Stamp("Einde")
print(r)
if r != 'succes' : sys.exit(1)
