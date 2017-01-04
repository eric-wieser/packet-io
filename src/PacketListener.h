#pragma once

#include "packet_interfaces.h"

namespace packetio {

template<typename LambdaType> class LambdaPointer {};
/**
 * @brief      A wrapper for storing a pointer to a lambda function, that works with captured variables
 *
 * @tparam     Out   lambda return type
 * @tparam     In    lambda argument types
 */
template<typename Out, typename ...In> class LambdaPointer<Out(In...)> {
    Out (*function)(void *, In...);
    void *context;

    static Out _no_context_handler(void* context, In... arguments) {
        return ((Out (*)(In...))context)(arguments...);
    }
public:
    /**
     * @brief      Create a LambdaPointer from a raw function pointer
     *
     * @param[in]  fptr  The context-less function
     */
    LambdaPointer(Out (*fptr)(In...)) {
        context = reinterpret_cast<void*>(fptr);
        function = _no_context_handler;
    }
    /**
     * @brief      Create a LambdaPointer from a lambda, possibly with captures
     *
     * @param      lambda  A pointer to the lambda function. Because this a
     *                     pointer, this must be allocated on the stack.
     */
    template <typename T>
    LambdaPointer(T *lambda) {
        context = static_cast<void*>(lambda);
        function = [](void *context, In... arguments) {
            return ((T *)context)->operator()(arguments...);
        };
    }
    //! Invoke the underlying function
    Out operator()(In ... in)
    {
      return function(context, in...);
    }

    //! Determine if the reference has been initialized
    operator bool() {
        return context != nullptr;
    }
};

/**
 * @brief      Class for listening for packets on a PacketStream
 *
 * @tparam     BufferSize  The size of the buffer to allocate for incoming packets
 */
template<size_t BufferSize = 256>
class PacketListener_ {
public:
    //! Types of error that can occur when receiving a packet
    enum class Error {
        Overflow,  //!< The end of the message could not be recieved because the buffer overflowed
        Framing    //!< The message was framed incorrectly
    };
    typedef LambdaPointer<void(uint8_t*, size_t, Error)> ErrorHandler;
    typedef LambdaPointer<void(uint8_t*, size_t)> MessageHandler;

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
    /**
     * @brief      Construct a listener for the given packet stream
     */
    PacketListener_(PacketStream& base) : _base(base) {}

    /**
     * @brief      Read as much as possible from the underlying stream.
     *             If new packets are completed or errors occur, invoke the
     *             appropriate handler
     */
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

    /**
     * @brief      Set the handler to invoke when a message is recieved
     *
     * @param[in]  handler  The handler to invoke. This will be called with the
     *                      pointer to the start of the message, and its length.
     *
     * Note that because this is a LambdaPointer, the lambda function must have
     * been allocated on the stack, such that its lifetime exceeds that of this
     * object.
     */
    void onMessage(MessageHandler handler) {
        _messageHandler = handler;
    }

    /**
     * @brief      Set the handler to invoke when an error occurs
     *
     * @param[in]  handler  The handler to invoke. This will be called with the
     *                      pointer to the start of the message, and its length.
     *
     * Note that because this is a LambdaPointer, the lambda function must have
     * been allocated on the stack, such that its lifetime exceeds that of this
     * object.
     */
    void onError(ErrorHandler handler) {
        _errorHandler = handler;
    }
};

//! Convenience typedef for a PacketListener with a sensible buffer size
typedef PacketListener_<> PacketListener;

}
