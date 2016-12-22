#ifndef mock_print_h
#define mock_print_h

#include <stdint.h>

#include "Print.h"
#include "Stream.h"

class MockBuffer {
public:
	virtual const uint8_t* get_buffer() const = 0;
	virtual uint8_t* get_buffer() = 0;
	virtual size_t get_len() const = 0;
	inline uint8_t operator[](size_t i) const {
		return get_buffer()[i];
	}
	inline uint8_t& operator[](size_t i) {
		return get_buffer()[i];
	}
};

template<size_t N>
class MockABuffer : public MockBuffer {
private:
	uint8_t _buffer[N];
public:
	virtual const uint8_t* get_buffer() const { return _buffer; };
	virtual uint8_t* get_buffer() { return _buffer; };
	virtual size_t get_len() const { return N; }
};

class MockStream : public Stream {
private:
	const MockBuffer& _src;
	size_t _index = 0;
public:
	MockStream(const MockBuffer& src): _src(src) { }
	virtual int peek() override {
		if (_index < _src.get_len())
			return _src[_index];
		else
			return -1;
	}
	virtual int read() override {
		if (_index < _src.get_len())
			return _src[_index++];
		else
			return -1;
	}
	virtual int available() override {
		return _src.get_len() - _index;
	}
	virtual void flush() override {
		_index = _src.get_len();
	}

private:
	size_t write(uint8_t) { return 0; }
	size_t write(const uint8_t*, size_t) { return 0; }
};

class MockPrint : public Print, public MockBuffer {
private:
	MockBuffer& _dest;
	size_t _len = 0;
public:
	MockPrint(MockBuffer& dest): _dest(dest) { }
	virtual size_t write(uint8_t val) {
		if(_len >= _dest.get_len()) return 0;
		_dest[_len++] = val;
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
		return _dest.get_buffer();
	}
	virtual uint8_t* get_buffer() override {
		return _dest.get_buffer();
	}
	virtual size_t get_len() const override {
		return _len;
	}
};

class Failer {
private:
	size_t _fail_len;
	const size_t* _fail_on;
	size_t _fail_at = 0;

	bool _is_bad(size_t where) {
		for(size_t i = 0; i < _fail_len; i++) {
			if(_fail_on[i] == where) {
				return true;
			}
		}
		return false;
	}
public:
	bool check_bad() {
		return _is_bad(_fail_at++);
	}

	template<int M>
	Failer(const size_t (&fail_on)[M])
		: _fail_len(M), _fail_on(fail_on) { }
};

class FailingPrint : public Print {
protected:
	MockPrint& _base;
	Failer _f;

public:
	FailingPrint(MockPrint& base, Failer&& f)
		: _base(base), _f(f) { }

	virtual size_t write(uint8_t val) {
		if (_f.check_bad()) return 0;
		return _base.write(val);
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
};

class FailingStream : public Stream {
protected:
	MockStream& _base;
	Failer _f;

public:
	FailingStream(MockStream& base, Failer&& f)
		: _base(base), _f(f) { }

	virtual int read() {
		if (_f.check_bad()) return -1;
		return _base.read();
	}
	virtual int peek() {
		if (_f.check_bad()) return -1;
		return _base.peek();
	}
	virtual int available() {
		return _base.available();
	}
	virtual void flush() {
		return _base.flush();
	}

private:
	size_t write(uint8_t) { return 0; }
	size_t write(const uint8_t*, size_t) { return 0; }
};
#endif