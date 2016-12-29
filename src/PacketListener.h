#pragma once

#include "packet_interfaces.h"

#include "../test/test_helpers.h"

namespace packetio {

// A wrapper for a lambda function, that works with captured variables
template<typename LambdaType> class Handler {};
template<typename Out, typename ...In> class Handler<Out(In...)> {
    Out (*function)(void *, In...);
    void *context;

    static Out _no_context_handler(void* context, In... arguments) {
        return ((Out (*)(In...))context)(arguments...);
    }
public:
    Handler(Out (*fptr)(In...)) {
        context = reinterpret_cast<void*>(fptr);
        function = _no_context_handler;
    }
    template <typename T>
    Handler(T &lambda) {
        context = static_cast<void*>(&lambda);
        function = [](void *context, In... arguments) {
            return ((T *)context)->operator()(arguments...);
        };
    }
    Out operator()(In ... in)
    {
      return function(context, in...);
    }
    operator bool() {
        return context != nullptr;
    }
};

template<size_t BufferSize = 256>
class PacketListener_ {
public:
    enum class Error {
        Overflow,
        Framing
    };
    typedef Handler<void(uint8_t*, size_t, Error)> ErrorHandler;
    typedef Handler<void(uint8_t*, size_t)> MessageHandler;

private:
    PacketStream& _base;
    uint8_t _buffer[BufferSize];
    size_t _buffer_index = 0;

    ErrorHandler _errorHandler = nullptr;
    MessageHandler _messageHandler = nullptr;

    inline void _onMessage(uint8_t* message, size_t len) {
        if(_messageHandler) _messageHandler(message, len);
    }
    inline void _onError(uint8_t* message, size_t len, Error e) {
        if(_errorHandler) _errorHandler(message, len, e);
    }
public:
    PacketListener_(PacketStream& base) : _base(base) {}

    void update() {
        int a = 0;
        while((a = _base.available()) > 0) {
            if(_buffer_index >= BufferSize) {
                _onError(_buffer, _buffer_index, Error::Overflow);
                _buffer_index = 0;
                _base.next();
                return;
            }

            int c = _base.read();
            if(c == PacketStream::EOF) {
                return;
            }
            else if(c == PacketStream::EOP) {
                _onMessage(_buffer, _buffer_index);
                _buffer_index = 0;
                _base.next();
            }
            else {
                _buffer[_buffer_index++] = c;
            }
        }
    }

    void onMessage(MessageHandler handler) {
        _messageHandler = handler;
    }

    void onError(ErrorHandler handler) {
        _errorHandler = handler;
    }
};

typedef PacketListener_<> PacketListener;

}
