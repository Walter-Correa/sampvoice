/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <type_traits>

#include <system/types.hpp>
#include <other/utils.hpp>

template <class... Types>
struct DataPacket {

    DataPacket() noexcept = default;
    ~DataPacket() noexcept = default;
    DataPacket(const DataPacket&) noexcept = default;
    DataPacket(DataPacket&&) noexcept = default;
    DataPacket& operator=(const DataPacket&) noexcept = default;
    DataPacket& operator=(DataPacket&&) noexcept = default;

public:

    static constexpr size_t Count() noexcept
    {
        return sizeof...(Types);
    }

    static constexpr size_t Size() noexcept
    {
        return (... + sizeof(Types));
    }

private:

    template <class Type>
    static inline void _bswap_helper(Type& value) noexcept
    {
        if constexpr (std::is_same_v<Type,  sword_t> || std::is_same_v<Type,  uword_t>
                   || std::is_same_v<Type, sdword_t> || std::is_same_v<Type, udword_t>
                   || std::is_same_v<Type, sqword_t> || std::is_same_v<Type, uqword_t>
                   || std::is_same_v<Type, fdword_t> || std::is_same_v<Type, fqword_t>)
        {
            utils::bswap(&value);
        }
    }

public:

    void ToggleEndian() noexcept
    {
        ubyte_t* iterator = _buffer;
        ((_bswap_helper(*reinterpret_cast<Types*>(iterator)),
            iterator += sizeof(Types)), ...);
    }

public:

    template <bool Endian = HostEndian>
    void Get(Types&... buffers) const noexcept
    {
        const ubyte_t* iterator = _buffer;
        ((buffers = *reinterpret_cast<const Types*>(iterator),
            iterator += sizeof(Types)), ...);

        if constexpr (Endian != HostEndian)
            { ((_bswap_helper(buffers)), ...); }
    }

    template <bool Endian = HostEndian>
    void Set(Types... values) noexcept
    {
        if constexpr (Endian != HostEndian)
            { ((_bswap_helper(values)), ...); }

        ubyte_t* iterator = _buffer;
        ((*reinterpret_cast<Types*>(iterator) = values,
            iterator += sizeof(Types)), ...);
    }

private:

    ubyte_t _buffer[Size()];

};

template <bool Endian = HostEndian, class... Types>
DataPacket<Types...> MakePacket(const Types... values) noexcept
{
    DataPacket<Types...> packet;
    packet.template Set<Endian>(values...);
    return packet;
}
