# Europa PS2 Texture File (.tex)

On PS2, the Europa engine uses a homebrew texture format, known as "YATF" or, ~~probably~~ definitely **Y**et **A**nother **T**exture **F**ormat. Glad to know this team had some humour to them.

# Versions

| Version | Details                                                                                                          |
| ------- | ---------------------------------------------------------------------------------------------------------------- |
| 1       | Introduced in Starfighter.                                                                                       |
| 2       | Introduced in Jedi Starfighter. Breaks the FourCC magic and adds a 4bpp mode while jumbling all the other modes. |

# Header

The .tex file header is one of the few filetypes in Europa which uses a proper FourCC magic. However this becomes rapidly annoying as you'll see later.

```cpp
struct yatfHeader {
    uint32_t magic;
    uint16_t version;
    Format format; // Version-specific
    uint8_t unknown; // ?
    uint32_t unknownZeroed; // Zeroed on all files

    uint32_t width;
    uint32_t height;
};
```

### Version 1

Version 1 writes its magic (in LE) as `FTAY`.

Version 1 formats are:

| Int | Format type            |
| --- | ---------------------- |
| 0   | 8bpp CLUT (256 colors) |
| 2   | 24bpp direct color     |
| 3   | 32bpp RGBA8888         |

### Version 2

Version 2 writes its magic (in LE) as `YATF`. I have no clue why they did this because it pretty much means they broke the format anyways.

Version 2 formats are:

| Int | Format type            |
| --- | ---------------------- |
| 1   | 8bpp CLUT (256 colors) |
| 3   | 24bpp direct color     |
| 4   | 32bpp RGBA8888         |
| 5   | 4bpp CLUT (16 colors)  |
