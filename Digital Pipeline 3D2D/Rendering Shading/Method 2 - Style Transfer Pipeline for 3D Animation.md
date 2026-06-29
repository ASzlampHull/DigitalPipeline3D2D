[[SOURCES#^93be46|SOURCE]] #Methods

1. Input preparation – Take a 3D scene and a single 2D style exemplar (MatCap-like, drawn on a reference sphere) as input. Extract guide images from the 3D scene (e.g., diffuse, normal, outlines, world position).
2. Separate layer processing – Run style transfer separately for base touch layer, outline layer, and shadows to enable independent control and easier compositing.
3. Patch-based style transfer – Use classical image analogies (patch-based texture synthesis) rather than neural methods to preserve fine texture details and ensure stable, predictable outputs.
4. Single-color intermediate output – Transfer only the touch texture of a single target medium to the whole scene, producing a single-colored output to avoid artifacts from per-color transfer.
5. Temporal noise reduction – Reduce flickering by adding an advection step: use previous frame’s stylized output as a style exemplar, transfer it to the current frame using world position guides, then include that advected result as an additional guide in the current frame’s main transfer.
6. Compositing stage – Combine the stylized base touch, outlines, and shadows, then apply coloring and additional artistic adjustments (e.g., color filters, mixing multiple styles) to produce the final full-color stylized animation.

# Results
![[Pasted image 20260610192646.png]]