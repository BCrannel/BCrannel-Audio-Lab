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

Development status (2026-11-07):
- **Voice:** working first DSP pass includes a two-pole high-pass, fixed low/mid/high cleanup EQ cuts, a slow RMS leveler with up to 6 dB of gain change, soft saturation, output shaping and wet/dry reverb. A linked, zero-latency peak limiter clamps the final output to a configurable ceiling from −0.5 to −6 dBFS with instant gain reduction and an 80 ms release. This controls digital peaks; it does not guarantee safe acoustic volume in every monitoring or broadcast chain. The custom dark BCrannel editor includes a parameter-linked bypass button for quick A/B checks. Cleanup EQ is fixed-frequency rather than user-selectable. The de-ess control remains transient softening and is not a frequency-selective de-esser. Leveler does not perform intelligent gain learning or keyboard removal. Input/output, amount, leveler, cleanup bands, high-pass, color, limiter and reverb mix are connected. Several shared controls shown in the generic editor are not connected to Voice and are not represented as finished features.
- **Space:** single JUCE reverb engine with room size, damping/tone, width and fixed wet/dry balance. Independent room and reverb stages, selectable types, modulation, attack controls and presets are pending.
- **Master:** stereo mid/side width, gentle saturation/smoothing and output level. Learn/reference matching, high-frequency-only width and monitoring are pending.
- **Effector:** radio/telephone/wireless controls currently blend the same saturation effect; distinct filter models and tone shaping are pending.
- **Harmony:** pitch and harmony generation are not implemented; this plugin is transparent.
- **Tune:** pitch correction is not implemented; this plugin is transparent.

The generic JUCE parameter editor currently exposes shared development controls; some controls are not connected in every plugin. These are development builds, not release-ready plugins. All six are intended to remain free with no trial timer. The Windows x64 VST3 build and host compatibility must be confirmed before release. StreamGear compatibility must be tested by scanning and loading the plugins in StreamGear.

## License

MIT. No NoroStudio code, presets, assets, UI, or reverse engineering is used.

