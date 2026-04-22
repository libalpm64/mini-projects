#pragma once

#include <windows.h>
#include <nmmintrin.h>
#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include "lazy.h"
#include "oxorany_include.h"
#include "Imports.h"
#include <unordered_map>

namespace integrity
{
    class check
    {
    public:
        check()
            : module(get_module_handle_as()), has_initial_checksums(Encrypt(0)) {
        }

        // Method to retrieve the initial checksum for the first section
        void get_initial_checksum()
        {
            if (!has_initial_checksums)
            {
                auto sections = retrieve_sections();
                if (!sections.empty())
                {
                    const auto& first_section = sections.front();
                    initial_checksums[reinterpret_cast<const char*>(first_section.name)] = first_section.checksum;
                }
                has_initial_checksums = Encrypt(1);
            }
        }

        // Method to compare the current checksum of the first section to the initial checksum
        bool compare_to_initial_checksum()
        {
            if (has_initial_checksums)
            {
                auto sections = retrieve_sections();
                if (!sections.empty())
                {
                    const auto& first_section = sections.front();
                    return first_section.checksum != initial_checksums[reinterpret_cast<const char*>(first_section.name)];
                }
            }
            return Encrypt(0);
        }

    private:
        struct section
        {
            std::uint8_t* name = {};
            void* address = {};
            std::size_t size = {};
            std::size_t characteristics = {};
            std::uint32_t checksum = {};

            bool operator==(const section& other) const noexcept
            {
                return this->checksum == other.checksum;
            }
        };

        std::uintptr_t module = {};
        std::unordered_map<std::string, std::uint32_t> initial_checksums;
        bool has_initial_checksums;

        std::vector<section> retrieve_sections() const noexcept
        {
            std::vector<section> result;

            PIMAGE_NT_HEADERS nt_headers = get_nt_headers(get_dos_header());
            if (!nt_headers) return result;

            PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt_headers);

            for (std::uint16_t index = false; index < nt_headers->FileHeader.NumberOfSections; index++, section++)
            {
                void* address = get_address_from_va<void*>(section->VirtualAddress);
                if (address)
                {
                    result.push_back({
                        section->Name,
                        address,
                        section->Misc.VirtualSize,
                        section->Characteristics,
                        crc32(address, section->Misc.VirtualSize)
                        });
                }
            }

            return result;
        }

        const PIMAGE_DOS_HEADER get_dos_header() const noexcept
        {
            return reinterpret_cast<PIMAGE_DOS_HEADER>(module);
        }

        const PIMAGE_NT_HEADERS get_nt_headers(const PIMAGE_DOS_HEADER dos_header) const noexcept
        {
            return reinterpret_cast<PIMAGE_NT_HEADERS>(module + dos_header->e_lfanew);
        }

        template <typename type = std::uintptr_t>
        const type get_address_from_va(std::uintptr_t virtual_address) const noexcept
        {
            return reinterpret_cast<type>(module + virtual_address);
        }

        static HMODULE get_module_handle() noexcept
        {
            return Imports.get_module_handleA(NULL); // Get handle to the main executable
        }

        template <typename type = std::uintptr_t>
        static const type get_module_handle_as() noexcept
        {
            return reinterpret_cast<type>(get_module_handle());
        }

        static std::uint32_t crc32(void* data, std::size_t size) noexcept
        {
            std::uint32_t result = false;

            for (std::size_t index = false; index < size; ++index)
            {
                result = _mm_crc32_u8(result, reinterpret_cast<std::uint8_t*>(data)[index]);
            }

            return result;
        }
    };
}
