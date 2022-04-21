#include "Exceptions.hpp"
#include "ByteStream.hpp"

ByteStream::ByteStream(size_t size, uint8_t byte) :
	_bytes(size, byte)
{
}

ByteStream::ByteStream(std::span<uint8_t> data) :
	_bytes(data.begin(), data.end())
{
}

ByteStream::ByteStream(std::initializer_list<uint8_t> data) :
	_bytes(data)
{
}

ByteStream::ByteStream(const std::string& data)
{
	*this << data;
}

ByteStream& ByteStream::operator << (const uint8_t byte)
{
	_bytes.emplace_back(byte);
	return *this;
}

ByteStream& ByteStream::operator << (std::span<uint8_t> data)
{
	std::copy(data.begin(), data.end(), std::back_inserter(_bytes));
	return *this;
}

ByteStream& ByteStream::operator << (const Pointer& ptr)
{
	std::copy(
		std::cbegin(ptr.Bytes),
		std::cend(ptr.Bytes),
		std::back_inserter(_bytes));

	return *this;
}

ByteStream& ByteStream::operator << (const std::string& bytes)
{
	std::stringstream stream(bytes);

	std::string raw;

	auto notDigit = [](char c)->bool
	{
		return !std::isxdigit(c);
	};

	while (std::getline(stream, raw, ' '))
	{
		if (raw.length() != 2)
		{
			throw ArgumentException(
				"Arguments have to be between 00-FF (zero padded) and separated with space!");
		}

		if (std::any_of(raw.cbegin(), raw.cend(), notDigit))
		{
			throw ArgumentException("Only hexadecimal characters accepted!");
		}

		int value = std::stoi(raw, nullptr, 16);

		if (value < 0 || value > 255)
		{
			throw ArgumentException("A byte has to be between 0-255!");
		}

		*this << static_cast<uint8_t>(value);
	}

	return *this;
}

void ByteStream::Fill(size_t n, uint8_t byte)
{
	std::fill_n(std::back_inserter(_bytes), n, byte);
}

ByteStream::operator std::span<uint8_t>()
{
	return _bytes;
}

uint8_t& ByteStream::operator [](size_t i)
{
	return _bytes[i];
}

uint8_t* ByteStream::Data()
{ 
	return _bytes.data();
}

size_t ByteStream::Size() const
{
	return _bytes.size();
}

std::ostream& operator << (std::ostream& os, const ByteStream& bs)
{
	std::ios_base::fmtflags formatFlags = os.flags();

	os.setf(std::ios::hex, std::ios::basefield);
	os.setf(std::ios::uppercase);
	os.fill('0');

	const std::string sep1 = " ";
	std::string sep2 = "";

	for (uint16_t byte : bs._bytes)
	{
		os << sep2 << std::setw(2) << byte;
		sep2 = sep1;
	}

	os.flags(formatFlags);
	
	return os;
}