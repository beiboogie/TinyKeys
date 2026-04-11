# QuickStart / 快速上手指南

Welcome to **TinyKeys**. This document serves as your synthesizer operation manual.

欢迎使用 **TinyKeys**。本文档将指导您掌握这台命令行合成器的演奏技巧与声音设计。

------

## 1. Performance & Controls / 演奏与控制

### Keyboard Layout / 键盘布局

You only need your QWERTY keyboard to start playing. The layout is designed to mimic an isomorphic instrument (default setting is like a bass guitar or stringed instrument).
您只需使用 QWERTY 键盘即可开始演奏。默认布局设计旨在模拟同构乐器（如贝斯或弦乐器）。

```apl
[1] [2] [3] [4] [5] [6] [7] [8] [9] [0] [-] [=] [BS]
  [Q] [W] [E] [R] [T] [Y] [U] [I] [O] [P] [[] []]
   [A] [S] [D] [F] [G] [H] [J] [K] [L] [;] [']
[SH] [Z] [X] [C] [V] [B] [N] [M] [,] [.] [/]   [↑]
[ALT]     [          SPACE          ]       [←][↓][→]
```

### Command Shortcuts / 快捷键指令

- `[↑][↓][←][→]`: **Navigate Menu / 菜单导航**
- `[CTRL] + [↑]/[↓]`: **Adjust Value / 精细调节数值**
- `[Mouse Wheel]`: **Quick Tweak / 滚轮快速调节** (根据 `Wheel` 行的 `Assign` 逻辑)
- `[CTRL] + [S]`: **Save Interface / 进入保存界面**
  - `[ENTER]`: **Overwrite / 覆盖当前预设**
  - `[CTRL] + [ENTER]`: **Save As / 另存为新预设** (在底栏输入名称)
- `[ESC]`: **Exit or Cancel / 退出程序或取消保存**

### Note Layout / 音符排列规律

All other keys are dedicated to performance. The default mapping follows a strict geometric rule:
其他按键均为演奏键。默认映射遵循严格的几何规律：

*   The lowest note is at the top-left `[1]`, and the highest is at the bottom-right `[/]`.
    左上角 `[1]` 为最低音，右下角 `[/]` 为最高音。
*   Adjacent keys in the **same row** are separated by a **semitone (Minor 2nd)**.
    **同一行**相邻的按键相差**半音（小二度）**。
*   Adjacent keys in the **same column (vertical)** are separated by a **Perfect 4th**. This is identical to the standard tuning of a Bass guitar or the lower strings (E, A, D, G) of a standard guitar.
    **同一列（垂直方向）**相邻的按键相差**纯四度**。这与标准贝斯的定弦，或吉他低音弦（E、A、D、G）的音程关系完全相同。

------

## 2. Parameter Guide / 参数详解

### System: Global Settings / 全局管理

| Setting / 设置项 | Explanation / 解释说明                    | Chinese / 中文释义                 |
| -------------------- | --------------------------------------------- | -------------------------------------- |
| `ShowKbd`            | Toggle the visual keyboard display.           | 显示/隐藏虚拟键盘动画                  |
| `Gain`             | Fre-FX Gain.                         | ADSR后，效果器前的增益 |
| `Master`             | Global output volume.                         | 全局输出音量（最后级增益）             |
| `Preset`             | Switch between `.tkp` files in the directory. | 预设切换（自动识别目录下的 .tkp 文件） |

### Tune: Pitch & Dynamics / 音调与动态

| Setting / 设置项 | Explanation / 解释说明                     | Chinese / 中文释义           |
| -------------------- | ---------------------------------------------- | -------------------------------- |
| `Semitone`           | Transpose by semitones.                        | 半音偏移                         |
| `Octave`             | Transpose by octaves.                          | 八度偏移                         |
| `P-Drift`            | Random pitch instability per note (0-5 cents). | 音高漂移（模拟硬件的不稳定性）   |
| `V-Drift`            | Random volume variation per note.              | 音量随机偏差（增加演奏的人性化） |

### Synth: Envelope & Filter / 合成与滤波

| Setting / 设置项 | Explanation / 解释说明     | Chinese / 中文释义                |
| -------------------- | ------------------------------ | ------------------------------------- |
| `A/D/S/R`            | Standard ADSR volume envelope. | 经典的 ADSR 振幅包络                  |
| `LP` (Cutoff)        | Low-pass filter frequency.     | 低通滤波器截止频率                    |
| `Q` (Res)            | Filter resonance peak.         | 滤波器谐振（增加音色的尖锐度/哇音感） |

### "VB-2" Vibrato / 音高颤音（揉弦）效果器

Simulates pitch modulation. Activated by the `[SPACE]` key.
模拟音高调制。通过 `[SPACE]` 键激活。

| Setting / 设置项 | Explanation / 解释说明                                       | Chinese / 中文释义                              |
| :--------------- | :----------------------------------------------------------- | :---------------------------------------------- |
| `Spd` (Speed)    | Rate of the LFO pitch modulation (in Hz).                    | 调制速度（LFO频率）                             |
| `Dep` (Depth)    | Intensity of the pitch variation (in cents).                 | 调制深度（音高变化的幅度，单位：音分）          |
| `Mode`           | `Latch`: Tap to toggle on/off. `Unlatch`: Active only while held. | 触发模式（Latch: 按下切换 / Unlatch: 按住生效） |
| `Rise`           | Time taken for the vibrato to fade in to full depth.         | 渐入时间（揉弦效果达到最大深度的缓冲时间）      |

### "Trelicopter" Tremolo / 音量颤音效果器

Simulates amplitude (volume) modulation. Activated by the `[L-SHIFT]` key.
模拟振幅（音量）调制。通过 `[L-SHIFT]` 键激活。

| Setting / 设置项 | Explanation / 解释说明                             | Chinese / 中文释义                        |
| :--------------- | :------------------------------------------------- | :---------------------------------------- |
| `Spd` (Speed)    | Rate of the volume oscillation (in Hz).            | 振荡速度                                  |
| `Dep` (Depth)    | Intensity of the volume reduction.                 | 振荡深度（音量削减的幅度）                |
| `Bias`           | Adjusts the symmetry of the waveform (Duty cycle). | 波形偏置（调节振荡波形的占空比/非对称性） |

### "RE-20" Tape Echo / 磁带延迟效果器

An analog-tape-style delay with saturation and wow/flutter. Activated by the `[L-ALT]` key.
模拟磁带风格延迟，带有饱和度与磁带抖动效果。通过 `[L-ALT]` 键激活。

| Setting / 设置项  | Explanation / 解释说明                                  | Chinese / 中文释义                       |
| :---------------- | :------------------------------------------------------ | :--------------------------------------- |
| `T` (Time)        | Delay time (in milliseconds).                           | 延迟时间（回声间隔，单位：毫秒）         |
| `M` (Mix)         | Dry/Wet signal blend. Higher means louder echoes.       | 混合比例（干湿比，值越大回声越响）       |
| `FB` (Feedback)   | Amount of signal fed back. Higher means more repeats.   | 反馈量（值越大回声重复次数越多）         |
| `Sa` (Saturation) | Tape saturation/distortion on the delayed signal.       | 磁带饱和度（为回声添加温暖的模拟失真）   |
| `Sp` (LFO Speed)  | Speed of the tape mechanism modulation (Wow & Flutter). | 磁带抖动速度（LFO频率）                  |
| `Dp` (LFO Depth)  | Intensity of the tape mechanism modulation.             | 磁带抖动深度（模拟电机不稳定的音高偏移） |

### Wheel / 滚轮控制

Adds a performance layer for live gestures without changing the current menu focus.
提供一个独立的实时控制层，让您在不切换当前菜单焦点的情况下进行实时调制。

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `Assign` | Selects which parameter the mouse wheel controls. Current targets are `Any`,  `None`, `Gain`, `Vol`, and `Cutoff`. | 指定鼠标滚轮控制的参数。可选目标包括`Any`、 `None`、`Gain`、 `Vol`、`Cutoff` |
| `Mode` | Toggles between `Mouse` and `Pad`. Switching to `Pad` reverses the scroll direction for a natural trackpad feel. | 切换 `Mouse` (鼠标) 或 `Pad` (触控板) 模式。后者会反转滚动方向以适配触控板逻辑 |
| `Sense` | Scales wheel sensitivity to match different mice and DPI settings. | 调整滚轮灵敏度，以适配不同鼠标与滚轮分辨率 |

------

## 3. Preset & Configuration / 预设与进阶配置

### .tkp Files

Each preset is stored as a `.tkp` file (Plain text/INI format). You can share these files or back them up.

每个预设都存储为 `.tkp` 文件。它们本质上是纯文本，您可以自由分享或编辑。

### Manual-Only Settings / 仅限手动修改的参数

The following parameters **cannot** be changed within the software UI. You can open the `.tkp` file with a text editor (like Notepad) to modify them:

以下参数**无法**在软件界面中修改，可以通过文本编辑器手动更改：

```
# Manual Config in .tkp / 预设文件中的手动配置项
row0_start=A1    # Starting pitch for the top row (1 to BS)
row1_start=D2    # Starting pitch for the second row (Q to ])
row2_start=G2    # Starting pitch for the third row (A to ')
row3_start=C3    # Starting pitch for the bottom row (Z to /)
```

> **Tip**: After manually editing and saving a `.tkp` file, simply re-select the preset in TinyKeys to apply the new row mappings.
>
> **提示**：手动修改并保存文件后，在软件中重新切回该预设即可应用新的行映射。
