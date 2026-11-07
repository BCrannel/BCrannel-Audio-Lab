# BCrannel Audio Lab

A free, original VST3 plugin suite for StreamGear Mixer and other VST3 hosts.

## Project goal: six free alternatives

The goal is to implement the useful feature set of the six reference plugins as original software, with our own interface, DSP, code, and presets. No trial timer, subscription, paid unlock, or feature lock is planned.

| Plugin | Feature parity target |
|---|---|
| **VoiceProcessor** | Voice analysis/learn, adjustable processing amount, de-essing, low/mid/high cleanup, tone/color blend, leveler, reverb, output limiter, stream/monitor mix controls where the host supports them |
| **Harmony** | Automatic/manual key selection, key-change tracking, up to two harmony voices, selectable intervals, chord/line modes, double, humanize, tone, width, harmony/monitor levels, keyboard display |
| **Tune** | Real-time pitch correction, key and scale selection, retune speed, fast/medium/slow behavior, humanize, flex-tune, tracking, A4/detune reference, low-latency mode and latency display |
| **Space** | Independent room and reverb stages, room/verb types, size and decay, tone filters, diffusion/modulation/attack controls, and original presets |
| **Master** | Learn/reference mode, smooth/harshness control, high-frequency stereo width with mono compatibility, stream-listen monitoring where supported |
| **Effector** | Voice character effects such as radio, telephone, and wireless/radio-transmission styles, with adjustable intensity and tone |

These are feature targets based on publicly described product functions. Exact internal algorithms, latency, and sound cannot be promised to match proprietary plugins; implementations will be independently designed and validated by ear and measurement.

## Build Windows plugins

1. Open the **Actions** tab in this repository.
2. Select **Build Windows VST3**.
3. Click **Run workflow** and confirm.
4. When the run finishes successfully, open the run and download the artifact named **BCrannel-Audio-Lab-VST3-Windows-x64**.
5. Extract the ZIP. Add the extracted VST3 bundles to a VST3 scan location supported by your host, then run the host's VST scan.

## Current development status

The first DSP foundation pass is now in the source:
- **VoiceProcessor:** adjustable input/output, gentle level conditioning, soft saturation/color, output ceiling and basic reverb foundation. The current de-ess control is only a simple transient-softening approximation, not a frequency-selective de-esser. Voice analysis, spectral cleanup, and proper stream/monitor routing are still to be built.
- **Space:** JUCE reverb engine with room, damping/tone, width and wet/dry foundations. Separate room/reverb engines, preset library and remaining shaping controls are still to be built.
- **Master:** stereo mid/side width and gentle smoothing foundation. Learn/reference matching and high-frequency-only width are still to be built.
- **Effector:** adjustable saturation/character foundation. Distinct radio, telephone and wireless filter models are still to be built.
- **Harmony:** pitch/harmony engine not implemented yet.
- **Tune:** pitch-correction engine not implemented yet.

The generic JUCE parameter editor currently exposes development controls; a custom BCrannel interface is planned. These are development builds, not release-ready plugins. All six are intended to remain free with no trial timer. They are built as 64-bit Windows VST3 plugins; compatibility must be confirmed by scanning and loading them in StreamGear.

## License

MIT. No NoroStudio code, presets, assets, UI, or reverse engineering is used.
