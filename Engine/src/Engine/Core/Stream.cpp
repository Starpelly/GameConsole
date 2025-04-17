// Modified from https://github.com/NoelFB/blah/blob/master/src/blah_stream.cpp

#include "Engine/Core/SoulcastEngine.hpp"

using namespace Soulcast;

// Stream Base Class Implementation

size_t Stream::Pipe(Stream& stream, size_t length)
{
	const int BUFFER_LENGTH = 4096;
	size_t result = 0;

	char buffer[BUFFER_LENGTH];
	while (length > 0)
	{
		auto step = length;
		if (step > BUFFER_LENGTH)
			step = BUFFER_LENGTH;

		auto count = Read(buffer, step);
		auto wrote = stream.Write(buffer, count);
		result += wrote;
		length -= step;

		if (count < step || wrote < count)
			break;
	}

	return result;
}

size_t Stream::Read(void* buffer, size_t length)
{
	return read_data(buffer, length);
}

string Stream::ReadString(int length)
{
	string result;

	if (length < 0)
	{
		char next;
		while (Read(&next, 1) && next != '\0')
			result += next;
	}
	else
	{
		for (int i = 0; i < length; i++)
			result += '\0';

		Read((void*)(result.c_str()), length);
	}

	return result;
}

string Stream::ReadLine()
{
	string result;

	char next;
	while (Read(&next, 1) && next != '\n' && next != '\0')
		result += next;

	return result;
}

size_t Stream::Write(const void* buffer, size_t length)
{
	return write_data(buffer, length);
}

size_t Stream::Write(const string& string)
{
	return Write(string.c_str(), string.length());
}

uint8 Stream::Read_uint8(Endian endian)
{
	uint8 value; read_data(&value, sizeof(uint8));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

uint16 Stream::Read_uint16(Endian endian)
{
	uint16 value; read_data(&value, sizeof(uint16));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

uint32 Stream::Read_uint32(Endian endian)
{
	uint32 value; read_data(&value, sizeof(uint32));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

uint64 Stream::Read_uint64(Endian endian)
{
	uint64 value; read_data(&value, sizeof(uint64));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

int8 Stream::Read_int8(Endian endian)
{
	int8 value; read_data(&value, sizeof(int8));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

int16 Stream::Read_int16(Endian endian)
{
	int16 value; read_data(&value, sizeof(int16));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

int32 Stream::Read_int32(Endian endian)
{
	int32 value; read_data(&value, sizeof(int32));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

int64 Stream::Read_int64(Endian endian)
{
	int64 value; read_data(&value, sizeof(int64));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

float32 Stream::Read_float32(Endian endian)
{
	float value; read_data(&value, sizeof(float));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

float64 Stream::Read_float64(Endian endian)
{
	double value; read_data(&value, sizeof(double));
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return value;
}

size_t Stream::Write_uint8(uint8 value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(uint8));
}

size_t Stream::Write_uint16(uint16 value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(uint16));
}

size_t Stream::Write_uint32(uint32 value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(uint32));
}

size_t Stream::Write_uint64(uint64 value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(uint64));
}

size_t Stream::Write_int8(int8 value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(int8));
}

size_t Stream::Write_int16(int16 value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(int16));
}

size_t Stream::Write_int32(int32 value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(int32));
}

size_t Stream::Write_int64(int64 value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(int64));
}

size_t Stream::Write_float32(float value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(float));
}

size_t Stream::Write_float64(double value, Endian endian)
{
	if (!Math::is_endian(endian)) Math::swap_endian(&value);
	return write_data(&value, sizeof(double));
}


// File Stream Implementation

FileStream::FileStream(const std::filesystem::path& path, FileMode mode)
	: m_file(File::Open(path, mode)) {
}

FileStream::FileStream(const Ref<File>& file)
	: m_file(file) {
}

size_t FileStream::length() const
{
	return (m_file ? m_file->length() : 0);
}

size_t FileStream::position() const
{
	return (m_file ? m_file->position() : 0);
}

size_t FileStream::seek(size_t seek_to)
{
	return (m_file ? m_file->seek(seek_to) : 0);
}

size_t FileStream::read_data(void* ptr, size_t length)
{
	return (m_file ? m_file->read(ptr, length) : 0);
}

size_t FileStream::write_data(const void* ptr, size_t length)
{
	return (m_file ? m_file->write(ptr, length) : 0);
}

bool FileStream::is_open() const
{
	return m_file ? true : false;
}

bool FileStream::is_readable() const
{
	return m_file && m_file->Mode() != FileMode::CreateWrite;
}

bool FileStream::is_writable() const
{
	return m_file && m_file->Mode() != FileMode::OpenRead;
}


// Memory Stream Implementation

MemoryStream::MemoryStream(uint8* data, size_t length)
	: m_data(data), m_const_data(nullptr), m_length(length), m_position(0) {
}

MemoryStream::MemoryStream(const uint8* data, size_t length)
	: m_data(nullptr), m_const_data(data), m_length(length), m_position(0) {
}

size_t MemoryStream::length() const
{
	return m_length;
}

size_t MemoryStream::position() const
{
	return m_position;
}

size_t MemoryStream::seek(size_t seek_to)
{
	return m_position = (seek_to < 0 ? 0 : (seek_to > m_length ? m_length : seek_to));
}

size_t MemoryStream::read_data(void* ptr, size_t len)
{
	const uint8* src = (m_data ? m_data : m_const_data);

	if (src == nullptr || ptr == nullptr || len <= 0 || m_length <= 0 || m_position >= m_length)
		return 0;

	if (len > m_length - m_position)
		len = m_length - m_position;

	memcpy(ptr, src + m_position, len);
	m_position += len;
	return len;
}

size_t MemoryStream::write_data(const void* ptr, size_t len)
{
	if (m_data == nullptr || ptr == nullptr || len <= 0 || m_length <= 0 || m_position >= m_length)
		return 0;

	if (len > m_length - m_position)
		len = m_length - m_position;

	memcpy(m_data + m_position, ptr, len);
	m_position += len;
	return len;
}

bool MemoryStream::is_open() const { return (m_data || m_const_data) && m_length > 0; }
bool MemoryStream::is_readable() const { return (m_data || m_const_data) && m_length > 0; }
bool MemoryStream::is_writable() const { return m_data != nullptr && m_length > 0; }
uint8* MemoryStream::data() { return m_data; }
const uint8* MemoryStream::data() const { return (m_data ? m_data : m_const_data); }


// Buffer Stream Implementation

BufferStream::BufferStream(int capacity)
{
	m_buffer.resize(capacity);
}

size_t BufferStream::length() const
{
	return m_buffer.size();
}

size_t BufferStream::position() const
{
	return m_position;
}

size_t BufferStream::seek(size_t seek_to)
{
	return m_position = (seek_to < 0 ? 0 : (seek_to > m_buffer.size() ? m_buffer.size() : seek_to));
}

size_t BufferStream::read_data(void* ptr, size_t len)
{
	if (ptr == nullptr || len <= 0)
		return 0;

	if (len > m_buffer.size() - m_position)
		len = m_buffer.size() - m_position;

	memcpy(ptr, m_buffer.data() + m_position, (size_t)len);
	m_position += len;
	return len;
}

size_t BufferStream::write_data(const void* ptr, size_t len)
{
	if (len < 0)
		return 0;

	// resize
	if (m_position + len > m_buffer.size())
		resize(m_position + len);

	// copy data
	if (ptr != nullptr)
		memcpy(m_buffer.data() + m_position, ptr, (size_t)len);

	// increment position
	m_position += len;

	// return the amount we wrote
	return len;
}

bool BufferStream::is_open() const { return true; }
bool BufferStream::is_readable() const { return true; }
bool BufferStream::is_writable() const { return true; }

void BufferStream::resize(size_t length)
{
	m_buffer.resize(length);
}

void BufferStream::clear()
{
	m_buffer.clear();
	m_position = 0;
}

uint8* BufferStream::data()
{
	return m_buffer.data();
}

const uint8* BufferStream::data() const
{
	return m_buffer.data();
}