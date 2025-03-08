import pefile
import struct

def analyze_binary(file_path):
    pe = pefile.PE(file_path)

    print(f"Analyzing binary: {file_path}")
    print(f"Entry Point: 0x{pe.OPTIONAL_HEADER.AddressOfEntryPoint:X}")
    print(f"Image Base: 0x{pe.OPTIONAL_HEADER.ImageBase:X}\n")
    print("Sections:")
    for section in pe.sections:
        print(f"  Name: {section.Name.decode().strip(chr(0))}, "
              f"Virtual Address: 0x{section.VirtualAddress:X}, "
              f"Size: 0x{section.Misc_VirtualSize:X}")


    print("\nGrabbing Imports:")
    if hasattr(pe, "DIRECTORY_ENTRY_IMPORT"):
        for entry in pe.DIRECTORY_ENTRY_IMPORT:
            print(f"  DLL: {entry.dll.decode()}")
            for imp in entry.imports:
                print(f"    Function: {imp.name.decode() if imp.name else 'Ordinal ' + str(imp.ordinal)}")

    print("\nGrabbing Exports:")
    if hasattr(pe, "DIRECTORY_ENTRY_EXPORT"):
        for exp in pe.DIRECTORY_ENTRY_EXPORT.symbols:
            print(f"  Function: {exp.name.decode()}, Ordinal: {exp.ordinal}, "
                  f"Address: 0x{exp.address:X}")

    print("\nDumping Strings:")
    for section in pe.sections:
        data = section.get_data()
        strings = data.split(b"\x00")
        for string in strings:
            try:
                decoded_string = string.decode("utf-8")
                if decoded_string.isprintable() and len(decoded_string) > 3:
                    print(f"  Found String: {decoded_string}")
            except UnicodeDecodeError:
                continue

    print("\nHooking Lookdown Browser")
    for section in pe.sections:
        data = section.get_data()
        offset = 0
        while offset < len(data):
            if data[offset] == 0xE8:
                rel_offset = struct.unpack("<i", data[offset + 1:offset + 5])[0]
                call_address = section.VirtualAddress + offset + 5 + rel_offset
                print(f"  Potential Hook Call at: 0x{call_address:X}")
            offset += 1

if __name__ == "__main__":
    binary_path = "LockDownBrowser.exe"
    analyze_binary(binary_path)