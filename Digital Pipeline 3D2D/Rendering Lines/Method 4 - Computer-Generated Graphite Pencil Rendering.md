[[SOURCES#^41dd56|SOURCE]]

1. Simulated drawing materials – Modeled graphite pencil (hardness, point shape, pressure distribution) and drawing paper (tooth/texture as a height field, lead accumulation).
2. Defined pencil stroke primitive – Stroke path with character function controlling pressure, point shape, waviness, slanting, and wrist/arm movement.
3. Defined mark-making primitive – Collections of parallel strokes (hatching, zigzag, feathering) to create tones and textures.
4. Rendered outlines – Drew visible edges using uniform, accented, or sketchy strokes, with pressure varied by edge lightness values.
5. Rendered tonal shading – For each visible face: computed shading direction (projected normal), placed mark-making primitive orthogonal to it, clipped strokes to the face.
6. Used a tone value chart – Precomputed mapping of target tone (from flat-shaded Phong illumination) to pencil hardness, pressure, and number of layers.
7. Matched target tone – Looked up chart to determine layers, pressure, and pencil grade for each face or shadow.
8. Controlled drawing composition – Ordered and repeated rendering steps (preparatory sketch → delineation → shading → finishing) with configurable pencil parameters per step.

# Results
![[Pasted image 20260610190133.png]]
![[Pasted image 20260610190159.png]]