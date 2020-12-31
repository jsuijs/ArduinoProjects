import sys, json

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
   print("Search source for ** HOW-TO-PROCEED ** for guidlines.")

# ** HOW-TO-PROCEED **
#
# Well, it's work.
# General:
#  * all (most?) methodes are already in the class ArduinoCliClient (that's why you get the HOW-TO-PROCEED message)
#  * use client_example\main.go as a reference.
#     * line 70-200 are base for the call in 'main' of this source.
#     * the function called (line 200 forward) are a reference for the method implementation.
#  * There are 4 things to watch for
#     e.g: rmsg =self.client.BoardDetails(board_pb2.BoardDetailsReq(fqbn=fqbn, instance=self.instance))
#     find the 'rcp' line in the proto file, in this case:
#        rpc BoardDetails(BoardDetailsReq) returns (BoardDetailsResp);
#     -> BoardDetailsReq is the message sent. Look for 'message BoardDetailsReq' in a 'proto' file,
#        as it gives a great overview of the parameters.
#     -> BoardDetailsResp is the type of the return message (rmsg). Look for 'message BoardDetailsResp' in a 'proto' file,
#        as it gives a great overview of the parameters.
#     -> board_pb2 prefix of messages, since it is defined in board.proto.
#     -> self.client since board_pb2 is imported from commands.
#  * BoardDetails is an example of a non-stream call.  And a good example of how to document, based on the .proto file.
#  * Some (most?) rcp definition have a 'stream' return-type, e.g. rpc Init(InitReq) returns (stream InitResp) {}
#    See 'def Compile' of how these can be implemented in Python.

import time
StartTime = time.perf_counter()
PrevTime  = time.perf_counter()
def Stamp(Tag) :
   global PrevTime, StartTime
   Now = time.perf_counter()
   print("Stamp %s - step: %f sec, Total: %f sec" % (Tag, Now - PrevTime, Now-StartTime))
   PrevTime = Now

class ArduinoCliClient:

   #---------------------------------------------------------------------------
   # default constructor
   #---------------------------------------------------------------------------
   def __init__(self):
      self.Verbose         = 1
      self.channel         = grpc.insecure_channel('localhost:50051')
      self.client          = commands_pb2_grpc.ArduinoCoreStub(self.channel)
      self.settingsClient  = settings_pb2_grpc.SettingsStub(self.channel)
      Stamp("SettingsClient")

      try:
         self.instance        =  self.InitInstance()
      except:
         print("Error in InitInstance... Is the daemon running?")
         sys.exit(1)
      Stamp("InitInstance")

      self.UpdateIndex()
      Stamp("UPdateIndex");

   # Ask Deamon to create instance and get its ID
   def InitInstance(self) :
      # create instance
      rstream = self.client.Init(commands_pb2.InitReq())

      # process data, we receive from stream
      for r in rstream:
         Unexptected = True
         if hasattr(r, 'instance'):
            print("Instance:", r.instance)
            instance = r.instance
            Unexptected = False

         if hasattr(r, 'platforms_index_errors'):
            if len(r.platforms_index_errors) > 0 :
               print("InstanceInit platforms_index_errors:", r.platforms_index_errors)
            Unexptected = False

         if hasattr(r, 'libraries_index_error'):
            if r.libraries_index_error != '' :
               print("InstanceInit libraries_index_error:", r.libraries_index_error)
            Unexptected = False

         if hasattr(r, 'download_progress'):
            print("InstanceInit download_progress:", r.download_progress)
            Unexptected = False

         if hasattr(r, 'task_progress'):
            print("InstanceInit task_progress:", r.task_progress)
            Unexptected = False

         if Unexptected: print("InstanceInit !!unexpected attribute!!", r)

      return instance

   #---------------------------------------------------------------------------
   # UpdateIndex
   #---------------------------------------------------------------------------
   # With a brand new instance, the first operation should always be updating the index.
   def UpdateIndex(self) :
      # create instance
      rstream = self.client.UpdateIndex(commands_pb2.UpdateIndexReq(instance=self.instance))

      # process data, we receive from stream
      for r in rstream:
         if hasattr(r, 'download_progress'):
            if self.Verbose > 0 :
               print("#", r.download_progress, end='')
            continue

         print("UpdateIndex !!unexpected!!", r)
      print()

   #---------------------------------------------------------------------------
   # Version -  get version of Arduino-CLI.
   #---------------------------------------------------------------------------
   # Can be called without any setup or init procedure.
   def Version(self) :
      response = self.client.Version(commands_pb2.VersionReq())
      return response.version

   #---------------------------------------------------------------------------
   # LoadSketch -
   def LoadSketch(self, sketch_path) :
      return self.client.LoadSketch(commands_pb2.LoadSketchReq(sketch_path=sketch_path))

   #---------------------------------------------------------------------------
   # GetAll -
   #---------------------------------------------------------------------------
   #  rpc GetAll(GetAllRequest) returns (RawData);
   #
   #  message GetAllRequest {}
   #
   #  message RawData {
   #
   #     // The settings, in JSON format.
   #     string jsonData = 1;
   #
   def GetAll(self) :
      response = self.settingsClient.GetAll(settings_pb2.GetAllRequest())
      return json.loads(response.jsonData)

   #---------------------------------------------------------------------------
   # GetValue - Get the value of a specific setting.
   #---------------------------------------------------------------------------
   #   callGetValue(settingsClient)
   #
   #  rpc GetValue(GetValueRequest) returns (Value);
   #
   #  message GetValueRequest {
   #
   #     // The key of the setting.
   #     string key = 1;
   #
   #  message Value {
   #
   #     // The key of the setting.
   #     string key = 1;
   #
   #     // The setting, in JSON format.
   #     string jsonData = 2;
   #
   def GetValue(self, key) :
      WarnWarn() ### NOT DONE #######################################################################
      try:
         p = settings_pb2.GetValueRequest(key=key)
         print("GetValue: >", p, "<")
         response = self.settingsClient.GetValue(p)
         return json.loads(response.jsonData)
      except grpc.RpcError as rpc_error_call:
         details = rpc_error_call.details()
         print("GetValue ERROR: " + details)
         return 'error'

   #---------------------------------------------------------------------------
   # Merge - Set multiple settings values at once.
   #---------------------------------------------------------------------------
   #  rpc Merge(RawData) returns (MergeResponse);
   #
   #  message RawData {
   #
   #     // The settings, in JSON format.
   #     string jsonData = 1;
   #
   #  message MergeResponse {}
   #
   def Merge(self, jsonData) :
      WarnWarn()
      self.settingsClient.Merge(settings_pb2.RawData(jsonData=jsonData))
      return


#func callMerge(client settings.SettingsClient) {
#	bulkSettings := `{"foo": "bar", "daemon":{"port":"422"}}`
#	_, err := client.Merge(context.Background(),
#		&settings.RawData{
#			JsonData: bulkSettings,
#		})
#
#	if err != nil {
#		log.Fatalf("Error merging settings: %s", err)
#	}
#}


   #---------------------------------------------------------------------------
   # SetValue - Set the value of a specific setting.
   #---------------------------------------------------------------------------
   #  rpc SetValue(Value) returns (SetValueResponse);
   #
   #  message Value {
   #
   #     // The key of the setting.
   #     string key = 1;
   #
   #     // The setting, in JSON format.
   #     string jsonData = 2;
   #
   #  message SetValueResponse {}
   #
   def SetValue(self, key, jsonData) :
      self.settingsClient.SetValue(settings_pb2.Value(key=key, jsonData=jsonData))
      return

   #---------------------------------------------------------------------------
   #   // Let's search for a platform (also known as 'core') called 'samd'.   log.Println("calling PlatformSearch(samd)")
   #   callPlatformSearch(client, instance)
   def PlatformSearch(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Install arduino:samd@1.6.19   log.Println("calling PlatformInstall(arduino:samd@1.6.19)")
   #   callPlatformInstall(client, instance)
   def PlatformInstall(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Now list the installed platforms to double check previous installation
   #   // went right.   log.Println("calling PlatformList()")
   #   callPlatformList(client, instance)
   def PlatformList(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Upgrade the installed platform to the latest version.   log.Println("calling PlatformUpgrade(arduino:samd)")
   #   callPlatformUpgrade(client, instance)
   def PlatformUpgrade(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #  BoardsDetails - get board-details for fqbn
   #---------------------------------------------------------------------------
   def BoardsDetails(self, fqbn) :
      return self.client.BoardDetails(board_pb2.BoardDetailsReq(fqbn=fqbn, instance=self.instance))

   #---------------------------------------------------------------------------
   # BoardAttach -
   #---------------------------------------------------------------------------
   #  rpc BoardAttach(BoardAttachReq) returns (stream BoardAttachResp);
   #
   #  message BoardAttachReq {
   #
   #     // Arduino Core Service instance from the `Init` response.
   #     Instance instance = 1;
   #
   #     // The board's URI (e.g., /dev/ttyACM0).
   #     string board_uri = 2;
   #
   #     // Path of the sketch to attach the board to. The board attachment
   #     // metadata will be saved to `{sketch_path}/sketch.json`.
   #     string sketch_path = 3;
   #
   #     // Duration in seconds to search the given URI for a connected board before
   #     // timing out. The default value is 5 seconds.
   #     string search_timeout = 4;
   #
   #  message BoardAttachResp {
   #
   #     // Description of the current stage of the board attachment.
   #     TaskProgress task_progress = 1;
   #
   def BoardAttach(self, sketch_path, board_uri, search_timeout=5) :
      WarnWarn() ### NOT DONE #######################################################################

      p = board_pb2.BoardAttachReq(instance=self.instance, board_uri=board_uri, sketch_path=sketch_path, search_timeout=str(search_timeout));
      print("BoardAttach: >", p, "<")
      rstream = self.client.BoardAttach(p)

      # process data, we receive from stream
      #-------------------------------------
      try:
         for r in rstream:
            if hasattr(r, 'task_progress'):
               if self.Verbose > 0 :
                  print("#", r.task_progress, end='')
               continue

            print("BoardAttach !!unexpected!!", r)
         print()

      except grpc.RpcError as rpc_error_call:
#         code = rpc_error_call.code()
#         print(code, file=sys.stderr)
         details = rpc_error_call.details()
         print("BoardAttach ERROR: " + details)
         return 'error'

      return 'succes'


   #---------------------------------------------------------------------------
   # Compile - compile a sketch
   #---------------------------------------------------------------------------
   # message CompileReq {
   #   Instance   instance = 1;                    // Arduino Core Service instance from the `Init` response.
   #   string     fqbn = 2;                        // Fully Qualified Board Name, e.g.: `arduino:avr:uno`. If this field is not defined, the FQBN of the board attached to the sketch via the `BoardAttach` method is used.
   #   string     sketchPath = 3;                  // The path where the sketch is stored.
   #   bool       showProperties = 4;              // Show all build preferences used instead of compiling.
   #   bool       preprocess = 5;                  // Print preprocessed code to stdout instead of compiling.
   #   string     buildCachePath = 6;              // Builds of 'core.a' are saved into this path to be cached and reused.
   #   string     buildPath = 7;                   // Path to use to store the files used for the compilation. If omitted, a directory will be created in the operating system's default temporary path.
   #   repeated   string buildProperties = 8;      // List of custom build properties separated by commas.
   #   string     warnings = 9;                    // Used to tell gcc which warning level to use. The level names are: "none", "default", "more" and "all".
   #   bool       verbose = 10;                    // Turns on verbose mode.
   #   bool       quiet = 11;                      // Suppresses almost every output.
   #   string     vidPid = 12;                     // VID/PID specific build properties.
   #   int32      jobs = 14;                       // The max number of concurrent compiler instances to run (as `make -jx`). If jobs is set to 0, it will use the number of available CPUs as the maximum.
   #   repeated   string libraries = 15;           // List of custom libraries paths separated by commas.
   #   bool       optimizeForDebug = 16;           // Optimize compile output for debug, not for release.
   #   string     export_dir = 18;                 // Optional: save the build artifacts in this directory, the directory must exist.
   #   bool       clean = 19;                      // Optional: cleanup the build folder and do not use any previously cached build
   #   bool       export_binaries = 20;            // When set to `true` the compiled binary will be copied to the export directory.
   #   bool       create_compilation_database_only = 21;
   #                                               // When set to `true` only the compilation database will be produced and no actual build will be performed.
   #   map<string, string> source_override = 22;   // This map (source file -> new content) let the builder use the provided content instead of reading the corresponding file on disk. This is useful for IDE that have unsaved changes in memory. The path must be relative to the sketch directory. Only files from the sketch are allowed.
   # }
   def Compile(self, fqbn, sketch_path, warnings='all', verbose=1, clean=False) :
      # note: sketch_path translates to SketchPath
      p = compile_pb2.CompileReq(instance=self.instance, fqbn=fqbn, sketchPath=sketch_path, warnings=warnings, verbose=verbose, clean=clean)
      p.buildCachePath  = 'C:\\RobotLib\\Boards\\ArduinoStm32\\Sample_project_1\\ABuild' # 'core' is appended
      p.buildPath       = 'C:\\RobotLib\\Boards\\ArduinoStm32\\Sample_project_1\\ABuild'
      p.export_dir      = 'C:\\RobotLib\\Boards\\ArduinoStm32\\Sample_project_1\\ABuild\\export'
      p.libraries.append('C:\\RobotLib\\Boards\\ArduinoStm32\\Sample_project_1\\Build\\RobotLib.a')
#      p.quit = True
      print("CompileReq: >", p, "<")
      rstream = self.client.Compile(p)

      # process data, we receive from stream
      # ------------------------------------
      # bytes     out_stream = 1;               // The output of the compilation process.
      # bytes     err_stream = 2;               // The error output of the compilation process.
      # string    build_path = 3;               // The compiler build path
      # repeated  Library used_libraries = 4;   // The libraries used in the build
      # repeated  ExecutableSectionSize executable_sections_size = 5;
      #                                         // The size of the executable split by sections

      try:
         for r in rstream:
            Unexptected = True
            if hasattr(r, 'out_stream'):
               if self.Verbose > 0 :
                  print(r.out_stream.decode("utf-8"), end='')
               Unexptected = False

            if hasattr(r, 'err_stream'):
               ErrorString = r.err_stream.decode("utf-8")
               if ErrorString != '' :
                  print("ERR#", ErrorString, end='')
               Unexptected = False

            if hasattr(r, 'build_path'):
               if len(r.build_path) > 0 :
                  print("build#", r.build_path)
               Unexptected = False

            if hasattr(r, 'used_libraries'):
               if len(r.used_libraries) > 0 :
                  print("lib#", r.used_libraries)
               Unexptected = False

            if hasattr(r, 'executable_sections_size'):
               if len(r.executable_sections_size) > 0 :
                     print("exe#", r.executable_sections_size)
               Unexptected = False

            if Unexptected : print("Compile !!unexpected Attribute!!", r)

      except grpc.RpcError as rpc_error_call:
#         code = rpc_error_call.code()
#         print(code, file=sys.stderr)
         details = rpc_error_call.details()
         print("Compile ERROR: " + details)
         return 'error'

      print()

   #---------------------------------------------------------------------------
   # Upload - Upload a sketch
   #---------------------------------------------------------------------------
   # rpc Upload(UploadReq) returns (stream UploadResp);
   #
   #  message UploadReq {
   #
   #     // Arduino Core Service instance from the `Init` response.
   #     Instance instance = 1;
   #
   #     // Fully qualified board name of the target board (e.g., `arduino:avr:uno`).
   #     // If this field is not defined, the FQBN of the board attached to the sketch
   #     // via the `BoardAttach` method is used.
   #     string fqbn = 2;
   #
   #     // Path where the sketch to be uploaded is stored. Unless the `import_file`
   #     // field is defined, the compiled binary is assumed to be at the location and
   #     // filename under this path where it is saved by the `Compile` method.
   #     string sketch_path = 3;
   #
   #     // The port of the board.
   #     string port = 4;
   #
   #     // Whether to turn on verbose output during the upload.
   #     bool verbose = 5;
   #
   #     // After upload, verify that the contents of the memory on the board match the
   #     // uploaded binary.
   #     bool verify = 6;
   #
   #     // When `import_file` is specified, it overrides the `import_dir` and `sketch_path`
   #     // params.
   #     string import_file = 7;
   #
   #     // Custom path to a directory containing compiled files. When `import_dir` is
   #     // not specified, the standard build directory under `sketch_path` is used.
   #     string import_dir = 8;
   #
   #     // The programmer to use for upload. If set an UploadUsingProgrammer is triggered
   #     // instead of a normal upload. The UploadUsingProgrammer call may also be used for
   #     // explicit error check.
   #     string programmer = 9;
   #  }
   #
   #  message UploadResp {
   #
   #     // The output of the upload process.
   #     bytes out_stream = 1;
   #
   #     // The error output of the upload process.
   #     bytes err_stream = 2;
   #  }
   #
   def Upload(self, fqbn, port, sketch_path, verbose=3, clean=False) :
      p = upload_pb2.UploadReq(instance=self.instance, fqbn=fqbn, sketch_path=sketch_path, port=port, verbose=verbose, verify=0, import_file='', import_dir='')
      print("UploadReq: >", p, "<")
      rstream = self.client.Upload(p)

      for r in rstream:
         Unexptected = True
         if hasattr(r, 'out_stream'):
            if self.Verbose > 0 :
               print(r.out_stream.decode("utf-8"), end='')
            Unexptected = False

         if hasattr(r, 'err_stream'):
            print("err", r.err_stream)
            Unexptected = False

         if Unexptected : print("Upload !!unexpected attribute!!", r)
      print()

   #---------------------------------------------------------------------------
   #   // List all boards    log.Println("calling BoardListAll(mkr)")
   #   callListAll(client, instance)
   def ListAll(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // List connected boards   log.Println("calling BoardList()")
   #   callBoardList(client, instance)
   def BoardList(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Watch for boards connection and disconnection   log.Println("calling BoardListWatch()")
   #   callBoardListWatch(client, instance)
   def BoardListWatch(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Uninstall a platform   log.Println("calling PlatformUninstall(arduino:samd)")
   #   callPlatformUnInstall(client, instance)
   def PlatformUnInstall(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Update the Library index   log.Println("calling UpdateLibrariesIndex()")
   #   callUpdateLibraryIndex(client, instance)
   def UpdateLibraryIndex(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Download a library   log.Println("calling LibraryDownload(WiFi101@0.15.2)")
   #   callLibDownload(client, instance)
   def LibDownload(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Install a library    log.Println("calling LibraryInstall(WiFi101@0.15.1)")
   #   callLibInstall(client, instance, "0.15.1")
   def LibInstall(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Replace the previous version   log.Println("calling LibraryInstall(WiFi101@0.15.2)")
   #   callLibInstall(client, instance, "0.15.2")
   def LibInstall(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Upgrade all libs to latest     log.Println("calling LibraryUpgradeAll()")
   #   callLibUpgradeAll(client, instance)
   def LibUpgradeAll(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Search for a lib using the 'audio' keyword   log.Println("calling LibrarySearch(audio)")
   #   callLibSearch(client, instance)
   def LibSearch(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // List the dependencies of the ArduinoIoTCloud library   log.Println("calling LibraryResolveDependencies(ArduinoIoTCloud)")
   #   callLibraryResolveDependencies(client, instance)
   def LibraryResolveDependencies(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // List installed libraries   log.Println("calling LibraryList")
   #   callLibList(client, instance)
   def LibList(self) :
      WarnWarn()

   #---------------------------------------------------------------------------
   #   // Uninstall a library        log.Println("calling LibraryUninstall(WiFi101)")
   #   callLibUninstall(client, instance)
   def LibUninstall(self) :
      WarnWarn()

if __name__ == '__main__':

   Stamp("Programstart")

   if False:
      TestSketchPath = 'C:\\GitHub\\ArduinoProjects\\ArduinoCli-PythonClient\\hello'
      TestFqbn       = 'arduino:avr:nano:cpu=atmega328old'
      TestPort       = 'COM13'

   if True:
      TestSketchPath = 'C:\\RobotLib\\Boards\\ArduinoStm32\\Sample_project_1'
      TestFqbn       = 'STM32:stm32:GenF4:pnum=DIYMORE_F407VGT,upload_method=swdMethod,xserial=none,usb=none,xusb=FS,opt=osstd,rtlib=nano'
      TestPort       = ''

   # create instance & setup client(s)
   Acc = ArduinoCliClient()
   Stamp("Create ArduinoCliClient")

   if False:
      print("calling Version")
      print(Acc.Version())    # get version of Arduino-CLI

   if False:
      print("calling SetValue")
      # create in root of settings dict:
      # "foo": {
      #     "data": "aap",
      #     "downloads": "noot",
      #     "user": "mies"
      # }
      JsonData = '{"data": "aap", "downloads": "noot", "user": "mies"}'
      Acc.SetValue('foo', JsonData) # Set value

   if True:
      print("calling SetValue")
      # create '"boom": "roos"' at the root of the settings dict.
      JsonData = '"roos"'  # single string in json format
      Acc.SetValue('boom', JsonData) # Set value

   if False:
      print("calling Merge")
      JsonData = '{"boom": "vis", "daemon":{"vuur":"pim"}}' # first key/value pair does not seem to work...
      Acc.Merge(JsonData) # Set multiple values

   if True:
      print("calling GetAll")
      JsonData = Acc.GetAll() # All the settings
      print(json.dumps(JsonData, indent=4, sort_keys=True))

   # GetValue -----------------------------------------------------------------
   if True:
      # How to specify the key properly?
      # single level: 'boom'   - gaat soms goed?!!
      # multi level???  'library:enable_unsafe_install', 'library:{enable_unsafe_install}', 'library{enable_unsafe_install}', library[enable_unsafe_install]
      print("calling GetValue")
      print(Acc.GetValue("daemon"))

   if False:
      print("calling LoadSketch")
      print(Acc.LoadSketch(TestSketchPath))

   if False:
      print("calling BoardDetails")
      print(Acc.BoardsDetails(TestFqbn ))

   if False:
      print("calling BoardAttach")
      print(Acc.BoardAttach(TestSketchPath, TestPort))

   # Compile ------------------------------------------------------------------
   if False:
      # TODO: cwd op project directory zetten,anders wordt link_opt.h blijbaar niet meegelinkt...
      # Testen met de cli (zonder daemon) geeft wel deze info; makkelijker maken om
      # de commando's voor de cli te genereren uit dit programma (of documenteren..)
      # c:\RobotLib\Tools\Arduino\arduino-cli compile -b STM32:stm32:GenF4:pnum=DIYMORE_F407VGT,upload_method=swdMethod,xserial=none,usb=none,xusb=FS,opt=osstd,rtlib=nano C:\\RobotLib\\Boards\\ArduinoStm32\\Sample_project_1
      print("calling Compile")
      r = Acc.Compile(sketch_path=TestSketchPath, fqbn=TestFqbn, clean=False)
      Stamp("Compile")
      print(r)
      if r != 'succes' : sys.exit(1)

   # Upload -------------------------------------------------------------------
   if False:
      print("calling Upload")
      print(Acc.Upload(fqbn=TestFqbn, sketch_path=TestSketchPath, port=TestPort))
      Stamp("Upload")




   # todo   // Merge applies multiple settings values at once.
   # todo   log.Println("calling Merge(`{\"foo\": \"bar\", \"daemon\":{\"port\":\"422\"}}`)")
   # todo   callMerge(settingsClient)
   # todo
   # todo   // Let's search for a platform (also known as 'core') called 'samd'.
   # todo   log.Println("calling PlatformSearch(samd)")
   # todo   callPlatformSearch(client, instance)
   # todo
   # todo   // Install arduino:samd@1.6.19
   # todo   log.Println("calling PlatformInstall(arduino:samd@1.6.19)")
   # todo   callPlatformInstall(client, instance)
   # todo
   # todo   // Now list the installed platforms to double check previous installation
   # todo   // went right.
   # todo   log.Println("calling PlatformList()")
   # todo   callPlatformList(client, instance)
   # todo
   # todo   // Upgrade the installed platform to the latest version.
   # todo   log.Println("calling PlatformUpgrade(arduino:samd)")
   # todo   callPlatformUpgrade(client, instance)
   # todo
   # todo  // List all boards
   # todo  log.Println("calling BoardListAll(mkr)")
   # todo  callListAll(client, instance)
   # todo
   # todo  // List connected boards
   # todo  log.Println("calling BoardList()")
   # todo  callBoardList(client, instance)
   # todo
   # todo  // Watch for boards connection and disconnection
   # todo  log.Println("calling BoardListWatch()")
   # todo  callBoardListWatch(client, instance)
   # todo
   # todo  // Uninstall a platform
   # todo  log.Println("calling PlatformUninstall(arduino:samd)")
   # todo  callPlatformUnInstall(client, instance)
   # todo
   # todo  // Update the Library index
   # todo  log.Println("calling UpdateLibrariesIndex()")
   # todo  callUpdateLibraryIndex(client, instance)
   # todo
   # todo  // Download a library
   # todo  log.Println("calling LibraryDownload(WiFi101@0.15.2)")
   # todo  callLibDownload(client, instance)
   # todo
   # todo  // Install a library
   # todo  log.Println("calling LibraryInstall(WiFi101@0.15.1)")
   # todo  callLibInstall(client, instance, "0.15.1")
   # todo
   # todo  // Replace the previous version
   # todo  log.Println("calling LibraryInstall(WiFi101@0.15.2)")
   # todo  callLibInstall(client, instance, "0.15.2")
   # todo
   # todo  // Upgrade all libs to latest
   # todo  log.Println("calling LibraryUpgradeAll()")
   # todo  callLibUpgradeAll(client, instance)
   # todo
   # todo  // Search for a lib using the 'audio' keyword
   # todo  log.Println("calling LibrarySearch(audio)")
   # todo  callLibSearch(client, instance)
   # todo
   # todo  // List the dependencies of the ArduinoIoTCloud library
   # todo  log.Println("calling LibraryResolveDependencies(ArduinoIoTCloud)")
   # todo  callLibraryResolveDependencies(client, instance)
   # todo
   # todo  // List installed libraries
   # todo  log.Println("calling LibraryList")
   # todo  callLibList(client, instance)
   # todo
   # todo  // Uninstall a library
   # todo  log.Println("calling LibraryUninstall(WiFi101)")
   # todo  callLibUninstall(client, instance)