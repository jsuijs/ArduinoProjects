# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
"""Client and server classes corresponding to protobuf-defined services."""
import grpc

from settings import settings_pb2 as settings_dot_settings__pb2


class SettingsStub(object):
    """The Settings service provides an interface to Arduino CLI's configuration
    options
    """

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.GetAll = channel.unary_unary(
                '/cc.arduino.cli.settings.Settings/GetAll',
                request_serializer=settings_dot_settings__pb2.GetAllRequest.SerializeToString,
                response_deserializer=settings_dot_settings__pb2.RawData.FromString,
                )
        self.Merge = channel.unary_unary(
                '/cc.arduino.cli.settings.Settings/Merge',
                request_serializer=settings_dot_settings__pb2.RawData.SerializeToString,
                response_deserializer=settings_dot_settings__pb2.MergeResponse.FromString,
                )
        self.GetValue = channel.unary_unary(
                '/cc.arduino.cli.settings.Settings/GetValue',
                request_serializer=settings_dot_settings__pb2.GetValueRequest.SerializeToString,
                response_deserializer=settings_dot_settings__pb2.Value.FromString,
                )
        self.SetValue = channel.unary_unary(
                '/cc.arduino.cli.settings.Settings/SetValue',
                request_serializer=settings_dot_settings__pb2.Value.SerializeToString,
                response_deserializer=settings_dot_settings__pb2.SetValueResponse.FromString,
                )


class SettingsServicer(object):
    """The Settings service provides an interface to Arduino CLI's configuration
    options
    """

    def GetAll(self, request, context):
        """List all the settings.
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Merge(self, request, context):
        """Set multiple settings values at once.
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def GetValue(self, request, context):
        """Get the value of a specific setting.
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def SetValue(self, request, context):
        """Set the value of a specific setting.
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_SettingsServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'GetAll': grpc.unary_unary_rpc_method_handler(
                    servicer.GetAll,
                    request_deserializer=settings_dot_settings__pb2.GetAllRequest.FromString,
                    response_serializer=settings_dot_settings__pb2.RawData.SerializeToString,
            ),
            'Merge': grpc.unary_unary_rpc_method_handler(
                    servicer.Merge,
                    request_deserializer=settings_dot_settings__pb2.RawData.FromString,
                    response_serializer=settings_dot_settings__pb2.MergeResponse.SerializeToString,
            ),
            'GetValue': grpc.unary_unary_rpc_method_handler(
                    servicer.GetValue,
                    request_deserializer=settings_dot_settings__pb2.GetValueRequest.FromString,
                    response_serializer=settings_dot_settings__pb2.Value.SerializeToString,
            ),
            'SetValue': grpc.unary_unary_rpc_method_handler(
                    servicer.SetValue,
                    request_deserializer=settings_dot_settings__pb2.Value.FromString,
                    response_serializer=settings_dot_settings__pb2.SetValueResponse.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'cc.arduino.cli.settings.Settings', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))


 # This class is part of an EXPERIMENTAL API.
class Settings(object):
    """The Settings service provides an interface to Arduino CLI's configuration
    options
    """

    @staticmethod
    def GetAll(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/cc.arduino.cli.settings.Settings/GetAll',
            settings_dot_settings__pb2.GetAllRequest.SerializeToString,
            settings_dot_settings__pb2.RawData.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def Merge(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/cc.arduino.cli.settings.Settings/Merge',
            settings_dot_settings__pb2.RawData.SerializeToString,
            settings_dot_settings__pb2.MergeResponse.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def GetValue(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/cc.arduino.cli.settings.Settings/GetValue',
            settings_dot_settings__pb2.GetValueRequest.SerializeToString,
            settings_dot_settings__pb2.Value.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def SetValue(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/cc.arduino.cli.settings.Settings/SetValue',
            settings_dot_settings__pb2.Value.SerializeToString,
            settings_dot_settings__pb2.SetValueResponse.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)
