#include "byte_stream.hh"

#include <iostream>
#include <string.h>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//ByteStream::ByteStream(const size_t capacity) { DUMMY_CODE(capacity); }
ByteStream::ByteStream(const size_t capacity) // _buffer默认初始化也必须写在初始化列表中?否则报错
	:_buffer(), _readIndex(0), _writeIndex(0), _capacity(capacity),
   	_readCount(0), _writeCount(0),
   	_input_ended(false), _eof(false)
{
	_buffer.reserve(_capacity);
}

size_t ByteStream::write(const string &data) {
	size_t writen = 0;
	if (remaining_capacity() >= data.size())	// 剩余空间足够
	{
		writen = data.size();
		_writeCount += data.size();
		if (_capacity-_writeIndex >= data.size())	// 尾部空间足够
		{
			memcpy(&_buffer[_writeIndex], &data[0], data.size());
			_writeIndex += data.size();
		}
		else										// 需要先搬移，再添加
		{
			size_t payload = _writeIndex - _readIndex;
			memcpy(&_buffer[0], &_buffer[_readIndex], _writeIndex-_readIndex);
			memcpy(&_buffer[buffer_size()], &data[0], data.size());
			_readIndex = 0;
			_writeIndex = payload + data.size();
		}
	}
	else											// 剩余空间不够, 就把buffer写满，其它的不管,丢弃
	{
		cout << "there is no enough space to be write\n";

		writen = remaining_capacity();
		_writeCount += remaining_capacity();
		memcpy(&_buffer[0], &_buffer[_readIndex], _writeIndex-_readIndex);
//		_readIndex = 0;
//		_writeIndex = _capacity;
		memcpy(&_buffer[buffer_size()], &data[0], remaining_capacity());
//		_writeCount += remaining_capacity();
		_readIndex = 0;
		_writeIndex = _capacity;
	}

	cout << writen << " bytes writen in this write\n";
	return writen;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {   // 偷看,并不改变readCount
	if (len <= buffer_size())
	{
		return string(&_buffer[_readIndex], len);
	}
	else
	{
		cout << "there is no enough data to be read\n";
		return string(&_buffer[_readIndex], _writeIndex-_readIndex);
	}
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len)			// 这里才统计readCount
{
	if (len < buffer_size())	// 注意这里写成 < 而不是 <=
	{
		_readIndex += len;
		_readCount += len; 		// 这句不要忘了
	}
	else
	{
		cout << "all data are pop, buffer is empty now\n";
		_readCount += buffer_size();	// 这句不要忘了
		_readIndex = 0;
		_writeIndex = 0;

		//_eof = true;			// 这里不能直接置为true, _eof为真当且仅当 _input_ended && buffer_empty()
	}
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {   // 测试程序中好像并没有调用这个函数,而是分别调用peek pop
	string str = peek_output(len);
	pop_output(len);
	return str;
}

void ByteStream::end_input() { _input_ended = true; } 	// 表示写端已近写完了，不再写了

bool ByteStream::input_ended() const { return _input_ended; }

size_t ByteStream::buffer_size() const { return _writeIndex - _readIndex; }

bool ByteStream::buffer_empty() const { return buffer_size() == 0; }

//bool ByteStream::eof() const { return _eof; }
//bool ByteStream::eof() const { return _eof || (input_ended()&&buffer_empty()); }
bool ByteStream::eof() const { return input_ended()&&buffer_empty(); }  // 注意这里的逻辑判断，我们其实是不需要_eof标志的

size_t ByteStream::bytes_written() const { return _writeCount; }

size_t ByteStream::bytes_read() const { return _readCount; }

size_t ByteStream::remaining_capacity() const { return _capacity - buffer_size(); }
