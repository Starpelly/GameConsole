// Modified from https://github.com/NoelFB/blah/blob/master/include/blah_stream.h

#pragma once

namespace Soulcast
{
	class Stream
	{
	public:
		Stream() = default;
		Stream(const Stream&) = delete;
		Stream& operator=(const Stream&) = delete;

		virtual ~Stream() = default;

		// returns the length of the stream
		virtual size_t length() const = 0;

		// returns the position of the stream
		virtual size_t position() const = 0;

		// seeks the position of the stream
		virtual size_t seek(size_t position) = 0;

		// returns true of the stream is open
		virtual bool is_open() const = 0;

		// returns true of the stream is readable
		virtual bool is_readable() const = 0;

		// returns true of the stream is writable
		virtual bool is_writable() const = 0;

		// pipes the contents of this stream to another stream
		size_t Pipe(Stream& to, size_t length);

		// reads the amount of bytes into the given buffer, and returns the amount read
		size_t Read(void* buffer, size_t length);

		// reads a string. if length < 0, assumes null-terminated
		std::string ReadString(int length = -1);

		// reads a string until a newline '\n' or null-terminator '\0' is found
		std::string ReadLine();

		// reads a number, returns the value read.
		uint8  Read_uint8(Endian endian = Endian::Little);
		uint16 Read_uint16(Endian endian = Endian::Little);
		uint32 Read_uint32(Endian endian = Endian::Little);
		uint64 Read_uint64(Endian endian = Endian::Little);
		int8  Read_int8(Endian endian = Endian::Little);
		int16 Read_int16(Endian endian = Endian::Little);
		int32 Read_int32(Endian endian = Endian::Little);
		int64 Read_int64(Endian endian = Endian::Little);
		float32 Read_float32(Endian endian = Endian::Little);
		float64 Read_float64(Endian endian = Endian::Little);

		// writes the amount of bytes to the stream from the given buffer, and returns the amount written
		size_t Write(const void* buffer, size_t length);

		// writes the contents of a string to the stream
		size_t Write(const string& string);

		// writes a number, returns number of bytes written
		size_t Write_uint8(uint8  value, Endian endian = Endian::Little);
		size_t Write_uint16(uint16 value, Endian endian = Endian::Little);
		size_t Write_uint32(uint32 value, Endian endian = Endian::Little);
		size_t Write_uint64(uint64 value, Endian endian = Endian::Little);
		size_t Write_int8(int8  value, Endian endian = Endian::Little);
		size_t Write_int16(int16 value, Endian endian = Endian::Little);
		size_t Write_int32(int32 value, Endian endian = Endian::Little);
		size_t Write_int64(int64 value, Endian endian = Endian::Little);
		size_t Write_float32(float32 value, Endian endian = Endian::Little);
		size_t Write_float64(float64 value, Endian endian = Endian::Little);

	protected:
		// reads the amount of bytes into the given buffer, and returns the amount read
		virtual size_t read_data(void* buffer, size_t length) = 0;

		// writes the amount of bytes to the stream from the given buffer, and returns the amount written
		virtual size_t write_data(const void* buffer, size_t length) = 0;
	};

	// File Stream reads & writes over a File handle
	class FileStream : public Stream
	{
	public:
		FileStream() = default;
		FileStream(const std::filesystem::path& path, FileMode mode);
		FileStream(const Ref<File>& file);

		size_t length() const override;
		size_t position() const override;
		size_t seek(size_t position) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;

	protected:
		size_t read_data(void* ptr, size_t length) override;
		size_t write_data(const void* ptr, size_t length) override;

	private:
		Ref<File> m_file;
	};

	// Memory Stream moves over an existing buffer.
	// The Buffer must exist for the duration of the Memory Stream.
	class MemoryStream : public Stream
	{
	public:
		MemoryStream() = default;
		MemoryStream(uint8* data, size_t length);
		MemoryStream(const uint8* data, size_t length);

		size_t length() const override;
		size_t position() const override;
		size_t seek(size_t seek_to) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;

		uint8* data();
		const uint8* data() const;

	protected:
		size_t read_data(void* ptr, size_t length) override;
		size_t write_data(const void* ptr, size_t length) override;

	private:
		uint8* m_data = nullptr;
		const uint8* m_const_data = nullptr;
		size_t m_length = 0;
		size_t m_position = 0;
	};

	// Buffer Stream reads and writes to an internal buffer.
	// It will grow the capacity if it needs to while writing.
	class BufferStream : public Stream
	{
	public:
		BufferStream() = default;
		BufferStream(int capacity);

		size_t length() const override;
		size_t position() const override;
		size_t seek(size_t seek_to) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;

		void resize(size_t length);
		void clear();
		uint8* data();
		const uint8* data() const;

	protected:
		size_t read_data(void* ptr, size_t length) override;
		size_t write_data(const void* ptr, size_t length) override;

	private:
		std::vector<uint8> m_buffer;
		size_t m_position = 0;
	};
}