# TinyKeys

A lightweight, low-latency, command-line based Isomorphic Keyboard synthesizer for Windows.
一款适用于 Windows 的轻量级、低延迟、命令行同构键盘合成器。

> Check out the [📖 快速上手指南 (QuickStart)](QuickStart.md) to master the controls in 2 minutes!

## Introduction

An isomorphic keyboard arranges notes in a consistent geometric pattern, making chords and scales maintain the same shape regardless of the musical key. This project maps a standard QWERTY keyboard into a 4-row isomorphic layout, turning your computer keyboard into a playable instrument.

This project is built upon **TinySoundFont (tsf)** and **miniaudio**. TinySoundFont is an excellent, dependency-free SoundFont2 synthesizer library written in a single C file. Special thanks to Bernhard Schelling for creating TinySoundFont, and David Reid for miniaudio, which made this project possible.

## Features & Usage Guide

### 🎹 Isomorphic Keyboard Layout

The program uses 4 rows of your QWERTY keyboard (46 keys in total):

- **Row 0** (13 keys): `1` to `Backspace`
- **Row 1** (12 keys): `Q` to `]`
- **Row 2** (11 keys): `A` to `'`
- **Row 3** (10 keys): `Z` to `/`

Each row ascends chromatically from left to right. Polyphony is fully supported.

### 🎛️ Terminal User Interface (TUI) & Settings Menu

[<img src="https://i.postimg.cc/6pQxjZyP/e30b6a7d87297c2ab7feb8dfcf5aa873.png" alt="e30b6a7d87297c2ab7feb8dfcf5aa873.png" style="zoom:33%;" />](https://postimg.cc/XptPXpKK)

The program features an animated terminal UI. You can navigate the `SettingsMenu` using your keyboard's arrow keys:

- **Left/Right**: Switch between different parameters.
- **Up/Down**: Adjust values.;

**Global Parameters:**

- `Semitone` / `Octave`: Transpose the entire keyboard.
- `ShowKbd`: Toggle the visual representation of the keyboard at the bottom.
- `Master`: Master volume output (0% to 200%).

**ADSR Envelope:**

- `A` (Attack), `D` (Decay), `S` (Sustain), `R` (Release)
  Fine-tune the synthesizer's volume envelope shape.

### 🎸 Built-in Effects (Stompboxes)

TinyIsomorphicKeyboard includes three built-in effects, each with its own dynamic visualizer bar at the bottom of the screen.

1. **"VB-2" Vibrato** (Pitch Modulation)
   - **Toggle**: Press `SPACE` to activate.
   - **Mode**: Switch between `Latch` (toggle on/off) and `Unlatch` (active only while holding SPACE).
   - **Parameters**: `Spd` (Speed in Hz), `Dep` (Depth in cents), `Rise` (Fade-in time).
2. **"Trelicopter" Tremolo** (Volume Modulation)
   - **Toggle**: Press `Left SHIFT` to activate.
   - **Parameters**: `Spd` (Speed in Hz), `Dep` (Depth in %), `Bias` (Waveform shape offset).
3. **"RE-20" Tape Echo** (Delay)
   - **Toggle**: Press `Left ALT` to activate.
   - **Parameters**: `T` (Time in ms), `M` (Mix %), `FB` (Feedback %), `Sa` (Tape Saturation %), `Sp` & `Dp` (Wow & Flutter LFO Speed/Depth).

### ⚙️ Configuration & Presets

The keyboard automatically looks for a `config.ini` in the executable directory to load your favorite settings. Since the latest update, **all synthesis parameters** can be saved and loaded.

- **CTRL + S**: Save current settings to `config.ini` (Instant Preset).

#### 📝 The `config.ini` Format

You can manually edit the file to define your startup state. Here are the most common parameters:

Ini, TOML

```
# Note Mappings
row0_start=A1
row1_start=D2
row2_start=G2
row3_start=C3

# ADSR Envelope
attack=0.01
decay=0.25
sustain=0.8
release=0.1

# Effects Control
vb2_speed=1.3
vb2_depth=16
trem_speed=3.0
re20_time=400
re20_feedback=40
etc.
```

#### 🚀 Command Line Usage

For power users, you can bypass the default config or override specific rows:

- **Load specific preset**: `isomorphic_keyboard.exe --config chill_lofi.ini`
- **Quick note override**: `isomorphic_keyboard.exe row0=D5`

## Deployment & Build Instructions

This project is written in standard C11 and is currently designed for Windows (uses Windows API for low-latency async key polling).

### Requirements

- A Windows environment.
- MinGW-w64 or MSYS2 with `gcc` installed.
- CMake (Optional, if using the CMake build method).

### Build using Makefile

Simply open your terminal in the project root and run:

```bash
make
```

This will compile `src/*.c` and output `isomorphic_keyboard.exe`.

### Build using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Run the resulting executable directly in your terminal. For the best experience, use a modern terminal emulator like Windows Terminal.

## License

This project is licensed under the MIT License. 
