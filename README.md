# 🔐 Terminal Authentication System

A secure, cross-platform, command-line authentication system built in C. It provides a robust foundation for user registration and login with professional security features like password masking, duplicate validation, and brute-force protection.

## Features

- **Password Masking**: Hides password input with `*` characters (cross-platform support for Windows and Unix-like systems).
- **Brute-Force Protection**: Locks the user out after 3 failed login attempts.
- **Input Validation**: Strict validation for email formats, phone numbers (digits only), and password length.
- **Duplicate Prevention**: Checks for existing usernames or emails before allowing registration.
- **File Persistence**: Securely stores user records in a binary file (`user.dat`).
- **Robust UX**: Continuous menu loop, automatic buffer clearing, and "Press Enter to continue" prompts for a smooth terminal experience.

## Tech Stack

- **Language**: C (C99 Standard)
- **Libraries**: `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<ctype.h>`
- **Platform-Specific**: `<conio.h>` (Windows) / `<termios.h>` & `<unistd.h>` (Linux/macOS)

## Getting Started

### Prerequisites
Make sure you have a C compiler installed:
- **Windows**: [MinGW](http://mingw.org/) or [MSVC](https://visualstudio.microsoft.com/)
- **Linux**: `gcc` (`sudo apt install gcc`)
- **macOS**: `clang` (via Xcode Command Line Tools: `xcode-select --install`)

### Compilation & Execution

1. **Clone the repository**:
   ```bash
   git clone https://github.com/abullahaljehan-me/authentication-system-c.git
   cd authentication-system-c
