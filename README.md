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

## Current implementation status — prototypes only

The current source is an early build scaffold, not feature parity yet:
- **VoiceProcessor:** conservative peak conditioning only.
- **Harmony:** subtle delayed stereo doubling, not intelligent pitch harmonies.
- **Tune:** pass-through placeholder; pitch correction is not implemented yet.
- **Space:** simple short feedback ambience.
- **Master:** gentle fixed gain trim, not a full mastering chain.
- **Effector:** soft saturation.

All six are intended to remain free with no trial timer. They are built as 64-bit Windows VST3 plugins. Compatibility with StreamGear must be confirmed by scanning and loading them in the user's installed version.

## License

MIT. No NoroStudio code, presets, assets, UI, or reverse engineering is used.
