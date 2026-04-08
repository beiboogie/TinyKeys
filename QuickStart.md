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

*   `[L-SHIFT]`: **Tremolo Switch / Tremolo音量颤音开关**
*   `[L-ALT]`: **Tape Echo Switch / 磁带延迟开关**
*   `[SPACE]`: **Vibrato Switch / Vibrato音高颤音开关**
*   `[←][→]`: **Select Parameter / 选择菜单参数**
*   `[↑][↓]`: **Adjust Value / 调节参数值**
*   `[CTRL] + [S]`: **Save Preset / 保存当前所有参数为预设**

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

## 2. Settings Menu / 合成器参数设置

The Settings Menu allows you to sculpt your tone and adjust global behaviors. Use the arrow keys to navigate and modify these values.
设置菜单允许您塑造音色并调整全局行为。使用方向键导航并修改这些数值。

### Global & Master / 全局与主控参数

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `Semitone` | Transpose the entire keyboard by semitones. | 半音移调（移调整个键盘） |
| `Octave` | Transpose the entire keyboard by octaves. | 八度移调 |
| `ShowKbd` | Toggle the visual keyboard UI on/off to save screen space. | 界面虚拟键盘显示开关 |
| `Master` | Master output volume (0% - 200%). Watch out for clipping! | 主输出音量（注意防爆音） |

### ADSR Envelope / 包络线发生器

Shapes the volume contour of every note you play.
塑造每个音符的音量轮廓。

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `A` (Attack) | Time taken to reach maximum volume after key press. | 起音时间（按下后达到最大音量的时间） |
| `D` (Decay) | Time taken to drop from peak volume to sustain level. | 衰减时间（从最大音量降至延音电平的时间） |
| `S` (Sustain) | The volume level held while the key remains pressed. | 延音电平（按住按键时保持的音量大小） |
| `R` (Release) | Time taken for the sound to fade out after key release. | 释音时间（松开按键后声音淡出的时间） |

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

A warm, analog-style delay with saturation and wow/flutter. Activated by the `[L-ALT]` key.
温暖的模拟磁带风格延迟，带有饱和度与磁带抖动效果。通过 `[L-ALT]` 键激活。

| Setting / 设置项 | Explanation / 解释说明 | Chinese / 中文释义 |
| :--- | :--- | :--- |
| `T` (Time) | Delay time (in milliseconds). | 延迟时间（回声间隔，单位：毫秒） |
| `M` (Mix) | Dry/Wet signal blend. Higher means louder echoes. | 混合比例（干湿比，值越大回声越响） |
| `FB` (Feedback) | Amount of signal fed back. Higher means more repeats. | 反馈量（值越大回声重复次数越多） |
| `Sa` (Saturation)| Tape saturation/distortion on the delayed signal. | 磁带饱和度（为回声添加温暖的模拟失真） |
| `Sp` (LFO Speed) | Speed of the tape mechanism modulation (Wow & Flutter). | 磁带抖动速度（LFO频率） |
| `Dp` (LFO Depth) | Intensity of the tape mechanism modulation. | 磁带抖动深度（模拟电机不稳定的音高偏移） |