#Design 

- All done in the GLSL Fragment shader.
- Method 2

|   |   |   |   |
|---|---|---|---|
|**Character**|**Icon for field**|**Icon for method**|**Visibility**|
|`-`|![](https://plantuml.com/img/private-field.png)|![](https://plantuml.com/img/private-method.png)|`private`|
|`#`|![](https://plantuml.com/img/protected-field.png)|![](https://plantuml.com/img/protected-method.png)|`protected`|
|`~`|![](https://plantuml.com/img/package-private-field.png)|![](https://plantuml.com/img/package-private-method.png)|`package private`|
|`+`|![](https://plantuml.com/img/public-field.png)|![](https://plantuml.com/img/public-method.png)|`public`|

```plantuml
class DiffuseBSDF {
+ vec3 results
- void CalculateNormals()
- void ComputeDiffuseFactor()
- void ComputeDiffuseBSDF()
}
class LightingToRGB {
+ vec3 results
- void CalculateLights()
- void CalculateColours(vec3 BSDFresults)
}
DiffuseBSDF -- LightingToRGB
class ColourRamp {
- vec3 darkColours
- vec3 midColours
- vec3 brightColours
+ vec3 convertedColours
- void ConvertRGBToLuminance(vec3 RGBresults)
- void ColourRamp(float luminance)
- void MixSmoothSteps()
}
LightingToRGB -- ColourRamp
class Emission {
- float emissionStrength
+ vec3 finalColours
- void CalculateEmission(vec3 RampResults)
}
ColourRamp -- Emission
```
