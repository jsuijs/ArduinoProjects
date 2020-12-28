py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. commands/commands.proto
py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. commands/common.proto
py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. commands/board.proto
py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. commands/compile.proto
py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. commands/core.proto
py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. commands/upload.proto
py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. commands/lib.proto

py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. debug/debug.proto
py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. monitor/monitor.proto
py -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. settings/settings.proto
