# TinyKeys

A lightweight, low-latency, command-line based Isomorphic Keyboard synthesizer for Windows.

一款适用于 Windows 的轻量级、低延迟、命令行同构键盘合成器。

> Check out the  [📖 快速上手指南 (QuickStart)](QuickStart.md) for detailed controls and operation tips!

------

## Introduction

An isomorphic keyboard arranges notes in a consistent geometric pattern, making chords and scales maintain the same shape regardless of the musical key. **TinyKeys** maps a standard QWERTY keyboard into a 4-row isomorphic layout, turning your computer keyboard into a expressive musical instrument.

This project is built upon **TinySoundFont (tsf)** and **miniaudio**. Special thanks to Bernhard Schelling for creating TinySoundFont, and David Reid for miniaudio, which made this project possible.

------

## Key Features

### 🎹 Isomorphic Layout

- **Consistent Fingerings**: The 4-row layout ensures that intervals and chord shapes remain identical across all keys.
- **Full Polyphony**: Supports playing multiple notes simultaneously with high responsiveness.
- **Customizable Base**: Each row's starting pitch can be individually configured.

### 🎛️ Terminal User Interface (TUI)

- **Real-time Feedback**: Visualizers for LFO (Vibrato/Tremolo) and Delay feedback.
- **Dynamic Keyboard Display**: A visual representation of the keyboard that lights up during performance.
- **Integrated Settings**: Access and modify synthesis parameters directly within the terminal.

### 🧪 Synthesis & Effects

- **ADSR Envelope**: Fully adjustable Attack, Decay, Sustain, and Release.
- **SYNTH Filter Layer**: Adds a resonant low-pass stage with `LP` (cutoff) and `Q` controls for musical sweep effects.
- **Modulation**: Built-in Vibrato (with optional Rise time) and Tremolo effects.
- **Tape Echo**: A high-quality Delay effect with adjustable feedback, saturation, and modulation.

### 📂 Configuration & Presets

- **Registry-Based System**: A robust internal data-driven architecture for managing parameters.
- **Instant Persistence**: Save current parameters to a configuration file at any time.
- **CLI Support**: Override settings or load specific presets directly via command-line arguments.
- **Wheel Routing**: Mouse wheel input can control the selected parameter, or be assigned to `Master` / `Cutoff` with adjustable sensitivity.

### 🎚 Performance Additions

- **Post-FX Master Volume**: `Master` now works as a true output-stage gain, so held notes react immediately to volume changes.
- **SYNTH Row**: The second settings row now combines `A, D, S, R, LP, Q` for faster tone shaping.
- **Wheel Row**: A dedicated `Wheel` row provides `Assign` and `Sense` for live gesture control.

------

## Deployment & Build Instructions

This project is written in standard C11 and is designed for Windows (utilizing Windows API for low-latency async key polling).

### Requirements

- A Windows environment.
- **MinGW-w64** or **MSYS2** with `gcc` installed.
- **CMake** (Optional, for CMake build method).

### Build using Makefile

Simply open your terminal in the project root and run:

Bash

```
make
```

This will compile the source files and output `TinyKeys.exe`.

### Build using CMake

Bash

```
mkdir build
cd build
cmake ..
cmake --build .
```

------

## License

This project is licensed under the **MIT License**. 
