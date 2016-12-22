#ifndef mock_print_h
#define mock_print_h

#include <stdint.h>

#include "Print.h"
#include "Stream.h"

class MockStringBuffer;

class MockBuffer {
public:
	virtual const uint8_t* get_buffer() const = 0;
	virtual size_t get_len() const = 0;
};

class MockStream : public Stream {
private:
	MockBuffer& _src;
	size_t _index = 0;
public:
	MockStream(MockBuffer& src): _src(src) { }
	virtual int peek() override {
		if (_index < _src.get_len())
			return _src.get_buffer()[_index];
		else
			return -1;
	}
	virtual int read() override {
		if (_index < _src.get_len())
			return _src.get_buffer()[_index++];
		else
			return -1;
	}
	virtual int available() override {
		return _src.get_len() - _index;
	}
	virtual void flush() override {
		_index = _src.get_len();
	}
};


template<size_t N=20>
class MockPrint : public Print, public MockBuffer {
private:
	size_t _len = 0;
	uint8_t _buffer[N];
public:
	virtual size_t write(uint8_t val) {
		if(_len >= N) return 0;
		_buffer[_len++] = val;
		return 1;
	}
	virtual size_t write(const uint8_t* vals, size_t n) override {
		size_t i = 0;
		while(i < n) {
			if(!write(vals[i])) return i;
			i++;
		}
		return i;
	}
	using Print::write;
	virtual const uint8_t* get_buffer() const override {
		return _buffer;
	}
	virtual size_t get_len() const override {
		return _len;
	}
};

template<size_t N>
class FailingMockPrint : public MockPrint<N> {
protected:
	size_t _fail_len;
	const size_t* _fail_on;
	size_t _fail_at = 0;

	virtual bool _check_bad(size_t where) {
		for(size_t i = 0; i < _fail_len; i++) {
			if(_fail_on[i] == where) {
				return true;
			}
		}
		return false;
	}
public:
	template<int M>
	FailingMockPrint(const size_t (&fail_on)[M]) : _fail_len(M), _fail_on(fail_on)
	{}

	virtual size_t write(uint8_t val) {
		if (_check_bad(_fail_at++)) return 0;
		return MockPrint<N>::write(val);
	}
	using MockPrint<N>::write;

	FailingMockPrint(const FailingMockPrint& that) = delete;
};

#endif