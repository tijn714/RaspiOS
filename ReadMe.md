# RaspiOS

A hobby operating system for the Raspberry Pi 4, built from scratch for the AArch64 architecture.

**Developer**: Tijn Rodrigo ([@tijn714](https://github.com/tijn714))  
**Project Name**: RaspiOS  

## Overview

RaspiOS is a lightweight, custom-built hobby operating system designed for the Raspberry Pi 4, targeting the AArch64 architecture. As more companies shift toward AArch64, this project aims to provide a minimal yet functional OS for people who are interesed in AArch64 OS development.

## Requirements

To build and run RaspiOS, you’ll need the following tools:

- **AArch64 ELF Toolchain**: Required for compiling the OS for the Raspberry Pi 4’s AArch64 architecture. Install the `aarch64-elf-toolchain` or a similar toolchain.
- **QEMU Emulation Layer**: Used to emulate the Raspberry Pi 4 environment. Ensure you have `qemu-system-aarch64` installed.

### Suggested Installation

Debian based system:
```bash
# For the toolchain
sudo apt install gcc-aarch64-linux-gnu

# Qemu
sudo apt install qemu-system-arm
```

MacOS:
```bash
# For the toolchain (Ubuntu/Debian example)
brew install aarch64-elf-gcc aarch64-elf-binutils

# For QEMU
brew install qemu
```


## Building the Project

1. Clone the repository:
   ```bash
   git clone https://github.com/tijn714/RaspiOS.git
   cd RaspiOS
   ```

2. Build the kernel image:
   ```bash
   make
   ```
   This generates `kernel8.img`, the bootable image for the Raspberry Pi 4.

3. Run the OS in QEMU:
   ```bash
   make run
   ```
   This launches the OS in an emulated Raspberry Pi 4 environment.

*Note*: The `make run` command is optimized for macOS (Cocoa framework is used for the GUI). Users on other systems (e.g., Linux or Windows) might need to modify the QEMU command in the `Makefile` to match their setup.

## Collaboration

Contributions are welcome! If you’d like to collaborate:

1. Fork the repository.
2. Create a new branch for your feature or fix. Name the branch based on what you’re implementing (e.g., `feature-memory-management` or `add-bootloader`).
3. Submit a pull request with your changes.

Feel free to reach out via GitHub Issues for questions or ideas.

## License

You are free to use this project for **learning** purposes.
<br><Br>**DO NOT USE FOR ANY COMMERCIAL USE CASES SINCE THIS IS ONLY A HOBBY PROJECT AND WILL NOT BECOME ANYTHING SERIOUS ANYTIME SOON!**

## Acknowledgments

- Built with inspiration from the growing AArch64 ecosystem.
- For more resources visit: [osdev.org](https://wiki.osdev.org/Expanded_Main_Page)

---
