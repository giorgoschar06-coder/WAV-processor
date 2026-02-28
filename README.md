# 🎵 WAV Audio File Processor

## ✨ Overview

This C program reads and processes WAV audio files. It validates file integrity and offers three main features: 
* Checking for the validation of the file (**info**)
* Adjusting playback speed (**rate**)
* Isolating the left or right audio channel (**channel**) for stereo files.


---

## 📝 Table of Contents
* [Prerequisites](#-prerequisites)
* [Compilation and Execution](#-compilation-and-execution)
* [Usage Instructions](#-usage-instructions)
* [General Functionality](#-general-functionality)
* [Internal Logic](#-internal-logic)
* [References](#-references)

---

## ⚙️ Prerequisites

* Compatible with any **C Compiler**.
* **GCC** (GNU Compiler Collection) is recommended.
* **Libraries used:** `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<limits.h>`, `<math.h>`
---

## 🛠️ Compilation and Execution
Assuming the source code is in `soundwave.c`.

### 👾 Compilation Steps
1. Open your **Terminal**.
2. Run the following `gcc` command:

```bash
gcc -Ofast -Wall -Wextra -Werror -pedantic -o soundwave soundwave.c -lm
```

Note: The -lm flag is required to link the math library for the round() function.

### 🏃 How to Run

The program reads audio data from **stdin** and outputs to **stdout** using redirection (`<`, `>`).

**1)** For the **info** of the sound file:
```bash
./soundwave info < input.wav
```
**2)** For the playback speed **(rate)** of the sound:
```bash
./soundwave rate  fp_rate < input.wav > output.wav
```
*(**fp_rate**>0.0)*

**3)** For the audio **channel** that will be isolated:
```bash
./soundwave channel  left < input.wav > output.wav
./soundwave channel  right < input.wav > output.wav
```

---

## 📜 Usage Instructions

* **Validation:** The audio file must be a valid WAV format; otherwise, the program will terminate with an error message.

```bash
test1$ ./soundwave rate 0.5 < bad_riff.wav > bad_riff1.wav
Error! "RIFF" not found
```
<u>The characteristics of a valid WAV file are located here:</u>
<https://docs.fileformat.com/el/audio/wav/>

* **Subcommands:** You must strictly provide one of the subcommands: `info`, `rate`, or `channel`.

```bash
test1$ ./soundwave < good.wav > good1.wav
Error! No subcommand provided

test1$ ./soundwave hello < good.wav > good1.wav
Error! Unknown subcommand: hello
```

* **Parameters:** The `rate` and `channel` commands must require a second argument.

```bash
test1$ ./soundwave rate < good.wav > good1.wav
Error! No subcommand provided

test1$ ./soundwave channel < good.wav > good1.wav
Error! No subcommand provided
```

* **rate:** Accepts only positive `double` values.

```bash
test1$ ./soundwave rate 0.0 < good.wav > good1.wav
Error! Rate input should be >0
```

* **channel:** Accepts only `left` or `right`.

```bash
test1$ ./soundwave channel centre < good.wav > good1.wav
Error! Channel input should be "left" or "right"
```

[!IMPORTANT]
> Input/Output redirection (`<` and `>`) is mandatory for the program to function correctly.


---

## 💻 General Functionality

Initially, the program stored the entire audio file in a large buffer using dynamic memory allocation (`malloc()`). However, this was memory-intensive and inefficient for large files.

### Current Optimized Logic
The program now processes data in small chunks (**2 or 4 bytes at a time**). It reads the header, converts bytes to the appropriate types (unsigned integers/characters) using **Little Endian** logic, and validates the file structure on the fly.
* **INFO:** Prints detailed technical specifications of the WAV file.
```bash
test1$ ./soundwave info < MyWay.wav

size of file: 53045316
size of format chunk: 16
WAVE type format: 1
mono/stereo: 2
sample rate: 48000
bytes/sec: 192000
block alignment: 4
bits/sample: 16
size of data chunk: 53045180
```

* **RATE:** Modifies the `SampleRate` and `BytesPerSec` fields in the header. 

*As data is read and validated, it is immediately streamed to **stdout** to maintain a **near-zero memory footprint**.*

```bash
test1$ ./soundwave info < BohemianRhapsody.wav

size of file: 63414308
size of format chunk: 16
WAVE type format: 1
mono/stereo: 2
sample rate: 44100
bytes/sec: 176400
block alignment: 4
bits/sample: 16
size of data chunk: 63414272

test1$ ./soundwave rate 2.0 < BohemianRhapsody.wav > BohemianRhapsody1.wav

test1$ ./soundwave info < BohemianRhapsody1.wav

size of file: 63414308
size of format chunk: 16
WAVE type format: 1
mono/stereo: 2
sample rate: 88200
bytes/sec: 352800
block alignment: 4
bits/sample: 16
size of data chunk: 63414272
```

* **CHANNEL:** For stereo files, the program modifies the header (changing channels from 2 to 1 and updating the `BlockAlign`) and then filters the data stream to output only the requested channel's bytes.

```bash
test1$ ./soundwave info < Serenade.wav

size of file: 34127908
size of format chunk: 16
WAVE type format: 1
mono/stereo: 2
sample rate: 44100
bytes/sec: 176400
block alignment: 4
bits/sample: 16
size of data chunk: 34127872

test1$ ./soundwave channel left < Serenade.wav > Serenade1.wav

test1$ ./soundwave info < Serenade1.wav

size of file: 17063972
size of format chunk: 16
WAVE type format: 1
mono/stereo: 1
sample rate: 44100
bytes/sec: 88200
block alignment: 2
bits/sample: 16
size of data chunk: 17063936
```


 ---

## ⚒️ Internal Logic

The program is modular, using specific functions for readability and maintenance.

| Category | Function | Description |
| :--- | :--- | :--- |
| **I/O & Conversion** | `readB1` | Reads a single byte using `getchar()`. |
| | `read2bytes` | Converts 2 bytes to `unsigned int` (Little Endian). |
| | `read4bytes` | Converts 4 bytes to `unsigned int` (Little Endian). |
| | `IntToChar` | Converts integers back to bytes for output. |
| **Header Validation** | `RIFF`, `WAVE`, `fmt ` | Validates mandatory WAV signature strings. |
| | `WAVE_type` | Validates the audio format type. |
| | `mono_stereo` | Determines channel count. |
| | `SampleRate` | Captures/updates sampling frequency. |
| | `BlockAlign` | Validates block alignment integrity. |
| | `DataChunk` | Locates the start of the audio data stream. |
| **Features** | `info`, `rate`, `channel` | Implements core program logic based on user input. |

---

## 📚 References

* **This project was set as one of the asignments of the first semester in the class of progintro in the University of Athens.** 

---

**1)** Description of WAV files:https://docs.fileformat.com/el/audio/wav/

**2)** Description of commands getchar()-putchar():https://www.geeksforgeeks.org/c/getchar-function-in-c/

**3)** Description of pointers: https://progintro.github.io/assets/pdf/lec11.pdf

**4)** Usage of malloc(): https://progintro.github.io/assets/pdf/lec13.pdf

**5)** Description of the project assignment: https://progintro.github.io/assets/pdf/hw1.pdf

**6)** WAV files for testing: https://github.com/progintro/wav

**7)** Markdown Tutorial(for README.md): https://www.youtube.com/watch?v=XuPg3l6MiEk&pp=0gcJCSUKAYcqIYzv
