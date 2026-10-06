# Next build target: v0.2 visual loop

Replace privileged object/goal coordinates with camera-derived estimates while keeping the controller API unchanged.

1. Synthetic overhead RGB renderer.
2. OpenCV detector baseline.
3. Observation adapter.
4. Visual grasp verifier.
5. Lighting, blur, occlusion, distractor, and camera-shift perturbations.
6. Detection/tracking and grasp-verification metrics.

Acceptance target: >=95% success over 100 randomized nominal-rendering scenes using vision-derived coordinates.
