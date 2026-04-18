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

## Supported Image Formats

- JPG/JPEG
- PNG
- BMP
- GIF
- TIFF
- WebP

## How to Use

1. **Run the Application**: Double-click `RandomFileOpener.exe`
2. **Click the Button**: Press the "随机打开文件" (Randomly Open File) button
3. **Automatic File Opening**: The application will:
   - Scan the current directory and all subdirectories
   - Prioritize image files
   - Randomly select a file
   - Open it with the system's default application

## Compilation

### Using MinGW

```bash
gcc -o RandomFileOpener.exe main.c -mwindows
```

### Using MSVC

```bash
cl /FeRandomFileOpener.exe main.c /link user32.lib gdi32.lib shell32.lib
```

## Project Structure

```
RandomFileOpener/
├── main.c          # Main source code
├── RandomFileOpener.exe  # Compiled executable
├── README.md       # This documentation
└── .gitignore      # Git ignore file
```

## Technical Details

- **Language**: C
- **API**: Win32 API
- **Dependencies**: None (pure Win32 API)
- **Window Size**: 300x150 pixels
- **Button Size**: 200x30 pixels
- **Window Style**: Overlapped window, always on top

## Error Handling

The application includes basic error handling for:
- Window creation failures
- Directory scanning failures
- File path retrieval failures
- Memory allocation failures

## License

MIT License

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
