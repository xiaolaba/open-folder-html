# Custom URI Scheme File Explorer Launcher

A lightweight, cross-platform workflow solution that allows web applications (`.html` pages running in a browser) to open local Windows File Explorer directories or native applications (like Notepad) via a custom URL protocol scheme. 

This project uses a compiled C++ Win32 wrapper (`open-folder.exe`) to securely handle, decode, and sanitize arguments passed from the browser before triggering system processes.

## 🚀 Features

* **Custom URL Protocols:** Register `mynotepad://`, `myexplorer://`, and `myexplorersubfolder://` custom URI schemes.
* **Web-to-Local Integration:** Launch local directory views directly from standard HTML links or button clicks.
* **Base64 & UTF-8 Robustness:** Handles complex file paths, spaces, Asian language characters (such as Traditional Chinese), and special symbols safely using standard Base64 encoding.
* **One-Click Registry Setup:** Easily install or completely tear down the protocol definitions using `.reg` files.

---

## 🛠️ Project Structure

* `open-folder.html`: The web frontend interface. Features an interactive UI with dynamic path injection and standard Javascript `btoa()` encoding to transport full paths safely.
* `open-folder.exe`: The pre-compiled C++ Win32 background handler (built via MinGW). It intercepts the browser URL argument, strips the protocol prefix, decodes the path string, and calls the native Windows API.
* `open-folder-html-protocol.reg`: Registry script to register the custom URI schemes into `HKEY_CLASSES_ROOT`.
* `open-folder-html-remove-protocol.reg`: Cleanup script to completely remove the custom URI protocols from your system registry.
* `demo.JPG`: Visual preview/screenshot of the application workflow in action.

---

## ⚙️ Architecture & Data Flow

Standard browser security restrictions prevent websites from directly calling local system binaries. This project bypasses that safely using the following flow: