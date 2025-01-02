
# `.cex` - Compiled Executable Format

`.cex` is a cross-platform compiled executable file format that allows packaging source code (C, C++, Python, Ruby, Fortran, Lisp) along with its dependencies into a single `.cex` file. `.cex` files are lightweight, portable, and executable on any system with the `.cexr` runtime.

---

## Features
- **Multi-language support**: Supports C, C++, Python, Ruby, Fortran, and Lisp.
- **Cross-platform**: Works on any operating system with the `.cexr` runtime.
- **Self-contained**: Packages source files and metadata into a single file.
- **Simple configuration**: Uses `config.toml` for easy setup.

---

## Installation

### **1. Clone the Repository**
```bash
git clone https://github.com/your-username/cex.git
cd cex
```

### **2. Install Dependencies**
#### On Linux (Debian-based systems):
```bash
sudo apt update
sudo apt install -y build-essential g++ gcc gfortran python3 ruby sbcl zlib1g-dev libjsoncpp-dev
```

#### On Windows (MSYS2):
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-g++ mingw-w64-x86_64-gfortran python ruby sbcl zlib jsoncpp
```

### **3. Compile `cexc` and `cexr`**
```bash
g++ -std=c++20 -o cexc cexc.cpp -lz -ljsoncpp -ltoml11
g++ -std=c++20 -o cexr cexr.cpp -lz -ljsoncpp
```

---

## Usage

### **Creating a `.cex` File**
1. Create a folder containing your source code and a `config.toml` file.
2. Use the `cexc` compiler to package the folder into a `.cex` file:
   ```bash
   ./cexc <folder_name> <output.cex>
   ```

#### **Example**
For a Python project:
```bash
./cexc my_python_project python.cex
```

---

### **Running a `.cex` File**
Use the `cexr` runtime to execute the `.cex` file:
```bash
./cexr <file.cex>
```

#### **Example**
```bash
./cexr python.cex
```

---

## `config.toml` Format

Each project must include a `config.toml` file with the following structure:

```toml
[general]
entry_point = "main.cpp"  # Entry point for the project (e.g., Python, C++ file)

[dependencies]
libraries = ["gtkmm-3.0", "pthread"]  # Libraries to link (for C/C++)
packages = ["requests"]              # (Optional) Python packages or Ruby gems
```

---

## Supported Languages

| Language   | File Extensions | How it Works                                   |
|------------|-----------------|-----------------------------------------------|
| **Python** | `.py`           | Directly executed via `python3`.              |
| **Ruby**   | `.rb`           | Directly executed via `ruby`.                 |
| **Lisp**   | `.lisp`         | Executed via `sbcl --script`.                 |
| **C**      | `.c`            | Compiled to binary via `gcc`.                 |
| **C++**    | `.cpp`          | Compiled to binary via `g++`.                 |
| **Fortran**| `.f90`, `.f95`  | Compiled to binary via `gfortran`.            |

---

## Example Projects

### **Python Example**
**Folder Structure**:
```
python_project/
├── hello.py
└── config.toml
```

**`hello.py`**:
```python
print("Hello from Python!")
```

**`config.toml`**:
```toml
[general]
entry_point = "hello.py"
[dependencies]
packages = ["requests"]
```

**Command**:
```bash
./cexc python_project python.cex
./cexr python.cex
```

---

### **C++ Example**
**Folder Structure**:
```
cpp_project/
├── hello.cpp
└── config.toml
```

**`hello.cpp`**:
```cpp
#include <iostream>
int main() {
    std::cout << "Hello from C++!" << std::endl;
    return 0;
}
```

**`config.toml`**:
```toml
[general]
entry_point = "hello.cpp"
[dependencies]
libraries = ["pthread"]
```

**Command**:
```bash
./cexc cpp_project cpp.cex
./cexr cpp.cex
```

---

## Contributing
Contributions are welcome! Feel free to submit a pull request or open an issue.

---

## License
This project is licensed under the MIT License.
