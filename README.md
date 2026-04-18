# Random File Opener

A simple Windows application that randomly opens files from the current directory and its subdirectories, with priority given to image files.

## Features

- **Pure Win32 API Implementation**: No third-party dependencies
- **Window Always on Top**: Ensures the application window stays visible
- **Simple UI**: Clean and minimal interface with only essential elements
- **Recursive File Scanning**: Scans all subdirectories from the executable's location
- **Image File Priority**: Prefers to open image files when available
- **System Default Application**: Opens files using the system's default program
- **Error Handling**: Basic error handling for robustness
- **Chinese Language Support**: Proper display of Chinese characters
- **Command-Line Interface**: Full CLI support for terminal execution

## Installation

### Pre-built Executable

Download the latest release from the [GitHub Releases](https://github.com/uuvccc/RandomFileOpener/releases) page.

### Build from Source

#### Using MinGW

```bash
gcc -o RandomFileOpener.exe main.c -mwindows
```

#### Using MSVC

```bash
cl /FeRandomFileOpener.exe main.c /link user32.lib gdi32.lib shell32.lib
```

## Usage

### GUI Mode

Launch the application without any arguments to open the graphical user interface:

```bash
RandomFileOpener.exe
RandomFileOpener.exe --gui
RandomFileOpener.exe -g
```

Click the "随机打开文件" (Randomly Open File) button to open a random file.

### Command-Line Mode

The application can be run entirely from the command line without displaying a GUI window.

#### Basic Commands

```bash
# Show help information
RandomFileOpener.exe --help
RandomFileOpener.exe -h

# Show version information
RandomFileOpener.exe --version
RandomFileOpener.exe -v

# Open a random file from the current directory
RandomFileOpener.exe -i

# Open a random image from the current directory
RandomFileOpener.exe --images-only

# Open a random file from a specific directory
RandomFileOpener.exe "C:\path\to\directory"

# Open a random image from a specific directory
RandomFileOpener.exe -i -d "C:\path\to\directory"
RandomFileOpener.exe -i "C:\path\to\directory"
```

#### Command-Line Options

| Option | Description |
|--------|-------------|
| `-h, --help` | Show help message and exit |
| `-v, --version` | Show version information and exit |
| `-g, --gui` | Launch GUI window (default when no args provided) |
| `-i, --images-only` | Only select from image files |
| `-d, --directory DIR` | Specify target directory |

#### Examples

```bash
# Launch GUI window
RandomFileOpener.exe

# Open a random image from current directory
RandomFileOpener.exe -i

# Open a random file from specified directory
RandomFileOpener.exe C:\Users\Public\Pictures

# Open a random image from specified directory
RandomFileOpener.exe -i -d C:\Users\Public\Pictures

# Use with environment variables
RandomFileOpener.exe %USERPROFILE%\Pictures

# Use with relative paths
RandomFileOpener.exe .\photos
```

## Supported Image Formats

- JPG/JPEG
- PNG
- BMP
- GIF
- TIFF
- WebP

## Project Structure

```
RandomFileOpener/
├── main.c                   # Main source code
├── RandomFileOpener.exe     # Compiled executable
├── README.md                # This documentation
├── .gitignore               # Git ignore file
└── .github/
    └── workflows/
        └── build-release.yml # CI/CD workflow
```

## Technical Details

- **Language**: C
- **API**: Win32 API
- **Dependencies**: None (pure Win32 API)
- **Window Size**: 300x150 pixels
- **Button Size**: 200x30 pixels
- **Window Style**: Overlapped window, always on top
- **CLI Support**: Full command-line interface with help and version info

## Error Handling

The application includes basic error handling for:
- Window creation failures
- Directory scanning failures
- File path retrieval failures
- Memory allocation failures
- Invalid command-line arguments

## Cross-Platform Compatibility

While this application uses Win32 API for Windows-specific features (GUI and ShellExecute), the command-line parsing and file scanning logic is designed to be portable. For non-Windows platforms, you can use the `-i` flag in WSL or virtualized environments.

## License

MIT License

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
