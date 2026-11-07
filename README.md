# BCrannel Audio Lab

A free, original VST3 plugin suite prototype for StreamGear Mixer and other VST3 hosts.

## Build Windows plugins
1. Open the **Actions** tab in this repository.
2. Select **Build Windows VST3**.
3. Click **Run workflow** and confirm.
4. When the run finishes successfully, open the run and download the artifact
   named **BCrannel-Audio-Lab-VST3-Windows-x64**.
5. Extract the ZIP. Add the extracted `.vst3` bundles to a VST3 scan location
   supported by your host, then run the host's VST scan.

## Prototype status — please read
These are original starter implementations, not a finished or equivalent clone
of any commercial plugin:
- **VoiceProcessor:** conservative peak conditioning only. No neural denoising,
  spectral keyboard removal, or desk-impact separation yet.
- **Harmony:** subtle delayed stereo doubling, not intelligent pitch harmonies.
- **Tune:** pass-through placeholder; pitch correction is not implemented yet.
- **Space:** simple short feedback ambience.
- **Master:** gentle fixed gain trim, not a full mastering chain.
- **Effector:** soft saturation.

All six are free and have no trial timer. Test at low monitor volume first.
They are built as 64-bit Windows VST3 plugins. Compatibility with StreamGear
must be confirmed by scanning and loading them in the user's installed version.

## License
MIT. No NoroStudio code, presets, assets, UI, or reverse engineering is used.
