#ifdef ARDUINO
  #include <Print.h>
#else
  #ifndef Print_H
  #define Print_H
  #include <string.h>
  class Print
  {
    public:
      virtual size_t write(uint8_t) = 0;
      size_t write(const char *str) {
        if (str == NULL) return 0;
        return write((const uint8_t *)str, strlen(str));
      }
      virtual size_t write(const uint8_t *buffer, size_t size) = 0;
      size_t write(const char *buffer, size_t size) {
        return write((const uint8_t *)buffer, size);
      }
  };
  #endif
#endif
