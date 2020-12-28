import grpc

from commands  import board_pb2,    board_pb2_grpc
from commands  import commands_pb2, commands_pb2_grpc
from commands  import common_pb2,   common_pb2_grpc
from commands  import compile_pb2,  compile_pb2_grpc
from commands  import core_pb2,     core_pb2_grpc
from commands  import lib_pb2,      lib_pb2_grpc
from commands  import upload_pb2,   upload_pb2_grpc
from debug     import debug_pb2,    debug_pb2_grpc
from monitor   import monitor_pb2,  monitor_pb2_grpc
from settings  import settings_pb2, settings_pb2_grpc

from inspect import currentframe
def WarnWarn():
    print("Warning: untested (or even not implemented) function at line", currentframe().f_back.f_lineno)

class ArduinoCliClient:

   # default constructor
   def __init__(self):
      self.channel = grpc.insecure_channel('localhost:50051')
      self.client  = commands_pb2_grpc.ArduinoCoreStub(self.channel)
      self.settingsClient = settings_pb2_grpc.SettingsStub(self.channel)

      # create instance
      rstream = self.client.Init(commands_pb2.InitReq())

      # process data, we receive from stream
      for r in rstream:
         if hasattr(r, 'instance'):
            print("Instance:", r.instance)
            self.instance = r.instance
            continue

         if hasattr(r, 'platforms_index_errors'):
            print("InstanceInit platforms_index_errors:", r.platforms_index_errors)
            continue

         if hasattr(r, 'libraries_index_error'):
            print("InstanceInit libraries_index_error:", r.libraries_index_error)
            continue

         if hasattr(r, 'download_progress'):
            print("InstanceInit download_progress:", r.download_progress)
            continue

         if hasattr(r, 'task_progress'):
            print("InstanceInit task_progress:", r.task_progress)
            continue

         print("InstanceInit !!unexpected!!", r)

      print("todo: callUpdateIndex")
      #     // With a brand new instance, the first operation should always be updating
      #     // the index.
      #     log.Println("calling UpdateIndex")
      #     callUpdateIndex(client, instance)


   # Version -  get version of Arduino-CLI.
   # Can be called without any setup or init procedure.
   def Version(self) :
      response = self.client.Version(commands_pb2.VersionReq())
      return response.version
      WarnWarn()

   # LoadSketch -
   def LoadSketch(self, sketch_path) :
      return self.client.LoadSketch(commands_pb2.LoadSketchReq(sketch_path=sketch_path))

   #   // Use SetValue to configure the arduino-cli directories.   log.Println("calling SetValue")
   #   callSetValue(settingsClient)
   def SetValue(self) :
      WarnWarn()

   #   // List all the settings.   log.Println("calling GetAll()")
   #   callGetAll(settingsClient)
   def GetAll(self) :
      WarnWarn()

   #   // Merge applies multiple settings values at once.   log.Println("calling Merge(`{\"foo\": \"bar\", \"daemon\":{\"port\":\"422\"}}`)")
   #   callMerge(settingsClient)
   def Merge(self) :
      WarnWarn()

   #   // Get the value of the foo key.   log.Println("calling GetValue(foo)")
   #   callGetValue(settingsClient)
   def GetValue(self) :
      WarnWarn()

   #   // Let's search for a platform (also known as 'core') called 'samd'.   log.Println("calling PlatformSearch(samd)")
   #   callPlatformSearch(client, instance)
   def PlatformSearch(self) :
      WarnWarn()

   #   // Install arduino:samd@1.6.19   log.Println("calling PlatformInstall(arduino:samd@1.6.19)")
   #   callPlatformInstall(client, instance)
   def PlatformInstall(self) :
      WarnWarn()

   #   // Now list the installed platforms to double check previous installation
   #   // went right.   log.Println("calling PlatformList()")
   #   callPlatformList(client, instance)
   def PlatformList(self) :
      WarnWarn()

   #   // Upgrade the installed platform to the latest version.   log.Println("calling PlatformUpgrade(arduino:samd)")
   #   callPlatformUpgrade(client, instance)
   def PlatformUpgrade(self) :
      WarnWarn()

   #  BoardsDetails - get board-details for fqbn
   def BoardsDetails(self, fqbn) :
      return self.client.BoardDetails(board_pb2.BoardDetailsReq(fqbn=fqbn, instance=self.instance))

   #   // Attach a board to a sketch.   // log.Println("calling BoardAttach(serial:///dev/ttyACM0)")
   #   // Uncomment if you do have an actual board connected.
   #   // callBoardAttach(client, instance)
   def BoardAttach(self) :
      WarnWarn()

   #   // Compile a sketch   log.Println("calling Compile(arduino:samd:mkr1000, VERBOSE, hello.ino)")
   #   callCompile(client, instance)
   def Compile(self) :
      WarnWarn()

   #   // List all boards    log.Println("calling BoardListAll(mkr)")
   #   callListAll(client, instance)
   def ListAll(self) :
      WarnWarn()

   #   // List connected boards   log.Println("calling BoardList()")
   #   callBoardList(client, instance)
   def BoardList(self) :
      WarnWarn()

   #   // Watch for boards connection and disconnection   log.Println("calling BoardListWatch()")
   #   callBoardListWatch(client, instance)
   def BoardListWatch(self) :
      WarnWarn()

   #   // Uninstall a platform   log.Println("calling PlatformUninstall(arduino:samd)")
   #   callPlatformUnInstall(client, instance)
   def PlatformUnInstall(self) :
      WarnWarn()

   #   // Update the Library index   log.Println("calling UpdateLibrariesIndex()")
   #   callUpdateLibraryIndex(client, instance)
   def UpdateLibraryIndex(self) :
      WarnWarn()

   #   // Download a library   log.Println("calling LibraryDownload(WiFi101@0.15.2)")
   #   callLibDownload(client, instance)
   def LibDownload(self) :
      WarnWarn()

   #   // Install a library    log.Println("calling LibraryInstall(WiFi101@0.15.1)")
   #   callLibInstall(client, instance, "0.15.1")
   def LibInstall(self) :
      WarnWarn()

   #   // Replace the previous version   log.Println("calling LibraryInstall(WiFi101@0.15.2)")
   #   callLibInstall(client, instance, "0.15.2")
   def LibInstall(self) :
      WarnWarn()

   #   // Upgrade all libs to latest     log.Println("calling LibraryUpgradeAll()")
   #   callLibUpgradeAll(client, instance)
   def LibUpgradeAll(self) :
      WarnWarn()

   #   // Search for a lib using the 'audio' keyword   log.Println("calling LibrarySearch(audio)")
   #   callLibSearch(client, instance)
   def LibSearch(self) :
      WarnWarn()

   #   // List the dependencies of the ArduinoIoTCloud library   log.Println("calling LibraryResolveDependencies(ArduinoIoTCloud)")
   #   callLibraryResolveDependencies(client, instance)
   def LibraryResolveDependencies(self) :
      WarnWarn()

   #   // List installed libraries   log.Println("calling LibraryList")
   #   callLibList(client, instance)
   def LibList(self) :
      WarnWarn()

   #   // Uninstall a library        log.Println("calling LibraryUninstall(WiFi101)")
   #   callLibUninstall(client, instance)
   def LibUninstall(self) :
      WarnWarn()


#response = client.GetAll(commands_pb2.LoadSketchReq(sketch_path='C:\\GitHub\\ArduinoProjects\\ArduinoCli-PythonClient\\hello'))
#print(response)


# message BoardAttachReq {
#     // Arduino Core Service instance from the `Init` response.
#     Instance instance = 1;
#     // The board's URI (e.g., /dev/ttyACM0).
#     string board_uri = 2;
#     // Path of the sketch to attach the board to. The board attachment
#     // metadata will be saved to `{sketch_path}/sketch.json`.
#     string sketch_path = 3;
#     // Duration in seconds to search the given URI for a connected board before
#     // timing out. The default value is 5 seconds.
#     string search_timeout = 4;
#response = client.BoardAttach(board_pb2.BoardAttachReq(board_uri='com3:', sketch_path='C:\\GitHub\\ArduinoProjects\\ArduinoCli-PythonClient\\hello'))
#print(response)


# ok
#response = self.client.BoardDetails(board_pb2.BoardDetailsReq(fqbn='arduino:avr:uno', instance=self.instance))
#print(response)

if __name__ == '__main__':

   # create instance & setup client(s)
   Acc = ArduinoCliClient()

   # get version of Arduino-CLI
   print("calling Version")
   print(Acc.Version())

   print("calling LoadSketch")
   print(Acc.LoadSketch('C:\\GitHub\\ArduinoProjects\\ArduinoCli-PythonClient\\hello'))

   #   // Use SetValue to configure the arduino-cli directories.
   #   log.Println("calling SetValue")
   #   callSetValue(settingsClient)
   #
   #   // List all the settings.
   #   log.Println("calling GetAll()")
   #   callGetAll(settingsClient)
   #
   #   // Merge applies multiple settings values at once.
   #   log.Println("calling Merge(`{\"foo\": \"bar\", \"daemon\":{\"port\":\"422\"}}`)")
   #   callMerge(settingsClient)
   #
   #   // Get the value of the foo key.
   #   log.Println("calling GetValue(foo)")
   #   callGetValue(settingsClient)
   #
   #   // Let's search for a platform (also known as 'core') called 'samd'.
   #   log.Println("calling PlatformSearch(samd)")
   #   callPlatformSearch(client, instance)
   #
   #   // Install arduino:samd@1.6.19
   #   log.Println("calling PlatformInstall(arduino:samd@1.6.19)")
   #   callPlatformInstall(client, instance)
   #
   #   // Now list the installed platforms to double check previous installation
   #   // went right.
   #   log.Println("calling PlatformList()")
   #   callPlatformList(client, instance)
   #
   #   // Upgrade the installed platform to the latest version.
   #   log.Println("calling PlatformUpgrade(arduino:samd)")
   #   callPlatformUpgrade(client, instance)

   print("calling BoardDetails")
   print (Acc.BoardsDetails('arduino:avr:uno'))

   #   // Attach a board to a sketch.
   #   // Uncomment if you do have an actual board connected.
   #   // log.Println("calling BoardAttach(serial:///dev/ttyACM0)")
   #   // callBoardAttach(client, instance)
   #
   #   // Compile a sketch
   #   log.Println("calling Compile(arduino:samd:mkr1000, VERBOSE, hello.ino)")
   #   callCompile(client, instance)
   #
   #   // List all boards
   #   log.Println("calling BoardListAll(mkr)")
   #   callListAll(client, instance)
   #
   #   // List connected boards
   #   log.Println("calling BoardList()")
   #   callBoardList(client, instance)
   #
   #   // Watch for boards connection and disconnection
   #   log.Println("calling BoardListWatch()")
   #   callBoardListWatch(client, instance)
   #
   #   // Uninstall a platform
   #   log.Println("calling PlatformUninstall(arduino:samd)")
   #   callPlatformUnInstall(client, instance)
   #
   #   // Update the Library index
   #   log.Println("calling UpdateLibrariesIndex()")
   #   callUpdateLibraryIndex(client, instance)
   #
   #   // Download a library
   #   log.Println("calling LibraryDownload(WiFi101@0.15.2)")
   #   callLibDownload(client, instance)
   #
   #   // Install a library
   #   log.Println("calling LibraryInstall(WiFi101@0.15.1)")
   #   callLibInstall(client, instance, "0.15.1")
   #
   #   // Replace the previous version
   #   log.Println("calling LibraryInstall(WiFi101@0.15.2)")
   #   callLibInstall(client, instance, "0.15.2")
   #
   #   // Upgrade all libs to latest
   #   log.Println("calling LibraryUpgradeAll()")
   #   callLibUpgradeAll(client, instance)
   #
   #   // Search for a lib using the 'audio' keyword
   #   log.Println("calling LibrarySearch(audio)")
   #   callLibSearch(client, instance)
   #
   #   // List the dependencies of the ArduinoIoTCloud library
   #   log.Println("calling LibraryResolveDependencies(ArduinoIoTCloud)")
   #   callLibraryResolveDependencies(client, instance)
   #
   #   // List installed libraries
   #   log.Println("calling LibraryList")
   #   callLibList(client, instance)
   #
   #   // Uninstall a library
   #   log.Println("calling LibraryUninstall(WiFi101)")
   #   callLibUninstall(client, instance)