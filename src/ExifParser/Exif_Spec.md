# JPEG File Structure #

Every JPEG starts with the Start of Image (SOI) tag which is 0xFF 0xD8

The EXIF header meta data contains a maximum of 64KB (0xFFFF bytes) of data

[Source](https://en.wikipedia.org/wiki/Exif#:~:text=Exif%20metadata%20are%20restricted%20in,a%20single%20JPEG%20APP1%20segment)

At the very end of the file should be the End of Image (EOI) tag which is 0xFF 0xD9

At offset 2 is a tag at identifies the APPX marker

0xFF 0xE0 is for APP0

0xFF 0xE1 is for APP1

The next two bytes is the length of the APPX data section.

The length is the value shown plus two bytes for the length itself.
<h2>App0</h2>
0xFF 0xE0 0x00 0x10 means there's an APP0 data section with a length of 0x10 bytes of data starting after 0xE0.

For the purposes of this parser, the data contained in App0 is only used to check if the file is valid.
<h2>App1</h2>
0xFF 0xE1 0xB1 0x06 means there's an APP1 data section with a length of 0xB106 bytes of data starting after 0xE1.

Afer the length, the next six bytes are the characters 'Exif' followed by two 0x00 bytes in a structure called the EXIF header.

The next structure is the TIFF header, which is 8 bytes in length. The start of the TIFF header is used as the starting point for IFD offsets.
* 2 bytes for endianess
* 2 bytes that should be 0x002A, corrected for endianess
* 4 bytes which indicates where the start of the IFD data is based on the start of the TIFF header. Usually this is 0x00000008 (corrected for endianess) which indicates that the first IFD is right after this value.

<h3>Image File Directory</h3>
IFD (Image File Directory) entries contain information about what kind of meta data exists, what offset in the file the data is, the data type of that meta data, and how many entries there are.
All IFD data should be corrected for endianess.
* The first 2 bytes pointing to the start of the IFD data contains how many entries there are. The IFD entries are right after this length
* Each IFD entry is twelve bytes in length
    * 2 bytes for the entry type
    * 2 bytes for the data format
    * 4 bytes for the number of components. For example if the data format is char then this represents the number of characters in a string.
    * 4 bytes for the offset of the start of the data based on the start of the TIFF header. However if the data can fit in 4 bytes then this is where the data exists.


<h2>Sources</h2>
[MIT EXIF Documention](https://www.media.mit.edu/pia/Research/deepview/exif.html)
