#ifdef ARDUINO
  #include <Stream.h>
#else
  #ifndef Stream_H
  #define Stream_H

  inline int millis() {
    static int val = 0;
    return val += 500;
  }

  #include <string.h>
  #include "Print.h"
  class Stream : Print
  {
    protected:
      unsigned long _timeout;      // number of milliseconds to wait for the next char before aborting timed read
      unsigned long _startMillis;  // used for timeout measurement
    public:
      virtual int available() = 0;
      virtual int read() = 0;
      virtual int peek() = 0;
      virtual void flush() = 0;

      Stream() {_timeout=1000;}

      size_t readBytes(char *buffer, size_t length)
      {
        size_t count = 0;
        while (count < length) {
          int c = timedRead();
          if (c < 0) break;
          *buffer++ = (char)c;
          count++;
        }
        return count;
      }
      size_t readBytes( uint8_t *buffer, size_t length) { return readBytes((char *)buffer, length); }

    protected:
      // private method to read stream with timeout
      int timedRead()
      {
        int c;
        _startMillis = millis();
        do {
          c = read();
          if (c >= 0) return c;
        } while(millis() - _startMillis < _timeout);
        return -1;     // -1 indicates timeout
      }

      // private method to peek stream with timeout
      int timedPeek()
      {
        int c;
        _startMillis = millis();
        do {
          c = peek();
          if (c >= 0) return c;
        } while(millis() - _startMillis < _timeout);
        return -1;     // -1 indicates timeout
      }
  };
  #endif
#endif
