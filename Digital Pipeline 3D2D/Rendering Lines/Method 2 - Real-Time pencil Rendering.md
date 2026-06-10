[[SOURCES#^fb8c2a|SOURCE]]

1. Use the normal and depth maps to find silhouettes and feature edges, then produce a grayscale contour image and modulate contour brightness with lighting.
2. Simulate the hand-wobble with y= a * sin(bx+c)+r:
	1. a controls amplitude
	2. b controls frequency
	3. c shifts the wave
	4. r adds randomness
	5. They distort the texture coordinates of a screen-space mesh and redraw the contour through it.
3. Renders 3–5 slightly different contour versions, layered together.
4. Projected the [[SOURCES#^5d2ace|Pencil texture generation]] onto the contours.
5. Calculate principal curvature directions over the mesh, which indicates how the surface flows.
6. Rotate texture coordinates so pencil strokes align with surface curvature.
7. For each triangle:
	1. Each vertex has its own curvature direction.
	2. Three rotated pencil textures are generated.
	3. These textures are blended together.
8. In dark regions, draws another set along the maximum curvature direction.
9. Generate a paper height field, convert it into a normal map, then use those normals during shading.
10. Adjust brightness and contrast.
# Results
![[Pasted image 20260610175651.png]]