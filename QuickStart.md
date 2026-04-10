# QuickStart / 快速上手指南

Welcome to **TinyKeys**. This document serves as your synthesizer operation manual, guiding you through performance techniques and sound design parameters. 
欢迎使用 **TinyKeys**。本文档作为您的合成器操作手册，将指导您掌握演奏技巧与声音设计参数。

---

## 1. Performance Interface / 演奏界面

You only need your QWERTY keyboard to start playing. The layout is designed to mimic an isomorphic instrument (default setting is like a bass guitar or stringed instrument).
您只需使用 QWERTY 键盘即可开始演奏。其布局设计旨在模拟同构乐器（如贝斯或弦乐器）。

```apl
[1] [2] [3] [4] [5] [6] [7] [8] [9] [0] [-] [=] [BS]
  [Q] [W] [E] [R] [T] [Y] [U] [I] [O] [P] [[] []]
   [A] [S] [D] [F] [G] [H] [J] [K] [L] [;] [']
[SH] [Z] [X] [C] [V] [B] [N] [M] [,] [.] [/]   [↑]
[ALT]     [          SPACE          ]       [←][↓][→]
```

### Control Keys / 控制键说明

*   `[↑][↓][←][→]`: **Select Parameter / 菜单导航**
*   `[CTRL] + [↑]/[↓]`: **Adjust Value / 调节参数值**
*   `[Mouse Wheel]`: **Wheel Control / 滚轮控制**
*   `[CTRL] + [S]`: **Save Preset / 保存预设**
*   `[L-SHIFT]`: **Tremolo Switch / Tremolo音量颤音开关**
*   `[L-ALT]`: **Tape Echo Switch / 磁带延迟开关**
*   `[SPACE]`: **Vibrato Switch / Vibrato音高颤音开关**

When `Wheel: Assign` is set to `None`, the mouse wheel controls the currently selected menu item.  
当 `Wheel: Assign` 设为 `None` 时，鼠标滚轮会控制当前选中的菜单项。  

When `Wheel: Assign` is set to `Master` or `Cutoff`, the mouse wheel always controls that target first, regardless of cursor position.  
当 `Wheel: Assign` 设为 `Master` 或 `Cutoff` 时，无论光标停在哪个菜单项上，鼠标滚轮都会优先控制该目标。  


### Note Layout / 音符排列规律

All other keys are dedicated to performance. The default mapping follows a strict geometric rule:
其他按键均为演奏键。默认映射遵循严格的几何规律：

*   The lowest note is at the top-left `[1]`, and the highest is at the bottom-right `[/]`.
    左上角 `[1]` 为最低音，右下角 `[/]` 为最高音。
*   Adjacent keys in the **same row** are separated by a **semitone (Minor 2nd)**.
    **同一行**相邻的按键相差**半音（小二度）**。
*   Adjacent keys in the **same column (vertical)** are separated by a **Perfect 4th**. This is identical to the standard tuning of a Bass guitar or the lower strings (E, A, D, G) of a standard guitar.
    **同一列（垂直方向）**相邻的按键相差**纯四度**。这与标准贝斯的定弦，或吉他低音弦（E、A、D、G）的音程关系完全相同。

---

## 2. Settings Menu  /  合成器参数设置

The Settings Menu allows you to sculpt your tone and adjust global behaviors. Use the arrow keys to navigate and modify these values.
设置菜单允许您塑造音色并调整全局行为。使用方向键导航并修改这些数值。



### Global & Master / 全局与主控参数

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `Semitone` | Transpose the entire keyboard by semitones. | 半音移调（移调整个键盘） |
| `Octave` | Transpose the entire keyboard by octaves. | 八度移调 |
| `ShowKbd` | Toggle the visual keyboard UI on/off to save screen space. | 界面虚拟键盘显示开关 |
| `Master` | Master output volume (0% - 200%). Applied at the final output stage, so sustained notes also respond in real time. | 主输出音量（在最终输出级生效，持续音也会实时响应） |

### SYNTH / 合成器核心层

Shapes the core tone before modulation and delay. This row combines the ADSR envelope with a resonant low-pass filter.
这一行负责调制和延迟之前的核心音色塑形，将 ADSR 包络与共振低通滤波器合并在一起。

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `A` (Attack) | Time taken to reach maximum volume after key press. | 起音时间（按下后达到最大音量的时间） |
| `D` (Decay) | Time taken to drop from peak volume to sustain level. | 衰减时间（从最大音量降至延音电平的时间） |
| `S` (Sustain) | The volume level held while the key remains pressed. | 延音电平（按住按键时保持的音量大小） |
| `R` (Release) | Time taken for the sound to fade out after key release. | 释音时间（松开按键后声音淡出的时间） |
| `LP` (Cutoff) | Resonant low-pass cutoff frequency. Uses a logarithmic response for smoother low-end sweeps. | 低通截止频率（使用对数曲线，低频扫动更细腻） |
| `Q` | Resonance amount of the low-pass filter. Higher values make the cutoff peak more pronounced. | 低通滤波器共振强度（值越高，截止点峰值越明显） |

### "VB-2" Vibrato / 音高颤音（揉弦）效果器

Simulates pitch modulation. Activated by the `[SPACE]` key.
模拟音高调制。通过 `[SPACE]` 键激活。

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `Spd` (Speed) | Rate of the LFO pitch modulation (in Hz). | 调制速度（LFO频率） |
| `Dep` (Depth) | Intensity of the pitch variation (in cents). | 调制深度（音高变化的幅度，单位：音分） |
| `Mode` | `Latch`: Tap to toggle on/off. `Unlatch`: Active only while held. | 触发模式（Latch: 按下切换 / Unlatch: 按住生效） |
| `Rise` | Time taken for the vibrato to fade in to full depth. | 渐入时间（揉弦效果达到最大深度的缓冲时间） |

### "Trelicopter" Tremolo / 音量颤音效果器

Simulates amplitude (volume) modulation. Activated by the `[L-SHIFT]` key.
模拟振幅（音量）调制。通过 `[L-SHIFT]` 键激活。

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `Spd` (Speed) | Rate of the volume oscillation (in Hz). | 振荡速度 |
| `Dep` (Depth) | Intensity of the volume reduction. | 振荡深度（音量削减的幅度） |
| `Bias` | Adjusts the symmetry of the waveform (Duty cycle). | 波形偏置（调节振荡波形的占空比/非对称性） |

### "RE-20" Tape Echo / 磁带延迟效果器

An analog-tape-style delay with saturation and wow/flutter. Activated by the `[L-ALT]` key.
模拟磁带风格延迟，带有饱和度与磁带抖动效果。通过 `[L-ALT]` 键激活。

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `T` (Time) | Delay time (in milliseconds). | 延迟时间（回声间隔，单位：毫秒） |
| `M` (Mix) | Dry/Wet signal blend. Higher means louder echoes. | 混合比例（干湿比，值越大回声越响） |
| `FB` (Feedback) | Amount of signal fed back. Higher means more repeats. | 反馈量（值越大回声重复次数越多） |
| `Sa` (Saturation)| Tape saturation/distortion on the delayed signal. | 磁带饱和度（为回声添加温暖的模拟失真） |
| `Sp` (LFO Speed) | Speed of the tape mechanism modulation (Wow & Flutter). | 磁带抖动速度（LFO频率） |
| `Dp` (LFO Depth) | Intensity of the tape mechanism modulation. | 磁带抖动深度（模拟电机不稳定的音高偏移） |

### Wheel / 滚轮控制

Adds a performance layer for live gestures without changing the current menu focus.
提供一个独立的现场控制层，让您在不切换当前菜单焦点的情况下进行实时调制。

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `Assign` | Selects which parameter the mouse wheel controls. Current built-in targets are `None`, `Master`, and `Cutoff`. | 指定鼠标滚轮控制的参数，当前内置目标为 `None`、`Master`、`Cutoff` |
| `Sense` | Scales wheel sensitivity to match different mice and DPI settings. | 调整滚轮灵敏度，以适配不同鼠标与滚轮分辨率 |

## 3. Preset Function / 预设功能

Press `[CTRL] + [S]` to save your current settings to `config.ini` in the root directory. These values will be loaded automatically on startup.
您可以按下`[CTRL] + [S]`来保存预设，预设文件是软件根目录的`config.ini`。在软件启动时会自动读取其中的数值。
